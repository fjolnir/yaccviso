/* YaccViso - a tool for visualizing dependencies of nonterminals in a yacc  */
/*            grammar file                                                   */
/* Copyright (C) 1997  Leon Aaron Kaplan, email: e9325704@stud1.tuwien.ac.at */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 */
/* ----------------------------------------------- end of legal stuff ------ */

/* file:        malloc.c                                                     
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:                                                                     
 * last mod.:                                                                
 * descr.:      malloc(3) and free(3) wrapper functions. Used for controlling
 *              if everything that was malloced is freed. Is extended 
 *              to a simple garbage collector.
 */

#include "malloc.h"
#include "error.h"

static int cntmalloc = 0;
static memmap *memstart = NULL;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : my_malloc
 * description    : mallocs and logs what has been malloced
 *                  
 * returns        : void *
 * input          : size
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void *my_malloc(size_t size) {
  memmap *tmpptr;

  cntmalloc++;

  if (NULL == (tmpptr = malloc(sizeof(memmap))))
    bailout("could not malloc garbage collector entry");

  /* initialize entry */
  tmpptr->memptr = malloc(size);
  if (tmpptr->memptr == NULL)
    return NULL;		/* nothing makes sense anymore at this point */
  tmpptr->size = size;
  tmpptr->bFreed = FALSE;
  
  /* insert entry into linked list */
  tmpptr->next = memstart;
  memstart = tmpptr;

  pdebug(DIO, "my_malloc: cntmalloc = %d", cntmalloc);
  
  return tmpptr->memptr;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : my_free
 * description    : frees malloced data
 *                  
 * returns        : void
 * input          : ptr to data
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void my_free(void *nptr) {

  cntmalloc--;
  free(nptr);
  pdebug(DIO, "my_free: cntmalloc = %d", cntmalloc);

  return;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : pmalloc
 * description    : debugging routine
 *                  
 * returns        : OK, ERROR
 * input          : void
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int pmalloc(void) {

  pdebug(DIO, "malloc cnt = %d", cntmalloc);
  return OK;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : cleanup
 * description    : frees all malloced data
 *                  
 * returns        : void
 * input          : void
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int cleanup(void) {
  memmap *tmpptr;
  memmap *tmp2ptr;		/* runs behind tmpptr in second loop */
  int i = 0;

  pdebug(DIO, "cleanup: starting clean up process");

  for (tmpptr = memstart; tmpptr != NULL; tmpptr = tmpptr->next) { 
    if (tmpptr->bFreed) { 
      pdebug(DIO, "cleanup: warning: already freed that entry"); 
    } /* end if */
    /* else search if tmpptr->memptr matches something which we freed 
       already */
    /* Note: this is for test purposes only, it runs with O(n^2) !!! */
    /*     for (tmp2ptr = memstart; tmp2ptr != tmpptr && tmp2ptr != NULL;  */
    /* 	 tmp2ptr = tmp2ptr->next) { */
    /*       if (tmpptr->memptr == tmp2ptr->memptr) { */
    /* 	pdebug(DIO, "cleanup: warning: already freed %p memptr",  */
    /* 	       tmpptr->memptr);  */
    /*       }  */
    /*     } */
    my_free((void *)tmpptr->memptr); 
  }  /* end for */

  /* now free the memmap itself */
  pdebug(DIO, "cleanup: now cleaning up the memmap itself"); 
  for (tmp2ptr = tmpptr = memstart; tmpptr != NULL;  
       tmpptr = tmpptr->next, i++) { 
    if (i > 0) { 
      free(tmp2ptr); 
    } 
    tmp2ptr = tmpptr; 
  } 
  if (i > 0) { 
    free(tmp2ptr);		/* free last element */
  }  
  
  pdebug(DIO, "cleanup: freed %d memmap entries. cleanup is finished", i);
  
  return OK;
}








