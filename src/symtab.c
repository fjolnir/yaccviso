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

/* file:        symtab.c                                                     
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:                                                                     
 * last mod.:   97.01.07                                                       
 * descr.:                                                                   
 */

/****************************************************************************
 * filename             : symtab.c 
 * author               : aka
 * date                 : Sun Nov 17 00:05:26 MET 1996
 * last modified        : 
 * depends on           : symbtab.h
 * used for             : scanner.lex
 * produces file        : symtab.o
 * -------------------------------------------------------------------------
 * description          : functions for creating, initialising, finding stuff,
 * 			  inserting stuff, etc. in to a symbol table
 *
 *
 * open problems        : replace asserts with something better
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

#include "symtab.h"
#include <assert.h>


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


/****************************************************************************
 * section        : global vars
 * description    :
 *
 *
 ****************************************************************************/ 

symtab		*symGlobalSymtab = NULL;
symtab		*symCurrSymtab = NULL;



/**************************************************************************n
 * section        : functions
 * description    :
 *
 *
 ****************************************************************************/ 




/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : symcreate
 * description    : creates a new symbol table and appends a ptr to it
 *                  at the end of the linked list of symbol tables.
 *                  Initialization of the new symbol table is done also
 * returns        : NULL on failure, otherwise ptr to new symtab
 * input          : void
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
symtab *symcreate(void) {
  symtab *tmpptr;
  symtab *tmpptr2;
  
  /* first create a new symtab structure */
  tmpptr2 = my_malloc(sizeof(struct symtab));
  if (tmpptr2 == NULL) {
    /* something went wrong - not enough memory? */
    bailout("could not create a new symbol table");
  }
  
  /* sanity check - if list of symtabs already exists then */
  if (symGlobalSymtab != NULL) {
    
    /* first go to the end of the list of symbol tables */
    for (tmpptr = symGlobalSymtab; tmpptr->nextptr != NULL; 
	 tmpptr = tmpptr->nextptr) {
      
      ;	/* do nothing */
    } 
    /* now insert a new symbol table structure after tmpptr */
    tmpptr->nextptr = tmpptr2;
  }
  
  
  /* initialze the new symbol table */
  tmpptr2->lastid = 0;
  tmpptr2->nextptr = NULL;
  if (syminit(tmpptr2)) {
    return  tmpptr2;
  }
  else {
    return NULL;	/* something in syminit went wrong */
  }
  
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : syminit
 * description    : initializes the given symbol table
 * returns        : OK, ERROR
 * input          : symbol table
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int syminit(symtab *sym) {

  /* sanity check: check if sym == NULL */
  if (sym == NULL) {
    bailout("syminit: sym == NULL");
  }
    
  sym->symstart = NULL;
  
  return OK;
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : symlookupLexem
 * description    : searches for the given lexem in the given symbol table
 * returns        : ERROR, OK, ptr to symbol table entry in case of OK
 * input          : symbol table, lexem, ptr in which to store the ptr to
 *                  the symentry
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int symlookupLexem(symtab *sym, char *lexem, symentry **entryptr) {
  symentry *tmpptr;

  /* sanity check: check if sym == NULL */
  if (sym == NULL) {
    bailout("symlookupLexem: sym == NULL");
  }

  tmpptr = sym->symstart;
  
  while (tmpptr != NULL) {
    if (tmpptr->lexemptr != NULL) {
      if (0 == strcmp(lexem, tmpptr->lexemptr)) {
	*entryptr = tmpptr;
	return OK;
      }
    }
    tmpptr = tmpptr->nextptr;
  }
  
  /* so we didn't find it in the symtab "sym", let's look in the
     global symtab
  */
  tmpptr = symGlobalSymtab->symstart;
  while (tmpptr != NULL) {
    if (tmpptr->lexemptr != NULL) {
      if (0 == strcmp(lexem, tmpptr->lexemptr)) {
	*entryptr = tmpptr;
	return OK;
      }
    }
    tmpptr = tmpptr->nextptr;
  }
  
  /* at this point we really couldn't find the lexem */
  return ERROR;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : syminsertLexem
 * description    : inserts the given lexem (string) at the head of the
 *                  list of symentries of the given symtab and passes 
 *                  back a ptr to the new entry via entryptr
 * returns        : OK, ERROR
 * input          : symbol table, lexem, ptr in which to store a ptr to
 *                  new symentry
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int syminsertLexem(symtab *sym, char *lexem, symentry **entryptr) {
  symentry *tmpptr = NULL;	/* ptr to new symtab entry */
  
  /* sanity check: check if sym == NULL */
  if (sym == NULL) {
    bailout("syminsertLexem: sym == NULL");
  }

  sym->lastid++;		/* we need a new higher id value */

  /* reserve some space for new symbol table entry */
  tmpptr = my_malloc(sizeof(symentry));
  if (tmpptr == NULL) {
    bailout("syminsertLexem: could not malloc");
  }
  
  /* initialize new symbol table entry with reasonable values */
  tmpptr->id = sym->lastid;
  tmpptr->declared = FALSE;
  tmpptr->type = symtype_unknown;
  tmpptr->lexemptr = NULL;
  tmpptr->nextptr = NULL;
  
  /* reserve some space for tmpptr->lexemptr */
  tmpptr->lexemptr = my_malloc((size_t)(strlen(lexem) + 1));	/* lexem + '\0' */
  if (tmpptr->lexemptr == NULL) {
    bailout("syminsertLexem: could not malloc");
  }
  
  /* copy lexem into tmpptr->lexemptr */
  strcpy(tmpptr->lexemptr, lexem);
  
  /* now we are ready to insert the new symtab entry into the symtab 
     list. Here we simply insert it into the head of the list.
  */
  if (sym->symstart == NULL) {
    sym->symstart = tmpptr;
  }
  else {
    tmpptr->nextptr = sym->symstart;
    sym->symstart = tmpptr;
  }
  *entryptr = tmpptr;
  
  return OK;
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : symprintSymtab
 * description    : prints out informational messages for debugging a symtab
 * returns        : OK, ERROR
 * input          : pointer to symtab to be printed
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int     symprintSymtab(symtab *sym) {
	symentry *tmpptr;

	pdebug(DSYMTAB, "printing SYMTAB");
	for (tmpptr = sym->symstart; tmpptr != NULL; tmpptr = tmpptr->nextptr) {
		pdebug(DSYMTAB, "id = %d", tmpptr->id);
		pdebug(DSYMTAB, "declared = %d", tmpptr->declared);
		switch (tmpptr->type) {
			case symtype_token:
				pdebug(DSYMTAB, "type = symtype_token");
				break;
			case symtype_nt:
				pdebug(DSYMTAB, "type = symtype_nt");
				break;
			case symtype_semvalspec:
				pdebug(DSYMTAB, "type = symtype_semvalspec");
				break;
			case symtype_tnum:
				pdebug(DSYMTAB, "type = symtype_tnum");
				break;
			case symtype_charconstant:
				pdebug(DSYMTAB, "type = symtype_charconstant");
				break;
			case symtype_string_literal:
				pdebug(DSYMTAB, 
				       "type = symtype_string_literal");
				break;
			case symtype_unknown:
				pdebug(DSYMTAB, "type = symtype_unknown");
				break;
			case symtype_none:
				pdebug(DSYMTAB, "type = symtype_none");
				break;
			default:
				pdebug(DFATAL, "oops! symptrintSymtab: should "
					"have some kind of type");
				exit(EXIT_FAILURE);
		} /* end switch */
		pdebug(DSYMTAB, "lexemptr = %s", tmpptr->lexemptr);
		pdebug(DSYMTAB, "");
	} /* end for */
	
	return OK;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : symprintAllSymtabs
 * description    : traverses the linked list of symtabs and prints each symtab
 * returns        : OK, ERROR
 * input          : nothing
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int symprintAllSymtabs() {
	symtab *tmpptr;
	int i = 1;

	for (tmpptr = symGlobalSymtab; tmpptr != NULL; 
		tmpptr = tmpptr->nextptr, i++) {
		pdebug(DSYMTAB, "Printing the %d. symtab", i);
		pdebug(DSYMTAB, "-----------------------");
		symprintSymtab(tmpptr);
	}

	
	
	return OK;
}




