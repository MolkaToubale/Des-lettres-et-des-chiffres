
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define tpsmax 30 //cte temps max de saisie
#define GREEN   "\x1b[32m" //code couleur verte
#define YELLOW  "\x1b[33m" //code couleur jaune
#define RED     "\x1b[31m" //code couleur rouge
#define BLUE    "\x1b[34m" //code couleur bleu
#define RESET_COLOR "\x1b[0m"  //code couleur par d�faut du terminal


typedef struct joueur {
	char nom[20];
    char mot[20];
    int score;
    char tir[11];
} joueur;
//////////////////////////////Fonction qui permet au joueur de choisir le nbr de voyelles dans le tirage////////////////////////////////////////
int ChoisirNbrVoyelle() {
    int v;
    do {
        printColor("Choisissez au moins 2 voyelles: ",YELLOW);
        scanf("%d", &v);
        while (getchar() != '\n');
    } while (v < 2); //au moins deux voyelles
    return v;
}

//////////////////////////////////// Fonction pour imprimer du texte en couleur////////////////////////////////////
void printColor(const char *text, const char *color) {
    printf("%s%s%s", color, text, RESET_COLOR);
}


////////////////////////////////////TIRAGE LETTRES///////////////////////////////////////////////
char* TirageLettres(int v) {
    srand(time(NULL));
    int i,j,ind,lv,lc;

    char *voyelles = "AEIOUY";
    char *consonnes = "BCDFGHJKLMNPQRSTVWXZ";
    char *voy, *cons, *tirage;

    voy = (char*)malloc(v + 1);
    cons = (char*)malloc(10 - v + 1);
    tirage = (char*)malloc(11);

     lv = strlen(voyelles);//Nbr de voyelles
	 lc = strlen(consonnes);//nbr de consonnes

    for (i = 0; i < v; i++) {
        ind = rand() % lv;
        voy[i] = voyelles[ind];//recup�ration des voyelles de la chaine voyelles
    }
    voy[i] = '\0';

    for (j = 0; j < 10 - v; j++) {
        ind = rand() % lc;
        cons[j] = consonnes[ind];//recup�ration des consonnes de la chaine consonnes
    }
    cons[j] = '\0';

    strcpy(tirage, voy);
    strcat(tirage, cons);

    free(voy);
    free(cons);

    return tirage;

}
//////////////////////////////////////////////////// Fonction qui renvoie le fichier contenant l'index de chaque caractere /////////////////////
void indexation(char *nom_fichier) {
    FILE *F = fopen(nom_fichier,"r");
    FILE *F1 = fopen("fichier_index.txt","w");
    char ch[100],c;
    int cpt,i;
    cpt = 0;
    i = 0;
    c = 'a';
    fprintf(F1, "A %d\n", 0);
    if (F != NULL) {
        if (F1 != NULL) {
            while (fgets(ch, sizeof(ch), F) != NULL) {
               if (tolower(ch[0]) == c) {
                cpt+=strlen(ch)+1;
               }
               else {
                        fprintf(F1, "%c %d\n",ch[0], cpt);
                        c = tolower(ch[0]); 
                        cpt+=strlen(ch)+1;}
            }
        }
    }
    fclose(F);
    fclose(F1);
}
///////////////////////////////////////////////////// Fonction qui envoie l'index de caractere pass� en parametre a partir du fichier index/////////////////////////////////////////

int lindex(char c,char *nom) {
    FILE *F  = fopen(nom,"r");
    char ch[100];
    char ch1[100];
    int x;
    if (F != NULL) {
        while (fgets(ch,sizeof(ch),F) != NULL) {
            if (c == 'A'){
                return 0;
            }
            else {
            if (toupper(ch[0]) + 1 == c) {
                fscanf(F,"%c %d",&ch1,&x);
                break;
            }}
        }
    }
    return x;
}

