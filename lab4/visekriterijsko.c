#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_DRETVI 5

struct dretva {
        int id;    // 1, 2, 3, ...
        int p;     // preostalo vrijeme rada
        int prio;  // prioritet
        int rasp;  // način raspoređivanja (za opcionalni zadatak)
};

int t;  // simulacija vremena (trenutno vrijeme)
int ima_dretvi;
int dretvi;
int dretve_u_redu = 0;
int id_counter = 1;

struct dretva *P[MAX_DRETVI];  // red pripravnih dretvi, P[0] = aktivna dretva

void generiraj_nasumicnu_dretvu(int *dretva)
{
        dretva[0] = rand() % 30 + 1; //generiraj trenutak u kojem ulazi  [1,30]
        dretva[1] = id_counter++;    //generiraj id                      
        dretva[2] = rand() % 10 + 1; //generiraj trajanje                [1,10]
        dretva[3] = rand() % 5 + 1;  //generiraj prioritet               [1,5]
        dretva[4] = rand() % 2;      //generiraj nacin rasporedivanja    [0,1]
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


void postavi_aktivnu_dretvu()
{
        //ako vec postoji aktivna dretva, funkcija ne radi nista
        if(P[0] == NULL){
                for(int i = 0; i < MAX_DRETVI - 1; i++)
                        P[i] = P[i + 1];
                
                P[MAX_DRETVI - 1] = NULL;
        }
}


void azuriraj_dretve()
{
        /*rasporedivanje po Prioritet + FIFO je pretpostavljeno rasporedivanje
        osim ako aktivna dretva naznaci da se treba provesti Prioritet + RR*/
	int zadnji = 0;
	if(P[0] != NULL && P[0] -> rasp == 1){
		/*trazenje zadnjeg indeksa sa istim 
                prioritetom kao aktivna dretva*/
		for(int i = 1; i < MAX_DRETVI; i++){
			if(P[i] != NULL && P[i] -> prio == P[0] -> prio)
				zadnji = i;
			else
				break;
			
		}

		struct dretva *temp = P[0];
		for(int i = 1; i <= zadnji; i++)
			P[i - 1] = P[i];
		
		P[zadnji] = temp;

	}else if(P[0] == NULL){
                postavi_aktivnu_dretvu();
        }
}


void dodaj_u_pripravne(struct dretva *dretva) 
{
        if(dretve_u_redu == MAX_DRETVI){
                ima_dretvi--;
                return;
        }
	int index = 0;
        for (int i = 0; i < MAX_DRETVI; i++) {
                //odredi na koji index treba postaviti dretvu
		if(P[i] == NULL){
			index = i;
			break;
		}else if(P[i] -> prio < dretva -> prio){
			index = i;
			break;
		}
        }

	for(int i = MAX_DRETVI - 1; i > index;i--){
		P[i] = P[i - 1];
	}
	P[index] = dretva;
        dretve_u_redu++;
        postavi_aktivnu_dretvu();
}

int main(void)
{

        srand(time(NULL));
        
        printf("Unesite zeljeni broj nasumicno generiranih dretvi: ");
        scanf("%d",&dretvi);
        ima_dretvi = dretvi;
        
        int nove[dretvi][5];
        for(int i = 0; i < dretvi; i++){
                generiraj_nasumicnu_dretvu(nove[i]);
        }
        
        
        t = 0;
        ispis_stanja(1);
        t++;
        while (ima_dretvi > 0) {
                if (P[0] != NULL) {
                        (P[0]->p)--;
                        if (P[0]->p == 0) {
                                printf("Dretva %d zavrsila\n", P[0]->id);
                                ima_dretvi--;
                                dretve_u_redu--;
                                free(P[0]);
                                P[0] = NULL;
                        }
                        azuriraj_dretve();
                }
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

                                printf("%3d --nova dretva ", t);
                                printf(
                                    "id=%d, p=%d, prio=%d, rasp=%d\n",
                                    nova_dretva->id, nova_dretva->p,
                                  nova_dretva->prio,nova_dretva->rasp);
                                
                                dodaj_u_pripravne(nova_dretva);
                        }
                }
		ispis_stanja(0);
                t++;
        }

        return 0;
}