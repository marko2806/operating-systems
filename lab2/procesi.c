#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int brojIteracija, brojProcesa;
int id;
int *a;

void potprogram_procesa() 
{
        for (int i = 0; i < brojIteracija; i++)
                (*a)++;
        return;
}

void obrisi_dijeljenu_memoriju(int sig) 
{
        (void)shmdt((char *)a);
        (void)shmctl(id, IPC_RMID, NULL);
        exit(0);
}

int main(int argc, char *argv[]) 
{

	if(argc != 3){
		fprintf(stderr,"Neispravan unos parametara: broj_procesa broj_iteracija\n");
		exit(1);
	}

        brojProcesa = atoi(argv[1]);
        brojIteracija = atoi(argv[2]);

        sigset(SIGINT, obrisi_dijeljenu_memoriju);
        id = shmget(IPC_PRIVATE, sizeof(int), 0600);
        

        if (id == -1) 
                exit(1);

        a = (int *)shmat(id, NULL, 0);

        for (int i = 0; i < brojProcesa; i++) {
                if (fork() == 0) {
                        potprogram_procesa();
                        exit(0);
                }
        }

        for (int j = 0; j < brojProcesa; j++) {
                wait(NULL);
        }

        printf("A=%d\n", *a);
        obrisi_dijeljenu_memoriju(0);
        return 0;
}