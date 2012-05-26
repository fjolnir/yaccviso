%{
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

/* file:        scanner.l                                                   
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        13.10.1997                                                   
 * last mod.:                                                                
 * descr.:      (f)lex file for YaccViso                                 
 * open probs.: clearCharbuf not efficient enough
 *              lines with > BUFMAX chars in the input stream
 *              will cause an error. 
 *		
 */

#include "global.h"
#include "scanner.h"
#include <ctype.h>
#include "error.h"
#include "symtab.h"
#include <stdio.h>


/****************************************************************************
 * section        : global vars for needed for lex.yy.c
 * description    :
 *
 ****************************************************************************/
unsigned int	linecnt = 1;            /* current line number */
unsigned int	charcnt = 0;            /* current char position in line */
unsigned int    lastStringStartPos = 0;	/* where did last '"' occur (column)?*/
unsigned int    lastStringStartLine= 0; /* where did last '"' occur (line)? */
char            linebuf[BUFMAX];	/* Buffer for current line of input */
                                        /* NOTE: the BUFMAX is a restriction
                                           for the compiler at the moment, but
                                           at least I check everytime, if we 
					   are withing array bounds
                                        */
int		bSkipC = FALSE;		/* if C Code should not be scanned */
int             bInCDeclar = FALSE;	/* flag if parser is in C Declar. */ 
int             bInTailSection = FALSE;	/* flag if parser is in tailsection */
#define INITIAL 0
static int	laststate=INITIAL;	/* last start state yylex was in */

%}

DIGIT		[0-9]
DIGITS		{DIGIT}+
ID		[A-Za-z_][_A-Za-z0-9]*
NT		[A-Za-z._][_.A-Za-z0-9]*
ALNUM		[a-zA-Z0-9]
L		[a-zA-Z_]
WS_NON_NL	[ \v\t\]
WS		[ \v\t\f\n\r]
H		[a-fA-F0-9]
E		[Ee][+-]?{DIGIT}+
FS		(f|F|l|L)
IS		(u|U|l|L)*

%x NTLHS

/* --- Token Section ------------------------------------------------------ */
%%
		/* General tokens  ---------------------------------------  */
			/* This is for switching (back) between states */
			{ pdebug(DSCANDBG, "last state was = %d", laststate);
			  pdebug(DSCANDBG, "bSkipC = %d, bInCDeclar = %d",
				 bSkipC, bInCDeclar);
			  if (bSkipC) {
				yylval.ccode = eatc();
			        pdebug(DSCANDBG, "eatc yylval = %s", 
				       yylval.ccode);
				return CCODE;
			  } 
			  else if (bInTailSection) {
			    return 0;	/* return finished to Yacc */
			  } 
			  else {
				BEGIN(laststate); 
				/* switch back to start state */ 
			  } /* end else */
			}

<*>\/\*			{ putbuf(yytext); eatcomment(); } 

		/* tokens for various whitespaces: ------------------------ */
<*>[ \v]+		{ pdebug(DSCANDBG, "found one or more spaces");
			  putbuf(yytext); }
<*>\t+			{ pdebug(DSCANDBG, "found one or more tabs"); {
			  /* Note: here we expand the each tab to 8 spaces */
			    int i,j;
			    for (i = 0; i < yyleng; i++) {
			      for (j = 0; j < 8; j++) {
				unput(' ');
			      }
			    } /* end outer for loop */
			  } /* end extra nesting */
			}
<*>[\n\r]		{ pdebug(DSCANDBG, "found a newline");
			  clearLinebuf();
			  /* reset character counter and inc linecounter 
			     since we are in a new line of input 
			  */ 
			}

		/* tokens for INITIAL state: ------------------------------ */
<INITIAL>'(\\.|[^\\'])+' { putbuf(yytext); 
			  pdebug(DSCANDBG, "found character CONSTANT");
                          yylval.symptr = ident();
			  return(CHARCONSTANT);  /* f.ex 'c' */ }
<INITIAL>";"		{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found ';'"); 
			  return(TSEMICOL); }
<INITIAL>"|"		{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found |");
			  return(TPIPE); }
<INITIAL>"{"		{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found '{'"); 
			  return(TOPENBRACES); }
<INITIAL>"}"		{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found '}'"); 
			  return(TCLOSEBRACES); }
