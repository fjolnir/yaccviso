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

/* file:        main.c
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:                                                                     
 * last mod.:   Wed Jan 21 02:44:24 CET 1998
 * descr.:                                                                   
 */

/****************************************************************************
 * filename             : main.c
 * author               : aka
 * date                 : Sat Jan 25 13:38:55 MET 1997
 * last modified        : Thu Jan 22 02:44:24 CET 1998
 * depends on           : 
 * used for             : -
 * produces file        : 
 * -------------------------------------------------------------------------
 * description          : Main Program routines
 *
 *
 *
 * open problems        :
 * 
 *
 *
 ****************************************************************************/ 

#include "global.h"
#include "error.h"
#include "symtab.h"
#include "tree.h"
#include "dependencies.h"
#include "vcg.h"

int optind;
extern int yyparse(void);		/* why does bison not do this? */   


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : usage
 * description    : prints out a usage message
 *                  
 *                  
 *                  
 * returns        : nothing as it exits via exit()
 * input          : void
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void usage(void) {
  static char optionsstr[] = "[-h] [-d DEBUGLEVEL] [inputfile]\n";
  static char usagestr[] =
    "usage: \n"
    "\t-h\t\tThis help text\n"
    "\t-d DEBUGLEVEL\tset the internal debug level to DEBUGLEVEL\n"
    "\t\t\tvalid DEBUGLEVELS: DFATAL (default), DSCANDBG, \n"
    "\t\t\tDPARSEDBG, DSEMANALY, DCODEGEN, DSYMTAB, DWARNING, DIO, \n"
    "\t\t\tDDEBUG (all of the above)\n"
    "\tinputfile\ta optional inputfile (if not supplied, stdin is used)\n";

  /* print out a usage description */
  pdebug(DIO, "usage: printing out usage text");
  fprintf(stderr, "%s %s\n%s", argv0, optionsstr, usagestr);
  exit(EXIT_FAILURE);

}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : chkargs
 * description    : checks the command line arguments. getopt is not used.
 *                  This is because getopt behaves differently on BSD 
 *                  and Linux, etc. Instead the following function checks the
 *                  arguments manually.
 * returns        : OK, or bails out via usage()
 * input          : int argc, char **argv as in main(...)
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int chkargs(int argc, char **argv) {
  char ch, nextch;
  int i;

  /* NO getopt(3)!! we will do this by hand because getopt has
     different semantics on UN*X machines.
  */

  /* we will have at most 6 arguments including the program name */
  if (argc > 6) {
    usage();	/* to many arguments - we will only accept one
		   input file 
		*/
  }

  /* check all arguments starting with 1 (i.e. ignore the program
     name) and for each check if it could be a command line option
     "-x" where x is one of the following "h", "d". If x is "d"
     then check if a valid DEBUGLEVEL was specified.
     If the argument doesn't start with a "-" then assume that the
     list of command line options is over and that the next parameter
     denotes a file name
  */
  for (i = 1; i < argc; i++) {	/* start with argv[1] 
				   and go up until the last
				   option switch 
				*/

    ch = *argv[i];
    nextch = *(argv[i] + 1);

    if (ch == '-') {
      if (nextch != '\0') {
				/* we potentialy have an option switch */
	switch(nextch) {
	case 'h':
	  usage();
	  break;
	case 'd':
	  if (argv[i + 1] == NULL) {
	    usage();
	  }
	  else if (strcmp(argv[i + 1], 
			  "DDEBUG") == 0) {
	    dbglvl = DDEBUG;
	    i++;
	  } 
	  else if (strcmp(argv[i + 1], 
			  "DFATAL") == 0) {
	    dbglvl = DFATAL;
	    i++;
	  }
	  else if (strcmp(argv[i + 1],
			  "DSCANDBG") == 0) {
	    dbglvl = DSCANDBG;
	    i++;
	  }
	  else if (strcmp(argv[i + 1],
			  "DPARSEDBG") == 0) {
	    dbglvl = DPARSEDBG;
	    i++;
	  }
	  else if (strcmp(argv[i + 1],
			  "DSEMANALY") == 0) {
	    dbglvl = DSEMANALY;
	    i++;
	  }
	  else if (strcmp(argv[i + 1],
			  "DSYMTAB") == 0) {
	    dbglvl = DSYMTAB;
	    i++;
	  }
	  else if (strcmp(argv[i + 1],
			  "DCODEGEN") == 0) {
	    dbglvl = DCODEGEN;
	    i++;
	  }
	  else if (strcmp(argv[i + 1],
			  "DIO") == 0) {
	    dbglvl = DIO;
	    i++;
	  }
	  else if (strcmp(argv[i + 1],
			  "DWARNING") == 0) {
	    dbglvl = DWARNING;
	    i++;
	  }
	  else { /* unknown argument */
	    fprintf(stderr, "unknown "
		    "DEBUGLEVEL\n");
	    usage();
	  } /* end if */
	  break;
	default:
	  usage();
	  break;
	} /* end switch */
      } /* end if (nextch != '\0') */
      else {
				/* we read a "-" without anything else */
				/* assume - as long as this is the last
				   parameter - that this means we should read 
				   from stdin
				*/
	if (i == argc - 1) {
	  optind = argc;
	  return OK;	
	}
	else {
	  usage();
	} /* end else */
      } /* end else */
    } /* end if */ 
    else {
      /* expect the following argv to be the filename */
      break;	
				/* out of the for loop - not beautiful, I 
				   know 
				*/
    } /* end else */	
  } /* end for */

  optind = i;

  return OK;
}


int main(int argc, char **argv) {
  extern FILE *yyin;

  /* initialization of various global vars */
  argv0 = argv[0];
  dbglvl = DFATAL | DWARNING;


#ifdef YYBISON
  /* check if we should do extra Bison debugging */
  if(argc > 1) {
    if (0 == strcmp(argv[1], "--debug")) {
      yydebug=1;
    }
#endif


    chkargs(argc, argv);
    argc -= optind;
    argv[0] = argv[optind];
    
    if (argc > 0) {
      yyin = fopen(argv[0],"rt");
      inputfilename = argv[0];
      if (yyin == NULL) {
	pdebug(DFATAL, "could not open file %s", argv[0]);
	exit(EXIT_FAILURE);
      } /* end if yyin == NULL */
    }
    else {
      yyin = stdin;
      inputfilename = "stdin";
    }

    symGlobalSymtab = symcreate();
    if (symGlobalSymtab == NULL) {
      pdebug(DFATAL, "could not create global symbol table");
      exit(EXIT_FAILURE);
    }
    symCurrSymtab = symGlobalSymtab;
    inittree();

    expect = "expecting start of yacc file";

    /* start parsing ... */
    yyparse();
   
    ptree(tree);
    symprintAllSymtabs();

    /* this will print out a parse tree for debugging purposes only */
    /*     pvcg("parsetree.vcg"); */

    semanaly(tree);

    /* this will print out the dep_tbl via pdebug(...) 
       for debugging purposes only */
    pdeptbl();

    /* print out the output files: one for VCG one for dot(1) */
	pdepvcg("depgraph.vcg");
    pdepdot("depgraph.dot");

    /* start up the user interface -- maybe it will come in the next rel. */
    /* ui_main(argc, argv); */

    /* free all malloced memory */
    cleanup();

    pmalloc();

    return 0;
  }

