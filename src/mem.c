/*****************************************************
* Copyright Grégory Mounié 2008-2013                *
* This code is distributed under the GLPv3 licence. *
* Ce code est distribué sous la licence GPLv3+.     *
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

void * zone_memoire = 0;

/* ecrire votre code ici */
void * tzl[WBUDDY_MAX_INDEX];
unsigned int sizeArray[WBUDDY_MAX_INDEX];
int subBuddy[WBUDDY_MAX_INDEX];

int 
mem_init()
{
    if (! zone_memoire)
        zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
    if (zone_memoire == 0)
    {
        perror("mem_init:");
        return -1;
    }
    
    /* ecrire notre code ici */  
    sizeArray[0] = 1;
    sizeArray[1] = 2;
    sizeArray[2] = 3;
    for(int i = 3; i < WBUDDY_MAX_INDEX; i++) {
        if(i%2 == 1) {
            sizeArray[i] = 2*sizeArray[i-2];
        } else {
            sizeArray[i] = 3*sizeArray[i-3];
        }
    }
    
    for(int i = 0; i < WBUDDY_MAX_INDEX -1; i++) {
        tzl[i] = NULL;
    }
    tzl[WBUDDY_MAX_INDEX -1] = zone_memoire;
    
    /* initialization of first list : only 1 block, last element, points on null */
    * (void **) tzl[WBUDDY_MAX_INDEX -1] = NULL;
    
    subBuddy[0] = -1;
    subBuddy[1] = -1;
    subBuddy[2] = 0;
    subBuddy[3] = 0;
    /* preuve de la formule laissée au lecteur */
    for(int i = 4 ; i <WBUDDY_MAX_INDEX; i++) {
        subBuddy[i] = 2 * (i / 2) -3;
        //printf("%i ", subBuddy[i]);
    }
    
    return 0;
}

void decoupe(unsigned int index_zone) {
    // Refuser de découper 2 et 1
    // On suppose qu'on ne demande jamais de découper une zone inexistante ; tzl[index_zone] existe et est non nulle
    void * zone_a_decouper = tzl[index_zone];
    tzl[index_zone] = * (void **) tzl[index_zone];
    if(tzl[WBUDDY_MAX_INDEX -1] == NULL) {
        //printf("Yo c'est null\n");
    } else {
        //printf("%p", tzl[WBUDDY_MAX_INDEX -1]);
    }
    void * large_new_zone = zone_a_decouper + sizeArray[subBuddy[index_zone]];
    * (void **) large_new_zone = (void **) tzl[index_zone -1];
    tzl[index_zone -1] = large_new_zone;
    * (void **) zone_a_decouper = (void **) tzl[subBuddy[index_zone]];
    tzl[subBuddy[index_zone]] = zone_a_decouper;
}

void *
mem_alloc(unsigned long size)
{
    //TODO vérifier si init a bien été appelé, ie. zone_memoire a reçu son malloc
    
    // 1°) on regarde s'il reste au moins 1 zone libre de taille >= size. Si c'est pas le cas, fail
    // 2°) On trouve la plus petite zone libre qui vérifie la condition 1°)
    // 3°) S'il existe une taille (dans sizeArray, le tableau) qui est plus petite que la zone libre trouvée
    
    if (size < 1) {
        /* smack a bitch(); */
        return (void *) 0;
    }


    //printf("Taille alloué : %lu\n", size);
    unsigned int smallest_zone = -1;
    
    // s'il reste à -1, pas de zone qui correspond, crash
    // on alloue jamais 4 !
    for(int i = 5; i < WBUDDY_MAX_INDEX; i++) {
        if(sizeArray[i] >= size) {
            if(tzl[i] != NULL) {
                smallest_zone = i;
                //printf("smallest_zone = %i\n",smallest_zone);
                break;
            }
        }
    }
    
    if(smallest_zone == -1) {
        perror("No free zone of sufficient size");
        return NULL;
    }
    
    bool found_smallest = false;
    while(!found_smallest) {
        
        unsigned int currentSize = smallest_zone;
        //printf("currentSize = %i\n",currentSize);

        if(currentSize <= 7 || sizeArray[currentSize -1] < size ) {
            found_smallest = true;
        } else if (sizeArray[subBuddy[currentSize]] < size) {
            decoupe(smallest_zone);
            smallest_zone = smallest_zone -1;
        } else {
            decoupe(smallest_zone);
            smallest_zone = subBuddy[smallest_zone];
        }
    }
    
    void * tmp = tzl[smallest_zone];
    tzl[smallest_zone] = * (void **) tmp;

    //printf ("Bloc alloué %p\nDe taille : %i\n",tmp, sizeArray[smallest_zone]);
    return tmp;
}