<INITIAL>"<"		{ putbuf(yytext); return(TLT); }
<INITIAL>">"		{ putbuf(yytext); return(TGT); }
<INITIAL>","		{ putbuf(yytext); return(TCOMMA); }
<INITIAL>\%\}		{ putbuf(yytext); pdebug(DSCANDBG, "found TRCURL");
			  return TRCURL;   /* The %} mark */ }
<INITIAL>0[xX]{H}+{IS}?	{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found Hex TNUM");
                          yylval.symptr = ident();
			  return(TNUM); } /* f.ex 0x6fL */
<INITIAL>0{DIGIT}+{IS}?	{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found Octal TNUM");
                          yylval.symptr = ident();
			  return(TNUM); }
<INITIAL>{DIGIT}+{IS}?	{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found decimal TNUM");
                          yylval.symptr = ident();
			  return(TNUM); }
<INITIAL>\"(\\.|[^\\"])*\" { putbuf(yytext); return(STRING_LITERAL); 
                          yylval.symptr = ident(); }
{NT}/({WS}|[ ])*\:	{ putbuf(yytext); pdebug(DSCANDBG, "found TNTLHS");
			  BEGIN(NTLHS); laststate = NTLHS; 
                          yylval.symptr = ident();
                          return TNTLHS; }
<INITIAL,NTLHS>{ID}	{ putbuf(yytext); 
			  pdebug(DSCANDBG, "found TIDENT = %s", yytext);
                          yylval.symptr = ident();
			  return TIDENT; }
{NT}			{ putbuf(yytext); pdebug(DSCANDBG, "found NT");
                          yylval.symptr = ident();
			  return TNT; }
{DIGITS}		{ putbuf(yytext); pdebug(DSCANDBG, "found TNUM");
                          yylval.symptr = ident();
			  return TNUM;           /* number [0-9]+ */ }
%left			{ putbuf(yytext); pdebug(DSCANDBG, "found TLEFT");
			  return TLEFT; }
%right			{ putbuf(yytext); pdebug(DSCANDBG, "found TRIGHT");
			  return TRIGHT; }
%nonassoc		{ putbuf(yytext); pdebug(DSCANDBG, "found TNONASSOC");
			  return TNONASSOC; }
%token			{ putbuf(yytext); pdebug(DSCANDBG, "found TTOKEN"); 
			  return TTOKEN; }
%prec			{ putbuf(yytext); pdebug(DSCANDBG, "found TPREC");
			  return TPREC; }
%type			{ putbuf(yytext); pdebug(DSCANDBG, "found TTYPE");
			  return TTYPE; }
%start			{ putbuf(yytext); pdebug(DSCANDBG, "found TSTART");
			  return TSTART; }
%union			{ putbuf(yytext); pdebug(DSCANDBG, "found TUNION");
			  return TUNION; }
%pure_parser		{ putbuf(yytext); 
                          pdebug(DSCANDBG, "found TPURE_PARSER");
			  return TPURE_PARSER; }
%semantic_parser	{ putbuf(yytext); pdebug(DSCANDBG, 
					"found TSEMANTIC_PARSER");
			  return TSEMANTIC_PARSER; }
%expect			{ putbuf(yytext); pdebug(DSCANDBG, "found TEXPECT");
			  return TEXPECT; }
%thong			{ putbuf(yytext); pdebug(DSCANDBG, "found TTHONG");
			  return TTHONG ; }
<NTLHS>:		{ putbuf(yytext);
			  pdebug(DSCANDBG, "found ':' in NTLHS");
			  BEGIN(INITIAL); laststate=INITIAL; }
\%\%			{ putbuf(yytext); pdebug(DSCANDBG, "found TMARK");
			  return TMARK;         /* The %% mark */ }