//////////////////////////////////////////////////////// Fonction pour la saisie avec limite de temps//////////////////////////////////////////
	void chrono(char *chaine) {
		double tps;
	    time_t debut, fin;
	    char *pos;
	    int i;
	    time(&debut);
	    printf("Vous avez %d secondes pour saisir: ", tpsmax);
	    gets(chaine);
	    time(&fin);
        // Convertir la cha�ne saisie en majuscules
   		for (i = 0; chaine[i] != '\0'; ++i) {
        chaine[i] = toupper(chaine[i]);
        }
	    tps = difftime(fin, debut); //difftime fct pr�d�finie de la bib time.h qui calcule la diff de temps
	    if (tps >= tpsmax) {
	        printColor("Temps �coul�.Vous avez d�pass� les 30 secondes!\n",RED);
	         strcpy(chaine,"");
	        while ((getchar()) != '\n');
	    } else {
	        pos= strchr(chaine, '\n');
	        if (pos != NULL) {
	            *pos= '\0';
	        }
	    }
	}

/////////////////////////////// Fonction pour v�rifier si un mot peut �tre form� avec les lettres disponibles//////////////////////////////////////////
/////////// Toutes les lettres du mot doivent apparaitre dans le tirage et leur nombre d'occurence doit �tre inf ou �gal � leur nbr d'occ dans le tirage///////////////////////
int PeutFormerMot(joueur* joueur, const char* mot) {
	int i;
	char* lettre;
    char tir[11];
    strcpy(tir, joueur->tir);//copie du tirage dans une chaine tir

    for ( i = 0; i < strlen(mot); i++) {
         lettre = strchr(tir, mot[i]);
        if (lettre == NULL) {

            return 0;  // La lettre n'est pas dans le tirage
        } else {
            *lettre = ' ';  // Marquer la lettre utilis�e en la rempla�ant par un espace
        }
    }

    return 1;
}

/////////////////////////////////////////Fonction pour trouver le mot le plus long///////////////////////////////////////////////////
char* PlusLongMot(joueur* joueur, const char* nomf) {
	int max = 0;//longueur max
    char aux[20];
    char* mot = NULL;
    FILE *f = fopen(nomf, "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier");
        exit(-1);
    }
    // V�rifier chaque mot dans le fichier
    while (fscanf(f, "%s", aux) != EOF) {
        // V�rifier si le mot peut �tre form� avec les lettres disponibles
        if (PeutFormerMot(joueur, aux) && strlen(aux) > max) {
            max = strlen(aux);

            if (mot != NULL) {
                free(mot);  // Lib�rer la m�moire si un mot plus long a �t� trouv�
            }
            mot=(char*)malloc(strlen(aux));
            strcpy(mot,aux); //copier le mot le plus long de la variable aux vers la var mot
        }
    }

    fclose(f);
    return mot;
}



///////////////////////////////////////// Fonction pour v�rifier si un mot est dans le dictionnaire//////////////////////////////////////
int MotValide(const char* mot, const char* nomf) {
	char aux[20];int x;
    FILE *f = fopen(nomf, "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier");
        exit(-1);
    }
    x = lindex(mot[0],"fichier_index.txt");
    fseek(f,x,SEEK_SET);

    while (fscanf(f, "%s", aux) != EOF) {
        if (strcmp(aux, mot) == 0) {
            fclose(f);
            return 1;  // Mot trouv� dans le dico
        }
    }

    fclose(f);
    return 0;  // mot introuvable
}

//////////////////////////////////////////////////////////////////MANCHE///////////////////////////////////////////////////////////////
// Fonction pour chaque tour de jeu
void tour(joueur *joueur) {
    int v;
    char *tirage;
    printf("Choisissez le nombre de voyelles votre tirage\n");
    v = ChoisirNbrVoyelle(); //Saisie nbr voyelles
    tirage = TirageLettres(v); //g�n�rer le tirage
    strcpy(joueur->tir,tirage); //affecter le tirage au joueur
    //Affichage du tirage
    printColor("Votre tirage est:",YELLOW);
	printf("%s\n",joueur->tir);
    // Saisie du mot avec limite de temps
    chrono(joueur->mot);
    // Recherche du mot le plus long
    printColor("le mot le plus long est : ",YELLOW);
	printf("%s\n", PlusLongMot(joueur, "dictionnaire.txt"));
}


