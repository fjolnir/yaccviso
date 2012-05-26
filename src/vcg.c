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

/* file:        vcg.c                                                        
 * YACCVISO                                                                  
 * author:      Leon Aaron Kaplan, e9325704@student.tuwien.ac.at 
 * date:        see below                                                    
 * last mod.:   Thu Jan 22 02:41:18 CET 1998
 * descr.:      taken from my "compiler construction" course and modified
 */


/****************************************************************************
 * filename             : vcg.c
 * author               : aka
 * date                 : Wed Jan 15 02:26:41 MET 1997
 * last modified        : 
 * depends on           : 
 * used for             : 
 * produces file        : -
 * -------------------------------------------------------------------------
 * description          : contains routines for creating a GDL (s. VCG) 
 *                        output file which will describe the parse tree
 *                        and can be read by vcg(1) 
 *                        (ftp://ftp.cs.uni-sb.de/pub/graphics/vcg).
 *                        In addition there are routines for output to dot(1)
 *
 * open problems        : 
 *
 ****************************************************************************/ 


#define NO_DOT_BUG 		/* if self edges are wrong with your version
				   of dot(1) then uncomment this define */

/****************************************************************************
 * section        : includes
 * description    :
 *
 *
 ****************************************************************************/ 

#include "vcg.h"
#include "y.tab.h"

/****************************************************************************
 * section        : global vars
 * description    :
 *
 *
 ****************************************************************************/ 

struct idtab {
  int   id;
  const char *str;
};
struct idtab idtab[] = {
  { spec,                               "spec" },
  { defsection,                         "defsection" },
  { bisonDeclars,                       "bisonDeclars" },
  { bisonDeclar,                        "bisonDeclar" },
  { CDeclar,                            "CDeclar" }, 
  { yaccIdentifier,                     "yaccIdentifier" }, 
  { unionDeclar,                        "unionDeclar" },
  { rword,                              "rword" }, 
  { tag,                                "tag" }, 
  { nlist,                              "nlist" }, 
  { nmno,                               "nmno" }, 
  { rulesection,                        "rulesection" }, 
  { rules,                              "rules" }, 
  { rule,                               "rule" }, 
  { productions,                        "productions" }, 
  { production,                         "production" }, 
  { rulebody,                           "rulebody" }, 
  { prec,                               "prec" }, 
  { action,                             "action" }, 
  { tailsection,                        "tailsection" },
  { TNUM,                               "TNUM" },
  { CHARCONSTANT,                       "CHARCONSTANT" },  
  { STRING_LITERAL,                     "STRING_LITERAL" },
  { CCODE,                              "CCODE" },
  { TIDENT,                             "TIDENT" },
  { TNT,                                "TNT" },
  { TNTLHS,                             "TNTLHS" },
  { TLEFT,                              "%left" },
  { TRIGHT,                             "%right"}, 
  { TNONASSOC,                          "%nonassoc"}, 
  { TTOKEN,                             "%token" }, 
  { TPREC,                              "%prec" },
  { TTYPE,                              "%type" },
  { TSTART,                             "%start" },
  { TUNION,                             "%union" },
  { TPURE_PARSER,                       "%pure_parser" },
  { TSEMANTIC_PARSER,                   "%semantic_parser" },
  { TEXPECT,                            "%expect" },
  { TTHONG,                             "%thong" },
  { TMARK,                              "\%\%" },
  { TLCURL,                             "\%{" },
  { TRCURL,                             "\%}" },
  { TSEMICOL,                           "';'" },
  { TPIPE,                              "'|'" },
  { TOPENBRACES,                        "'{'" },
  { TCLOSEBRACES,                       "'}'" },
  { TLT,                                "'<'" },
  { TGT,                                "'>'" },
  { TCOMMA,                             "','" },
  { TEPSILON,                           "TEPSILON" },
  {-1,                                  0x0}
};

int     cnt = 0;
FILE    *fp;

/**************************************************************************n
 * section        : functions
 * description    :
 *
 *
 ****************************************************************************/ 

