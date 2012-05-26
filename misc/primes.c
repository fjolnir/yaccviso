/* primes - a small program that will use the sieve of Erasthotenes to       */
/*            calculate some prime numbers. Usefull for finding              */
/*            suitable upper bounds for hashtables                           */
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

/* file:                                                                     
 * primes
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:                                                                     
 * last mod.:                                                                
 * descr.:      self-explanatory. 
 */



#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define MAX 16*1024			       /* Change this value */
#define TRUE 1
#define FALSE 0

unsigned char primes[MAX];

int main(int argc, char **argv) {

  int i,j,k,l;

  primes[0]=FALSE;
  primes[1]=FALSE;
  primes[2]=TRUE;

  for (i=3; i < MAX; i++)
    primes[i] = TRUE;

  for (i=2; i < MAX; i++) {
    for (j = i + 1; j < MAX; j++) {
      if (primes[j] && j%i == 0) {	       /* j is multiple of i */
	primes[j] = FALSE;
      }	/* end if */
    } /* end for */
  } /* end for */

  for (i = 0; i < MAX; i++) {
    if (primes[i]) {
      printf("%d\n",i);
    } /* end if */
  } /* end for */
  return 0;
}
