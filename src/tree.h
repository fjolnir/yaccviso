#ifndef TREE_H
#define TREE_H
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

/* file:        tree.h                                                       
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        Sun Jan 12 14:05:14 MET 1997
 * last mod.:   97.01.05
 * descr.:      for building a parse tree. Used by yaccviso.y                
 */


/****************************************************************************
 * filename             : tree.h 
 * author               : aka
 * date                 : Sun Jan 12 14:05:14 MET 1997
 * last modified        : 97.01.05
 * depends on           : global.h symtab.h
 * used for             : tree.c, 
 * produces file        : 
 * -------------------------------------------------------------------------
 * description          : Definition of a node and a leaf of a parse tree
 * 			  
 *
 *
 * open problems        : 
 * 
 *
 *
 ****************************************************************************/ 

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
#include <stdio.h>
#include "global.h"
#include "symtab.h"
#include "error.h"


/****************************************************************************
 * section        : structs, typedefs, enums
 * description    : 
 *                  
 *                  
 ****************************************************************************/

typedef struct node node;
typedef struct childptr childptr;
typedef struct attrib attrib;

struct attrib {
  int		id;
  int		type;
  union val {
    int	i;
    char	c;
    char	*str;
    float	f;
    void	*vptr;
  } val;
  attrib		*nextptr;
};

struct childptr {
  node		*child;			/* ptr to child node */
  childptr	*nextptr;		/* ptr to next item in list of 
					   ptrs to children */
};

struct node {
  int		id;			/* identifies which (non)terminal */
  attrib       	*attriblst;		/* linked list of attributes */
  attrib       	*attribend;		/* ptr to end of this list */
  int		bLeaf;			/* TRUE if this node is a leaf, FALSE
					   if node is an inner node */
  symentry      *symptr;		/* ptr to symtab , only != NULL if
					   this node is a leaf */
  char          *actionptr;		/* ptr to action, only != NULL if
					   this node is leaf and id == CCODE */
  /* the following entries are only != NULL if the node is an inner
     node of the parse tree
  */
  childptr	*childrenstart;		/* linked list of ptrs to children */
  childptr	*childrenend;		/* ptr to end of linked list of ptrs 
					   to children */
};

typedef enum {				/* defines for all the nt symbols */
  spec, defsection, bisonDeclars, CDeclar, bisonDeclar, yaccIdentifier, 
  unionDeclar, rword, tag, nlist, nmno, rulesection, rules, rule, 
  productions, production, rulebody, prec, action, tailsection 
} nt_id_in_parse_tree;

typedef enum {				/* define Types of attribute values */
  aint, achar, astr, afloat, avoidptr
} attrib_type;

/* XXX */
/* defines for attribute ids */
/* the general syntax for these defines: A<grammarsymbol name>_<attribname> */
#define AYPROCDECL_SYMTAB	1
#define AYPROC_SYMTAB		2
#define AYPROCHEAD_SYMTAB	3



/****************************************************************************
 * section        : prototypes
 * description    :
 *
 *
 ****************************************************************************/

int 	inittree(void);
node	*mkleaf(int tokenid, symentry *sym, char *actionptr);
node	*mknode(int ntid);
attrib	*mkattrib(int id, int type, ...);
int	appendattr(node *nodeptr, attrib *aptr);
int	appendchld(node *nodeptr, node *chld);


/****************************************************************************
 * section        : global vars
 * description    :
 *
 *
 ****************************************************************************/

extern node	*tree;

#endif




