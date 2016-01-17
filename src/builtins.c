#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
#include "sexp.h"
#include "environment.h"
#include "eval.h"
#include "print.h"
sexp* builtin_add(sexp* args) {
  int i = 0;
  for (; !is_nil(args); args = cdr(args)) {
    sexp* s = car(args);
    typecheck(s, NUMBER_SEXP, "Adding a non-number");
    i += s->number;
  }
  return number_sexp(i);
}

sexp* builtin_sub(sexp* args) {
  TWO_ARGS("-");
  typecheck(obj1, NUMBER_SEXP, "Subtracting a non-number");
  typecheck(obj2, NUMBER_SEXP, "Subtracting a non-number");
  return number_sexp(obj1->number - obj2->number);
}

sexp* builtin_mul(sexp* args) {
  int i = 1;
  for (; !is_nil(args); args = cdr(args)) {
    sexp* s = car(args);
    typecheck(s, NUMBER_SEXP, "Multiplying a non-number");
    i *= s->number;
  }
  return number_sexp(i);
}

sexp* builtin_div(sexp* args) {
  TWO_ARGS("/");
  typecheck(obj1, NUMBER_SEXP, "Dividing a non-number");
  typecheck(obj2, NUMBER_SEXP, "Dividing a non-number");
  return number_sexp(obj1->number / obj2->number);
}

sexp* builtin_equal(sexp* args) {
  TWO_ARGS("equal?")
  return boolean_sexp(equal(obj1, obj2));
}

sexp* builtin_head(sexp* args) {
  ONE_ARG("head");
  typecheck(obj, PAIR_SEXP, "Calling head on a non-pair");
  return car(obj);
}

sexp* builtin_tail(sexp* args) {
  ONE_ARG("tail");
  typecheck(obj, PAIR_SEXP, "Calling tail on a non-pair");
  return cdr(obj);
}

sexp* builtin_leq(sexp* args) {
  TWO_ARGS("<=");
  typecheck(obj1, NUMBER_SEXP, "Calling <= on a non-number");
  typecheck(obj2, NUMBER_SEXP, "Calling <= on a non-number");
  return boolean_sexp(obj1->number <= obj2->number);
}

sexp* builtin_eq(sexp* args) {
  TWO_ARGS("=");
  typecheck(obj1, NUMBER_SEXP, "Calling = on a non-number");
  typecheck(obj2, NUMBER_SEXP, "Calling = on a non-number");
  return boolean_sexp(obj1->number = obj2->number);
}

sexp* builtin_geq(sexp* args) {
  TWO_ARGS(">=");
  typecheck(obj1, NUMBER_SEXP, "Calling >= on a non-number");
  typecheck(obj2, NUMBER_SEXP, "Calling >= on a non-number");
  return boolean_sexp(obj1->number >= obj2->number);
}

sexp* builtin_lt(sexp* args) {
  TWO_ARGS("<");
  typecheck(obj1, NUMBER_SEXP, "Calling < on a non-number");
  typecheck(obj2, NUMBER_SEXP, "Calling < on a non-number");
  return boolean_sexp(obj1->number < obj2->number);
}

sexp* builtin_gt(sexp* args) {
  TWO_ARGS(">");
  typecheck(obj1, NUMBER_SEXP, "Calling > on a non-number");
  typecheck(obj2, NUMBER_SEXP, "Calling > on a non-number");
  return boolean_sexp(obj1->number > obj2->number);
}

sexp* builtin_pairq(sexp* args) {
  ONE_ARG("pair?");
  return boolean_sexp(obj->type == PAIR_SEXP);
}

sexp* builtin_pair(sexp* args) {
  TWO_ARGS("pair");
  return pair_sexp(obj1, obj2);
}

sexp* builtin_list(sexp* args) {
  return args;
}

sexp* builtin_display(sexp* args) {
  ONE_ARG("display");
  display_sexp(obj);
  return obj;
}

int n_gensyms = 0;
sexp* builtin_gensym(sexp* args) {
  char* symbol = NULL;
  asprintf(&symbol, "#gensym%d", n_gensyms);
  n_gensyms++;
  sexp* s = symbol_sexp(symbol);
  free(symbol);
  return s;
}

sexp* builtin_assoc(sexp* args) {
  TWO_ARGS("assoc");
  for (sexp* lst = obj2; !is_nil(lst); lst = cdr(lst)) {
    if (equal(obj1, caar(lst))) return car(lst);
  }
  return &false_sexp;
}

sexp* builtin_length(sexp* args) {
  ONE_ARG("length");
  int n = 0;
  for (sexp* lst = obj; !is_nil(lst); lst = cdr(lst)) {
    n++;  
  }
  return number_sexp(n);
}

sexp* builtin_append(sexp* args) {
  if (is_nil(args)) return &nil_sexp;
  if (is_nil(cdr(args))) return car(args);
  sexp* ret = builtin_append(cdr(args));
  for (sexp* lst = reverse(car(args)); !is_nil(lst); lst = cdr(lst)) {
    ret = pair_sexp(car(lst), ret);
  }
  return ret;
}

sexp* builtin_symbolq(sexp* args) {
  ONE_ARG("symbol?");
  return boolean_sexp(obj->type == SYMBOL_SEXP);
}

sexp* register_builtin(sexp* e, char* name, builtin_func func) {
  return define(e, name, builtin_sexp(func));
}



sexp* builtin_environment() {
  sexp* e = &nil_sexp;
  e = register_builtin(e, "+", builtin_add);
  e = register_builtin(e, "-", builtin_sub);
  e = register_builtin(e, "*", builtin_mul);
  e = register_builtin(e, "/", builtin_div);
  e = register_builtin(e, "equal?", builtin_equal);
  e = register_builtin(e, "<=", builtin_leq);
  e = register_builtin(e, "=", builtin_eq);
  e = register_builtin(e, ">=", builtin_geq);
  e = register_builtin(e, "<", builtin_lt);
  e = register_builtin(e, ">", builtin_gt);
  e = register_builtin(e, "head", builtin_head);
  e = register_builtin(e, "tail", builtin_tail);
  e = register_builtin(e, "pair?", builtin_pairq);
  e = register_builtin(e, "pair", builtin_pair);
  e = register_builtin(e, "list", builtin_list);
  e = register_builtin(e, "display", builtin_display);
  e = register_builtin(e, "gensym", builtin_gensym);
  e = register_builtin(e, "assoc", builtin_assoc);
  e = register_builtin(e, "length", builtin_length);
  e = register_builtin(e, "append", builtin_append);
  e = register_builtin(e, "symbol?", builtin_symbolq);

  e = add_defn(e, defn_sexp(symbol_sexp("apply"), &apply_func_sexp));

  return e;
}