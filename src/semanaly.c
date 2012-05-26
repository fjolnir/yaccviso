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

/* file:        semanaly.c                                                   
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        see below                                                    
 * last mod.:   see below                                                    
 * descr.:                                                                   
 */

/****************************************************************************
 * filename             : semanaly.c
 * author               : aka
 * date                 : Mon Jan 20 18:29:52 MET 1997
 * last modified        : 1997 Jan 08
 * depends on           : global.h, tree.h
 * used for             : 
 * produces file        : semanaly.o
 * -------------------------------------------------------------------------
 * description          :
 *
 *
 *
 * open problems        : traversing of the tree should be redone so that
 *                        it will be easier to understand the code
 *
 *
 ****************************************************************************/ 

#include "semanaly.h"
#include "y.tab.h"
#include "dependencies.h"

dep_tbl_entry *chkTSTARTYaccIdentifier(node *nptr) {
  childptr *tmpptr;
  dep_tbl_entry *tmp_dep_tbl_ptr;
  
  tmpptr = nptr->childrenstart;
  /* safety check */
  if (tmpptr == NULL || tmpptr->child == NULL || 
      tmpptr->child->symptr == NULL) {
    bailout("chkTSTARTYaccIdentifier: unknown or invalid child type");
  }

  tmp_dep_tbl_ptr = insert_gram_el(tmpptr->child->symptr, gram_el_type_nt);
  /* semval will be specified later */

  return tmp_dep_tbl_ptr;
  

}

dep_tbl_entry *chkNmnoYaccIdentifier(node *nptr, symentry *semval, 
				 int which_rword) {
  childptr *tmpptr;
  dep_tbl_entry *tmp_dep_tbl_ptr;
  symentry *tmp_symptr;

  tmpptr = nptr->childrenstart;
  /* safety check */
  if (tmpptr->child == NULL || tmpptr->child->symptr == NULL) {
    bailout("chkYaccIdentifier: invalid child");
  }
  tmp_symptr = tmpptr->child->symptr;

  /* we want to associate types and semvals with the yaccIdentifier 
   * and first insert it into the dep_tbl 
   */
  if (tmpptr->child->id == TIDENT) {
    if (which_rword == TTOKEN) {
      tmp_dep_tbl_ptr = insert_gram_el(tmpptr->child->symptr, 
				       gram_el_type_token);
      tmp_dep_tbl_ptr->semvalspec = semval;
    } 
    else {
      /* must be a TTYPE */
      tmp_dep_tbl_ptr = insert_gram_el(tmpptr->child->symptr, gram_el_type_nt);
      tmp_dep_tbl_ptr->semvalspec = semval;
    }
  }
  else if (tmpptr->child->id == TNT) {
    tmp_dep_tbl_ptr = insert_gram_el(tmpptr->child->symptr, gram_el_type_nt);
    tmp_dep_tbl_ptr->semvalspec = semval;      
  }
  else {
    bailout("chkYaccIdentifier: unknown child type");
  } /* end if elseif else */
  return tmp_dep_tbl_ptr;
  
}

dep_tbl_entry *chkNmnoCHARCONSTANT(node *nptr, symentry *semval, 
				 int which_rword) {
  node *tmpptr;
  dep_tbl_entry *tmp_dep_tbl_ptr;
  symentry *tmp_symptr;

  tmpptr = nptr;
  /* safety check */
  if (tmpptr == NULL || tmpptr->symptr == NULL) {
    bailout("chkNmnoCHARCONSTANT: node");
  }
  tmp_symptr = tmpptr->symptr;

  /* we want to associate types and semvals with the CHARCONSTANT
   * and first insert it into the dep_tbl 
   */
  tmp_dep_tbl_ptr = insert_gram_el(tmpptr->symptr, 
				       gram_el_type_token);
  tmp_dep_tbl_ptr->semvalspec = semval;

  return tmp_dep_tbl_ptr;
  
}


