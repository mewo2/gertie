(define nil '())
(define car head)
(define cdr tail)
(define caar (lambda (x) (head (head x))))
(define cadr (lambda (x) (head (tail x))))
(define cdar (lambda (x) (tail (head x))))
(define cddr (lambda (x) (tail (tail x))))
(define caddr (lambda (x) (head (tail (tail x)))))

(define cons pair)

(define first head)
(define second cadr)
(define third caddr)

(define not (lambda (x) (if x #f #t)))

(define add1
  (lambda (x) (+ x 1)))
(define sub1
  (lambda (x) (- x 1)))

(define map
  (lambda (f lst)
    (if (pair? lst) 
      (pair 
        (apply f (head lst)) 
        (map f (tail lst)))
      nil
    )))

(define yes (lambda (x) #t))
(define no (lambda (x) #f))

(define macro-transformers nil)

(define register-macro
  (lambda (keyword test transform)
    (begin
      (define lst (assoc keyword macro-transformers))
      (define new-entry (pair test transform))
      (if lst
        (fail)
        (set! macro-transformers 
          (pair (pair keyword (list new-entry)) macro-transformers))
        ))))

(define apply-macro-list
  (lambda (form lst)
    (if (pair? lst)
      (begin
        (define macro (head lst))
        (define others (tail lst))
        (define test (head macro))
        (define transform (tail macro))
        (if (test form) 
          (transform form)
          (apply-macro-list form others)))
      'macro-not-found)))

(define macro-expand
  (lambda (form)
    (if (pair? form)
      (begin
        (define f (head form))
        (define args (tail form))
        (define macro-list (assoc f macro-transformers))
        (if macro-list
          (begin
            (define applied (apply-macro-list form (tail macro-list)))
            (if (equal? applied 'macro-not-found)
              (map macro-expand form)
              (macro-expand applied)))
          (map macro-expand form)))
      form
    )))

(define expand-quasiquote
  (lambda (form)
    (if (pair? form)
      (begin
        (define kw (head form))
        (if (equal? kw 'unquote)
          (second form)
          (begin
            (define expand
              (lambda (item)
                (if (pair? item)
                  (if (equal? (head item) 'unquote)
                    (list 'list (second item))
                    (if (equal? (head item) 'unquote-splicing)
                      (second item)
                      (list 'list (expand-quasiquote item))))
                  (list 'list (expand-quasiquote item)))))
            (pair 'append (map expand form)))))
      (if (symbol? form)
        (list 'quote form)
        form))))

(register-macro
  'quasiquote
  yes
  (lambda (form) (expand-quasiquote (second form))))

(register-macro 
  'define 
  (lambda (form) (pair? (second form)))
  (lambda (form) 
    (begin
      (define arglist (second form))
      (define body (third form))
      `(define ,(head arglist) (lambda ,(tail arglist) ,body)))))

(register-macro
  'let 
  yes
  (lambda (form) 
    (begin
      (define arglist (second form))
      (define body (third form))
      (if (pair? arglist)
        (begin
          (define argpair (head arglist))
          (define rest (tail arglist))
          (define var (first argpair))
          (define val (second argpair))
          `(let ,rest ((lambda (,var) ,body) ,val)))
        body))))

(register-macro
  'or
  yes
  (lambda (form)
    (let
      ((args (tail form)))
      (if (pair? args)
        (if (pair? (tail args))
          `(if ,(head args) ,(head args) (or ,@(tail args)))
          (head args))
        #f))))

(register-macro
  'and
  yes
  (lambda (form)
    (let
      ((args (tail form)))
      (if (pair? args)
        (if (pair? (tail args))
          `(if ,(head args) (and ,@(tail args)) #f)
          (head args))
        #t))))

(register-macro
  'lambda
  (lambda (form) (> (length form) 3))
  (lambda (form)
    (let
      ((body (cddr form)))
      `(lambda (second form) (begin ,@body)))))

(register-macro
  'cond
  yes
  (lambda (form)
    (if (pair? (tail form))
      (let
        ((first-cond (second form))
         (rest-conds (cddr form)))
        (let
          ((test (first first-cond))
           (expr (second first-cond)))
          (if (equal? test 'else)
            expr
            `(if ,test ,expr (cond ,@rest-conds)))))
      #f)))

(define (in? elt lst)
  (if (pair? lst)
    (or 
      (equal? elt (head lst))
      (in? elt (tail lst)))
    #f))

(register-macro
  'case
  yes
  (lambda (form)
    (let ((keysym (gensym)))
      (let
        ((key (second form))
         (exprs (cddr form))
         (expand-expr 
          (lambda (ex)
            (begin
              (define vals (head ex))
              (define body (second ex))
              (define test 
                (if (equal? 'else vals)
                  'else
                  `(in? ,keysym ',vals)))
              `(,test ,body)))))
        `(let ((,keysym ,key))
          (cond ,@(map expand-expr exprs)))))))

(define (for-each proc args)
  (if (pair? args)
    (begin
      (define res (apply proc (head args)))
      (pair res (for-each proc (tail args))))
    nil))
