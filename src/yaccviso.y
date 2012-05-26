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

/* file:        yaccviso.y
 * YACCVISO
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at
 * date:        13.10.1997
 * last mod.:
 * descr.:      bison file for yaccviso. Also builds up a parse tree.
 * open probs:	Does not handle strings "foobar" in RHS of yacc rule yet.
 */

#include "global.h"
#include "symtab.h"
#include "error.h"
#include "tree.h"

extern int yylex(void);

%}

%union {
  char           *ccode;		/* a char* that holds the c code*/
  node           *nptr;
  symentry	 *symptr;		/* for non terminals (LHS, RHS), 
					   terminals, identifiers  */
}

%token <symptr> TNUM		/* various numbers in C (float, hex, ...) */
%token <symptr> CHARCONSTANT 
%token <symptr> STRING_LITERAL
%token <ccode> CCODE 

%token <symptr> TIDENT			/* generic identifier */
%token <symptr> TNT			/* non-terminals, like TIDENT,
					   but can also have a '.' */
%token <symptr> TNTLHS			/* non terminal left hand side */
%token TLEFT				/* "%left" */
%token TRIGHT				/* "%right" */
%token TNONASSOC			/* "%nonassoc" */
%token TTOKEN				/* "%token" */
%token TPREC				/* "%prec" */
%token TTYPE				/* "%type" */
%token TSTART				/* "%start" */
%token TUNION				/* "%union" */
%token TPURE_PARSER			/* "%pure_parser" */
%token TSEMANTIC_PARSER			/* "%semantic_parser" */
%token TEXPECT				/* "%expect" */
%token TTHONG				/* "%thong" */
%token TMARK				/* The %% mark */
%token TLCURL				/* The %{ mark */
%token TRCURL				/* The %} mark */
%token TSEMICOL				/* ';' */
%token TPIPE				/* '|' */
%token TOPENBRACES			/* '{' */
%token TCLOSEBRACES			/* '}' */
%token TLT				/* '<' */
%token TGT				/* '>' */
%token TCOMMA				/* ',' */
%token TEPSILON				/* empty string. Used for empty prods*/



%type <nptr> spec defsection bisonDeclars CDeclar bisonDeclar
%type <nptr> yaccIdentifier unionDeclar rword tag nlist nmno 
%type <nptr> rulesection rules rule productions production 
%type <nptr> rulebody prec action tailsection

%start spec
/* ------------ End Bison declarations                                     */


%%
spec            : defsection 
		   { expect = "expecting \%\%"; }
		TMARK 
		   { expect = "expecting one or more productions"; }
		rulesection 
		   { expect = "expecting \%\% or non terminal on LHS"; }
		TMARK 
		   { expect = "expecting tailsection";
		     bInTailSection = TRUE; } 
		tailsection
		  {{ node *tmpptr1 = mknode(spec);
		     node *tmpptr2 = mkleaf(TMARK, NULL, NULL); 
		     node *tmpptr3 = mkleaf(TMARK, NULL, NULL); 
		     appendchld(tmpptr1, $1);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, $5);
		     appendchld(tmpptr1, tmpptr3);
		     appendchld(tmpptr1, $9);
		     $$ = tmpptr1;
		     tree = $$; 
		     expect = "expecting EOF"; 
		     pdebug(DPARSEDBG,"matched spec - EVERYTHING MATCHED :)");}}
                ;
