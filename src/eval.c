#include <stdlib.h>

#include "eval.h"
#include "sexp.h"
#include "environment.h"
#include "gc.h"
#include "error.h"

sexp* eval_in_env(sexp* s, sexp** pe, int gc) {
  sexp* acc = &nil_sexp;
  sexp* expr = compile(s, &halt_sexp);
  sexp* env = *pe;
  sexp* rib = &nil_sexp;
  sexp* stack = &nil_sexp;
  while (1) {
    switch (expr->type) {
      case HALT_SEXP:
        *pe = env;
        return acc;
      case REFER_SEXP:
        acc = lookup(car(expr), env);
        if (acc == NULL) {
            error("Lookup not found: %s", car(expr)->symbol);
        }
        expr = cdr(expr);
        break;
      case CONST_SEXP:
        acc = car(expr);
        expr = cdr(expr);
        break;
      case LAMBDA_SEXP:
        acc = closure_sexp(caar(expr), env, cdar(expr));
        expr = cdr(expr);
        break;
      case TEST_SEXP:
        expr = acc->number ? car(expr) : cdr(expr);
        break;
      case SET_SEXP:
        set_in_env(env, car(expr), acc);
        expr = cdr(expr);
        break;
      case DEFINE_SEXP:
        env = add_defn(env, defn_sexp(car(expr), acc));
        expr = cdr(expr);
        break;
      case FRAME_SEXP:
        stack = pair_sexp(pair_sexp(car(expr), env), pair_sexp(rib, stack));
        rib = &nil_sexp;
        expr = cdr(expr);
        break;
      case STORE_SEXP:
        rib = pair_sexp(acc, rib);
        expr = cdr(expr);
        break;
      case APPLY_SEXP:
        if (acc->type == APPLY_FUNC_SEXP) {
          acc = car(rib);
          rib = cdr(rib);
        }
        if (acc->type == CLOSURE_SEXP) {
          env = add_defns(cdar(acc), caar(acc), rib);
          expr = cdr(acc);
          rib = &nil_sexp;
          break;
        } else if (acc->type == BUILTIN_SEXP) {
          acc = acc->builtin(rib);
        }
      case RETURN_SEXP:
        expr = caar(stack);
        env = cdar(stack);
        rib = cadr(stack);
        stack = cddr(stack);
        if (gc && need_gc()) {
          mark(acc);
          mark(expr);
          mark(env);
          mark(rib);
          mark(stack);
          sweep();
        }
        break;
      default:
        error("Invalid opcode: %d", s->type);
    }
  }
}

sexp* compile(sexp* s, sexp* next) {
  if (s->type == PAIR_SEXP) {
    sexp* head = car(s);
    sexp* tail = cdr(s);
    if (head->type == SYMBOL_SEXP) {
      if (has_symbol(head, "quote")) {
        typecheck(tail, PAIR_SEXP, "Invalid quote");
        return const_sexp(car(tail), next);
      } else if (has_symbol(head, "lambda")) {
        sexp* formals = car(tail);
        sexp* body = cadr(tail);
        return lambda_sexp(formals, compile(body, &return_sexp), next);
      } else if (has_symbol(head, "if")) {
        sexp* test = car(tail);
        sexp* pass = compile(cadr(tail), next);
        sexp* fail = compile(caddr(tail), next);
        return compile(test, test_sexp(pass, fail));
      } else if (has_symbol(head, "set!")) {
        sexp* var = car(tail);
        sexp* val = cadr(tail);
        return compile(val, set_sexp(var, next));
      } else if (has_symbol(head, "define")) {
        sexp* var = car(tail);
        sexp* val = cadr(tail);
        return define_sexp(var, compile(val, set_sexp(var, next)));
      } else if (has_symbol(head, "begin")) {
        for (sexp* lst = reverse(tail); !is_nil(lst); lst = cdr(lst)) {
          next = compile(car(lst), next);
        }
        return next;
      }
    }

    sexp* c = compile(head, &apply_sexp);
    for (;!is_nil(tail); tail = cdr(tail)) {
      c = compile(car(tail), store_sexp(c));
    }
    // return frame_sexp(next, c);
    return next->type == RETURN_SEXP ? c : frame_sexp(next, c);

  } else if (s->type == SYMBOL_SEXP) {
    return refer_sexp(s, next);
  } else {
    return const_sexp(s, next);
  }
}