int chkNmno(node *nptr, symentry *semval, int which_rword) {
  childptr *tmpptr;

  tmpptr = nptr->childrenstart;
  if (tmpptr->child->id == yaccIdentifier) {
    chkNmnoYaccIdentifier(tmpptr->child, semval, which_rword);
  }
  if (tmpptr->child->id == CHARCONSTANT) {
    chkNmnoCHARCONSTANT(tmpptr->child, semval, which_rword);
  }
  if (nptr->childrenstart != nptr->childrenend) {
    if (tmpptr->nextptr->child->id == TNUM) {
      ;					/* do nothing with it */
    }
    else {
      bailout("chkNmno: unknown type of second child");
    }
  }
  return OK;
}


int chkRword(node *nptr) {
  childptr *tmpptr;
  int retval;

  tmpptr = nptr->childrenstart;
  if (tmpptr->child == NULL) {
    bailout("chkRword: child does not exist");
  }

  switch(tmpptr->child->id) {
  case TTOKEN:
    retval = TTOKEN;
    break;
  case TLEFT:
    retval = TTOKEN;		       /* we abstract here: we are only  */
    break;
  case TRIGHT:			       /* interested if its a token or a  */
    retval = TTOKEN;		       /* non terminal */
    break;
  case TNONASSOC:
    retval = TTOKEN;
    break;
  case TTYPE:
    retval = TTYPE;
    break;
  default:
    bailout("chkRword: unknown child type");
  } /* end switch */

  return retval;
}

symentry *chkTag(node *nptr) {
  childptr *tmpptr;
  node *tmpnodeptr;

  tmpptr = nptr->childrenstart;
  if (tmpptr->child->id == TEPSILON) {
    return NULL;		       /* no semantic value type */
  }
  if (tmpptr->child->id == TLT) {
    if (tmpptr->nextptr->child->id == TIDENT) {
      tmpnodeptr = tmpptr->nextptr->child;	/* move to child */
      tmpnodeptr->symptr->type = symtype_semvalspec;
      return tmpnodeptr->symptr;
    }
    else {
      bailout("chkTag: unknown type of second child");
    }
  }
  else {
    bailout("chkTag: unknown child type");
  }
  return NULL;				/* not neccessary, but why not */
}


int chkNlist(node *nptr, symentry *semval, int which_rword) {
  childptr *tmpptr;

  tmpptr = nptr->childrenstart;
  if (tmpptr->child->id == nmno) {
    chkNmno(tmpptr->child, semval, which_rword);
  }
  else if (tmpptr->child->id == nlist) {
    if (tmpptr->nextptr->child->id == TCOMMA) {
      chkNmno(tmpptr->nextptr->nextptr->child, semval, which_rword);
    }
    else if (tmpptr->nextptr->child->id == nmno) {
      chkNmno(tmpptr->nextptr->child, semval, which_rword);
    }
    else {
      bailout("chkNlist: unknown type of second child");
    }
  }
  else {
    bailout("chkNlist: unknown child type");
  }
  return OK;
}


int chkUnionDeclar(node *nptr) {
/*   node *tmpptr;  */
  
  /* XXX should we do something here? At the moment we will 
     ignore it */

  return OK;
}

