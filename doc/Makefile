# Makefile for docu
# Note: you must have latex and dvips. If not then just use the precompiled
# file "docu.ps" or "docu.dvi"

RM=		rm -f

LATEX=		latex
DVIPS=		dvips
DVIPSOPT=	

all:		docu.ps

docu.ps:	docu.dvi
	$(DVIPS) $(DVIPSOPT) -o docu.ps docu.dvi

docu.dvi:	docu.tex
	$(LATEX) docu.tex
	$(LATEX) docu.tex

clean:
	$(RM) docu.ps docu.dvi docu.aux tocu.toc docu.log *core*
