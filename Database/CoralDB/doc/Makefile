DOCNAME = CoralDB

PS = $(DOCNAME:%=%.ps)
PDF = $(DOCNAME:%=%.pdf)
DVI = $(DOCNAME:%=%.dvi)

all: ps 
ps: $(PS)
pdf: $(PDF)

LATEXOPT := -interaction=nonstopmode -file-line-error-style

%.pdf: %.ps
	ps2pdf $<

%.ps: %.dvi
	dvips -o $@ $<

%.dvi: %.tex $(shell /bin/ls *.tex *.eps)
	$(RM) $(PS) $(PDF) $(DVI)
	latex $(LATEXOPT) $<
	latex $(LATEXOPT) $<
	latex $(LATEXOPT) $<

clean:
	$(RM) $(DOCNAME:%=%.log) $(DOCNAME:%=%.aux) $(DOCNAME:%=%.tmp) $(DOCNAME:%=%.lot) $(DOCNAME:%=%.lof)  $(DOCNAME:%=%.toc)
	$(RM) $(DVI) $(PS) $(PDF)

.PHONY: all clean ps pdf

.DELETE_ON_ERROR:

#EOF
