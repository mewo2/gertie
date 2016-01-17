#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void error(const char* fmt, ...) {
  va_list argp;
  printf("Error: ");
  va_start(argp, fmt);
  vprintf(fmt, argp);
  va_end(argp);
  printf("\n");
  exit(1);
}