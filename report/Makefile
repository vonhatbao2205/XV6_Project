.PHONY: all clean

all: Report.pdf

Report.pdf: Report.tex $(wildcard images/* fonts/*)
	latexmk -xelatex -interaction=nonstopmode -halt-on-error Report.tex

clean:
	latexmk -c Report.tex
