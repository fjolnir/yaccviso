#ifndef GLOBAL_H
#define GLOBAL_H

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

/* file:        global.h                                                     
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        13.10.1997                                                   
 * last mod.:                                                                
 * descr.:      global .h file for yaccviso                                  
 */


/****************************************************************************
 * section        : defines
 * description    : 
 *
 ****************************************************************************/ 
#define         OK              1
#define         ERROR           0

#define         TRUE            1
#define         FALSE           0

#define         DDEBUG          0xFFFF
#define         DSCANDBG        1
#define         DPARSEDBG       2
#define         DSEMANALY       4
#define         DCODEGEN        8
#define         DSYMTAB         16
#define         DIO             32
#define         DWARNING        128
#define         DFATAL          256

#define		BUFMAX		4096
#define         HASHMAX         16381	/* a prime number */

/****************************************************************************
 * section        : includes
 * description    : these are the include files that most of the 
 *                  files will need. So we include them once here
 *
 ****************************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "malloc.h"


/****************************************************************************
 * section        : typdefs, structs
 * description    : 
 *                  
 *
 ****************************************************************************/ 


/****************************************************************************
 * section        : declarations of the vars defined in globalvars.c
 * description    : 
 *                  
 *
 ****************************************************************************/ 
extern		char	*argv0;         /* command name */
extern          char    *inputfilename;	/* name of the input file or "stdin" */
extern		int	dbglvl;         /* debug level */
extern unsigned int	linecnt;        /* current line number */
extern unsigned int	charcnt;        /* current char position in line */
extern		char    linebuf[BUFMAX]; /* Buffer for current line of input */
                                        /* NOTE: the BUFMAX is a restriction
                                           for the compiler at the moment, but
                                           at least I check, if we are withing
                                           array bounds */
extern          int     yyleng;		/*  thx 2 flex, we have to do that */
extern          char    *expect;        /* string for storing messages 
                                           "expecting ..." */
extern          FILE    *outfile;       /* fd of outputfile */

extern		int	bSkipC;		/* Flag. If TRUE, then scanner will
					   go into a state that will eat up C
					   code. Controlled by yaccviso.y. */
extern          int     bInCDeclar;     /* flag if in C Declar section */
extern          int     bInTailSection;	/* flag if parser is in tailsection */

#endif /* GLOBAL_H */
