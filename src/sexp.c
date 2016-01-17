#include <string.h>
#include <stdlib.h>

#include "sexp.h"
#include "error.h"
#include "gc.h"

sexp nil_sexp = {{0}, NIL_SEXP};
sexp false_sexp = {{0}, BOOLEAN_SEXP};
sexp true_sexp = {{1}, BOOLEAN_SEXP};
sexp return_sexp = {{0}, RETURN_SEXP};
sexp apply_sexp = {{0}, APPLY_SEXP};
sexp halt_sexp = {{0}, HALT_SEXP};
sexp apply_func_sexp = {{0}, APPLY_FUNC_SEXP};


void typecheck(sexp* s, sexptype type, char* msg) {
  if (s->type != type) {
    error(msg);
  }
}

sexp* number_sexp(int number) {
  sexp* s = new_sexp(NUMBER_SEXP);
  s->number = number;
  return s;
}

sexp* symbol_sexp(char* symbol) {
  sexp* s = new_sexp(SYMBOL_SEXP);
  s->symbol = strdup(symbol);
  return s;
}

sexp* boolean_sexp(int boolean) {
  if (boolean) {
    return &true_sexp;
  } else {
    return &false_sexp;
  }
}
sexp* pair_sexp(sexp* head, sexp* tail) {
  if (tail == NULL) tail = &nil_sexp;
  sexp* s = new_sexp(PAIR_SEXP);
  car(s) = head;
  cdr(s) = tail;
  return s;
}

sexp* defn_sexp(sexp* head, sexp* tail) {
  sexp* s = pair_sexp(head, tail);
  s->type = DEFN_SEXP;
  return s;
}

sexp* string_sexp(char* symbol) {
  sexp* s = new_sexp(STRING_SEXP);
  s->symbol = strdup(symbol);
  return s;
}

sexp* builtin_sexp(builtin_func func) {
  sexp* s = new_sexp(BUILTIN_SEXP);
  s->builtin = func;
  return s;
}

sexp* frame_sexp(sexp* now, sexp* next) {
  sexp* s = new_sexp(FRAME_SEXP);
  car(s) = now;
  cdr(s) = next;
  return s;
}

sexp* set_sexp(sexp* var, sexp* next) {
  sexp* s = new_sexp(SET_SEXP);
  car(s) = var;
  cdr(s) = next;
  return s;
}

sexp* lambda_sexp(sexp* formals, sexp* body, sexp* next) {
  sexp* s = new_sexp(LAMBDA_SEXP);
  car(s) = pair_sexp(formals, body);
  cdr(s) = next;
  return s;
}

sexp* closure_sexp(sexp* formals, sexp* env, sexp* body) {
  sexp* s = new_sexp(CLOSURE_SEXP);
  car(s) = pair_sexp(formals, env);
  cdr(s) = body;
  return s;
}

sexp* store_sexp(sexp* next) {
  sexp* s = new_sexp(STORE_SEXP);
  cdr(s) = next;
  return s;
}

sexp* test_sexp(sexp* pass, sexp* fail) {
  sexp* s = new_sexp(TEST_SEXP);
  car(s) = pass;
  cdr(s) = fail;
  return s;
}

sexp* refer_sexp(sexp* ref, sexp* next) {
  sexp* s = new_sexp(REFER_SEXP);
  typecheck(ref, SYMBOL_SEXP, "Attempting to set a non-symbol reference");
  car(s) = ref;
  cdr(s) = next;
  return s;
}

sexp* define_sexp(sexp* var, sexp* next) {
  sexp* s = new_sexp(DEFINE_SEXP);
  car(s) = var;
  cdr(s) = next;
  return s;
}

sexp* const_sexp(sexp* val, sexp* next) {
  sexp* s = new_sexp(CONST_SEXP);
  car(s) = val;
  cdr(s) = next;
  return s;
}

int equal(sexp* obj1, sexp* obj2) {
  if (obj1->type != obj2->type) return 0;
  switch (obj1->type) {
    case SYMBOL_SEXP:
    case STRING_SEXP:
      return strcmp(obj1->symbol, obj2->symbol) == 0;
    case NUMBER_SEXP:
    case BOOLEAN_SEXP:
      return obj1->number == obj2->number;
    case NIL_SEXP:
      return 1;
    case PAIR_SEXP:
      return equal(car(obj1), car(obj2)) && equal(cdr(obj1), cdr(obj2));
    case BUILTIN_SEXP:
      return obj1->builtin == obj2->builtin;
    default:
      return obj1 == obj2;
  }
}

sexp* list1(sexp* obj) {
  return pair_sexp(obj, &nil_sexp);
}

sexp* list2(sexp* obj1, sexp* obj2) {
  return pair_sexp(obj1, pair_sexp(obj2, &nil_sexp));
}

sexp* quoted(sexp* obj) {
  return list2(symbol_sexp("quote"), obj);
}

sexp* quasiquoted(sexp* obj) {
  return list2(symbol_sexp("quasiquote"), obj);
}

sexp* unquoted(sexp* obj) {
  return list2(symbol_sexp("unquote"), obj);
}

sexp* unquote_spliced(sexp* obj) {
  return list2(symbol_sexp("unquote-splicing"), obj);
}

sexp* reverse(sexp* lst) {
  sexp* new_lst = &nil_sexp;
  for (;!is_nil(lst); lst = cdr(lst)) {
    new_lst = pair_sexp(car(lst), new_lst);
  }
  return new_lst;
}

int has_symbol(sexp* s, char* symbol) {
  typecheck(s, SYMBOL_SEXP, "Testing symbol value of non-symbol");
  return strcmp(s->symbol, symbol) == 0;
}

int is_nil(sexp* s) {
  return s->type == NIL_SEXP;
}
