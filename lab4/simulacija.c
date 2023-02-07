//dretve se rasporeduju po kriteriju RR(round robin)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int t;  // simulacija vremena (trenutno vrijeme)
int ima_dretvi;
int dretvi;
int dretve_u_redu = 0;
int id_counter = 1;

#define MAX_DRETVI 5
struct dretva *P[MAX_DRETVI];  // red pripravnih dretvi, P[0] = aktivna dretva

struct dretva {
        int id;    // 1, 2, 3, ...
        int p;     // preostalo vrijeme rada
        int prio;  // prioritet
        int rasp;  // način raspoređivanja (za opcionalni zadatak)
};


void generiraj_nasumicnu_dretvu(int *dretva) 
{
        dretva[0] = rand() % 30 + 1;  // generiraj trenutak u kojem ulazi [1,30]
        dretva[1] = id_counter++;     // generiraj id
        dretva[2] = rand() % 10 + 1;  // generiraj trajanje               [1,10]
        dretva[3] = rand() % 5 + 1;   // generiraj prioritet              [1,5]
        dretva[4] = rand() % 2;       // generiraj nacin rasporedivanja   [0,1]
}

void ispis_stanja(int ispisi_zaglavlje) 
{
        int i;

        if (ispisi_zaglavlje) {
                printf("  t    AKT");
                for (i = 1; i < MAX_DRETVI; i++) 
                        printf("     PR%d", i);
                printf("\n");
        }

        printf("%3d ", t);
        for (i = 0; i < MAX_DRETVI; i++)
                if (P[i] != NULL)
                        printf("  %d/%d/%d ", P[i]->id, P[i]->prio, P[i]->p);
                else
                        printf("  -/-/- ");
        printf("\n");
}

void ukloni_praznine() 
{
        for (int i = 0; i < MAX_DRETVI; i++) {
                if (P[i] == NULL) {
                        for (int j = i + 1; j < MAX_DRETVI; j++) {
                                if (P[j] != NULL) {
                                        P[i] = P[j];
                                        P[j] = NULL;
                                        break;
                                }
                        }
                }
        }
}

void rasporedi_po_rr() 
{
        ukloni_praznine();

        int prvi = 0;
        int zadnji = 0;
        for (int i = 0; i < MAX_DRETVI; i++) {
                if (P[i] != NULL)
                        zadnji = i;
        }

        struct dretva *temp = P[0];
        for (int i = 0; i < zadnji; i++)
                P[i] = P[i + 1];
        
        P[zadnji] = temp;
}

void dodaj_u_pripravne(struct dretva *dretva) 
{
        /* ako je red popunjen, nasumicno generirana 
        dretva koja zeli uci se odbacuje.
        Ne utjece na rad dretvi koje su vec u redu.*/
        if (dretve_u_redu == MAX_DRETVI) {
                ima_dretvi--;
                return;
        }

        // dodavanje dretve
        for (int i = 0; i < MAX_DRETVI; i++) {
                if (P[i] == NULL) {
                        P[i] = dretva;
                        break;
                }
        }
        dretve_u_redu++;
}

int main() 
{
        t = 0;
        srand(time(NULL));

        printf("Unesite zeljeni broj nasumicno generiranih dretvi: ");
        scanf("%d", &dretvi);
        ima_dretvi = dretvi;

        int nove[dretvi][5];
        for (int i = 0; i < dretvi; i++)
                generiraj_nasumicnu_dretvu(nove[i]);
        

        ispis_stanja(1);
        t++;
        while (ima_dretvi > 0) {
                // ako postoji aktivna dretva
                if (P[0] != NULL) {
                        (P[0]->p)--;
                        if (P[0]->p == 0) {
                                printf("Dretva %d zavrsila\n", P[0]->id);
                                ima_dretvi--;
                                dretve_u_redu--;
                                free(P[0]);
                                P[0] = NULL;
                        }
                        rasporedi_po_rr();
                }
                // trazenje dretvi koje zele uci u procesor
                for (int i = 0; i < dretvi; i++) {
                        if (nove[i][0] == t) {

                                struct dretva *nova_dretva =
                                    (struct dretva *)malloc(
                                        sizeof(struct dretva));

                                nova_dretva->id = nove[i][1];
                                nova_dretva->p = nove[i][2];
                                nova_dretva->prio = nove[i][3];
                                nova_dretva->rasp = nove[i][4];

                                ispis_stanja(0);
                                printf(
                                    "%3d --nova dretva id=%d, p=%d, prio=%d\n",
                                    t, nova_dretva->id, nova_dretva->p,
                                    nova_dretva->prio);

                                dodaj_u_pripravne(nova_dretva);
                        }
                }
                ispis_stanja(0);
                t++;
        }
        return 0;
}