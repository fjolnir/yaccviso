#ifndef SEMANLY_H
#define SEMANALY_H
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

/* file:        semanaly.h                                                   
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        see below                                                
 * last mod.:                                                                
 * descr.:                                                                   
 */

/****************************************************************************
 * filename             : semanaly.h
 * author               : aka
 * date                 : Tue Jan 21 20:28:14 MET 1997
 * last modified        : 1997 Jan 08
 * depends on           : global.h, y.tab.h
 * used for             : semanaly.c
 * produces file        : -
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
#include "symtab.h"
#include "error.h"


/****************************************************************************
 * section        : prototypes
 * description    : only "semanaly(...)" is interesting for the outside
 *
 *
 ****************************************************************************/ 

int semanaly(node *nptr);	/* use only this function for the invocation 
				   of the sem. analysis. This will call all
				   following functions and descend recursively
				   into them
				*/
int semanaly(node *nptr);

#endif /* SEMANANLY_H */