const char *getstrIdtab(int id) {
  int i;
  
  for (i = 0 ; idtab[i].id != -1; i++) {
    if (idtab[i].id == id) {
      return idtab[i].str;
    }
  } 
  return NULL;
}

char *convString2vcg(char *string) {
  static int buflen;
  static char* buf;
  int i, j;

  if (buflen <= strlen(string)) {
    pdebug(DIO, "convString2vcg: resizing internal buf");
    /* forget old buf. It will be garbage collected later */
    pdebug(DIO, "convString2vcg: old buf len = %d", buflen);
    /* new buflen value + safety room */
    buflen = strlen(string)*6 + 1 + BUFMAX; 
    pdebug(DIO, "convString2vcg: new buf len = %d", buflen);

    if (NULL == (buf = my_malloc((size_t)buflen))) {
      bailout("convString2vcg: could not malloc internal buffer");
    }
    /* init new buffer for safety sake */
    for (i = 0; i < buflen; i++) {
      buf[i] = '\0';
    } /* end for */
  } /* end if */

  for (i = 0, j = 0; i < strlen(string); i++) {
    switch(string[i]) {
    case '\\':
      buf[j++] = '\\';                  /* change \ to \fi092 which means \ */
      buf[j++] = 'f';
      buf[j++] = 'i';
      buf[j++] = '0';
      buf[j++] = '9';
      buf[j++] = '2';
      break;
    case '\n':
      buf[j++] = '\\';                  /* change newline to \n */
      buf[j++] = 'n';      
      break;
    case '"':
      buf[j++] = '\\';                  /* change " to \fi034 which means " */
      buf[j++] = 'f';      
      buf[j++] = 'i';      
      buf[j++] = '0';      
      buf[j++] = '3';      
      buf[j++] = '4';      
      break;
    case '%':
      buf[j++] = '\\';                  /* change % to \fi037 which means % */
      buf[j++] = 'f';      
      buf[j++] = 'i';      
      buf[j++] = '0';      
      buf[j++] = '3';      
      buf[j++] = '7';      
      break;
    default:
      buf[j++] = string[i];             /* copy the other chars of string */
      break;
    } /* end switch */
  } /* end for */
  buf[j] = '\0';                        /* terminate string */
  return buf;
}

int pnode(node *nptr, int id, const char *label) {
  char *tmpptr;

  fprintf(fp, "\tnode: { title: \"%d\" label: \"%s\" ", id, label);
  if (nptr->bLeaf && (nptr->symptr != NULL)) {
    fprintf(fp, "info1: \"%s\" ", nptr->symptr->lexemptr);
  }
  if (nptr->bLeaf && (nptr->actionptr != NULL)) {
    tmpptr = convString2vcg(nptr->actionptr);
    pdebug(DSEMANALY, "pnode: printing action = %s", tmpptr);
    fprintf(fp, "info1: \"%s\" ", tmpptr);
  }
  fprintf(fp, "horizontal_order: %d }\n", id);
  return OK;
}

int pedge (node *nptr, int id, int parentid) {
  
  fprintf(fp, "\tedge: { sourcename: \"%d\" targetname: "
          "\"%d\" color: black thickness: 1 }\n", 
          parentid, id);
  return OK;
}

int pvcg(const char *outfile) {
  childptr *tmpptr;
  
  if (NULL == (fp = fopen(outfile, "w"))) {
    pdebug(DFATAL, "could not create file %s for output of VCG "
           "graph", outfile);
    return ERROR;
  }
  
  fprintf(fp, "graph: {\n");
  fprintf(fp, "\ttitle: \"Parsetree\"\n");

  cnt = 0;

  /* print the root of the tree */
  pnode (tree, 0, "spec");
  
  /* start off at all first level nodes */
  for (tmpptr = tree->childrenstart; tmpptr != NULL; 
       tmpptr = tmpptr->nextptr) {
    vcg(tmpptr->child, 0);
  }
  
  fprintf(fp, "\n\n}\n");
  
  fclose(fp);

  return OK;
}