int chkBisonDeclar(node *nptr) {
  childptr *tmpptr;
  symentry *semval;
  int which_rword;		       /* one of the #defines in y.tab.h */ 
  dep_tbl_entry* tmp_dep_tbl_ptr;

  tmpptr = nptr->childrenstart;

  switch(tmpptr->child->id) {
  case TSTART:
    /* safety check */
    if (tmpptr->nextptr == NULL || tmpptr->nextptr->child == NULL ||
	tmpptr->nextptr->child->id != yaccIdentifier || 
	tmpptr->nextptr->child->childrenstart == NULL || 
	tmpptr->nextptr->child->childrenstart->child == NULL ||
	tmpptr->nextptr->child->childrenstart->child->symptr == NULL) {
      bailout("chkBisonDeclar: TSTART without proper second child type");
    }
    /* save the start symbol somewhere and say that this gram_el is a nt */
    /*  we have to adapt semval later, this will be done in nlist */
    tmp_dep_tbl_ptr = chkTSTARTYaccIdentifier(tmpptr->nextptr->child);
    gram_start = tmp_dep_tbl_ptr;
    break;

  case unionDeclar:
    chkUnionDeclar(tmpptr->child);
    break;

  case TPURE_PARSER:
    /* do nothing */
    break;

  case TSEMANTIC_PARSER:
    /* do nothing */
    break;

  case TEXPECT:
    /* do nothing */
    break;

  case TTHONG:
    /* do nothing */
    break;

  case rword:
    which_rword = chkRword(tmpptr->child); 
    semval = chkTag(tmpptr->nextptr->child);
    chkNlist(tmpptr->nextptr->nextptr->child, semval, which_rword);
    break;

  default:
    bailout("chkBisonDeclar: unknown type of child in node"); 
    break;
  } /* end switch */
  return OK;
}


int chkCDeclar(node *nptr) {
  childptr *tmpptr;

  /* ignore it ... at the moment we do nothing with it */
  return OK;
}


int chkBisonDeclars(node *nptr) {
  childptr *tmpptr;

  tmpptr = nptr->childrenstart;

  if(tmpptr->child->id == bisonDeclar) {
    chkBisonDeclar(tmpptr->child);
  }
  else if (tmpptr->child->id == CDeclar) {
    chkCDeclar(tmpptr->child);
  }
  else {
    bailout("chkBisonDeclars: unknown child type");
  }

  return OK;
}


int chkDefsection(node *nptr) {
  childptr *tmpptr;
  
  tmpptr = nptr->childrenstart;
  if (tmpptr->child->id == defsection) {
    chkDefsection(tmpptr->child);
    chkBisonDeclars(tmpptr->nextptr->child);
  } 
  else if (tmpptr->child->id == TEPSILON) {
    ;
  }
  else {
    bailout("chkDefsection: unknown child type");
  }
  return OK;
}


dep_tbl_bucket *chkRulebodyYaccIdentifier(node *nptr, 
		   dep_tbl_entry *ntlhs_ptr, dep_class curr_dep_class,
		   int curr_gram_el_no, char *action) {

  childptr *tmpptr;
  node *tmpnodeptr;
  dep_tbl_bucket *bucket;
  
  tmpptr = nptr->childrenstart;
  tmpnodeptr = tmpptr->child;

  if (tmpnodeptr->id == TIDENT) {
    if (NULL == lookup_gram_el(tmpnodeptr->symptr)) {
      /* first we have to insert it */
      insert_gram_el(tmpnodeptr->symptr, gram_el_type_unknown);
    }
    bucket = append_bucket(ntlhs_ptr, tmpnodeptr->symptr, curr_dep_class,
			   action);
    return bucket;
  }
  else if (tmpnodeptr->id == TNT) {
    if (NULL == lookup_gram_el(tmpnodeptr->symptr)) {
      /* first we have to insert it */
      insert_gram_el(tmpnodeptr->symptr, gram_el_type_nt);
    }
    bucket = append_bucket(ntlhs_ptr, tmpnodeptr->symptr, curr_dep_class,
			   action);
    return bucket;
  }
  else
    bailout("chkRulebodyYaccIdentifier: unknown type of child");

  return NULL;				/* not needed, but why not */
}