void recherche_buddy (void *ptr, unsigned long size, 
                        unsigned int *buddysize, void** adr_buddy ) 
{
    unsigned int idx_cour = WBUDDY_MAX_INDEX - 1;
    unsigned int idx_cible = 1;

    while (sizeArray[idx_cible] != size) idx_cible++;
    
    void * min = zone_memoire;
    void * max = zone_memoire + sizeArray[WBUDDY_MAX_INDEX - 1];

    //recherche du buddy
    while (idx_cour > idx_cible - 3) {
        if (min == ptr && idx_cour == idx_cible) 
            break;
        if ((min + sizeArray[subBuddy[idx_cour]]) <= ptr) {
            *adr_buddy = min;
            *buddysize = sizeArray[subBuddy[idx_cour]];
            min += sizeArray[subBuddy[idx_cour]];
            idx_cour -= 1;
        } 
        else {
            *buddysize = sizeArray[idx_cour - 1];
            max = min + sizeArray[subBuddy[idx_cour]];
            *adr_buddy = max;
            idx_cour = subBuddy[idx_cour];
        }
    }
}

int
on_retrieve_la_taille(void * ptr) 
{
    unsigned int idx_cour = WBUDDY_MAX_INDEX - 1;
    void * min = zone_memoire;

    //printf("\n\nRetrieve!\n");

    while (idx_cour > 7) {
        //printf("idx_cour = %i\n",idx_cour);
        if ((min + sizeArray[subBuddy[idx_cour]]) <= ptr) {
            idx_cour = idx_cour -1;
            min = min + sizeArray[subBuddy[idx_cour]];
        } 
        else {
            idx_cour = subBuddy[idx_cour];
        }
    }

    if(idx_cour == 7) {
        return 16;
    } else {
        return 8;
    }
}

int 
mem_free(void *ptr, unsigned long size)
{
        
    printf("Taille libéré : %lu\n", size);
    //vérif
    if (ptr < zone_memoire || 
            ptr > zone_memoire + sizeArray[WBUDDY_MAX_INDEX-1]) {
        perror ("Address out of bounds\n");
        return  -1;
    }

    unsigned int buddysize = 0;
    void * adr_buddy = NULL;
    unsigned int i = 0;
    //printf ("mem_free en entrée : ptr = %p\n de size %lu\n", ptr, size);

    if (size <= 8) {
        // either 8 or 16
        size = on_retrieve_la_taille(ptr);
        printf("mem_free : retrieve_la_taille a trouvé => %lu\n", size);
    } else if (size <= 16) {
        size = 16;
    } else {
        while (i < WBUDDY_MAX_INDEX && sizeArray[i] < size) i++;
        size = sizeArray[i];
    }

    //si buddy est dans TZL : fusion
    //et on répète
    
    bool is_buddy_found = true;
    while (is_buddy_found) {
        if (size == sizeArray[WBUDDY_MAX_INDEX - 1])
            break;

        //rechercher buddy

        printf ("pré_recherche_buddy : size = %lu\n ptr = %p\n",
                size, ptr);
        recherche_buddy(ptr, size, &buddysize, &adr_buddy);
        printf ("post_recherche_buddy : buddysize = %u\n adr_buddy = %p\n",
                buddysize, adr_buddy);
        
        //recherche buddy dans TZL
        i = 0;
        while (i < WBUDDY_MAX_INDEX && sizeArray[i] < buddysize) i++;

        void * adr_prec = NULL;
        void * adr_cour = tzl[i];
        while (adr_cour != NULL && adr_cour != adr_buddy) {
            adr_prec = adr_cour;
            adr_cour = *(void **) adr_cour;
        }
        printf("adr_prec %p\nadr_cour %p\n", adr_prec, adr_cour);

        if (adr_cour != NULL) {
            //buddy retrouvé, go le déchainer
            if (adr_cour == tzl[i]) {
                tzl[i] = * (void **) adr_cour;
            }
            else {
                * (void **) adr_prec = * (void **) adr_cour;
            }
            * (void **) adr_cour = NULL;
            //recomposition
            ptr = ptr < adr_buddy?ptr:adr_buddy;
            size = size + buddysize;
        } 
        else {
            is_buddy_found = false;
        }
    }

    i = 0;
    while (sizeArray[i] != size) i++;
    //printf ("i le fourbe : %u\n", i);

    //réinsère dans la tzl le bloc libéré
    //printf ("\n\nmem_free : ptr = %p\n avec size = %lu\n", ptr, size);
    void * tmp = tzl[i];
    * (void **) ptr = tmp;
    tzl[i] = ptr;

    return 0;
}


int
mem_destroy()
{
    /* ecrire votre code ici */
    
    free(zone_memoire);
    zone_memoire = 0;
    return 0;
}

