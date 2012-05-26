#ifndef SCANNER_H
#define SCANNER_H
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

/* file:        scanner.h                                                       
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@stud1.tuwien.ac.at 
 * date:	see below
 * last mod.:                                                                
 * descr.:      see below                                     
 */

/* **************************************************************************
* filename             : scanner.h 
* author               : aka 
* date                 : Sun Nov 17 12:18:18 MET 1996
* last modified        : Sun Nov 17 12:18:18 MET 1996 
* depends on           : - 
* used for             : scanner.lex 
* produces file        : - 
* -------------------------------------------------------------------------
* description          : place to put extra definitions for scanner.l
*
*
*
* open problems        :
*
*
*
*************************************************************************** */ 


/****************************************************************************
 * section        :     includes
 * description    :
 *
 *
 ****************************************************************************/ 

#include "global.h"
#include "error.h"
#include "symtab.h"
#include "tree.h"

#include "y.tab.h"


/****************************************************************************
 * section        :     defines
 * description    :
 *
 *
 ****************************************************************************/ 
#define           CBUF               BUFMAX
#define           CBUFGROWSIZE       BUFMAX

/****************************************************************************
 * section        :     enums
 * description    :
 *
 *
 ****************************************************************************/ 

/****************************************************************************
 * section        :     typdefs, structs
 * description    :
 *
 *
 ****************************************************************************/ 

/****************************************************************************
 * section        :     prototypes
 * description    :
 *
 *
 ****************************************************************************/ 

int putbuf(char *str);
int clearLinebuf(void);
int eatcomment(void);
int eatRestOfLine(void);
int appendbuf(char c);
char* eatc(void);
int chk4str(char *str);
int getnextc(char **buf, unsigned int *lastbufpos, int buflen);
int ungetnextc(char c, char *buf, unsigned int *lastbufpos);
symentry *ident(void);

#endif /* SCANNER_H */

