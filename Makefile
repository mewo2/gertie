CFILES = 	src/lexer.c \
			src/error.c \
			src/gc.c \
			src/sexp.c \
			src/builtins.c \
			src/environment.c \
			src/eval.c \
			src/print.c \
			src/parser.c

HFILES = $(CFILES:.c=.h)

bin/gertie: src/gertie.c $(CFILES) $(HFILES)
	mkdir -p bin
	clang src/gertie.c $(CFILES) -o bin/gertie -g

test: bin/gertie test.py prelude.grt
	python test.py

clean:
	rm -rf bin/*
