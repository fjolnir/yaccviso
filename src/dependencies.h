#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H
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
 * last mod.:                                                                
 * descr.:      In the following we use the term "gram_el" (grammar element)
 *              to denote both non terminals or terminals either on the
 *              LHS or the RHS of a production.
 */


/****************************************************************************
 * section        : defines
 * description    : 
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
#include "error.h"
#include "symtab.h"

/****************************************************************************
 * section        : typedefdefs, structs
 * description    : 
 *                  
 *
 ****************************************************************************/ 
typedef struct dep_tbl_bucket dep_tbl_bucket;
typedef struct dep_tbl_rbucket dep_tbl_rbucket;
typedef struct dep_tbl_entry dep_tbl_entry;

typedef enum {				/* type: token or non terminal */
  gram_el_type_token, gram_el_type_nt, gram_el_type_none, gram_el_type_unknown
} gram_el_type;

typedef int dep_class;

typedef  char*     taction;		/* text of the action */
typedef  int       ntid;

struct dep_tbl_bucket {			/* linked list of RHS elements */
  int             id;	       		/* unique id, unused at the moment */
  dep_class       class;		/* which dependency class */
  dep_tbl_entry   *rhsptr;		/* gram_el on the RHS */
  int             no;		       	/* number of occurence on RHS */
  taction         actionptr;	       	/* ptr to text of action */
  dep_tbl_bucket  *next;	       	/* next thing on the RHS */
  dep_tbl_bucket  *prev;		/* previous thing on the RHS */
};

struct dep_tbl_rbucket {		/* Reverse bucket linked list */
  int             id;	       		/* unique id, unused at the moment */
  dep_class       class;		/* which dependency class */
  dep_tbl_entry   *lhsptr;		/* gram_el on the LHS */
  dep_tbl_rbucket *next;
  dep_tbl_rbucket *prev;
};

struct dep_tbl_entry {
  int             id;		       	/* unique id , unused at the moment */
  short           bUsed;		/* boolean. TRUE if entry is used */
  symentry        *symptr;		/* name of the ntlhs */
  gram_el_type    type;
  symentry        *semvalspec;		/* ptr to semantic value */
  int             no_of_buckets;	/* length of linked list bucket */
  dep_tbl_bucket  *bucket;		/* linked list of buckets */
  int             no_of_rbuckets;	/* length of linked list rbucket */
  dep_tbl_rbucket *rbucket;		/* linked list of rbuckets */
  dep_tbl_entry   *extension_next;	/* for collisions in hash table */
};

extern dep_tbl_entry dep_tbl[HASHMAX - 1];
extern int last_ntlhs_key;
extern dep_tbl_entry *gram_start;

/****************************************************************************
 * section        : prototypes
 * description    : 
 *                  
 *
 ****************************************************************************/ 
int init_dep_tbl(void);
dep_tbl_entry* insert_gram_el(symentry *symptr, gram_el_type type);
dep_tbl_bucket* append_bucket(dep_tbl_entry *where, symentry *what_symptr,
			      dep_class what_dep_class, char *action);
unsigned int hashfunc(char *name);
dep_tbl_entry *lookup_gram_el(symentry *symptr);

#endif /* DEPENDENCIES_H */
