#ifndef ERROR_H
#define ERROR_H
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

/* file:        error.h                                                 
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at
 * date:        see below                                                    
 * last mod.:                                                                
 * descr.:                                                                   
 */


/****************************************************************************
 * filename             : error.h
 * author               : aka
 * date                 : Sun Jan 12 03:05:38 MET 1997
 * last modified        : Sun Jan 12 03:05:38 MET 1997
 * depends on           : 
 * used for             : error.c
 * produces file        : -
 * -------------------------------------------------------------------------
 * description          : local definitions for error.c
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
#include <assert.h>
#include <stdarg.h>		/* let's hope this is standardized ;-) */


extern int	dbglvl;
extern char	*expect;		/* string for storing the message 
					"expecting ..."
					*/

/****************************************************************************
 * section        : prototypes
 * description    :
 *
 *
 ****************************************************************************/ 
int yyerror(const char *mesg);
void bailout(char *mesg);
int pdebug(int lvl, char *fmt, ...);


#endif /* ERROR_H */