int chkRulebody(node *nptr, dep_tbl_entry *ntlhs_ptr, 
		dep_class curr_dep_class) {
  childptr *tmpptr;
  int curr_gram_el_no = 0;
  dep_tbl_bucket *bucket;
  char *actionptr = NULL;

  tmpptr = nptr->childrenstart;
 
  /* safety check */
  /* XXX TODO */

  if (tmpptr->child->id == action) {
    if (tmpptr->child->childrenstart->child->id == TOPENBRACES) {
      /* we will ignore the first action in a production.
	 That is we will ignore the action in 
	 blabla -> { first_action } foobar ...
      */
      pdebug(DWARNING, "I will ignore the first action in the rule %s as I "
	     "do not consider it good practice to use an action as the "
	     "first thing in a rule without a terminal or non terminal "
	     "preceding it", ntlhs_ptr->symptr->lexemptr);
      
      return curr_gram_el_no;		/* i.e. return 0;  */
    } /* end if */
    else {
      return curr_gram_el_no;
    } 
  } /* end if else */
  else if (tmpptr->child->id == rulebody) {
    curr_gram_el_no = chkRulebody(tmpptr->child, ntlhs_ptr, curr_dep_class);
    curr_gram_el_no++;
    /* move ahead */
    tmpptr = tmpptr->nextptr;

    switch(tmpptr->child->id) {
    case yaccIdentifier:
      /* get action */
      if (tmpptr->nextptr->child->childrenstart->child->id == TOPENBRACES) {
	actionptr = 
	  tmpptr->nextptr->child->childrenstart->nextptr->child->actionptr;
      }
      bucket = chkRulebodyYaccIdentifier(tmpptr->child, ntlhs_ptr, 
					 curr_dep_class, curr_gram_el_no, 
					 actionptr);
      break;

    case CHARCONSTANT:
      if (tmpptr->nextptr->child->childrenstart->child->id == TOPENBRACES) {
	actionptr = 
	  tmpptr->nextptr->child->childrenstart->nextptr->child->actionptr;
      }
      bucket = append_bucket(ntlhs_ptr, tmpptr->child->symptr, curr_dep_class,
			     actionptr);
      break;

    case STRING_LITERAL:
      bailout("chkRulebody: STRING_LITERAL not yet implemented");
      break;

    default:
      bailout("chkRulebody: unkown type of second child");
    } /* end switch */
    
    return curr_gram_el_no;
  } /* end if */
  else
    bailout("chkRulebody: unknown type of child");

  return OK;				/* not neccessary but why not */
}

int chkPrec(node *nptr, dep_tbl_entry *ntlhs_ptr, dep_class class) {

  /* do something? */
  /* at the moment we won't use this information */

  return OK;
}

int chkProduction(node *nptr, dep_tbl_entry *ntlhs_ptr, int curr_dep_class) {
  childptr *tmpptr;
  
  tmpptr = nptr->childrenstart;

  /* safety check */
  if (tmpptr->child == NULL)
    bailout("chkProduction: no child");
  if (tmpptr->child->id != rulebody)
    bailout("chkProduction: unknown type of child");
  if (tmpptr->nextptr == NULL || tmpptr->nextptr->child == NULL)
    bailout("chkProduction: no second child");
  if (tmpptr->nextptr->child->id != prec)
    bailout("chkProduction: unknown type of second child");
  
  chkRulebody(tmpptr->child, ntlhs_ptr, curr_dep_class);
  chkPrec(tmpptr->nextptr->child, ntlhs_ptr, curr_dep_class);

  return OK;
}


int chkProductions(node *nptr, dep_tbl_entry *ntlhs_ptr) {
  childptr *tmpptr;
  int curr_dep_class = 1;

  tmpptr = nptr->childrenstart;

  /* safety check */
  if (tmpptr->child == NULL)
    bailout("chkProductions: no child");
  
  if (tmpptr->child->id == production) {
    chkProduction(tmpptr->child, ntlhs_ptr, 1);
    return 1;			/* first production */
  }
  else if (tmpptr->child->id == productions) {
    if (tmpptr->nextptr == NULL || tmpptr->nextptr->child == NULL)
      bailout("chkProductions: no second child");
    if (tmpptr->nextptr->child->id != TPIPE)
      bailout("chkProductions: unkonwn type of second child");
    if (tmpptr->nextptr->nextptr == NULL || 
	tmpptr->nextptr->nextptr->child == NULL)
      bailout("chkProductions: no third child");
    if (tmpptr->nextptr->nextptr->child->id != production)
      bailout("chkProductions: unknown type of third child");

    curr_dep_class = chkProductions(tmpptr->child, ntlhs_ptr) + 1;
    chkProduction(tmpptr->nextptr->nextptr->child, ntlhs_ptr, curr_dep_class);
    return curr_dep_class;
  }
  else
    bailout("chkProductions: unknown child type");
  return OK;				/* not neccessary but why not */
}


