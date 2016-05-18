# rpal-compiler
RPAL is a subset of PAL, the ‘‘Pedagogic Algorithmic Language’’. There are three versions of PAL:
RPAL, LPAL, and JPAL. The only one of interest here is RPAL. The ‘‘R’’ in RPAL stands for ‘‘right-ref-
erence’’, as opposed to ‘‘L’’ (LPAL) which stands for ‘‘left-reference’’. The logic behind this convention
comes from a tradition in programming languages, in which an identifier, when occurring on the left side of
an assignment, denotes the ‘‘left-value’’, i.e. its address, whereas if it occurs on the right side of the assign-
ment, it denotes the ‘‘right-value’’, i.e. the value stored at that address. An RPAL program is simply an
expression. RPAL has no concept of ‘‘assignment’’, nor even one of ‘‘memory’’. There are no loops, only
recursion. RPAL programs, as we shall see, consist exclusively of two notions: function definition and
function application.

1. Compile:
To comiple use make command

2. How to run:
	a)	./p1 FILE  --> This will print the output of the program
	b)	./p1 -ast FILE --> This will print the AST for the program and its output.
	c)	./p1 -debug FILE --> This will print the values to help debug, AST will not be printed here.
	d)	./p1 -st FILE --> This will print the ST for the program and its output.
	e)	./p1 -l FILE --> This will print cource code of the program and its output.
