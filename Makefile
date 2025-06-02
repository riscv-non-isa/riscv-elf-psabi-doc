NAME = riscv-abi

DATE = $(shell date '+%B %d, %Y')
MONTHYEAR = $(shell date '+%B %Y')

ADOCS = $(wildcard *.adoc)
ASCIIDOCTOR_FLAGS = \
    -a date="$(DATE)" \
    -a monthyear="$(MONTHYEAR)" \
    -v

.PHONY: all
all: $(NAME).pdf

ifeq ($(HTML), 1)
all: $(NAME).html
endif

.PHONY: clean
clean:
	rm -f $(NAME).pdf $(NAME).html

$(NAME).pdf: $(NAME).adoc $(ADOCS) resources/themes/risc-v_spec-pdf.yml
	asciidoctor-pdf \
	    $(ASCIIDOCTOR_FLAGS) \
	    -a compress \
	    -a pdf-style=resources/themes/risc-v_spec-pdf.yml \
	    -a pdf-fontsdir=resources/fonts \
	    $< -o $@

$(NAME).html: $(NAME).adoc $(ADOCS)
	asciidoctor \
	    $(ASCIIDOCTOR_FLAGS) \
	    $< -o $@

regen_vector_type_infos:
	python3 gen_vector_type_infos.py > vector_type_infos.adoc
