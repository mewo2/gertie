#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "error.h"
#include "gc.h"
#include "sexp.h"
#include "builtins.h"
#include "eval.h"
#include "print.h"
#include "parser.h"
#include "environment.h"

char* read_input(FILE* fp) {
  char* code = NULL;
  int length = 0;
  for (int i = 0; ; i++) {
    code = realloc(code, 1024 * (i+1));
    int n_read = fread(code + i*1024, 1, 1024, fp);
    length += n_read;
    if (n_read < 1024) {
      char* terminated = strndup(code, length);
      free(code);
      return terminated;
    }
  }
}

sexp* exec_in_env(char* code, sexp** pe, int gc) {
  tokenlist* tl = lex(code);
  sexp* s;

  while (tl) {
    sexp* macro_expand = symbol_sexp("macro-expand");
    sexp* quote = symbol_sexp("quote");
    tl = parse(tl, &s);
    sexp* has_macro_expand = lookup(macro_expand, *pe);
    if (has_macro_expand) {
      sexp* macro_s = list2(macro_expand, quoted(s));
      s = eval_in_env(macro_s, pe, gc);
      if (gc) {
        mark(*pe);
        mark(s);
        sweep();
      }
    }
    s = eval_in_env(s, pe, gc);
    if (gc) {
      mark(*pe);
      mark(s);
      sweep();
    }
  }
  return s;
}

int main(int argc, char* argv[]) {
  FILE* fp = fopen("prelude.grt", "r");
  char* prelude = read_input(fp);
  fclose(fp);

  sexp* e = builtin_environment();
  exec_in_env(prelude, &e, 1);

  if (argc == 1) {
    fp = stdin;
  } else {
    fp = fopen(argv[1], "r");
  }
  char* code = read_input(fp);
  fclose(fp);

  sexp* s = exec_in_env(code, &e, 1);
  gc_status();
  display_sexp(s);
}