int vcg(node *nptr, int parentid) {
  childptr *tmpptr;
  int currcnt;

  cnt++;
  currcnt = cnt;
  
  /* sanity checks */
  if (nptr == NULL) {
    pdebug(DFATAL, "oops! passed NULL to vcg routine");
    exit(EXIT_FAILURE);
  }
  
  /* printing of node */
  if (nptr->bLeaf) {
    pnode(nptr, currcnt, getstrIdtab(nptr->id));
  } 
  else {
    pnode(nptr, currcnt, getstrIdtab(nptr->id));
  } 
  
  /* print an edge between the parent and nptr */
  pedge(nptr, currcnt, parentid);
  
  if (!(nptr->bLeaf)) {
    /* print out all children of node */
    for (tmpptr = nptr->childrenstart; tmpptr != NULL; 
         tmpptr = tmpptr->nextptr) {
      vcg(tmpptr->child, currcnt);
    }
  }
  
  return OK;
}


int pdepentry(FILE *fp, dep_tbl_entry *entry_ptr) {
  char *tmp_char_ptr;
  dep_tbl_bucket *tmp_bucket_ptr;

  fprintf(fp, "\tnode: {\ttitle: \"%s\"\n", entry_ptr->symptr->lexemptr);
  fprintf(fp, "\t\tlabel: \"%s\"\n", entry_ptr->symptr->lexemptr);
  fprintf(fp, "\t\t/* number of buckets = %d */\n", entry_ptr->no_of_buckets);
  fprintf(fp, "\t\t/* bUsed = %d */\n", entry_ptr->bUsed);
  fprintf(fp, "\t\t/* type = %d */\n", entry_ptr->type);
  fprintf(fp, "\t}\n");         /* end of node */  

  /* now traversere all buckets of this node and print the edges */
  for (tmp_bucket_ptr = entry_ptr->bucket; tmp_bucket_ptr != NULL;
       tmp_bucket_ptr = tmp_bucket_ptr->next) {
    fprintf(fp, "\tedge: {\tsourcename: \"%s\"\n", 
            entry_ptr->symptr->lexemptr);
    fprintf(fp, "\t\ttargetname: \"%s\"\n", 
            tmp_bucket_ptr->rhsptr->symptr->lexemptr);
    if (tmp_bucket_ptr->actionptr != NULL) {
      tmp_char_ptr = convString2vcg(tmp_bucket_ptr->actionptr);
      fprintf(fp, "\t\tlabel: \"%s\"\n", tmp_char_ptr);
    } /* end if */
    fprintf(fp, "\t\tclass: %d\n", tmp_bucket_ptr->class);
    fprintf(fp, "\t}\n");

  } /* end for */
  return OK;
}


int pdepvcg(const char *outfilename) {
  dep_tbl_entry *dep_tbl_ptr;
  int i;

  if (NULL == (fp = fopen(outfilename, "w"))) {
    pdebug(DFATAL, "could not create file %s for output of VCG graph", 
           outfilename);
    return ERROR;
  }

  cnt = 0;

  fprintf(fp, "graph: {\n");
  fprintf(fp, "\ttitle: \"Dependencies in %s\"\n", inputfilename);
  fprintf(fp, "\tsplines: yes\n");
  fprintf(fp, "\tdisplay_edge_labels: no\n");

  /* traverse all nodes in tbl */
  for (i = 0; i < HASHMAX; i++) {
    if (dep_tbl[i].bUsed == TRUE) {
      pdepentry(fp, &(dep_tbl[i]));

      /* go thru all extensions of hash tbl */
      for (dep_tbl_ptr = dep_tbl[i].extension_next; dep_tbl_ptr != NULL;
           dep_tbl_ptr = dep_tbl_ptr->extension_next) {
        pdepentry(fp, dep_tbl_ptr);
      } /* end for */
    }
    /* if not bUsed continue in loop */
  }
  
  fprintf(fp, "\n\n}\n");
  fclose(fp);

  return OK;
}

