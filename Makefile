NAME = riscv-abi

DATE = $(shell date '+%B %d, %Y')
MONTHYEAR = $(shell date '+%B %Y')

.PHONY: all
all: $(NAME).pdf

.PHONY: clean
clean:
	rm -f $(NAME).pdf

$(NAME).pdf: $(NAME).adoc $(wildcard *.adoc) docs-resources/themes/riscv-pdf.yml
	asciidoctor-pdf \
	    -a compress \
	    -a date="$(DATE)" \
	    -a monthyear="$(MONTHYEAR)" \
	    -a pdf-style=docs-resources/themes/riscv-pdf.yml \
	    -a pdf-fontsdir=docs-resources/fonts \
	    -v \
	    $< -o $@
