from subprocess import Popen, PIPE, STDOUT
import sys

def gertie(code):
  pipe = Popen("bin/gertie", stdin=PIPE, stdout=PIPE, stderr=PIPE)
  return pipe.communicate(input=code + "\n")[0][:-1]

def run_test(code, expected):
  output = gertie(code)
  if output == expected:
    return None
  else:
    return output

def run_tests(tests):
  print "Running %d tests" % len(tests)
  fails = []
  for i, test in enumerate(tests):
    code, expected = test
    res = run_test(code, expected)
    if res is not None:
      fails.append((code, expected, res))
      print "x",
    else:
      print ".",
      sys.stdout.flush()
  print
  print "%d failure(s)" % len(fails)
  for code, expected, res in fails:
    print code
    print " => "
    print res
    print " Expected:"
    print expected
    print

tests = [
  ("1", "1"),
  ("()", "()"),
  ("#t", "#t"),
  ("#f", "#f"),
  ("#q", "Error: Invalid #-symbol - '#q'"),
  ("flibbertigibbet", "Error: Lookup not found: flibbertigibbet"),
  ("(", "Error: Unexpected end of input"),
  (")", "Error: Unexpected ')'"),
  ("(a", "Error: Unexpected end of input"),
  ("\"abc\"", "\"abc\""),
  ("\"abc 123\"", "\"abc 123\""),
  ("\"@&*!?\"", "\"@&*!?\""),



  ("(quote (1 2))", "(1 2)"),
  ("(\nquote (\t1  3\r2\t)\t)\n", "(1 3 2)"),
  ("(quote)", "Error: Invalid quote"),
  ("(quote ((a) b c d))", "((a) b c d)"),
  
  ("'(1 2 3)", "(1 2 3)"),
  ("''a", "(quote a)"),

  ("(if #t 3 7)", "3"),
  ("(if #f 3 7)", "7"),
  ("(if #t 1 explosion)", "1"),
  ("(if #f 1 explosion)", "Error: Lookup not found: explosion"),

  ("((lambda (x) 7) 3)", "7"),
  ("((lambda (x) (+ x x)) 5)", "10"),
  ("((lambda (x y) (equal? (+ x 1) y)) 4 5)", "#t"),
  ("(((lambda (x) (lambda (y) x)) 4) 5)", "4"),
  ("((lambda () 1))", "1"),

  ("(begin 1 2)", "2"),
  ("(begin (define x 1) x)", "1"),
  ("(begin (define double (lambda (x) (+ x x))) (double 2))", "4"),
  ("(begin (define x 1) (define x 3) x)", "3"),
  ("(begin (define x x) x)", "()"),

  ("(begin (define x 1) (set! x 3) x)", "3"),
  ("(begin (set! x 3) x)", "Error: Attempting to set! an undefined symbol: x"),

  ("(+ 1 2)", "3"),
  ("(+ + +)", "Error: Adding a non-number"),
  ("(+)", "0"),
  ("(+ #t #f)", "Error: Adding a non-number"),
  
  ("(- 2 1)", "1"),
  ("(- 1 2)", "-1"),
  ("(-)", "Error: Calling - on no arguments"),

  ("(equal? 0 0)", "#t"),
  ("(equal? 0 #f)", "#f"),
  ("(equal? + +)", "#t"),
  ("(equal? () ())", "#t"),
  ("(equal? (+ 1 5) 6)", "#t"),
  ("(equal?)", "Error: Calling equal? on no arguments"),
  ("(equal? 1)", "Error: Calling equal? on one argument"),
  ("(equal? 1 1 1)", "Error: Calling equal? on more than two arguments"),

  ("(head (quote (1 2 3)))", "1"),
  ("(head (quote (3)))", "3"),
  ("(head '())", "Error: Calling head on a non-pair"),

  ("(tail (quote (1 2 3)))", "(2 3)"),
  ("(tail (quote (3)))", "()"),

  ("""
    (define fact 
      (lambda (n) 
        (if (equal? n 1) 
          1 
          (* n (fact (- n 1)))
        )
      )
    )
    (fact 5)
  """, "120"),
  ("""
    (define fib
      (lambda (n)
        (if (<= n 1)
          1
          (+ (fib (- n 1)) (fib (- n 2)))
        )
      )
    )
    (fib 10)
  """, "89"),

  ("(add1 3)", "4"),
  ("(sub1 10)", "9"),

  ("(map add1 '(1 2 3))", "(2 3 4)"),
  ("(for-each display '(1 2 3))", "1\n2\n3\n(1 2 3)"),

  ("(define f (lambda (x) (cons (car x) (cdr x)))) (f '(1 2))", "(1 2)"),
  ("(define f (lambda (n) (if (<= n 10000) (f (+ 1 n)) #t))) (f 0)", "#t"),

  ("(list 'x 'y)", "(x y)"),
  ("(apply list 'x 'y)", "(x y)"),

  ("(let ((a 1) (b 2)) (+ a b))", "3"),
  ("(let ((x 2) (y 3)) (let ((x 7) (z (+ x y))) (* z x)))", "35"),
  ("(cond ((> 3 2) 'greater) ((< 3 2) 'less)) ", "greater"),
  ("(cond ((> 3 3) 'greater) ((< 3 3) 'less) (else 'equal))", "equal"),
  ("(case (* 2 3) ((2 3 5 7) 'prime) ((1 4 6 8 9) 'composite))", "composite"),
  ("(case (car '(c d)) ((a e i o u) 'vowel) ((w y) 'semivowel) (else 'consonant))", "consonant"),
  ("(and (= 2 2) (> 2 1))", "#t"),
  ("(and (= 2 2) (< 2 1))", "#f"),
  ("(and 1 2 'c '(f g))", "(f g)"),
  ("(and)", "#t"),
  ("(or (= 2 2) (> 2 1))", "#t"),
  ("(or (= 2 2) (< 2 1))", "#t"),
  ("(or #f #f #f)", "#f"),
  

  ("(let ((x 0) (y 1)) `(foo bar ,x ,y))", "(foo bar 0 1)"),
  ("(let ((x '(1 2 3))) `(a ,@x b))", "(a 1 2 3 b)"),
  ("`foo", "foo"),
  ("(let ((x 1)) `,x)", "1"),
]
if __name__ == "__main__":
  run_tests(tests)