/*
 * we create here nodes with labels of the form:
 *
 *    {<f0>Nonterminal|{{<class2>2|{<f1>|<f2>}}|{<class3>4|{<f3>|<f4>}}}}
 *
 * which should build boxes like
 *
 * +-----------------------------------------+
 * |               Nonterminal               |
 * +-------------------+---------------------+
 * |        2          |          3          |
 * +--------+----------+----------+----------+
 * |        |          |          |          |
 * +--------+----------+----------+----------+
 *
 * The  <f*> are used as dot 'ports' in the edge rules.
 *
 * If there are no non-empty rules for a nonterminal, we generate:
 *
 *      {<f0>Nonterminal}
 *
 * which corresponds with a box like
 *
 * +-----------------------------------------+
 * |               Nonterminal               |
 * +-------------------+---------------------+
 *
 */
int pdepdotentry_node(FILE *fp, dep_tbl_entry *entry_ptr) {
  char *nodename;
  dep_tbl_bucket *tmp_bucket_ptr;
  
  int last_class_nr = -1;
  int cur_class_nr = -1;
  int occ_nr = 0;

  nodename = entry_ptr->symptr->lexemptr;
  fprintf(fp, "\t\"%s\" [label=\"{<f0>%s", nodename, nodename);

  /* now traversere all buckets of this node and print the rule boxes */
  for (tmp_bucket_ptr = entry_ptr->bucket; tmp_bucket_ptr != NULL;
       tmp_bucket_ptr = tmp_bucket_ptr->next) {
    cur_class_nr = tmp_bucket_ptr->class;
    occ_nr ++;
    if (cur_class_nr != last_class_nr) {
        /* make a new box */
        /* special case for first one: open the all-rules-box */
        if (last_class_nr == -1) {
                fprintf(fp, "|"); /* seperator between rule name and all-rules-b
ox */
                fprintf(fp, "{"); /* open all-rules-box */
                fprintf(fp, "{"); /* open rule box */
        } else {
                fprintf(fp, "}"); /* close occ box */
                fprintf(fp, "}"); /* close rule box */
                fprintf(fp, "|"); /* rule box separator*/
                fprintf(fp, "{"); /* open rule box */
        }
        last_class_nr = cur_class_nr;
        fprintf(fp, "<class%d>%d", cur_class_nr, cur_class_nr);
        fprintf(fp, "|{"); /* open occ box */
    } else {
        fprintf(fp, "|"); /* occ box separator*/
    }
    fprintf(fp, "<f%d>", occ_nr); /* occ box entry*/
  } /* end for */
  
  /* close all 'sub' boxes that we opened */
  if (last_class_nr != -1) {
    fprintf(fp, "}"); /* close last occ box */
    fprintf(fp, "}"); /* close last rule box */
    fprintf(fp, "}"); /* close all-rules-box */
  }
  fprintf(fp, "}\""); /* close label */
  
  /* specify 'record' shape. We need this for the :f* ports to work */
  fprintf(fp, ",shape=record");
  
  /* specify the color if it is not a non terminal symbol */
  if (entry_ptr->type != gram_el_type_nt) {
    fprintf(fp, ",color=gold,style=filled");
  }

  /* other local options can be inserted here */

  /* close the node declaratin */
  fprintf(fp, "];\n");
  /* end of node */  
  return OK;
}


int pdepdotentry_edges(FILE *fp, dep_tbl_entry *entry_ptr) {
  char *nodename;
  char *targetname;
  dep_tbl_bucket *tmp_bucket_ptr;
  
  int cur_class_nr = -1;
  int occ_nr = 0;

  nodename = entry_ptr->symptr->lexemptr;

  /* now traversere all buckets of this node and print the edges */
  for (tmp_bucket_ptr = entry_ptr->bucket; tmp_bucket_ptr != NULL;
       tmp_bucket_ptr = tmp_bucket_ptr->next) {
    cur_class_nr = tmp_bucket_ptr->class;
    occ_nr ++;
    targetname = tmp_bucket_ptr->rhsptr->symptr->lexemptr;
#ifndef NO_DOT_BUG
    /* If this bug is present, self loops are reversed!
     * Our dot version shows this :-(
     * <Axel.Belinfante@cs.utwente.nl> Fri Jan 30 16:11:32 MET 1998
     */
    if (strcmp(nodename, targetname) == 0) {
      fprintf(fp, "\t\"%s\":f0 -> \"%s\":f%d;\n", nodename, targetname, occ_nr);
    } else {
      fprintf(fp, "\t\"%s\":f%d -> \"%s\":f0;\n", nodename, occ_nr, targetname);
    }
#else
    fprintf(fp, "\t\"%s\":f%d -> \"%s\":f0;\n", nodename, occ_nr, targetname);
#endif
  } /* end for */
  return OK;
}