///////////////////////////////////////////////Fonction pour le calcul du score///////////////////////////////////////////////

void CalculerScore(joueur* j1, joueur* j2, const char* dictionnaire) {
        int verifJ1 = (strlen(j1->mot) > 0) && (PeutFormerMot(j1, j1->mot)) && (MotValide(j1->mot, dictionnaire)); //var de validit�  des conds du 1er mot
        int verifJ2 = (strlen(j2->mot) > 0) && (PeutFormerMot(j2, j2->mot)) && (MotValide(j2->mot, dictionnaire)); //var de validit�  des conds du 2�me mot

    if (!verifJ1 && verifJ2 && (strlen(j1->mot) > strlen(j2->mot))) { //1er mot invalide 2�me mot valide et longueur du 1er sup � la longueur du 2�me
        j2->score += strlen(j1->mot);
    } else if (!verifJ2 && verifJ1 && (strlen(j2->mot) > strlen(j1->mot))) { //2eme mot invalide 1er mot valide et longueur du 2�me sup � la longueur du 1er
        j1->score += strlen(j2->mot);
    } else {

        if (verifJ1 && verifJ2) { //les deux mots sonts valides
            if (strlen(j1->mot) > strlen(j2->mot)) { //1er mot de longueur sup � la longueur du 2�me mot
                j1->score += strlen(j1->mot);
            } else if (strlen(j2->mot) > strlen(j1->mot)) { //1e mot de longueur inf � la longueur du 2�me mot
                j2->score += strlen(j2->mot);
            } else if (strlen(j1->mot) == strlen(j2->mot)) { //les deux mots ont la m�me longueur
                j1->score += strlen(j1->mot);
                j2->score += strlen(j2->mot);
            }
        } else if (verifJ1 && !verifJ2) { //1er mot valide uniquement

            j1->score += strlen(j1->mot);
        } else if (!verifJ1 && verifJ2) { //2�me mot valide uniquement

            j2->score += strlen(j2->mot);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////MAIN///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

int main() {
    joueur *j1, *j2;
    int j;
    j1 = (joueur*)malloc(sizeof(struct joueur));
    j2 = (joueur*)malloc(sizeof(struct joueur));

   if (j1!=NULL && j2!=NULL){
       	indexation("dictionnaire.txt"); //Cr�ation du fichier d'index
		j1->score=0;
		j2->score=0;
		printColor("Donnez le nom du 1er joueur\n",BLUE);
		scanf("%s",&j1->nom);
		printColor("Donnez le nom du 2�me joueur\n",BLUE);
		scanf("%s",&j2->nom);
		//6 manches
		for (j=1;j<7;j++){ 
		    
			// Tour du Joueur 1
            printColor("Tour de ",BLUE);
            printColor(j1-> nom,BLUE); 
            printf("\n");
            tour(j1);

            // Tour du Joueur 2
            printColor("Tour de ",BLUE);
            printColor(j2-> nom,BLUE); 
            printf("\n");
            tour(j2);
            
            // Calcul des scores � la fin de chaque manche
            CalculerScore(j1,j2,"dictionnaire.txt");

            // Affichage des scores apr�s chaque manche
            printf("Scores apr�s la manche %d: %s: %d, %s: %d\n", j,j1->nom, j1->score,j2->nom, j2->score);
		}
		//Resultat
		if(j1->score > j2->score){
			printColor(j1->nom, GREEN);
			printColor(" a gagn� la partie!\n", GREEN);
			printColor(j2->nom, RED);
			printColor(" a perdu!\n", RED);

		}	else if(j2->score > j1->score){
		    printColor(j2->nom, GREEN);
			printColor(" a gagn� la partie!\n", GREEN);
			printColor(j1->nom, RED);
			printColor(" a perdu!\n", RED);
		}else{

			printColor("Egalite\n", YELLOW);

		}

		free(j1);
		free(j2);
	}

    return 0;
}
