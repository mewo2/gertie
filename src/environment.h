#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "sexp.h"

sexp* add_defn(sexp* e, sexp* defn);
sexp* define(sexp* e, char* name, sexp* value);
sexp* add_defns(sexp* e, sexp* vars, sexp* vals);
sexp* set_in_env(sexp* e, sexp* var, sexp* val);
sexp* lookup(sexp* s, sexp* e);

#endif