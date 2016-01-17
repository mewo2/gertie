#include <stdlib.h>
#include <stdio.h>
#include "gc.h"

typedef struct sexplist_s sexplist;

struct sexplist_s {
  sexp* s;
  sexplist* next;
};

sexplist* spare_sexps = NULL;
sexplist* sexps_in_use = NULL;
int n_requests = 0;
int n_sexps = 0;
int n_allocs = 0;

int last_sweep = 0;

void free_sexplist(sexplist* l) {
  while (l != NULL) {
    sexplist* l2 = l->next;
    free(l);
    l = l2;
  }
}

void push_sexplist(sexplist** pl, sexp* s) {
  sexplist* l = *pl;
  *pl = calloc(sizeof(sexplist), 1);
  (*pl)->next = l;
  (*pl)->s = s;
}

sexp* pop_sexplist(sexplist** pl) {
  sexp* s = (*pl)->s;
  sexplist* l = (*pl)->next;
  free(*pl);
  *pl = l;
  return s;
}

int empty_sexplist(sexplist* l) {
  return l == NULL;
}

sexp* new_sexp(sexptype type) {
  sexp* s;
  n_requests++;
  n_sexps++;
  if (empty_sexplist(spare_sexps)) {
    s = calloc(sizeof(sexp), 1);
    n_allocs++;
  } else {
    s = pop_sexplist(&spare_sexps);
  }
  s->type = type;
  push_sexplist(&sexps_in_use, s);
  return s;
}

void mark(sexp* s) {
  sexplist* q = NULL;
  push_sexplist(&q, s);
  while (!empty_sexplist(q)) {
    s = pop_sexplist(&q);
    if (s->marked || is_nil(s)) continue;
    switch(s->type) {
      case PAIR_SEXP:
      case DEFN_SEXP:
      case FRAME_SEXP:
      case SET_SEXP:
      case CLOSURE_SEXP:
      case TEST_SEXP:
      case REFER_SEXP:
      case DEFINE_SEXP:
      case LAMBDA_SEXP:
      case CONST_SEXP:
        push_sexplist(&q, car(s));
      case STORE_SEXP:
        push_sexplist(&q, cdr(s));
        break;
      default:
        break;
    }
    s->marked = 1;
  }
}

void sweep() {
  sexplist** psl = &sexps_in_use;
  while (*psl) {
    sexplist* sl = *psl;
    sexp* s = sl->s;
    if (!s->marked) {
      n_sexps--;
      *psl = sl->next;
      free(sl);
      push_sexplist(&spare_sexps, s);
      if (s->type == SYMBOL_SEXP || s->type == STRING_SEXP) {
        free(s->symbol);
      }
    } else {
      psl = &(sl->next);
    }
    s->marked = 0;
  }
  last_sweep = n_requests;
}

int need_gc() {
  return n_requests - last_sweep > n_allocs / 2;
}

void gc_status() {
  fprintf(stderr, "GC %d allocations, %d active, %d requests\n", n_allocs, n_sexps, n_requests);
}