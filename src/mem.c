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

void *zone_memoire = 0;

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
    *(void **) zone_memoire = NULL;
    
    subBuddy[0] = -1;
    subBuddy[1] = -1;
    subBuddy[2] = 0;
    subBuddy[3] = 0;
    /* preuve de la formule laissée au lecteur */
    for(int i = 4 ; i <WBUDDY_MAX_INDEX; i++) {
        subBuddy[i] = 1 + 2 * (i-3)/2;
    }
    
    return 0;
}

void decoupe(unsigned int index_zone) {
    // TODO refuser de découper 2 et 1
    // On suppose qu'on ne demande jamais de découper une zone inexistante ; tzl[index_zone] existe et est non nulle
    void * zone_a_decouper = tzl[index_zone];
    //tzl[index_zone] = tzl[index_zone][0];
    void * large_new_zone = zone_a_decouper + sizeArray[subBuddy[index_zone]];
    //large_new_zone[0] = tzl[index_zone -1];
    tzl[index_zone -1] = large_new_zone;
    //zone_a_decouper[0] = tzl[subBuddy[index_zone]];
    tzl[subBuddy[index_zone]] = zone_a_decouper;
}

void *
mem_alloc(unsigned long size)
{
    
    // 1°) on regarde s'il reste au moins 1 zone libre de taille >= size. Si c'est pas le cas, fail
    // 2°) On trouve la plus petite zone libre qui vérifie la condition 1°)
    // 3°) S'il existe une taille (dans sizeArray, le tableau) qui est plus petite que la zone libre trouvée
    
    unsigned int smallest_zone = -1;
    
    // s'il reste à -1, pas de zone qui correspond, crash
    
    for(int i = 0; i < WBUDDY_MAX_INDEX; i++) {
        if(sizeArray[i] >= size) {
            if(tzl[i] != NULL) {
                smallest_zone = i;
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
        
        unsigned int currentSize = sizeArray[smallest_zone];
        
        if(sizeArray[currentSize -1] > size) {
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
    //tzl[smallest_zone] = tmp[0];
    
    return tmp;
}

int 
mem_free(void *ptr, unsigned long size)
{
    /* ecrire votre code ici */
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

