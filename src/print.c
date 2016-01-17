#include <stdio.h>

#include "print.h"
#include "sexp.h"

void display_sexp(sexp* s) {
  print_sexp(s);
  printf("\n");
}

void print_sexp(sexp* s) {
  switch (s->type) {
    case SYMBOL_SEXP:
      printf("%s", s->symbol);
      break;
    case NUMBER_SEXP:
      printf("%d", s->number);
      break;
    case PAIR_SEXP:
      printf("(");
      while (s->type == PAIR_SEXP) {
        print_sexp(car(s));
        s = cdr(s);
        if (s->type == PAIR_SEXP) printf(" ");
      }
      if (!is_nil(s)) {
        printf(" . ");
        print_sexp(s);
      }
      printf(")");
      break;
    case NIL_SEXP:
      printf("()");
      break;
    case BOOLEAN_SEXP:
      if (s->number) {
        printf("#t");
      } else {
        printf("#f");
      }
      break;
    case BUILTIN_SEXP:
      printf("#builtin(%p)", s->builtin);
      break;
    case STRING_SEXP:
      printf("\"%s\"", s->symbol);
      break;
    case DEFN_SEXP:
      printf("#defn(");
      print_sexp(car(s));
      printf(")");
      break;
    case CLOSURE_SEXP:
      printf("#closure(");
      print_sexp(caar(s));
      printf(" [");
      print_bytecode(cdr(s));
      printf("]");
      break;
    default:
      printf("*TYPE %d*", s->type);
  }
}

void print_bytecode(sexp* s) {
  switch (s->type) {
    case PAIR_SEXP:
      print_sexp(car(s));
      printf(" -> ");
      print_bytecode(cdr(s));
      break;
    case FRAME_SEXP:
      printf("#frame(");
      print_bytecode(car(s));
      printf(") -> ");
      print_bytecode(cdr(s));
      break;
    case HALT_SEXP:
      printf("#halt");
      break;
    case RETURN_SEXP:
      printf("#return");
      break;
    case APPLY_SEXP:
      printf("#apply");
      break;
    case DEFINE_SEXP:
      printf("#define(");
      print_sexp(car(s));
      printf(") -> ");
      print_bytecode(cdr(s));
      break;
    case REFER_SEXP:
      printf("#refer(");
      print_sexp(car(s));
      printf(") -> ");
      print_bytecode(cdr(s));
      break;
    case SET_SEXP:
      printf("#set(");
      print_sexp(car(s));
      printf(") -> ");
      print_bytecode(cdr(s));
      break;
    case STORE_SEXP:
      printf("#store -> ");
      print_bytecode(cdr(s));
      break;
    case TEST_SEXP:
      printf("#test T->[");
      print_bytecode(car(s));
      printf("] F->[");
      print_bytecode(cdr(s));
      printf("]");
      break;
    default:
      printf("*TYPE %d*", s->type);
  }
}