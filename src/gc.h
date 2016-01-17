#ifndef GC_H
#define GC_H

#include "sexp.h"

sexp* new_sexp(sexptype type);
void mark(sexp* s);
void sweep();
void gc_status();
int need_gc();

#endif