\%\{			{ putbuf(yytext); pdebug(DSCANDBG, "found TLCURL");
			  return TLCURL;   /* The %{ mark */ }
.			{ putbuf(yytext); pdebug(DSCANDBG, 
				"SHIT! found a TOTHER = %c, ignored", 
				yytext[0]);
			  /* do nothing */ 
			}
%%

/****************************************************************************
 * section        : functions
 * description    :
 *
 *
 ****************************************************************************/ 


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : clearLinebuf
 * description    : clears the internal buffer for error messages. Called
 *                  whenever a \n is found in the input stream
 * returns        :
 * input          :
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int clearLinebuf(void) {

  int i = 0;
	
  linecnt++; 

  pdebug(DSCANDBG, "linecnt = %u", linecnt);

  charcnt = 0;
  for (i = 0; i < BUFMAX; i++) {
    linebuf[i] = '\0';
  }

  return OK;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : putbuf
 * description    : appends str to an internal buffer which is used of
 *                  printing meaningful error messages
 * returns        : ERROR or OK
 * input          : string
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int putbuf(char *str) {

  if ((yyleng + charcnt) >= BUFMAX - 1) {
    pdebug(DFATAL, "Line %d too long. Internal buffer overflow",
	   linecnt);
    exit(EXIT_FAILURE);
  }

  strcat(linebuf,str);
  charcnt += yyleng;

  return OK;
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : appendbuf
 * description    : appends char to an internal buffer which is used of
 *                  printing meaningful error messages.
 *                  Increments charcnt as a side effect.
 * returns        : ERROR or OK
 * input          : char
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int appendbuf(char c) {

  charcnt++;
  if (charcnt > BUFMAX - 1) {
    pdebug(DFATAL, "Line %d too long. Internal buffer overflow",
	   linecnt);
    exit(EXIT_FAILURE);
  }
  linebuf[charcnt] = c;

  return OK;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : eatcomment
 * description    : eats up a comment. 
 *                  
 * returns        : OK
 * input          : void
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int eatcomment(void) {

  int c;

  pdebug(DSCANDBG, "eating comment");

  for ( ; ; ) {
    while ((c = input()) != '*' && c != EOF)
      if ('\n' == c)
	clearLinebuf();
      else
	appendbuf(c);    /* eat up text of comment */
    if (c == '*') {
      while ( (c = input()) == '*' )
	appendbuf(c);
      if (c == '/') {
	appendbuf(c);
	break;    /* found the end */
      }
    }
    if ( c == EOF ) 
      bailout("EOF in comment");
  } /* end for ( ; ; ) */

  return OK;
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : eatRestOfLine
 * description    : eats up the rest of the line in a #define, #include, ... 
 *                  If the line ends with a '\' continue eating the next line
 *
 * returns        : OK
 * input          : void
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int eatRestOfLine(void) {
  int c;
  int bBackslashFound = FALSE;
  int bInStr = FALSE;
  int bInCharConst = FALSE;
  int bDone = FALSE;
  
  pdebug(DSCANDBG, "eating rest of line start");
  
  c = input();
  if (c != '\n')
    appendbuf(c);
  else
    bDone = TRUE;
  
  while (!bDone) {
    pdebug(DSCANDBG, "eatRestOfLine: while loop, c = %c", c);
    if (!isspace(c))
      bBackslashFound = FALSE;    /* invalidate prior '\' */
    switch (c) {
    case '"':
      bInStr = !bInStr;
      break;
    case '\'':
      bInCharConst = !bInCharConst;
      break;
    case '\\':
      if (!bInStr && !bInCharConst) 
	bBackslashFound = TRUE;
      break;
    case '\n': 
      clearLinebuf();
      /* continue eating lines if '\' was the last thing */
      if (!bBackslashFound)	
	bDone = TRUE; 
      break;
    default:	/* ignore character */
	;
    } /* end switch */
    if (!bDone) {
      c = input();
      if (c != '\n')
	appendbuf(c);
    } /* end if */
  } /* end for */
  pdebug(DSCANDBG, "eat rest of line end: c = %c", c);
  
  return OK;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : eatc
 * description    : eats up all ANSI C code in actions, section 1 or 3
 *  Now.... as it seems that parsing code that the preprocessor
 *  normally handles, parsing ANSI C and parsing the yacc-augmented
 *  actions such as $$ = $1 + $3, parsing C Declaration sections, 
 *  parsing the tailsection ... as it seems that this is 
 *  a bit unpleasant by merely using flex and yacc, I wrote 
 *  this routine that will simply do the whole thing manually.
 *  Thus we have a better logical seperation:
 *  On the one hand we have flex and yacc which parse and scan
 *  the yacc part (dependencies) and on the other hand, eatc() is called 
 *  everytime actions, C header declarations or the tailsection are to be
 *  parsed.
 *  Whats more: yacc does the same. It ignores C Code
 *  Whats even more: yacc also uses gotos here. I don't like them but
 * in combination with the graph of the scanner which can be found in the
 * documentation this is more readable than nested ifs, cases, ...
 *
 * returns        : char* with the string which holds the C code
 * input          : void
 * 
 * TODO           : has to handle EOF and empty C Code correctly.
 *                  Also: distinguish between tailsection and "normal" C Code.
 *                  Handle Strings, Char constants, multiline strings and
 *                  defines.
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
char* eatc(void) {
  int nestinglvl = 1;			/* nestinglevel of '{' '}'s */
  int c;				/* current char in input stream */
  char *buf;
  int i, k;
  unsigned int lastbufpos = 0;
  unsigned int buflen = BUFMAX;

  pdebug(DSCANDBG, "entering eatc Mode");

  /* set up buffer that will hold the C Code */
  if (NULL == (buf = my_malloc((size_t)buflen))) {
    bailout("could not malloc buffer for storing C code");
  }
  /* for safeties sake: initialize the buffer */
  for (i = 0; i < buflen; i++) {
    buf[i] = '\0';
  }

 start:
  c = getnextc(&buf, &lastbufpos, buflen);
  switch(c) {
  case '\t':				/* found a tab */
    lastbufpos--;			/* overwrites \t in C code buf */
    /* Note: here we expand the each tab to 8 spaces */
    for (k = 0; k < 8; k++) {
      unput(' ');
    }
    goto start;
  case '"':				/* Found a string */
    lastStringStartPos = charcnt;
    lastStringStartLine = linecnt;
    string_start:
    c = getnextc(&buf, &lastbufpos, buflen);
    switch(c) {
    case '\\': 
      c = getnextc(&buf, &lastbufpos, buflen);
      switch(c) {
      case '"':
	goto string_start;		/* it was in fact an "\"" */
      default:
	goto string_start;		/* it was a plain "\<somechar>" */
      } /* end switch */
    case '"':				/* string ended */
      lastStringStartPos = 0;		/* reset last string position */
      lastStringStartLine = 0;
      goto start;			/* continue reading stuff */
    default:
      goto string_start;		/* continue reading stuff */
    } /* end switch */

  case '\'':				/* found a char const */
    char_const_start:
    c = getnextc(&buf, &lastbufpos, buflen);
    switch(c) {
    case '\\': 
      c = getnextc(&buf, &lastbufpos, buflen);
      switch(c) {
      case '\'':
	goto char_const_start;		/* it was in fact an "\'" */
      default:
	goto char_const_start;		/* it was a plain "\<somechar>" */
      } /* end switch */
    case '\'':
      goto char_const_end;
    default:
      goto char_const_start;
    } /* end switch */
    char_const_end:
    goto start;				/* continue reading stuff */
      
  case '/':				/* possible comment */
    c = getnextc(&buf, &lastbufpos, buflen);
    switch(c) {
    case '*':				/* yes, we found a comment */
      ungetnextc(c, buf, &lastbufpos);
      ungetnextc('/', buf, &lastbufpos);
      eatcomment();
      goto start;
    default:
	ungetnextc(c, buf, &lastbufpos); /* no, it was not a comment */
	goto start;
    } /* end switch */
  case '\n':				/* possible preprocessor directive */
    clearLinebuf();
    c = getnextc(&buf, &lastbufpos, buflen);
    switch(c) {
    case '#':
      eatRestOfLine();
      ungetnextc('\n', buf, &lastbufpos);
      linecnt--;
      goto start;
    default:
      ungetnextc(c, buf, &lastbufpos);
      goto start;
    } /* end switch */

  case '{':				/* handle nestinglvl */
    nestinglvl++;
    goto start;

  case '}':				/* handle nestinglvl */
    nestinglvl--;
    if (nestinglvl == 0) {
      ungetnextc(c, buf, &lastbufpos);
      goto end;
    }
    else
      goto start;

  case '%':				/* possible end of C Decl. section */
    c = getnextc(&buf, &lastbufpos, buflen);
    switch(c) {
      case '}':				/* yes, C Decl. section end */
	pdebug(DSCANDBG, "eatc: bInCDeclar = %d", bInCDeclar);
	if (bInCDeclar) {
	  ungetnextc(c, buf, &lastbufpos);
	  ungetnextc('%', buf, &lastbufpos);
	  goto end;
	} 
	else {
	  ungetnextc(c, buf, &lastbufpos);
	  goto start;
	} /* end if else */
    default:
      ungetnextc(c, buf, &lastbufpos);
      goto start;
    } /* end switch */
  default:				/* any other char */
    goto start;				/* ignore it */
  } /* end switch */
    
 end:
  pdebug(DSCANDBG, "leaving eatc mode");
  pdebug(DSCANDBG, "eatc: ccode buffer = %s", buf);
  return buf;
}



int ungetnextc(char c, char *buf, unsigned int *lastbufpos) {
  unput(c);
  charcnt--;

  (*lastbufpos)--;
  buf[(*lastbufpos)] = '\0';

  pdebug(DSCANDBG, "ungetnextc: ungetting %c", c);
  pdebug(DSCANDBG, "ungetnextc: *lastbufpos = %u", *lastbufpos);
  return OK;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : getnextc
 * description    : reads the next char from the input stream. If EOF
 *                  is encountered it prints out an error message.
 * returns        : TRUE or FALSE
 * input          : string
 * 
 * TODO           :  
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int getnextc(char **buf, unsigned int *lastbufpos, int buflen) {
  int c;
  char *buf2;

  /* check if buffer will be big enough */
  if ((*lastbufpos) + 1 >= buflen - 10) {
    /* resize buffer */
    buf2 = my_malloc((size_t)(buflen + CBUFGROWSIZE));
    if (buf2 == NULL) 
      bailout("getnextc: could not malloc new buf2");
    pdebug(DSCANDBG, "getnextc: had to malloc new buf2 and copy existing buf");
    strcpy(buf2, (*buf));
    /* forget old buf. It will be garbage collected later */
    (*buf) = buf2;
  }
  
  c = input();
  appendbuf(c);
  (*buf)[(*lastbufpos)] = c;
  (*buf)[(*lastbufpos)+1] = '\0';
  pdebug(DSCANDBG, "getnextc: appending \\0 to end of buf at pos: %u",
	 *lastbufpos+1);
  (*lastbufpos)++;

  pdebug(DSCANDBG, "getnextc: c = %c", c);
  pdebug(DSCANDBG, "getnextc: *lastbufpos = %u", *lastbufpos);
  pdebug(DSCANDBG, "getnextc: ccode buffer = %s", (*buf));

  if (c == EOF) {
    if (lastStringStartPos > 0) {
      pdebug(DFATAL, "premature EOF encountered, maybe we ran into an "
	     "unterminated string which starts at line %d, position %d?",
	     lastStringStartLine, lastStringStartPos);
      exit(EXIT_FAILURE);
    }
    else {
      pdebug(DFATAL, "premature EOF encountered");
      exit(EXIT_FAILURE);
    } /* end if */
  } /* end if */
  return c;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : chk4str
 * description    : checks if the following chars in the input stream
 *                  match str
 * returns        : TRUE or FALSE
 * input          : string
 * 
 * TODO           : has to handle EOF 
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int chk4str(char *str) {
  char *bufptr;
  int i, j;
  int bSucc = FALSE;
  
  pdebug(DSCANDBG, "start of chk4str, str = %s", str);
  
  if (NULL == (bufptr = my_malloc((size_t)(strlen(str)+1)))) 
    bailout("could not malloc internal buffer");
  
  for (i = 0; i < strlen(str); i++) {
    bufptr[i] = input();
  }
  bufptr[strlen(str)] = 0;
  
  pdebug(DSCANDBG, "filled bufptr = %s", bufptr);
  
  if (0 != strncmp(bufptr, str, strlen(str))) {
    pdebug(DSCANDBG, "strcmp != 0");
    bSucc = FALSE;	/* we were not successfull */
    
    return FALSE;
  } else {
    pdebug(DSCANDBG, "strcmp == 0");
    bSucc = TRUE;
  } /* end if */

  /* unput everything and return */
  for (j = i; j >= 0; j--) {
    unput(bufptr[j]);
  }
  if (!bSucc)
    return FALSE;
  else
    return TRUE;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : ident
 * description    : puts the current identifier into the current symtab
 * returns        : ptr to new symtab entry
 * input          : void
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
symentry *ident(void) {
  static symentry *symptr;	
  
  if (!symlookupLexem(symCurrSymtab, yytext, &symptr)) {
    if (!syminsertLexem(symCurrSymtab, yytext, &symptr)) {
      pdebug(DFATAL, "lex: could not insert identifier %s "
	     "into symtab, exiting.", yytext);
      exit(EXIT_FAILURE);
    }
  }
  
  return symptr;
}


