# YaccViso - a tool for visualizing dependencies of nonterminals in a yacc  */
#            grammar file                                                   */
# Copyright (C) 1997  Leon Aaron Kaplan, email: e9325704@stud1.tuwien.ac.at */
# This program is free software; you can redistribute it and/or modify      */
# it under the terms of the GNU General Public License as published by      */
# the Free Software Foundation; either version 2 of the License, or         */
# (at your option) any later version.                                       */
#                                                                           */
# This program is distributed in the hope that it will be useful,           */
# but WITHOUT ANY WARRANTY; without even the implied warranty of            */
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
# GNU General Public License for more details.                              */
#                                                                           */
# You should have received a copy of the GNU General Public License         */
# along with this program; if not, write to the Free Software               */
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 */
# ----------------------------------------------- end of legal stuff ------ */

# file:		Makefile
# YACCVISO
# author:	Leon Aaron Kaplan
# date:		14.10.1997
# last mod.:	
# descr.:	Makefile
#

CC=		gcc
LIBS=
OBJS=		yaccviso
CCOPTS=		-ansi -g -Wall

YACC=		bison
YACCOPT=	--output-file=y.tab.c --defines --verbose
#YACC=		byacc
#YACCOPT=	-dv
#YACC=		yacc
#YACCOPT=	-d

# The following lines only work with GNU make. Remove them and adapt them
# if you dont have GNU make.
ifeq ($(YACC),yacc)
 YACCLIB=-ly
else
 YACCLIB=
endif

# The following lines only make sure that on my computer the right flex is 
# used. Note that you must have flex 2.5.4 or greater.
#LEX=		lex
#LEXOPT=	
LEX=		flex
#LEX=		flex
LEXOPT=		-L -l

# The following lines only work with GNU make. Remove them and adapt them
# if you dont have GNU make.
ifeq ($(LEX),flex)
 LEXLIB=-lfl
else
 LEXLIB=-ll
endif

LIBS+=	$(YACCLIB) $(LEXLIB) 
# This will be used in future releases. Ignore at the moment
#LIBS+= -L/usr/X11R6/lib -L/usr/local/lib -lXext -lX11 -lgtk -lgdk -lglib -lm

# general Unix tools
RM=		rm -rf
CP=		cp -a

# ----------- Start rules
all:		$(OBJS)

yaccviso:	Makefile lex.yy.c y.tab.c globalvars.o error.o main.o \
		tree.o symtab.o dependencies.o semanaly.o vcg.o malloc.o 
	$(CC) $(CCOPTS) -o yaccviso  main.o globalvars.o error.o  \
		tree.o symtab.o dependencies.o semanaly.o vcg.o malloc.o \
		y.tab.c lex.yy.c $(LIBS)

lex.yy.c:	Makefile scanner.l global.h scanner.h
	$(LEX) $(LEXOPT) scanner.l

y.tab.c:	Makefile yaccviso.y global.h
	$(YACC) $(YACCOPT) yaccviso.y

globalvars.o:	globalvars.c global.h Makefile
	$(CC) $(CCOPTS) -o globalvars.o -c globalvars.c

malloc.o:	malloc.c malloc.h Makefile
	$(CC) $(CCOPTS) -o malloc.o -c malloc.c

main.o:		main.c global.h Makefile
	$(CC) $(CCOPTS) -o main.o -c main.c

error.o:	error.c error.h global.h Makefile
	$(CC) $(CCOPTS) -o error.o -c error.c

symtab.o:	symtab.c global.h symtab.h Makefile
	$(CC) $(CCOPTS) -o symtab.o -c symtab.c

semanaly.o:	semanaly.c semanaly.h Makefile
	$(CC) $(CCOPTS) -o semanaly.o -c semanaly.c

dependencies.o:	dependencies.c global.h dependencies.h Makefile
	$(CC) $(CCOPTS) -o dependencies.o -c dependencies.c

tree.o:		tree.c tree.h global.h Makefile
	$(CC) $(CCOPTS) -o tree.o -c tree.c

# ui.o is not used at the moment, but it will be in future releases.
# Ignore at the moment.
ui.o:		ui.c ui.h
	$(CC) $(CCOPTS) -o ui.o -c ui.c

vcg.o:		vcg.c global.h vcg.h Makefile
	$(CC) $(CCOPTS) -o vcg.o -c vcg.c

clean:
	$(RM) $(OBJS)
	$(RM) lex.yy.c core y.output y.tab.h y.tab.c make.err \
		*.o 


