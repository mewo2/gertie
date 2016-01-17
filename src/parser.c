#include <stdlib.h>

#include "lexer.h"
#include "sexp.h"
#include "error.h"
#include "gc.h"

tokenlist* parse(tokenlist* tl, sexp** ps) {
  sexp* s;
  token* tok = tl->tok;
  switch (tok->type) {
    case OPENPAR_TOKEN:
      if (tl->next == NULL) {
        error("Unexpected end of input");
      }
      if (tl->next->tok->type == CLOSEPAR_TOKEN) {
        *ps = &nil_sexp;
        return tl->next->next;
      }
      tl = tl->next;
      s = new_sexp(PAIR_SEXP);
      *ps = s;
      while (1) {
        tl = parse(tl, &car(s));
        if (tl == NULL) {
          error("Unexpected end of input");
        }
        if (tl->tok->type == CLOSEPAR_TOKEN) {
          cdr(s) = &nil_sexp;
          return tl->next;
        }
        cdr(s) = new_sexp(PAIR_SEXP);
        s = cdr(s);
      }
    case CLOSEPAR_TOKEN:
      error("Unexpected ')'");
    case SYMBOL_TOKEN:
      *ps = symbol_sexp(tok->symbol);
      return tl->next;
    case NUMBER_TOKEN:
      *ps = number_sexp(tok->number);
      return tl->next;
    case BOOLEAN_TOKEN:
      *ps = boolean_sexp(tok->number);
      return tl->next;
    case QUOTE_TOKEN:
      tl = parse(tl->next, &s);
      *ps = quoted(s);
      return tl;
    case QQ_TOKEN:
      tl = parse(tl->next, &s);
      *ps = quasiquoted(s);
      return tl;
    case UNQUOTE_TOKEN:
      tl = parse(tl->next, &s);
      *ps = unquoted(s);
      return tl;
    case SPLICE_TOKEN:
      tl = parse(tl->next, &s);
      *ps = unquote_spliced(s);
      return tl;
    case STRING_TOKEN:
      *ps = string_sexp(tok->symbol);
      return tl->next;
  }
}

