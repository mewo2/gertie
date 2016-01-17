#ifndef SEXP_H
#define SEXP_H

typedef struct sexp_s sexp;

typedef enum {
  SYMBOL_SEXP,
  NUMBER_SEXP,
  PAIR_SEXP,
  NIL_SEXP,
  BUILTIN_SEXP,
  BOOLEAN_SEXP,
  LAMBDA_SEXP,
  DEFN_SEXP,
  STRING_SEXP,
  APPLY_FUNC_SEXP,
  FRAME_SEXP,
  SET_SEXP,
  CLOSURE_SEXP,
  TEST_SEXP,
  STORE_SEXP,
  RETURN_SEXP,
  APPLY_SEXP,
  REFER_SEXP,
  DEFINE_SEXP,
  CONST_SEXP,
  HALT_SEXP
} sexptype;

typedef sexp* (*builtin_func) (sexp*);

struct sexp_s {
  union {
    int number;
    char* symbol;
    builtin_func builtin;
    struct {
      sexp* head;
      sexp* tail;
    } pair;
  };
  sexptype type;
  char marked;
};

void typecheck(sexp* s, sexptype type, char* msg);

extern sexp nil_sexp;
extern sexp false_sexp;
extern sexp true_sexp;
extern sexp return_sexp;
extern sexp apply_sexp;
extern sexp halt_sexp;
extern sexp apply_func_sexp;

sexp* number_sexp(int number);
sexp* symbol_sexp(char* symbol);
sexp* boolean_sexp(int boolean);
sexp* pair_sexp(sexp* head, sexp* tail);
sexp* defn_sexp(sexp* head, sexp* tail);
sexp* string_sexp(char* symbol);
sexp* builtin_sexp(builtin_func func);

sexp* frame_sexp(sexp* now, sexp* next);
sexp* set_sexp(sexp* var, sexp* next);
sexp* lambda_sexp(sexp* formals, sexp* body, sexp* next);
sexp* closure_sexp(sexp* formals, sexp* env, sexp* body);
sexp* store_sexp(sexp* next);
sexp* test_sexp(sexp* pass, sexp* fail);
sexp* refer_sexp(sexp* ref, sexp* next);
sexp* define_sexp(sexp* var, sexp* next);
sexp* const_sexp(sexp* val, sexp* next);

int equal(sexp* obj1, sexp* obj2);
sexp* reverse(sexp* lst);
sexp* list1(sexp* obj);
sexp* list2(sexp* obj1, sexp* obj2);
sexp* quoted(sexp* obj);
sexp* quasiquoted(sexp* obj);
sexp* unquoted(sexp* obj);
sexp* unquote_spliced(sexp* obj);

int has_symbol(sexp* s, char* symbol);

int is_nil(sexp* s);

#define car(p) (p->pair.head)
#define cdr(p) (p->pair.tail)
#define caar(p) car(car(p))
#define cadr(p) car(cdr(p))
#define cdar(p) cdr(car(p))
#define cddr(p) cdr(cdr(p))
#define caddr(p) car(cdr(cdr(p)))

#define ONE_ARG(name)   typecheck(args, PAIR_SEXP, "Calling " name " on no arguments"); \
                        sexp* obj = car(args); \
                        typecheck(cdr(args), NIL_SEXP, "Calling " name " on more than one argument");

#define TWO_ARGS(name)  typecheck(args, PAIR_SEXP, "Calling " name " on no arguments"); \
                        sexp* obj1 = car(args); \
                        typecheck(cdr(args), PAIR_SEXP, "Calling " name " on one argument"); \
                        sexp* obj2 = cadr(args); \
                        typecheck(cddr(args), NIL_SEXP, "Calling " name " on more than two arguments");

#endif