NAME = riscv-elf

DATE = $(shell date '+%B %d, %Y')

.PHONY: all
all: $(NAME).pdf

.PHONY: clean
clean:
	rm -rf build
	rm -f $(NAME).pdf

build:
	mkdir -p $@

build/include.md: $(NAME).md | build
	awk '/<!-- END COPYRIGHT -->/{b=0} b{print} /<!-- BEGIN COPYRIGHT -->/{b=1}' $< > $@

build/body.md: $(NAME).md | build
	awk 'BEGIN{b=1} /<!-- BEGIN TITLE -->/{b=0} b{print} /<!-- END TITLE -->/{b=1}' $< > $@

build/include.tex: build/include.md | build
	pandoc -o $@ $<

$(NAME).pdf: build/body.md build/include.tex $(NAME).yaml | build
	pandoc --metadata-file $(NAME).yaml --metadata date="$(DATE)" -o $@ $<
