# Gertie: a Lisp interpreter

This is a Lisp interpreter I developed over the course of a few weeks in 2014,
as a learning exercise. You probably don't want to use it for anything - it is
very much a product of a beginner.

The language itself is broadly Scheme-like, but not very well developed. The
type system is rudimentary, and the macros are extremely unhygienic. I got
bored before implementing `call/cc`, but all of the framework for it is there.

Some features:
    * Basic lispish syntax (`lambda`, `let`, `define`, `if`, etc)
    * A slightly janky macro system
    * More sophisticated syntax (`let*`, `cond`, etc) defined through macros
    * A mostly bootstrapped standard library
    * A mark-and-sweep garbage collector
    * A terrible hand-written lexer/parser
