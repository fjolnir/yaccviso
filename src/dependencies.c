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

/* file:        dependencies.c
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        1997.01.04                                                   
 * last mod.:                                                                
 * descr.:      hash table stuff                                 
 */

#include "symtab.h"
#include "dependencies.h"

dep_tbl_entry dep_tbl[HASHMAX - 1];
int last_ntlhs_key = 0;
dep_tbl_entry *gram_start = NULL;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : init_dep_tbl
 * description    : initializes the dep_tbl
 *                  
 * returns        : ERROR or OK
 * input          : void
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int init_dep_tbl(void) {
  int i;

  for (i = 0; i < HASHMAX; i++) {
    dep_tbl[i].id = 0;
    dep_tbl[i].bUsed = FALSE;
    dep_tbl[i].type = gram_el_type_none;
    dep_tbl[i].semvalspec = NULL;
    dep_tbl[i].no_of_buckets = 0;
    dep_tbl[i].bucket = NULL;
    dep_tbl[i].no_of_rbuckets = 0;
    dep_tbl[i].rbucket = NULL;
    dep_tbl[i].extension_next = NULL;
  } /* end for */

  return OK;
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : hashfunc
 * description    : hashfunc over strings
 *                  
 * returns        : index in tbl. (unsigned int)
 * input          : string
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
unsigned int hashfunc(char *name) {
  unsigned long i; 
  unsigned long index = 0;
  unsigned long retval = 0;

  for (i = 0; i <= strlen(name); i++) {
    index += ((unsigned long)name[i])*i;
  } /* end for */
  retval = index % HASHMAX;

  pdebug(DSEMANALY, "hashfunc: name %s is at index %lu", name, retval);

  return retval;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : insert_gram_el
 * description    : inserts a gram_el into the dep_tbl
 *                  
 * returns        : NULL on failure, ptr to new entry on success
 * input          : symptr of the symbol, type of the symbol (gram_el_type)
 * 
 *
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
dep_tbl_entry* insert_gram_el(symentry *symptr, gram_el_type type) {
  dep_tbl_entry *new_entry, *tmpptr;
  unsigned int index;

  /* safety check */
  if (NULL == symptr) {
    bailout("insert_gram_el: symptr == NULL");
  }
  if (NULL == symptr->lexemptr) {
    bailout("insert_gram_el: symptr->lexemptr == NULL");
  }

  /* malloc new stuff */
  if (NULL == (new_entry = my_malloc(sizeof(dep_tbl_entry)))) {
    bailout("could not malloc new entry in dep_tbl");
  } 

  index = hashfunc(symptr->lexemptr);

  last_ntlhs_key++;			/* unused at the moment */
  new_entry->id = last_ntlhs_key;	/* we wont need it, but ok */
  new_entry->bUsed = TRUE;
  new_entry->symptr = symptr;
  new_entry->type = type;
  new_entry->semvalspec = NULL;
  new_entry->no_of_buckets = 0;
  new_entry->bucket = NULL;
  new_entry->no_of_rbuckets = 0;
  new_entry->rbucket = NULL;
  new_entry->extension_next = NULL;

  if (dep_tbl[index].bUsed) {		
    /* entry alread used -> possibly we need an hash tbl ext. */
    /* => check all existing extensions, if none of them matches the */
    /* element wich we want to insert, then append new_entry at end of */
    /* extension linked list, otherwise return first match */

    /* first: check if dep_tbl[index] == element we want to insert */
    if (dep_tbl[index].symptr == symptr) {
	pdebug(DSYMTAB, "insert_gram_el: warning - entry already "
	       "exists: %s", symptr->lexemptr);
	/* forget new entry. It will be garbage collected later */
	return &(dep_tbl[index]);	/* entry already existed */
    }
    for (tmpptr = &(dep_tbl[index]); tmpptr->extension_next != NULL; 
	 tmpptr = tmpptr->extension_next) {
      if (tmpptr->symptr == symptr) {
	pdebug(DSYMTAB, "insert_gram_el: warning - entry already "
	       "exists: %s", symptr->lexemptr);
	/* forget new entry. It will be garbage collected later */
	return tmpptr;			/* entry already existed */
      }	/* end if */
    } /* end for */
    /* note: we are not finished with searching yet: it could be the last
       thing in the extension list */
    if (tmpptr->symptr == symptr) {
      /* forget new entry. It will be garbage collected later */
      return tmpptr;
    }
    /* when we reached this point then we havent found the entry */
    tmpptr->extension_next = new_entry; /* append new entry */
    return new_entry;
  }
  else {
    dep_tbl[index].id = new_entry->id;
    dep_tbl[index].bUsed = new_entry->bUsed;
    dep_tbl[index].symptr = new_entry->symptr;
    dep_tbl[index].semvalspec = new_entry->semvalspec;
    dep_tbl[index].type = new_entry->type;
    dep_tbl[index].no_of_buckets = new_entry->no_of_buckets;
    dep_tbl[index].bucket = new_entry->bucket;
    dep_tbl[index].no_of_rbuckets = new_entry->no_of_rbuckets;
    dep_tbl[index].rbucket = new_entry->rbucket;
    dep_tbl[index].extension_next = new_entry->extension_next;
    /* forget new entry. It will be garbage collected later */
    return &(dep_tbl[index]);
  } /* end if - else possible hash tbl extension*/
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 * function       : append_bucket
 * description    : appends a bucket to a dep_tbl_entry
 *                  
 * returns        : ptr to new bucket on success, NULL on failure
 * input          : dep_tbl_entry* where the bucket should be appended,
 *                  symptr of the object that the bucket should hold,
 *                  char* of action after the gram_el 
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
dep_tbl_bucket* append_bucket(dep_tbl_entry *where, symentry *what_symptr, 
			      dep_class what_dep_class, char *action) {
  dep_tbl_bucket *new_bucket, *tmpiterator;
  dep_tbl_rbucket *new_rbucket, *tmpriterator;
  dep_tbl_entry *tmpptr, *target;
  unsigned int target_index;
  int no;
  int bFound = FALSE;

  /* safety checks */
  if (where == NULL) {				
    bailout("oops, append_bucket: where == NULL");
  }
  if (what_symptr == NULL) {
    bailout("oops, append_bucket: what_symptr == NULL");
  }
  if (what_symptr->lexemptr == NULL) {
    bailout("oops, append_bucket: what_symptr->lexemptr == NULL");
  }
  

  no = where->no_of_buckets + 1;	/* we start counting at 1 like yacc */
  
  /* malloc new stuff */
  if (NULL == (new_bucket = my_malloc(sizeof(dep_tbl_bucket)))) {
    bailout("append_bucket: could not malloc new_bucket");
  }
  if (NULL == (new_rbucket = my_malloc(sizeof(dep_tbl_rbucket)))) {
    bailout("append_bucket: could not malloc new_rbucket");
  }
  
  pdebug(DSEMANALY, "append_bucket: appending %s to %s", 
	 what_symptr->lexemptr, where->symptr->lexemptr);

  /* init bucket */
  new_bucket->id = 0;			/* unused at the moment */
  new_bucket->class = what_dep_class;
  new_bucket->no = no;
  new_bucket->actionptr = action;
  new_bucket->next = NULL;
  new_bucket->prev = NULL;
  
  /* init rbucket */
  new_rbucket->id = 0;			/* unused at the moment */
  new_rbucket->lhsptr = where;
  new_rbucket->next = NULL;
  new_rbucket->prev = NULL;
  

  /* the following lines establish the link between the target of the bucket
     and the bucket
  */
  target_index = hashfunc(what_symptr->lexemptr);
  target = &(dep_tbl[target_index]);

  /* if target exists, link it in */
  if (target->bUsed) {
    if (target->symptr == what_symptr) {
      /* yes, link it in */
      new_bucket->rhsptr = target;
      bFound = TRUE;
    }
    else {				
      /* we did not find it immediately... */
      /* it might exists in the hash tbl extension linked list */
      for (tmpptr = target; tmpptr != NULL; tmpptr = tmpptr->extension_next) {
	if (tmpptr->symptr == what_symptr) {
	  /* yes, link it in */
	  target = tmpptr;
	  new_bucket->rhsptr = target;
	  bFound = TRUE;
	} /*  end if */
      } /* end for */
    } /* end else */
  }

  /* if we did not find it then forget it all */
  if (!bFound) {
    return NULL;
  } /* end if !bFound */

  /* this is the point of no return, things must be ok from now on */


  /* the following lines establish a link between the rbucket list of 
     the target and the new_rbucket
  */
  if (target->rbucket == NULL) {
    target->rbucket = new_rbucket;
  }
  else {
    for (tmpriterator = target->rbucket; tmpriterator->next != NULL;
	 tmpriterator = tmpriterator->next) {
      ;
    }
    tmpriterator->next = new_rbucket;
    new_rbucket->prev = tmpriterator;
  } /* end else */
  target->no_of_rbuckets++;
  /* end rbucket stuff */

  /* append the bucket to the linked list of buckets in where */
  /* case one: no buckets exists for where */
  if (where->bucket == NULL) {
    where->bucket = new_bucket;
  }
  else {				
    /* iterate thru list of buckets till the end */
    for (tmpiterator = where->bucket; tmpiterator->next != NULL;
	 tmpiterator = tmpiterator->next) {
      ;				/* NOP */
    }
    tmpiterator->next = new_bucket;
    new_bucket->prev = tmpiterator;
  } /*  end else */
  where->no_of_buckets++;

  return new_bucket;
}


dep_tbl_entry *lookup_gram_el(symentry *symptr) {
  unsigned int index;
  dep_tbl_entry *target, *tmpptr;

  index = hashfunc(symptr->lexemptr);
  target = &(dep_tbl[index]);

  /* if target exists, then we found it immediately */
  if (target->bUsed) {
    if (target->symptr == symptr) {
      /* yes, it exists immediately */
      return target;
    }
    else {				
      /* we did not find it immediately... */
      /* it might exists in the hash tbl extension linked list */
      for (tmpptr = target; tmpptr != NULL; tmpptr = tmpptr->extension_next) {
	if (tmpptr->symptr == symptr) {
	  /* yes, we found it in the extension list */
	  target = tmpptr;
	  return target;
	} /*  end if */
      } /* end for */
    } /* end else */
  }

  /* we did not find it */
  return NULL;
  
}
       
   


int pdeptbl(void) {
  int i;
  dep_tbl_entry *tmpentry;
  dep_tbl_bucket *tmpbucket;

  
  for (i = 0; i < HASHMAX; i++) {
    tmpentry = &(dep_tbl[i]);
    if (tmpentry->bUsed) {
      do {
	pdebug(DSEMANALY, "pdeptbl: entry @index i=%d : ", i);
	pdebug(DSEMANALY, "\taddress = 0x%x", tmpentry);
	pdebug(DSEMANALY, "\tid = %d, bUsed = %d", tmpentry->id,tmpentry->bUsed);
	pdebug(DSEMANALY, "\tsymptr = 0x%x", tmpentry->symptr);
	pdebug(DSEMANALY, "\tsymptr->lexemptr = %s", tmpentry->symptr->lexemptr);
	pdebug(DSEMANALY, "\ttype = ");
	switch (tmpentry->type) {
	case gram_el_type_token:
	  pdebug(DSEMANALY, "\t\tgram_el_type_token");
	  break;
	case gram_el_type_nt:
	  pdebug(DSEMANALY, "\t\tgram_el_type_nt");
	  break;
	case gram_el_type_none:
	  pdebug(DSEMANALY, "\t\tgram_el_type_none");
	  break;
	case gram_el_type_unknown:
	  pdebug(DSEMANALY, "\t\tgram_el_type_unknown");
	  break;
	default:
	  pdebug(DFATAL, "pdeptbl: oops!!! unknown type field");
	  exit(EXIT_FAILURE);
	} /* end switch */
	pdebug(DSEMANALY, "");
	pdebug(DSEMANALY, "\tsemvalspec = 0x%x", tmpentry->semvalspec);
	if (tmpentry->semvalspec != NULL && tmpentry->semvalspec->lexemptr != NULL)
	  pdebug(DSEMANALY, "\t*semvalspec = %s", 
		 tmpentry->semvalspec->lexemptr);

	/* bucket traversal */
	pdebug(DSEMANALY, "+\tno_of_buckets = %d", tmpentry->no_of_buckets);
	pdebug(DSEMANALY, "+\tbucketlist = ");

	for (tmpbucket = tmpentry->bucket; tmpbucket != NULL; 
	     tmpbucket = tmpbucket->next) {
	  pdebug(DSEMANALY, "+\t\tid = %d", tmpbucket->id);
	  pdebug(DSEMANALY, "+\t\tclass = %d", tmpbucket->class);
	  pdebug(DSEMANALY, "+\t\trhs = %s",tmpbucket->rhsptr->symptr->lexemptr);
	  pdebug(DSEMANALY, "+\t\tno = %d", tmpbucket->no);
	  pdebug(DSEMANALY, "+\t\taction = %s", tmpbucket->actionptr);
	} /* end for bucket */

	/* rbucket traversal */
	/* not doen at the moment */

	/* extension stuff */
	pdebug(DSEMANALY, "\textension_next = 0x%x", tmpentry->extension_next);
      } while  (NULL != (tmpentry = tmpentry->extension_next));
    } /* end if bUsed */
  } /* end for */
  return OK;
}

