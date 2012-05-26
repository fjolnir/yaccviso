#ifndef SYMTAB_H
#define SYMTAB_H
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

/* file:        symtab.h                                                     
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:                                                                     
 * last mod.:   97.01.07                                                     
 * descr.:                                                                   
 */

/****************************************************************************
 * filename             : symtab.h 
 * author               : aka
 * date                 : Sun Nov 17 00:05:26 MET 1996
 * last modified        : 
 * depends on           : global.h
 * used for             : symtab.c, 
 * produces file        : 
 * -------------------------------------------------------------------------
 * description          : Definition of a symbol table data structure
 * 			  
 *
 *
 * open problems        : This file was taken from my "compiler construction"
 *                        course and slightly modified. Thus it supplies
 *                        functions for multiple symtabs, we use here only
 *                        one.
 ****************************************************************************/ 


/****************************************************************************
 * section        : includes
 * description    : 
 *                  
 *                  
 ****************************************************************************/
#include <stdio.h>
#include "global.h"
#include "error.h"

/****************************************************************************
 * section        : structs, typedefs, enums
 * description    : definition of a symbol table entry.
 *                  A symbol table holds a linked list of symbol 
 *                  table entries.
 ****************************************************************************/

typedef enum {
  symtype_token, symtype_nt, symtype_semvalspec, symtype_tnum, 
  symtype_charconstant, symtype_string_literal, symtype_unknown, symtype_none 
} symtype;
 
typedef struct symtab symtab;
typedef struct symentry symentry;

struct symentry {	/* an entry (i.e. row) of a symbol table */
	int		id;
	int		declared;	/* was identifier defined? */
	symtype		type;		/* what type does is have? */
	char		*lexemptr;	/* string value of the lexem */
	symentry	*nextptr;
};

struct symtab {		/* a symbol table */

	int		lastid;
	symentry	*symstart;
	symtab		*nextptr;
};


extern symtab	 	*symGlobalSymtab;
extern symtab		*symCurrSymtab;



/****************************************************************************
 * section        : prototypes
 * description    :
 *
 *
 ****************************************************************************/

symtab	*symcreate(void);
int	syminit(symtab *sym);
int	symlookupLexem(symtab *sym, char *lexem, symentry **entryptr);
int	syminsertLexem(symtab *sym, char *lexem, symentry **entryptr);
int	symprintSymtab(symtab *sym);
int	symprintAllSymtabs();

#endif




