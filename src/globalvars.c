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

/* file:        globalvars.c                                                   
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        Do 16 Okt 21:19:07 1997
 * last mod.:                                                                
 * descr.:      see below                                                  
 */

/****************************************************************************
 * filename             : globalvars.c
 * author               : aka
 * date                 : Mon Nov 18 00:00:07 MET 1996
 * last modified        : Mon Nov 18 00:00:07 MET 1996
 * depends on           : -
 * used for             : global.h defines these vars as extern
 * produces file        :
 * -------------------------------------------------------------------------
 * description          : here you can find the definitions of global vars
 *                        WARNING!!!!! This file may only be linked once into
 *                        the final executable of course!!!!!!!!
 *
 * open problems        :
 * 
 *
 *
 ****************************************************************************/ 

#include "global.h"

char		*argv0;		/* command name */
char            *inputfilename;	/* input file name, "stdin" if stdin */
FILE		*outfile;	/* fd to output file */

