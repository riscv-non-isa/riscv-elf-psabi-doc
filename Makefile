NAME = riscv-abi

DATE = $(shell date '+%B %d, %Y')

.PHONY: all
all: $(NAME).pdf

.PHONY: clean
clean:
	rm -f $(NAME).pdf

$(NAME).pdf: $(NAME).adoc $(wildcard *.adoc) resources/themes/risc-v_spec-pdf.yml
	asciidoctor-pdf \
	    -a compress \
	    -a date="$(DATE)" \
	    -a pdf-style=resources/themes/risc-v_spec-pdf.yml \
	    -a pdf-fontsdir=resources/fonts \
	    -v \
	    $< -o $@
