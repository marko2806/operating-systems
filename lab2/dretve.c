#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int brojDretvi, brojIteracija;
int a;

void *potprogram_dretve() 
{
        for (int i = 0; i < brojIteracija; i++) 
		a++;

        return NULL;
}

int main(int argc, char *argv[]) 
{
	if(argc != 3){
		fprintf(stderr,"Neispravan unos parametara: broj_dretvi broj_iteracija\n");
		exit(1);
	}
        a = 0;
        brojDretvi = atoi(argv[1]);
        brojIteracija = atoi(argv[2]);

        pthread_t *dretva;

        dretva = malloc(brojDretvi * sizeof(pthread_t));

        for (int i = 0; i < brojDretvi; i++) {
                if (pthread_create(&dretva[i], NULL, potprogram_dretve, NULL)) {
                        fprintf(stderr, "Ne mogu stvoriti dretvu\n");
                        exit(1);
                }
        }
        for (int i = 0; i < brojDretvi; i++) 
		pthread_join(dretva[i], NULL);

        printf("A=%d\n", a);
        return 0;
}
