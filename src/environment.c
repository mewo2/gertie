#include <stdlib.h>

#include "environment.h"
#include "sexp.h"
#include "error.h"
#include "print.h"

sexp* add_defn(sexp* e, sexp* defn) {
  typecheck(defn, DEFN_SEXP, "Adding non-definition to environment");
  return pair_sexp(defn, e);
}

sexp* define(sexp* e, char* name, sexp* value) {
  return add_defn(e, defn_sexp(symbol_sexp(name), value));
}

sexp* add_defns(sexp* e, sexp* vars, sexp* vals) {
  if (is_nil(vars)) return e;
  return add_defns(add_defn(e, defn_sexp(car(vars), car(vals))), cdr(vars), cdr(vals));
}

sexp* set_in_env(sexp* e, sexp* var, sexp* val) {
  for (;!is_nil(e); e = cdr(e)) {
    sexp* defn = car(e);
    if (equal(var, car(defn))) {
      cdr(defn) = val;
      return &nil_sexp;
    }
  }
  error("Attempting to set! an undefined symbol: %s", var->symbol);
  return NULL;  
}

sexp* lookup(sexp* s, sexp* e) {
  if (s->type != SYMBOL_SEXP) {
    sexp* x = NULL;
    x->type = PAIR_SEXP;
  }
  typecheck(s, SYMBOL_SEXP, "Illegal lookup");
  for (;!is_nil(e); e = cdr(e)) {
    sexp* defn = car(e);
    typecheck(defn, DEFN_SEXP, "Non-definition in environment");
    if (equal(s, car(defn))) {
      return cdr(defn);
    }
  }
  return NULL;
}