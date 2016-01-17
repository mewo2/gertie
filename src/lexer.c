#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "error.h"

#define SYMBOL_CHARS "abcdefghijklmnopqrstuvwxyz?!+-*/=<>"
#define NUMBER_CHARS "1234567890"
#define WHITESPACE " \t\r\n"

typedef enum {
  BLANK_STATE,
  SYMBOL_STATE,
  NUMBER_STATE,
  HASH_STATE,
  COMMENT_STATE,
  STRING_STATE
} lexstate;

token openpar_tok = {OPENPAR_TOKEN, {0}};
token closepar_tok = {CLOSEPAR_TOKEN, {0}};
token false_tok = {BOOLEAN_TOKEN, {0}};
token true_tok = {BOOLEAN_TOKEN, {1}};
token quote_tok = {QUOTE_TOKEN, {0}};
token qq_tok = {QQ_TOKEN, {0}};
token unquote_tok = {UNQUOTE_TOKEN, {0}};
token splice_tok = {SPLICE_TOKEN, {0}};

tokenlist* emit(token* tok, tokenlist* tl) {
  if (tl->tok != NULL) {
    tl->next = calloc(sizeof(tokenlist), 1);
    tl = tl->next;
  }
  tl->tok = tok;
  return tl;
}

token* new_tok(tokentype type) {
  token* tok = calloc(sizeof(token), 1);
  tok->type = type;
  return tok;  
}

token* symbol_tok(char* start, int length) {
  token* tok = new_tok(SYMBOL_TOKEN);
  tok->symbol = strndup(start, length);
  return tok;
}

token* number_tok(char* start, int length) {
  token* tok = new_tok(NUMBER_TOKEN);
  char* tmp = strndup(start, length);
  tok->number = atoi(tmp);
  free(tmp);
  return tok;
}

token* string_tok(char* start, int length) {
  token* tok = new_tok(STRING_TOKEN);
  tok->symbol = strndup(start, length);
  return tok;
}

void print_token(token* tok) {
  switch (tok->type) {
    case OPENPAR_TOKEN:
      printf("OPENPAR");
      break;
    case CLOSEPAR_TOKEN:
      printf("CLOSEPAR");
      break;
    case SYMBOL_TOKEN:
      printf("SYMBOL(%s)", tok->symbol);
      break;
    case NUMBER_TOKEN:
      printf("NUMBER(%d)", tok->number);
      break;
    case BOOLEAN_TOKEN:
      printf("BOOLEAN(%s)", tok->number ? "TRUE" : "FALSE");
      break;
    case STRING_TOKEN:
      printf("STRING(\"%s\")", tok->symbol);
      break;
    case QUOTE_TOKEN:
      printf("QUOTE");
      break;
    case QQ_TOKEN:
      printf("QUASIQUOTE");
      break;
    case UNQUOTE_TOKEN:
      printf("UNQUOTE");
      break;
    case SPLICE_TOKEN:
      printf("SPLICE");
      break;
  }
}

void print_tokenlist(tokenlist* tl) {
  while (tl) {
    print_token(tl->tok);
    if (tl->next) printf(" ");
    tl = tl->next;
  }
  printf("\n");
}

tokenlist* lex(char* src) {
  char* ptr = src;
  lexstate state = BLANK_STATE;
  char* start = NULL;
  tokenlist* tl = calloc(sizeof(tokenlist), 1);
  tokenlist* tp = tl;
  int line = 1;
  while (*ptr) {
    if (*ptr == '\n') line++;
    switch (state) {
      case BLANK_STATE:
        if (*ptr == '(') {
          tp = emit(&openpar_tok, tp);
          ptr++;
        } else if (*ptr == ')') {
          tp = emit(&closepar_tok, tp);
          ptr++;
        } else if (*ptr == '#') {
          state = HASH_STATE;
          ptr++;
        } else if (*ptr == '\'') { 
          tp = emit(&quote_tok, tp);
          ptr++;
        } else if (*ptr == '`') { 
          tp = emit(&qq_tok, tp);
          ptr++;
        } else if (*ptr == ',') {
          ptr++;
          if (*ptr == '@') {
            tp = emit(&splice_tok, tp);
            ptr++;
          } else {    
            tp = emit(&unquote_tok, tp);
          }
        } else if (*ptr == ';') {
          state = COMMENT_STATE;
          ptr++;
        } else if (*ptr == '"') {
          state = STRING_STATE;
          ptr++;
          start = ptr;
        } else if (strchr(SYMBOL_CHARS, *ptr)) {
          start = ptr;
          state = SYMBOL_STATE;
        } else if (strchr(NUMBER_CHARS, *ptr)) {
          start = ptr;
          state = NUMBER_STATE;
        } else if (strchr(WHITESPACE, *ptr)) {
          ptr++;
        } else {
          error("Invalid character: '%s' at line %d", ptr, line);
        }
        break;
      case SYMBOL_STATE:
        if (strchr(SYMBOL_CHARS, *ptr) || strchr(NUMBER_CHARS, *ptr)) {
          ptr++;
        } else {
          int length = ptr - start;
          tp = emit(symbol_tok(start, length), tp);
          state = BLANK_STATE;
          start = NULL;
        }
        break;
      case NUMBER_STATE:
        if (strchr(NUMBER_CHARS, *ptr)) {
          ptr++;
        } else {
          int length = ptr - start;
          tp = emit(number_tok(start, length), tp);
          state = BLANK_STATE;
          start = NULL;
        }
        break;
      case HASH_STATE:
        if (*ptr == 't') {
          tp = emit(&true_tok, tp);
          state = BLANK_STATE;
          ptr++;
        } else if (*ptr == 'f') {
          tp = emit(&false_tok, tp);
          state = BLANK_STATE;
          ptr++;
        } else {
          error("Invalid #-symbol - '#%c'", *ptr);
        }
        break;
      case COMMENT_STATE:
        if (*ptr == '\n' || *ptr == '\r') {
          state = BLANK_STATE;
        }
        ptr++;
        break;
      case STRING_STATE:
        if (*ptr == '"') {
          int length = ptr - start;
          tp = emit(string_tok(start, length), tp);
          state = BLANK_STATE;
        }
        ptr++;
        break;
    }
  }
  return tl; 
}