int chkRule(node *nptr) {
  childptr *tmpptr;
  dep_tbl_entry *ntlhs_ptr;

  tmpptr = nptr->childrenstart;
  /* safety check */
  if (tmpptr->child == NULL)
    bailout("chkRule: no child");
  if (tmpptr->child->id != TNTLHS)
    bailout("chkRule: unknown child type");
  if (tmpptr->child->symptr == NULL)
    bailout("chkRule: TNTLHS has no entry in symtab");
  if (tmpptr->nextptr == NULL || tmpptr->nextptr->child == NULL)
    bailout("chkRule: no second child after TNTLHS");
  if (tmpptr->nextptr->child->id != productions)
    bailout("chkRule: unknown type of second child");

  /* insert TNTLHS into dep_tbl and say its a nt if not yet inserted 
   * If it is already inserted then it was either inserted by a "%type"
   * in which case it will hae a semval, otherwise it was inserted by 
   * a previous rule which had the same TNTLHS, in either case, it 
   * does not matter */
  ntlhs_ptr = insert_gram_el(tmpptr->child->symptr, gram_el_type_nt);
  ntlhs_ptr->type = gram_el_type_nt;	/* for safeties sake, lets do it */

  chkProductions(tmpptr->nextptr->child, ntlhs_ptr);

  return OK;
}


int chkRules(node *nptr) {
  childptr *tmpptr;

  tmpptr = nptr->childrenstart;

  /* safety check */
  if (tmpptr->child == NULL) 
    bailout("chkRules: no child");

  if (tmpptr->child->id == rule) {
    /* rules -> rule TSEMICOL */
    if (tmpptr->nextptr == NULL || tmpptr->nextptr->child == NULL)
      bailout("chkRules: No second child after rule");
    if(tmpptr->nextptr->child->id != TSEMICOL)
      bailout("chkRules: invalid second child after rule");

    chkRule(tmpptr->child);

  } 
  /* rules -> rules rule TSEMICOL */
  else if (tmpptr->child->id == rules) {
    if (tmpptr->nextptr == NULL || tmpptr->nextptr->child == NULL) 
      bailout("chkRules: no second child after rules");
    if (tmpptr->nextptr->child->id != rule)
      bailout("chkRules: invalid second child after rules");
    if (tmpptr->nextptr->nextptr == NULL || 
	tmpptr->nextptr->nextptr->child == NULL) 
      bailout("chkRules: no third child after rules");
    if (tmpptr->nextptr->nextptr->child->id != TSEMICOL)
      bailout("chkRules: invalid third child after rules");

    chkRules(tmpptr->child);
    chkRule(tmpptr->nextptr->child);
  }
  else 
    bailout("chkRules: unknown child type");

  return OK;
}


int chkRulesection(node *nptr) {
  childptr *tmpptr;
  
  tmpptr = nptr->childrenstart;

  /* safety check */
  if (tmpptr->child == NULL || tmpptr->child->id != rules) 
    bailout("chkRulesection: unknown or invalid child type");

  chkRules(tmpptr->child);

  return OK;

}

int semanaly(node *nptr) {
	node *tmpnodeptr;

	tmpnodeptr = nptr->childrenstart->child;
	
	chkDefsection(tmpnodeptr);
	chkRulesection(nptr->childrenstart->nextptr->nextptr->child);

	return OK;
}


