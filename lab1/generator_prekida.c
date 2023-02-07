#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int sig[] = {SIGUSR1, SIGUSR2, SIGTERM, SIGTTIN, SIGINT};
int pid = 0;

void prekidna_rutina(int sig) 
{
        kill(pid, SIGKILL);
        exit(0);
}

int main(int argc, char *argv[]) 
{
        pid = atoi(argv[1]);
        sigset(SIGINT, prekidna_rutina);
        srand(time(NULL));
        while (1) {
                /* odspavaj 3-5 sekundi */
                /* slučajno odaberi jedan signal (od 4) */
                /* pošalji odabrani signal procesu 'pid' funkcijom kill*/
                int sleepTime = rand() % 3 + 3;  //[3,5]
                sleep(sleepTime);
                int i = rand() % 4;  //[0,3]
                int signal = sig[i];
                kill(pid, signal);
        }
        return 0;
}