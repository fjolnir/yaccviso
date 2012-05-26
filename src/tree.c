/****************************************************************************
 * filename             : tree.c
 * author               : aka
 * date                 : Sun Jan 12 14:01:34 MET 1997
 * last modified        : Sun Jan 12 14:01:34 MET 1997
 * depends on           : global.h, tree.h
 * used for             :
 * produces file        : tree.o
 * -------------------------------------------------------------------------
 * description          : provides functions for creating nodes, etc. for
 *                        a parse tree
 *
 *
 * open problems        : chk ptree - attribute list
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

#include "tree.h"
#include "y.tab.h"
#include <stdarg.h>
#include <assert.h>



/****************************************************************************
 * section        : global vars
 * description    :
 *
 *
 ****************************************************************************/ 

node		*tree;


/**************************************************************************n
 * section        : functions
 * description    :
 *
 *
 ****************************************************************************/ 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : inittree
 * description    : initializes the parse tree
 * returns        : OK, ERROR
 * input          : void
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int 	inittree(void) {

  /* at the moment this does nothing, l8ter it might become useful */
  tree = NULL;
  
  return OK;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : mkleaf
 * description    : allocates a new leaf in the parsetree
 * returns        : ptr to new leaf 
 * input          : token number, ptr to symbol table where the leaf is
 *                  described in detail
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
node	*mkleaf(int tokenid, symentry *sym, char *actionptr) {
  node	*tmpptr = NULL;
	
  /* malloc new leaf node */
  tmpptr = my_malloc(sizeof(node));
  if (tmpptr != NULL) {
    tmpptr->attriblst = NULL;
    tmpptr->attribend = NULL;
    tmpptr->bLeaf = TRUE;
    tmpptr->id = tokenid;
    tmpptr->symptr = sym;	/* simple tokens not in the symtab
				   will get a symptr of NULL
				   per convention! 
				*/
    tmpptr->actionptr = actionptr;
    tmpptr->childrenstart = NULL;
    tmpptr->childrenend = NULL;
    pdebug(DSEMANALY, "mkleaf: tokenid = %d, action = %s", tokenid, actionptr);
  }
  else {
    pdebug(DFATAL, "could not malloc a new leaf of the parse "
	   "tree");
    exit(EXIT_FAILURE);
  }
  
  return tmpptr;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : mknode
 * description    : makes an inner node oof the parse tree
 * returns        : ptr to new node or NULL
 * input          : id of nonterminal symbol which belongs to the node,
 *                  
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
node	*mknode(int ntid) {
  node	*tmpptr;
  
  tmpptr = my_malloc(sizeof(node));
  if (tmpptr != NULL) {
    tmpptr->id = ntid;
    tmpptr->attriblst = NULL;
    tmpptr->attribend = NULL;
    tmpptr->bLeaf = FALSE;
    tmpptr->symptr = NULL;
    tmpptr->actionptr = NULL;
    tmpptr->childrenstart = NULL;
    tmpptr->childrenend = NULL;
  }
  return tmpptr;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : appendchld
 * description    : appends a ptr to the linked list of children of the node
 * returns        : OK or ERROR
 * input          : parent node, ptr to child (which is either a node or a 
 *                  leaf)
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int	appendchld(node *nodeptr, node *chld) {
  childptr	*tmpptr = NULL;

  /* sanity checks */
  if (nodeptr == NULL || chld == NULL) {
    pdebug(DFATAL, "oops! nodeptr or chld == NULL in appendchld");
    exit(EXIT_FAILURE);
  }
  if (nodeptr->bLeaf) {
    pdebug(DFATAL, "oops! trying to append children to leaf of "
	   "parse tree");
    exit(EXIT_FAILURE);
  }
  
  /* allocate space for new item in linked list of childrenptrs */
  tmpptr = my_malloc(sizeof(childptr));
  if (tmpptr == NULL) {
    return ERROR;
  }
  /* initialize tmpptr */
  tmpptr->child = chld;
  tmpptr->nextptr = NULL;
  
  if (nodeptr->childrenstart == NULL) {
    /* child to be appended will be the first entry in linked list
       of children of node 
    */
    nodeptr->childrenstart = tmpptr;
    nodeptr->childrenend = tmpptr;
    
  }
  else {
    /* append child to the end of linked list of children */
    nodeptr->childrenend->nextptr = tmpptr;
    nodeptr->childrenend = tmpptr;
  }	
  return OK;
}	


/* XXX */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : mkattrib
 * description    : makes an attribute data structure (s. tree.h)
 * returns        : ptr to attrib
 * input          : id, type, [int, char, char*, float]
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
attrib *mkattrib(int id, int type, ...) {
  va_list ap;
  int	i;
  char	c;
  char	*str;
  float	f;
  void	*vptr;
  attrib *tmpptr = NULL;	
  
  va_start(ap, type);
  switch (type) {
  case aint:
    i = va_arg(ap, int);
    break;
  case achar:
    c = va_arg(ap, char);
    break;
  case astr:
    str = va_arg(ap, char*);
    break;
  case afloat:
    f = va_arg(ap, float);
    break;
  case avoidptr:
    vptr = va_arg(ap, void*);
    break;
  default:
    pdebug(DFATAL, "oops! mkattrib: unknown type");
    exit(EXIT_FAILURE);
  }
  tmpptr = my_malloc(sizeof(attrib));
  /* sanity check */
  if (tmpptr == NULL) {
    pdebug(DFATAL, "could not malloc attribute");
    exit(EXIT_FAILURE);
  }
  tmpptr->id = id;
  tmpptr->type = type;
  tmpptr->nextptr = NULL;
  switch (type) {
  case aint:
    tmpptr->val.i = i;
    break;
  case achar:
    tmpptr->val.c = c;
    break;
  case astr:
    tmpptr->val.str = str;
    break;
  case afloat:
    tmpptr->val.f = f;
    break;
  case avoidptr:
    tmpptr->val.vptr = vptr;
    break;
  default:
    pdebug(DFATAL, "oops! mkattrib: unknown type");
    exit(EXIT_FAILURE);
  }
  
  va_end(ap);
  return tmpptr;
  
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : appendattr
 * description    : appends an attribute to a list of attributes of a node
 * returns        : OK, ERROR
 * input          : ptr to node, ptr to attribute data structure
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int appendattr(node *nodeptr, attrib *aptr) {
	
  if (nodeptr->attriblst == NULL) {
    nodeptr->attriblst = aptr;
    nodeptr->attribend = aptr;
  }
  else {
    nodeptr->attribend->nextptr = aptr;
    nodeptr->attribend = aptr;
  }
  
  return OK;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : ptree
 * description    : traverses the parse tree in depth first, in order 
 *                  (Stichwort: Front des Baumes) and prints out the 
 *                  information iff SEMANALY is part of the debuglvl 
 * returns        : OK, ERROR
 * input          : node pointer
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int ptree(node *nodeptr) {
  int numchildren = 0;
  childptr *tmpptr = NULL;
  attrib   *tmpptr2 = NULL;
  
  if (nodeptr != NULL) {
    /* check if node is a leaf */
    if (nodeptr->bLeaf) {
      pdebug(DSEMANALY, "leaf:");
      pdebug(DSEMANALY, "\ttoken id = %d", nodeptr->id);
      pdebug(DSEMANALY, "\tname of token = ");
      switch (nodeptr->id) {
      case TNUM:
	pdebug(DSEMANALY, "\t\tTNUM");
	break;
      case CHARCONSTANT:
	pdebug(DSEMANALY, "\t\tCHARCONSTANT");
	break;
      case STRING_LITERAL:
	pdebug(DSEMANALY, "\t\tSTRING_LITERAL");
	break;
      case CCODE:
	pdebug(DSEMANALY, "\t\tCCODE");
	break;
      case TIDENT:
	pdebug(DSEMANALY, "\t\tTIDENT");
	break;
      case TNT:
	pdebug(DSEMANALY, "\t\tTNT");
	break;
      case TNTLHS:
	pdebug(DSEMANALY, "\t\tTNTLHS");
	break;
      case TLEFT:
	pdebug(DSEMANALY, "\t\tTLEFT");
	break;
      case TRIGHT:
	pdebug(DSEMANALY, "\t\tTRIGHT");
	break;
      case TNONASSOC:
	pdebug(DSEMANALY, "\t\tTNONASSOC");
	break;
      case TSEMICOL:
	pdebug(DSEMANALY, "\t\tTSEMICOL");
	break;
      case TCOMMA:
	pdebug(DSEMANALY, "\t\tTCOMMA");
	break;
      case TTOKEN:
	pdebug(DSEMANALY, "\t\tTTOKEN");
	break;
      case TPREC:
	pdebug(DSEMANALY, "\t\tTPREC");
	break;
      case TEPSILON:
	pdebug(DSEMANALY, "\t\tTEPSILON");
	break;
      case TTYPE:
	pdebug(DSEMANALY, "\t\tTTYPE");
	break;
      case TSTART:
	pdebug(DSEMANALY, "\t\tTSTART");
	break;
      case TUNION:
	pdebug(DSEMANALY, "\t\tTUNION");
	break;
      case TPURE_PARSER:
	pdebug(DSEMANALY, "\t\tTPURE_PARSER");
	break;
      case TSEMANTIC_PARSER:
	pdebug(DSEMANALY, "\t\tTSEMANTIC_PARSER");
	break;
      case TEXPECT:
	pdebug(DSEMANALY, "\t\tTEXPECT");
	break;
      case TTHONG:
	pdebug(DSEMANALY, "\t\tTTHONG");
	break;
      case TMARK:
	pdebug(DSEMANALY, "\t\tTMARK");
	break;
      case TLCURL:
	pdebug(DSEMANALY, "\t\tTLCURL");
	break;
      case TRCURL:
	pdebug(DSEMANALY, "\t\tTRCURL");
	break;
      case TPIPE:
	pdebug(DSEMANALY, "\t\tTPIPE");
	break;
      case TOPENBRACES:
	pdebug(DSEMANALY, "\t\tTOPENBRACES");
	break;
      case TCLOSEBRACES:
	pdebug(DSEMANALY, "\t\tTCLOSEBRACES");
	break;
      case TLT:
	pdebug(DSEMANALY, "\t\tTLT");
	break;
      case TGT:
	pdebug(DSEMANALY, "\t\tTGT");
	break;
      default:
	pdebug(DFATAL, "oops! unknown token id in leaf in ptree");
	exit(EXIT_FAILURE);
      } /* end switch */
      
      if (nodeptr->symptr == NULL) {
	pdebug(DSEMANALY, "\tno symtabentry");
      }
      else {
	pdebug(DSEMANALY, "\tsymtabentry:");
	pdebug(DSEMANALY, "\t\tid = %d", nodeptr->symptr->id);
	pdebug(DSEMANALY, "\t\tdeclared = %d", nodeptr->symptr->declared);
	pdebug(DSEMANALY, "\t\ttype = %d", nodeptr->symptr->type);
	pdebug(DSEMANALY, "\t\tlexem = %s", nodeptr->symptr->lexemptr);
      } /* end else if symentry == NULL */
      if (nodeptr->actionptr == NULL) {
	pdebug(DSEMANALY, "\tno action");
      }
      else {
	pdebug(DSEMANALY, "\taction:");
	pdebug(DSEMANALY, nodeptr->actionptr);
      }
      
      pdebug(DSEMANALY, "");
    } /* end if node is a leaf */
    else { /* else it must be an inner node */
      /* first calculate the number of children */
      numchildren = 0;
      for (tmpptr = nodeptr->childrenstart; tmpptr != NULL; 
	   tmpptr = tmpptr->nextptr) {
				/* recursively descend tree */
	ptree(tmpptr->child);
	numchildren++;
      } /* end for */
      pdebug(DSEMANALY, "inner node:");
      pdebug(DSEMANALY, "\tnumber of children = %d",
	     numchildren);
      pdebug(DSEMANALY, "\tid for nonterminal = %d",
	     nodeptr->id);
      pdebug(DSEMANALY, "");
    } /*end else it must be an inner node */
    /* print the attribute list */
    pdebug(DSEMANALY, "\tattribute list:");
    for (tmpptr2 = nodeptr->attriblst; tmpptr2 != NULL;
	 tmpptr2 = tmpptr2->nextptr) {
      pdebug(DSEMANALY, "\t\tid = %d",tmpptr2->id);
      switch(tmpptr2->type) {
      case aint:
	pdebug(DSEMANALY,"int = %d",
	       tmpptr2->val.i);
	break;
      case achar:
	pdebug(DSEMANALY,"char = %c",
	       tmpptr2->val.c);
	break;
      case astr:
	pdebug(DSEMANALY,"str = %s",
	       tmpptr2->val.str);
	break;
      case afloat:
	pdebug(DSEMANALY,"float = %f",
	       tmpptr2->val.f);
	break;
      default:
	pdebug(DFATAL, "oops! attrib type"
	       "wrong");
	exit(EXIT_FAILURE);
      }	/* end switch */
    }
    pdebug(DSEMANALY, "");
  }
  return OK;
}
