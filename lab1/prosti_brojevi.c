#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
int pauza = 0;
unsigned long int broj = 1000000001;
unsigned long int zadnji = 1000000001;

void periodicki_ispis(int signal) 
{
        printf("zadnji prosti broj = %ld\n", zadnji);
}

void postavi_pauzu(int signal) 
{ 
	pauza = 1 - pauza; 
}

void prekini(int signal) 
{
        printf("zadnji prosti broj = %ld\n", zadnji);
        exit(0);
}

int prost(unsigned long n) 
{
        unsigned long i, max;

        if ((n & 1) == 0) /* je li paran? */
                return 0;

        max = (unsigned long int)sqrt(n);
        for (i = 3; i <= max; i += 2)
                if ((n % i) == 0) 
			return 0;

        return 1; /* broj je prost! */
}

int main(void) 
{
        struct itimerval t;

        /* povezivanje obrade signala SIGALRM sa funkcijom "periodicki_posao" */
        sigset(SIGALRM, periodicki_ispis);
        sigset(SIGTERM, prekini);
        sigset(SIGINT, postavi_pauzu);

        /* definiranje periodičkog slanja signala */
        /* prvi puta nakon: */
        t.it_value.tv_sec = 0;
        t.it_value.tv_usec = 500000;
        /* nakon prvog puta, periodicki sa periodom: */
        t.it_interval.tv_sec = 0;
        t.it_interval.tv_usec = 500000;

        /* pokretanje sata s pridruženim slanjem signala prema "t" */
        setitimer(ITIMER_REAL, &t, NULL);

        while (1) {
                if (prost(broj) == 1) 
                        zadnji = broj;
                
                broj++;
                while (pauza == 1){
                        pause();
		}
        }

        return 0;
}