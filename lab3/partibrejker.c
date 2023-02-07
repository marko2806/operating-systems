#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>


int brojStudenata = 0;
int partibrejkerUSobi = 0;
int studentiUSustavu;
pthread_mutex_t monitor;
pthread_cond_t partibrejkerRed;
pthread_cond_t studentiRed;

void *student(void *p) 
{
        int spavanjeMSek = rand() % 401 + 100;
        usleep(spavanjeMSek * 1000);

        int *broj = (int *)p;
        for (int i = 0; i < 3; i++) {
                pthread_mutex_lock(&monitor);
                while (partibrejkerUSobi == 1)
                        pthread_cond_wait(&studentiRed, &monitor);
                pthread_mutex_unlock(&monitor);

                pthread_mutex_lock(&monitor);
                brojStudenata++;
                if (brojStudenata >= 3)
                        pthread_cond_broadcast(&partibrejkerRed);
                printf("Student %d je usao u sobu\n", *broj);
                pthread_mutex_unlock(&monitor);

                int spavanje = rand() % 1001 + 1000;
                usleep(spavanje * 1000);

                pthread_mutex_lock(&monitor);
                brojStudenata--;
                printf("Student %d je izasao iz sobe\n", *broj);
                pthread_mutex_unlock(&monitor);

                spavanjeMSek = rand() % 1001 + 1000;
                usleep(spavanjeMSek * 1000);
        }
        pthread_mutex_lock(&monitor);
        studentiUSustavu--;
        if (studentiUSustavu == 0) 
		pthread_cond_broadcast(&partibrejkerRed);
        pthread_mutex_unlock(&monitor);
}

void *partibrejker(void *p) 
{
        while (studentiUSustavu > 0) {
                int spavanjeMSek = rand() % 901 + 100;
                usleep(spavanjeMSek * 1000);

                pthread_mutex_lock(&monitor);
                while (brojStudenata < 3 && partibrejkerUSobi == 0 &&
                       studentiUSustavu > 0) {
                        pthread_cond_wait(&partibrejkerRed, &monitor);
                }
                pthread_mutex_unlock(&monitor);

                pthread_mutex_lock(&monitor);
                if (partibrejkerUSobi == 0 && brojStudenata >= 3) {
                        partibrejkerUSobi = 1;
                        printf("Partibrejker je usao u sobu\n");
                }
                pthread_mutex_unlock(&monitor);

                pthread_mutex_lock(&monitor);
                if (brojStudenata == 0 && partibrejkerUSobi == 1) {
                        partibrejkerUSobi = 0;
                        printf("Partibrejker je izasao iz sobe\n");
                        pthread_cond_broadcast(&studentiRed);
                }
                pthread_mutex_unlock(&monitor);
        }
}


void prekidna_rutina(int sig)
{
	pthread_mutex_destroy(&monitor);
	pthread_cond_destroy(&studentiRed);
	pthread_cond_destroy(&partibrejkerRed);
	exit(0);
}

int main(int argc, char *argv[]) 
{
        if (argc != 2) {
                fprintf(
                    stderr,
                    "Neispravan unos parametara: broj_studenata_u_sustavu\n");
                exit(1);
        }

        studentiUSustavu = atoi(argv[1]);

	sigset(SIGINT,prekidna_rutina);

        pthread_mutex_init(&monitor, NULL);
        pthread_t *dretva = malloc(sizeof(pthread_t) * (studentiUSustavu + 1));

        srand(time(NULL));

        if (pthread_create(&dretva[studentiUSustavu], NULL, partibrejker,
                           NULL)) {
                printf("Ne mogu stvoriti dretvu\n");
                exit(1);
        }
        int *BR = malloc((studentiUSustavu + 1) * sizeof(int));
        for (int i = 0; i < studentiUSustavu; i++) {
                BR[i] = i;
                if (pthread_create(&dretva[i], NULL, student, &BR[i])) {
                        printf("Ne mogu stvoriti dretvu\n");
                        exit(1);
                }
        }

        for (int i = 0; i < studentiUSustavu + 1; i++)
                pthread_join(dretva[i], NULL);
	
	pthread_mutex_destroy(&monitor);
	pthread_cond_destroy(&studentiRed);
	pthread_cond_destroy(&partibrejkerRed);
	return 0;
}