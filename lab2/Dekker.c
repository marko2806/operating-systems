#include <malloc.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>


// sinkronizacija dvije dretve Dekkerovim algoritmom

atomic_int pravo;
atomic_int zastavica[2];
int brojIteracija;
int a;

void udi_u_kriticni_odsjecak(int i, int j) 
{
        zastavica[i] = 1;
        while (zastavica[j] != 0) {
                if (pravo == j) {
                        zastavica[i] = 0;
                        while (pravo == j)
                                ;
                        zastavica[i] = 1;
                }
        }
}

void izadi_iz_kriticnog_odjecka(int i) 
{
        pravo = 1 - pravo;
        zastavica[i] = 0;
}

void *proces(void *rbr) 
{
        int *br = (int *)rbr;
        for (int i = 0; i < brojIteracija; i++) {
                udi_u_kriticni_odsjecak(*br, 1 - *br);
                a++;
                izadi_iz_kriticnog_odjecka(*br);
        }

        return NULL;
}

int main(int argc, char *argv[]) 
{
	if(argc != 2){
		fprintf(stderr,"Neispravan unos parametara: broj_iteracija\n");
		exit(1);
	}
        pravo = 0;
        zastavica[0] = zastavica[1] = 0;
        a = 0;

        brojIteracija = atoi(argv[1]);
        int *BR;
        pthread_t *t;

        BR = malloc(2 * sizeof(int));
        t = malloc(2 * sizeof(pthread_t));
        for (int i = 0; i < 2; i++) {
                BR[i] = i;
                if (pthread_create(&t[i], NULL, proces, &BR[i])) {
                        fprintf(stderr, "Ne mogu stvoriti dretvu\n");
                        exit(1);
                }
        }
        for (int j = 0; j < 2; j++) 
                pthread_join(t[j], NULL);
        
        printf("A=%d\n", a);
        return 0;
}
