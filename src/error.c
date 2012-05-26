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

/* file:        error.c                                                      
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        see below                                                    
 * last mod.:                                                                
 * descr.:                                                                   
 */

/****************************************************************************
 * filename             : error.c
 * author               : aka
 * date                 : Sun Nov 17 23:55:30 MET 1996
 * last modified        : Sun Jan 12 02:58:33 MET 1997
 * depends on           : global.h
 * used for             : all .c files
 * produces file        :
 * -------------------------------------------------------------------------
 * description          : provides funtions for printing error messages:
 *
 *                           void bailout(char *mesg);
 *                           int pdebug(int lvl, char *fmt, ...);
 *
 * open problems        : yyerror must not always return OK as it is the 
 *                       case now
 *
 *
 ****************************************************************************/ 


/****************************************************************************
 * section        : includes
 * description    :
 *
 *
 ****************************************************************************/ 
#include "error.h"
#include <assert.h>
#include <stdarg.h>		/* let's hope this is standardized ;-) */


int	dbglvl;
char	*expect;		/* string for storing the message 
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



/****************************************************************************
 * section        : function implementations
 * description    :
 *
 *
 ****************************************************************************/ 
int yyerror(const char *mesg) {

	char lineindicator[BUFMAX] = ""; 
	signed int i;

	for (i=0; i <= charcnt ; i++) {
		if (i < (charcnt - yyleng)) {
			lineindicator[i] = ' ';
		}
		else {
			lineindicator[i] = '^';
		}
	}	/* end for */
	lineindicator[i] = '\0';
        pdebug(DFATAL, "syntax error in line %u, before position %u: %s\n"
		"%s\n%s\n%s\n", linecnt, charcnt, expect, linebuf,
		lineindicator,mesg);
	expect = "";	/* reset the expecting string */

	exit(EXIT_FAILURE);
} 

void bailout(char *mesg) {

	pdebug(DFATAL, mesg);
	exit(EXIT_FAILURE);
}

int pdebug(int lvl, char *mesg, ...) {
	va_list ap;

	/* initialize ap */
	va_start(ap,mesg);

	if (lvl & dbglvl) {
		/* first print the part including the program name and the 
		debug level
		*/
		switch (lvl) {
			case DSCANDBG:
				fprintf(stderr, "%s - scandbg: ", 
					argv0);
				break;
			case DPARSEDBG:
				fprintf(stderr, "%s - parsedbg: ", 
					argv0);
				break;
			case DSEMANALY:
				fprintf(stderr, "%s - semanaly: ", 
					argv0);
				break;
			case DCODEGEN:
				fprintf(stderr, "%s - codegen: ", 
					argv0);
				break;
			case DSYMTAB:
				fprintf(stderr, "%s - symtab: ", 
					argv0);
				break;
			case DIO:
				fprintf(stderr, "%s - IO: ", 
					argv0);
				break;
			case DWARNING:
				fprintf(stderr, "%s - warning: ", 
					argv0);
				break;
			case DFATAL:
				fprintf(stderr, "%s - fatal error: ", 
					argv0);
				break;
			case DDEBUG:
				fprintf(stderr, "%s - debug: ", 
					argv0);
				break;
			default:
				assert(0);
		} /* end switch */
		/* now print the actual error message */
		vfprintf(stderr, mesg, ap);
		fprintf(stderr,"\n");
	} /* end if */

	/* then flush */
	va_end(ap);
	fflush(stderr);	/* we want to see the result immediately */
	return OK;
}
