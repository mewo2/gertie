#ifndef LEXER_H
#define LEXER_H

typedef enum {
  OPENPAR_TOKEN,
  CLOSEPAR_TOKEN,
  SYMBOL_TOKEN,
  NUMBER_TOKEN,
  BOOLEAN_TOKEN,
  QUOTE_TOKEN,
  QQ_TOKEN,
  UNQUOTE_TOKEN,
  SPLICE_TOKEN,
  STRING_TOKEN
} tokentype;

typedef struct {
  tokentype type;
  union {
    int number;
    char* symbol;
  };
} token;

typedef struct tokenlist_s tokenlist;

struct tokenlist_s {
  token* tok;
  tokenlist* next;
};

tokenlist* lex(char* src);
void print_tokenlist(tokenlist* tl);

#endif