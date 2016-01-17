#ifndef EVAL_H
#define EVAL_H

#include "sexp.h"

sexp* compile(sexp* s, sexp* next);
sexp* eval_in_env(sexp* s, sexp** pe, int gc);

#endif