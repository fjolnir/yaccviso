#ifndef VCG_H
#define VCG_H
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

/* file:        vcg.h                                                        
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:                                                                     
 * last mod.:   1997 Jan 07                                                  
 * descr.:                                                                   
 */

/****************************************************************************
 * filename             : vcg.h 
 * author               : aka
 * date                 : Wed Jan 15 02:46:16 MET 1997
 * last modified        : 
 * depends on           : global.h 
 * used for             : vcg.c, main program
 * produces file        : 
 * -------------------------------------------------------------------------
 * description          : 
 * 			  
 *
 *
 * open problems        : 
 * 
 *
 *
 ****************************************************************************/ 



/****************************************************************************
 * section        : includes
 * description    :
 *
 *
 ****************************************************************************/

#include "global.h"
#include "tree.h"
#include "error.h"
#include "dependencies.h"


/****************************************************************************
 * section        : prototypes
 * description    :
 *
 *
 ****************************************************************************/
char *convString2vcg(char *string);
const char *getstrIdtab(int id); 
int vcg(node *nptr, int parentid); 
int pnode(node *nptr, int id, const char *label); 
int pedge (node *nptr, int id, int parentid); 
int pvcg(const char *outfile); 
int pdepentry(FILE *fp, dep_tbl_entry *entry_ptr);
int pdepvcg(const char *outfilename);
int pdepdotentry(FILE *fp, dep_tbl_entry *entry_ptr);
int pdepdot(const char *outfilename);




/****************************************************************************
 * section        : global vars
 * description    :
 *
 *
 ****************************************************************************/

extern int	cnt;
extern FILE	*fp;


#endif




