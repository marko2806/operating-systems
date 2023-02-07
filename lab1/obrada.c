#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <signal.h>
#include <stdio.h>

#define N 6 /* broj razina proriteta */

int OZNAKA_CEKANJA[N];
int PRIORITET[N];
int TEKUCI_PRIORITET;

int sig[] = {SIGUSR1, SIGUSR2, SIGTERM, SIGTTIN, SIGINT};
void zabrani_prekidanje() 
{
        int i;
        for (i = 0; i < 5; i++) 
		sighold(sig[i]);
}

void dozvoli_prekidanje() 
{
        int i;
        for (i = 0; i < 5; i++) 
		sigrelse(sig[i]);
}

void obrada_signala(int i) 
{
        /* obrada se simulira trošenjem vremena,
           obrada traje 5 sekundi, ispis treba biti svake sekunde */
        char uzorak[17 + 1] = " -  -  -  -  -  -";

        uzorak[3 * i + 1] = 'P';
        printf("%s\n", uzorak);

        for (int j = 1; j <= 5; j++) {
                uzorak[3 * i + 1] = 48 + j;  // ASCII 0 = 48, 1= 49...
                printf("%s\n", uzorak);

                /*
		ispis stanja varijabli:

                printf("O_CEK[%d %d %d %d %d %d] ", OZNAKA_CEKANJA[0],
                       OZNAKA_CEKANJA[1], OZNAKA_CEKANJA[2], OZNAKA_CEKANJA[3],
                       OZNAKA_CEKANJA[4], OZNAKA_CEKANJA[5]);
                printf("TEK_PRIOR = %d ", TEKUCI_PRIORITET);
                printf("PRIOR[%d %d %d %d %d %d]\n", PRIORITET[0], PRIORITET[1],
                       PRIORITET[2], PRIORITET[3], PRIORITET[4], PRIORITET[5]);
                */
                sleep(1);
        }
        uzorak[3 * i + 1] = 'K';
        printf("%s\n", uzorak);
        return;
}

void prekidna_rutina(int sig) 
{
        int n = -1;
        int x;
        zabrani_prekidanje();
        // odredivanje uzroka prekida, odnosno indeks i;
        switch (sig) {
                case SIGUSR1:
                        n = 1;
                        printf(" -  X  -  -  -  -\n");
                        break;
                case SIGUSR2:
                        n = 2;
                        printf(" -  -  X  -  -  -\n");
                        break;
                case SIGTERM:
                        n = 3;
                        printf(" -  -  -  X  -  -\n");
                        break;
                case SIGTTIN:
                        n = 4;
                        printf(" -  -  -  -  X  -\n");
                        break;
                case SIGINT:
                        n = 5;
                        printf(" -  -  -  -  -  X\n");
                        break;
                default:
                        break;
        }
        OZNAKA_CEKANJA[n]++;
        do {
                x = 0;
                for (int j = TEKUCI_PRIORITET + 1; j < N; j++) {
                        if (OZNAKA_CEKANJA[j] > 0 && j > TEKUCI_PRIORITET) 
                                x = j;
                }

                if (x > 0) {
                        OZNAKA_CEKANJA[x]--;
                        PRIORITET[x] = TEKUCI_PRIORITET;
                        TEKUCI_PRIORITET = x;
                        dozvoli_prekidanje();
                        obrada_signala(x);
                        zabrani_prekidanje();
                        TEKUCI_PRIORITET = PRIORITET[x];
                }
                dozvoli_prekidanje();
        } while (x > 0);
}

int main(void) 
{
        sigset(SIGUSR1, prekidna_rutina);
        sigset(SIGUSR2, prekidna_rutina);
        sigset(SIGTTIN, prekidna_rutina);
        sigset(SIGTERM, prekidna_rutina);
        sigset(SIGINT, prekidna_rutina);

        printf("Proces obrade prekida, PID=%d\n", getpid());
        printf("GP S1 S2 S3 S4 S5\n");
        for (int i = 1; i <= 10; i++) {
                sleep(1);
                printf(" %d  -  -  -  -  -\n", i);
        }
        printf("Zavrsio osnovni program\n");

        return 0;
}