defsection      : /* empty */ 
		  {{ node *tmpptr1 = mknode(defsection);
		     node *tmpptr2 = mkleaf(TEPSILON, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     expect = "expecting \%\% or a yacc declaration or "
			      "a \%{"; }}
		| defsection 
		   { expect = "expecting a yacc declaration"; }
		  bisonDeclars
		  {{ node *tmpptr = mknode(defsection);
		     appendchld(tmpptr, $1);
		     appendchld(tmpptr, $3);
		     $$ = tmpptr; 
		     expect = "expecting a yacc declaration or \%{ or \%\%";
		     pdebug(DPARSEDBG, "matched defsection - defsection "
				       "bisonDeclars"); }}
                ;
bisonDeclars    : bisonDeclar
		  {{ node *tmpptr = mknode(bisonDeclars);
		     appendchld(tmpptr, $1);
		     $$ = tmpptr; 
		     expect = "expecting a yacc declaration or \%{ or \%\%";
		     pdebug(DPARSEDBG, "matched bisonDeclars - "
				       "bisonDeclar"); }}
		| CDeclar 
		  {{ node *tmpptr = mknode(bisonDeclars);
		     appendchld(tmpptr, $1); 
		     $$ = tmpptr; 
		     expect = "expecting a yacc declaration or \%{ or \%\%";
		     pdebug(DPARSEDBG, "matched bisonDeclars - CDeclar"); }}
                ;
CDeclar         : TLCURL 
		   { expect = "expecting ANSI C Code"; 
		     bSkipC = TRUE; bInCDeclar = TRUE; } 
		  CCODE 
		   { expect = "expecting \%}";  
		   bSkipC = FALSE; bInCDeclar = FALSE; } 
		  TRCURL 
		  {{ node *tmpptr1 = mknode(CDeclar);
		     node *tmpptr2 = mkleaf(TLCURL, NULL, NULL);
		     node *tmpptr3 = mkleaf(CCODE, NULL, $3);
		     node *tmpptr4 = mkleaf(TRCURL, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, tmpptr3);
		     appendchld(tmpptr1, tmpptr4);
		     $$ = tmpptr1;
		     expect = "expecting a yacc declaration or \%{ or \%\%";
		     pdebug(DPARSEDBG, "matched CDeclar - TLCURL CCode "
				       "TRCURL"); }}
                ;
bisonDeclar     : TSTART 
		   { expect = "expecting identifier"; }
		  yaccIdentifier
		  {{ node *tmpptr1 = mknode(bisonDeclar);
		     node *tmpptr2 = mkleaf(TSTART, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, $3);
		     $$ = tmpptr1;
		     expect = "expecting a yacc declaration or \%{ or \%\%";
		     pdebug(DPARSEDBG, "matched bisonDeclar - "
			"TSTART yaccIdentifier"); }}
                | unionDeclar
		  {{ node *tmpptr = mknode(bisonDeclar);
		     appendchld(tmpptr, $1);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched bisonDeclar - unionDeclar"); 
		     expect = "expecting a yacc declaration or \%{ or \%\%"; }}
                | TPURE_PARSER
		  {{ node *tmpptr1 = mknode(bisonDeclar);
		     node *tmpptr2 = mkleaf(TPURE_PARSER, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched bisonDeclar - TPURE_PARSER"); 
		     expect = "expecting a yacc declaration or \%{ or \%\%"; }}
                | TSEMANTIC_PARSER
		  {{ node *tmpptr1 = mknode(bisonDeclar);
		     node *tmpptr2 = mkleaf(TSEMANTIC_PARSER, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched bisonDeclar - "
				       "TSEMANTIC_PARSER"); 
		     expect = "expecting a yacc declaration or \%{ or \%\%"; }}
                | TEXPECT 
		   { expect = "expecting number"; }
		  TNUM
		  {{ node *tmpptr1 = mknode(bisonDeclar);
		     node *tmpptr2 = mkleaf(TEXPECT, NULL, NULL);
		     node *tmpptr3 = mkleaf(TNUM, $3, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, tmpptr3);
		     $$ = tmpptr1;
		     $3->type = symtype_tnum;
		     pdebug(DPARSEDBG, "matched bisonDeclar - "
				       "TEXPECT TNUM"); 
		     expect = "expecting a yacc declaration or \%{ or \%\%"; }}
                | TTHONG
		  {{ node *tmpptr1 = mknode(bisonDeclar);
		     node *tmpptr2 = mkleaf(TTHONG, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched bisonDeclar - TTHONG"); 
		     expect = "expecting a yacc declaration or \%{ or \%\%"; }}
                | rword 
		   { expect = "expecting TLT or identifier"; }
		  tag 
		   { expect = "expecting identifier"; }
		  nlist
		  {{ node *tmpptr = mknode(bisonDeclar);
		     appendchld(tmpptr, $1);
		     appendchld(tmpptr, $3);
		     appendchld(tmpptr, $5);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched bisonDeclar - "
				       "rword tag nlist"); 
		     expect = "expecting a yacc declaration or \%{ or \%\%"; }}
                ;
yaccIdentifier	: TIDENT
		  {{ node *tmpptr1 = mknode(yaccIdentifier);
		     node *tmpptr2 = mkleaf(TIDENT, $1, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched yaccIdentifier - TIDENT"); 
		     expect = ""; }}
		| TNT
		  {{ node *tmpptr1 = mknode(yaccIdentifier);
		     node *tmpptr2 = mkleaf(TNT, $1, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched yaccIdentifier - TNT"); 
		     expect = ""; }}
		;
unionDeclar     : TUNION 
		   { expect = "expecting TOPENBRACES"; }
		  TOPENBRACES 
		   { bSkipC = TRUE; } 
		  CCODE 
		   { expect = "expecting '}'"; 		     
		     bSkipC = FALSE; }
		  TCLOSEBRACES
		  {{ node *tmpptr1 = mknode(unionDeclar);
		     node *tmpptr2 = mkleaf(TUNION, NULL, NULL);
		     node *tmpptr3 = mkleaf(TOPENBRACES, NULL, NULL);
		     node *tmpptr4 = mkleaf(CCODE, NULL, $5);
		     node *tmpptr5 = mkleaf(TCLOSEBRACES, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, tmpptr3);
		     appendchld(tmpptr1, tmpptr4);
		     appendchld(tmpptr1, tmpptr5);
		     $$ = tmpptr1;		     
		     pdebug(DPARSEDBG, "matched unionDeclar - "
			"TUNION TOPENBRACES struct TCLOSEBRACES"); 
		     expect = "expecting a yacc declaration or \%{ or \%\%"; }}
		;
rword           : TTOKEN
		  {{ node *tmpptr1 = mknode(rword);
		     node *tmpptr2 = mkleaf(TTOKEN, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched rword - TTOKEN"); 
		     expect = "expecting TLT or identifier"; }}
                | TLEFT
		  {{ node *tmpptr1 = mknode(rword);
		     node *tmpptr2 = mkleaf(TLEFT, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched rword - TLEFT"); 
		     expect = "expecting TLT or identifier"; }}
                | TRIGHT
		  {{ node *tmpptr1 = mknode(rword);
		     node *tmpptr2 = mkleaf(TRIGHT, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched rword - TRIGHT"); 
		     expect = "expecting TLT or identifier"; }}
                | TNONASSOC
		  {{ node *tmpptr1 = mknode(rword);
		     node *tmpptr2 = mkleaf(TNONASSOC, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched rword - TNONASSOC"); 
		     expect = "expecting TLT or identifier"; }}
                | TTYPE
		  {{ node *tmpptr1 = mknode(rword);
		     node *tmpptr2 = mkleaf(TTYPE, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched rword - TTYPE"); 
		     expect = "expecting TLT or identifier"; }}
                ;
tag             : TLT
		   { expect = "expecting identifier"; } 
		  TIDENT 
		   { expect = "expecting '>'"; }
		  TGT
		  {{ node *tmpptr1 = mknode(tag);
		     node *tmpptr2 = mkleaf(TLT, NULL, NULL); 
		     node *tmpptr3 = mkleaf(TIDENT, $3, NULL);
		     node *tmpptr4 = mkleaf(TGT, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, tmpptr3);
		     appendchld(tmpptr1, tmpptr4);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched tag - '<' TIDENT '>'"); 
		     expect = "expecting list of identifiers"; }}
                | {{ node *tmpptr1 = mknode(tag);
		     node *tmpptr2 = mkleaf(TEPSILON, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; }}
                ;
nlist           : nmno
		  {{ node *tmpptr = mknode(nlist);
		     appendchld(tmpptr, $1);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched nlist - nmno"); 
		     expect = "expecting a yacc declaration or \%{ or "
			      "\%\% or another identifier or number"; }}
                | nlist 
		   { expect = "expecting identifier or number"; }
		  nmno
		  {{ node *tmpptr = mknode(nlist);
		     appendchld(tmpptr, $1);
		     appendchld(tmpptr, $3);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched nlist - nlist nmno"); 
		     expect = "expecting a yacc declaration or \%{ or "
			      "\%\% or another identifier or number"; }}
                | nlist 
		   { expect = "expecting ','"; }
		  TCOMMA 
		   { expect = "expecting identifier or number"; }
		  nmno
		  {{ node *tmpptr = mknode(nlist);
		     appendchld(tmpptr, $1);
		     appendchld(tmpptr, $5);   /* we ignore the ',' */ 
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched nlist - nlist TCOMMA nmno"); 
		     expect = "expecting a yacc declaration or \%{ or "
			      "\%\% or another identifier or number"; }}
                ;
nmno            : yaccIdentifier
		  {{ node *tmpptr = mknode(nmno);
		     appendchld(tmpptr, $1);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched nmno - yaccIdentifier"); 
		     expect = ""; }}
                | yaccIdentifier 
		   { expect = "expecting number"; }
		  TNUM
		  {{ node *tmpptr1 = mknode(nmno);
		     node *tmpptr2 = mkleaf(TNUM, $3, NULL);
		     appendchld(tmpptr1, $1);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     $3->type = symtype_tnum;
		     pdebug(DPARSEDBG, "matched nmno - yaccIdentifier TNUM"); 
		     expect = ""; }}
                | CHARCONSTANT
		  {{ node *tmpptr1 = mknode(nmno);
		     node *tmpptr2 = mkleaf(CHARCONSTANT, $1, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     $1->type = symtype_charconstant;
		     pdebug(DPARSEDBG, "matched nmno - CHARCONSTANT"); 
		     expect = ""; }}
                | CHARCONSTANT 
		   { expect = "expecting number"; }
		  TNUM
		  {{ node *tmpptr1 = mknode(nmno);
		     node *tmpptr2 = mkleaf(CHARCONSTANT, $1, NULL);
		     node *tmpptr3 = mkleaf(TNUM, $3, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, tmpptr3);
		     $$ = tmpptr1;
		     $1->type = symtype_charconstant;
		     $3->type = symtype_tnum;
		     pdebug(DPARSEDBG, "matched nmno - yaccIdentifier TNUM"); 
		     expect = ""; }}
                ;
rulesection     : rules
		  {{ node *tmpptr = mknode(rulesection);
		     appendchld(tmpptr, $1);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched rulessection - rules"); 
		     expect = "expecting \%\%"; }}
                ;
rules           : rule 
		   { expect = "expecting ';'"; }
		  TSEMICOL 
		  {{ node *tmpptr1 = mknode(rules);
		     node *tmpptr2 = mkleaf(TSEMICOL, NULL, NULL);
		     appendchld(tmpptr1, $1);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched rules - rule TSEMICOL"); 
		     expect = "expecting another rule or \%\%"; }}
                | rules 
		   { expect = "expecting rule"; }
		  rule
		   { expect = "expecting ';'"; }
		  TSEMICOL
		  {{ node *tmpptr1 = mknode(rules);
		     node *tmpptr2 = mkleaf(TSEMICOL, NULL, NULL);
		     appendchld(tmpptr1, $1);
		     appendchld(tmpptr1, $3);
		     appendchld(tmpptr1, tmpptr2); 
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched rules - rules rule TSEMICOL"); 
		     expect = "expecting another rule or \%\%"; }}
                ;
rule            : TNTLHS 
		   { expect = "expecting body of rule"; }
		  productions 
		  {{ node *tmpptr1 = mknode(rule);
		     node *tmpptr2 = mkleaf(TNTLHS, $1, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, $3);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched rule - TNTLHS "
				       "rulebody prec"); 
		     expect = "expecting ';'"; }}
                ;
productions	: production
		  {{ node *tmpptr = mknode(productions);
		     appendchld(tmpptr, $1);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched productions - production"); 
		     expect = "expecting ';'"; }}
		| productions
		   { expect = "expecting '|'"; }
		  TPIPE
		   { expect = "expecting action, identifier or character "
			      "constant"; }
		  production
		  {{ node *tmpptr1 = mknode(productions);
		     node *tmpptr2 = mkleaf(TPIPE, NULL, NULL);
		     appendchld(tmpptr1, $1);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, $5);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched productions - productions "
				       "TPIPE production");
		     expect = "expecting ';'"; }}
		;
production	: rulebody
		   { expect = "expecting \"%prec\", '|' or ';'"; }
		  prec
		  {{ node *tmpptr = mknode(production);
		     appendchld(tmpptr, $1);
		     appendchld(tmpptr, $3);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched production - rulebody prec");
		     expect = "expecting '|' or ';'"; }}
		;  
rulebody        : action
		  {{ node *tmpptr = mknode(rulebody);
		     appendchld(tmpptr, $1);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched rulebody - action");
		     expect = "expecting identifier, character constant,  "
			      "\"%prec\" or ';'"; }}
                | rulebody 
		   { expect = "expecting identifier"; }
		  yaccIdentifier
		   { expect = "expecting identifier, '|', ';', action or "
			      "\"%prec\""; }
		  action
		  {{ node *tmpptr = mknode(rulebody);
		     appendchld(tmpptr, $1);
		     appendchld(tmpptr, $3);
		     appendchld(tmpptr, $5);
		     $$ = tmpptr;
		     pdebug(DPARSEDBG, "matched rulebody - rulebody "
				       "yaccIdentifier action"); 
		     expect = "expecting identifier, character constant,  "
			      "\"%prec\" or ';'"; }}
                | rulebody 
		   { expect = "expecting character constant"; }
		  CHARCONSTANT
		   { expect = "expecting identifier, '|', ';', action or "
			      "\"%prec\""; }
		  action
		  {{ node *tmpptr1 = mknode(rulebody);
		     node *tmpptr2 = mkleaf(CHARCONSTANT, $3, NULL);
		     appendchld(tmpptr1, $1);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, $5);
		     $$ = tmpptr1;
		     $3->type = symtype_charconstant;
		     pdebug(DPARSEDBG, "matched rulebody - rulebody "
				       "CHARCONSTANT action"); 
		     expect = "expecting identifier, character constant,  "
			      "\"%prec\" or ';'"; }}
                ;
prec            : /*empty */ 
		  {{ node *tmpptr1 = mknode(prec);
		     node *tmpptr2 = mkleaf(TEPSILON, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched prec - epsilon");
		     expect = "expecting '|' or ';' "; }}
                | TPREC 
		   { expect = "expecting identifier"; }
		  yaccIdentifier
		   { expect = "expecting action or '|' or ';'"; }
		  action
		  {{ node *tmpptr1 = mknode(prec);
		     node *tmpptr2 = mkleaf(TPREC, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, $3);
		     appendchld(tmpptr1, $5);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched prec - "
			"TPREC yaccIdentifier action"); 
		     expect = "expecting '|' or ';' "; }}
                | TPREC 
		   { expect = "expecting character constant"; }
		  CHARCONSTANT
		   { expect = "expecting action or '|' or ';'"; }
		  action
		  {{ node *tmpptr1 = mknode(prec);
		     node *tmpptr2 = mkleaf(TPREC, NULL, NULL);
		     node *tmpptr3 = mkleaf(CHARCONSTANT, $3, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, tmpptr3);
		     appendchld(tmpptr1, $5);
		     $$ = tmpptr1;
		     pdebug(DPARSEDBG, "matched prec - "
			"TPREC CHARCONSTANT action"); 
		     expect = "expecting '|' or ';' "; }}
                ;
action          : /* empty */
		  {{ node *tmpptr1 = mknode(action);
		     node *tmpptr2 = mkleaf(TEPSILON, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched action - epsilon");
		     expect = "expecting XXX "; }}
		| TOPENBRACES 
		   { /* skip Code as even Yacc itself ignores it */ 
		     bSkipC = TRUE; } 
		  CCODE
		   { expect = "expecting '}'"; 	     
		     bSkipC = FALSE; } 
		  TCLOSEBRACES
		   {{ node *tmpptr1 = mknode(action);
		     node *tmpptr2 = mkleaf(TOPENBRACES, NULL, NULL);
		     node *tmpptr3 = mkleaf(CCODE, NULL, $3);
		     node *tmpptr4 = mkleaf(TCLOSEBRACES, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     appendchld(tmpptr1, tmpptr3);
		     appendchld(tmpptr1, tmpptr4);
		     $$ = tmpptr1;
			     pdebug(DPARSEDBG, "matched action - "
				       "TOPENBRACES CCode TCLOSEBRACES"); 
		     expect = "expecting end or continuation of rule"; }}
                ;
tailsection     : /* empty rule as this does not matter */
		  {{ node *tmpptr1 = mknode(tailsection);
		     node *tmpptr2 = mkleaf(TEPSILON, NULL, NULL);
		     appendchld(tmpptr1, tmpptr2);
		     $$ = tmpptr1; 
		     pdebug(DPARSEDBG, "matched tailsection - TMARK, EOF"); 
		     expect = "expecting EOF"; }}
                ;
/* ------ End Bison Grammar ----------------------------------------------- */
%%







