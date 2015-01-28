/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

/* ecrire votre code ici */
void * tzl[40];
unsigned int size[40];
int subBuddy[40];

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
  size[0] = 1;
  size[1] = 2;
  size[2] = 3;
  for(int i = 3; i < 40; i++) {
      if(i%2 == 1) {
          size[i] = 2*size[i-2];
      } else {
          size[i] = 3*size[i-3];
      }
  }
  
  for(int i = 0; i < 39; i++) {
      tzl[i] = NULL;
  }
  tzl[39] = zone_memoire;
  
  /* initialization of first list : only 1 block, last element, points on null */
  zone_memoire[0] = NULL;
  
  subBuddy[0] = -1;
  subBuddy[1] = -1;
  subBuddy[2] = 0;
  subBuddy[3] = 0;
  /* preuve de la formule laissée au lecteur */
  for(int i = 4 ; i <40; i++) {
      subBuddy[i] = 1 + 2 * (i-3)/2;
  }

  return 0;
}

void *
mem_alloc(unsigned long size)
{

  /*
   * Bloc 8
   *  -> bloc 6  -> TZL[subBuddy[actuel]]
   *  -> bloc 2  -> TZL[actuel - 1]
   */

   

  
  return 0;  
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