int pdepdotentry_simple(FILE *fp, dep_tbl_entry *entry_ptr) {
  char *nodename;
  char *targetname;
  dep_tbl_bucket *tmp_bucket_ptr;
  
  int last_class_nr = -1;
  int cur_class_nr = -1;

  nodename = entry_ptr->symptr->lexemptr;
  fprintf(fp, "\t\"%s\";\n", nodename);
  /* end of node */  

  /* now traversere all buckets of this node and print the edges */
  for (tmp_bucket_ptr = entry_ptr->bucket; tmp_bucket_ptr != NULL;
       tmp_bucket_ptr = tmp_bucket_ptr->next) {
    cur_class_nr = tmp_bucket_ptr->class;
    if (cur_class_nr != last_class_nr) {
        /* make a new production rule node, and
         * make a link from the nonterminal node to the production rule node
         */
        last_class_nr = cur_class_nr;
        fprintf(fp, "\t\"%s rule %d\" [shape=record,color=green];\n", nodename, 
		cur_class_nr);
        fprintf(fp, "\t\"%s\" -> \"%s rule %d\" [color=green];\n", nodename, 
		nodename, cur_class_nr);
    }
    targetname = tmp_bucket_ptr->rhsptr->symptr->lexemptr;
    fprintf(fp, "\t\"%s rule %d\" -> \"%s\";\n", nodename, cur_class_nr, 
		targetname);

  } /* end for */
  return OK;
}


int pdepdot(const char *outfilename) {
  dep_tbl_entry *dep_tbl_ptr;
  int i;
  
#ifdef USE_THE_VERY_SIMPLE_ONE
  int (*function_array[])(FILE *fp, dep_tbl_entry *entry_ptr) = {
        pdepdotentry_simple,
        0
  };
#else
  int (*function_array[])(FILE *fp, dep_tbl_entry *entry_ptr) = {
        pdepdotentry_node,
        pdepdotentry_edges,
        0
  };
#endif
  int j;

  if (NULL == (fp = fopen(outfilename, "w"))) {
    pdebug(DFATAL, "could not create file %s for output of dot(1) graph", 
           outfilename);
    return ERROR;
  }

  cnt = 0;

  fprintf(fp, "digraph \"Dependencies in Yacc grammar\" {\n");
  fprintf(fp, "\tfontsize = 14;\n");
  fprintf(fp, "\tpage = \"8,11\";\n");
  fprintf(fp, "\tratio = auto;\n");
 /* other global options can be inserted here */

  /* do the thing for all functions that we have */
  for (j = 0; function_array[j] != 0; j++) {
    /* traverse all nodes in tbl */
    for (i = 0; i < HASHMAX; i++) {
      if (dep_tbl[i].bUsed == TRUE) {
        (*function_array[j])(fp, &(dep_tbl[i]));

        /* go thru all extensions of hash tbl */
        for (dep_tbl_ptr = dep_tbl[i].extension_next; dep_tbl_ptr != NULL;
                 dep_tbl_ptr = dep_tbl_ptr->extension_next) {
              (*function_array[j])(fp, dep_tbl_ptr);
        }       /* end for */
      }
      /* if not bUsed continue in loop */
    }
  }
  
  fprintf(fp, "\n\n}\n");
  fclose(fp);


  return OK;
}


