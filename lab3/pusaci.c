#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

sem_t *kriticni_odsjecak;
sem_t *pusac1;
sem_t *pusac2;
sem_t *pusac3;
sem_t *stol_prazan;
int *stol1;
int *stol2;

const char *sastojak(int i) 
{
        if (i == 0)
                return "duhan";
        else if (i == 1)
                return "papir";
        else if (i == 2)
                return "sibice";
        
}

int prvi_sastojak() 
{ 
	return rand() % 3; 
}

int drugi_sastojak(int s1) 
{
        int s2 = rand() % 3;
        while (s2 == s1) 
                s2 = rand() % 3;
        return s2;
}

void postavi_sastojke_na_stol(int s1, int s2) 
{
        *stol1 = s1;
        *stol2 = s2;
        printf("Trgovac stavlja: %s i %s\n", sastojak(*stol1),
               sastojak(*stol2));
}

void trgovac() 
{
        do {
                int s1 = prvi_sastojak();
                int s2 = drugi_sastojak(s1);
                sem_wait(kriticni_odsjecak);
                postavi_sastojke_na_stol(s1, s2);
                sem_post(kriticni_odsjecak);
                sem_post(pusac1);
                sem_post(pusac2);
                sem_post(pusac3);
                sem_wait(stol_prazan);
        } while (1);
}

int prvi_sastojak_pusaca(sem_t *p) 
{
        if (p == pusac1) 
                return 1;
        else if (p == pusac2) 
        	return 2;
        else if (p == pusac3) 
                return 0;
        else
                return 5;
        
}

int drugi_sastojak_pusaca(sem_t *p) 
{
        if (p == pusac1)
                return 2;
        else if (p == pusac2) 
                return 0;
        else if (p == pusac3) 
                return 1;
        else 
                return 5;
}

int na_stolu_sastojci(int s1, int s2) 
{
        if ((s1 == *stol1 && s2 == *stol2) || (s2 == *stol1 && s1 == *stol2))
                return 1;
        else
                return 0;
}

void uzmi_sastojke(sem_t *p) 
{
        int i = 0;
        if (p == pusac1)
                i = 1;
        else if (p == pusac2)
                i = 2;
        else if (p == pusac3)
                i = 3;

        printf("Pusac %d uzima sastojke: %s i %s\n\n", i, sastojak(*stol1),
               sastojak(*stol2));
        *stol1 = 5;
        *stol2 = 5;
        sleep(1);
}

void ispisi_sastojak_pusaca(sem_t *p) 
{
        if (p == pusac1)
                printf("Pusac 1 ima duhan\n");
        else if (p == pusac2)
                printf("Pusac 2 ima papir\n");
        else if (p == pusac3)
                printf("Pusac 3 ima sibice\n");
        else 
                printf("Dogodila se greska");
}

void pusac(sem_t *p) 
{
        int r1 = prvi_sastojak_pusaca(p);
        int r2 = drugi_sastojak_pusaca(p);
        do {
                sem_wait(kriticni_odsjecak);
                if (na_stolu_sastojci(r1, r2)) {
                        uzmi_sastojke(p);
                        sem_post(kriticni_odsjecak);
                        sem_post(stol_prazan);
                        // smotaj_zapali_pusi

                } else {
                        sem_post(kriticni_odsjecak);
                }
        } while (1);
}

void prekidna_rutina(int sig){
	shmdt((sem_t *)pusac1);
	shmdt((sem_t *)pusac2);
	shmdt((sem_t *)pusac3);
	shmdt((sem_t *)stol_prazan);
	shmdt((sem_t *)kriticni_odsjecak);
	shmdt((int *)stol1);
	shmdt((int *)stol2);

	sem_destroy(pusac1);
        sem_destroy(pusac2);
        sem_destroy(pusac3);
        sem_destroy(stol_prazan);
        sem_destroy(kriticni_odsjecak);

	exit(0);
}

int main() 
{
        int ID = shmget(IPC_PRIVATE, 5 * sizeof(sem_t) + 2 * sizeof(int), 0600);

	sigset(SIGINT,prekidna_rutina);
        srand(time(NULL));

        pusac1 = shmat(ID, NULL, 0);
        pusac2 = pusac1 + 1;
        pusac3 = pusac2 + 1;
        kriticni_odsjecak = pusac3 + 1;
        stol_prazan = kriticni_odsjecak + 1;
        stol1 = (int *)(stol_prazan + 1);
        stol2 = stol1 + 1;

        shmctl(ID, IPC_RMID, NULL);

        sem_init(pusac1, 1, 0);
        sem_init(pusac2, 1, 0);
        sem_init(pusac3, 1, 0);
        sem_init(stol_prazan, 1, 0);
        sem_init(kriticni_odsjecak, 1, 1);

        ispisi_sastojak_pusaca(pusac1);
        ispisi_sastojak_pusaca(pusac2);
        ispisi_sastojak_pusaca(pusac3);

        if (fork() == 0) {
                trgovac();
                exit(0);
        }
        if (fork() == 0) {
                pusac(pusac1);
                exit(0);
        }
        if (fork() == 0) {
                pusac(pusac2);
                exit(0);
        }
        if (fork() == 0) {
                pusac(pusac3);
                exit(0);
        }

        for (int i = 0; i < 4; i++) {
                wait(NULL);
        }

        sem_destroy(pusac1);
        sem_destroy(pusac2);
        sem_destroy(pusac3);
        sem_destroy(stol_prazan);
        sem_destroy(kriticni_odsjecak);
        return 0;
}
