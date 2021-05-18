# ComputerSecurityFinalProject
Software Lab project for computer security course

% I4900 Software Security Lab
%
%

## Due May 27th @ 12pm (noon)

## Overview

Think of this assignment as experimental lab work:  While I am asking you to
make demonstrations / proofs of concept for a few software security issues, I
am more interested in your reporting than whether or not you were able to
succeed in every case.  So please, from the outset, **take careful notes on
what you are doing**.  Try to write at a level of detail which would enable
anyone else to reproduce your results.  Please also include details of any
dead ends or other issues you ran into along the way.

Once more for emphasis: don't be stressed if you can't make everything work!

## Logistics

You can work in small groups (let's say $\approx 4$).  Please keep track of
your work in git.  It doesn't have to be a public repository -- you can just
send me a tarball at the end (including `.git/` so I have the history
please!).  Regarding the due date: please send me *something* by May 27th, as
I have to turn in final grades by the 28th.  I will continue to accept updates
if you have them any time up until the grade deadline (usually the evening on
the 28th).


## Part I: The Basics

For this section, begin with a program that has a potentially exploitable
buffer overrun on the stack.  You could use the following code to start with:

~~~~~~~~~~~{.c}
#include <stdio.h>

int main()
{
	char buffer[16];
	scanf("%s",buffer);
	printf("string read: %s\n",buffer);
	return 0;
}
~~~~~~~~~~~~~~~~~~

I want you to find out what it takes to trick this program into doing
something else by giving it a malicious string as input.  In particular, I
want you to put executable code in the input string, and convince the program
to run it.  As we saw in lecture, there are number of protections in place to
prevent this type of very straightforward attack.  You will have to disable
at least the following (and if not, tell me how you did it!):

* Stack canaries (use `-fno-stack-protector` in gcc).
* `W^X` protection to make the stack executable.  You should be able to do
  this by compiling the program using `-z execstack` on gcc.
* ASLR.  You can accomplish this via `setarch x86_64 -R ./test` assuming your
  compiled program is named `test`.  Replace `x86_64` with your architecture
  if necessary (just run `uname -m` on linux to see what it is).

Ideally, your executable code would do something potentially useful to an
attacker, like starting a shell.

*Note:* the `scanf` function will stop reading at white space, which might be
inconvenient if the machine code you want to execute contains the bytes
corresponding to spaces or tabs.  You could use `gets` instead which only
stops reading at a newline.

### Hints

Try to do as much of this as you can on your own, but if you need a guide,
here is a walk-through of someone doing pretty much the same thing:
[ROP tutorial](https://crypto.stanford.edu/~blynn/rop/)


## Part II: Uncontrolled Format Strings

For this section, I want you to write a proof of concept exploit for a simple
program similar to the above, but with an uncontrolled format string:

~~~~~~~~~~~{.c}
#include <stdio.h>

int main()
{
	const int n = 128;
	char buffer[n];
	fgets(buffer,n,stdin);
	printf(buffer);
	return 0;
}
~~~~~~~~~~~~~~~~~~

Note that this time, a stack buffer overrun is no longer possible, as `fgets`
will not read more than `n` characters.

Perhaps proceed in the following increments:

1. Find an input format string that overwrites a local variable in the
   program.  You can introduce such a local artificially to the program above
   (it will serve no purpose except for this test).
2. Modify your format string to overwrite a return address.  If it helps, you
   can move the vulnerable code from `main` to an independent function.
3. Set that return address to point into a buffer of code which is also an
   input from the format string.

Does your proof of concept still work even when `-fstack-protector` is
enabled?

## Part III: Side Channels From the Cache

Make a proof of concept program that shows how measuring timing of memory
access is indeed sufficient to learn a value involved in some data-dependent
memory operation.  Proceed as follows: read a random byte (could be from the
`rand()` function of `libc`, or `/dev/urandom`) and save it in a variable.
Use this byte to access part of an array.  Afterwards, measure the timing of
access to various locations in the array in an attempt to learn the value of
the random byte.  Print your guess and the actual value, and see how well you
can do.

If you are met with reasonable success with the first step, try to recreate a
most basic Spectre proof of concept.  For example, allocate an array on the
heap, and compute the offset from that array to a local variable in `main`.
Try to speculatively execute an out of bounds access which would resolve to
the local variable in `main`, and see if you can't deduce its contents via a
timing experiment as above.


## Part IV (Bonus): ROP

The end of this tutorial (referenced above) attempts a proof of concept for
Return Oriented Programming:
[ROP tutorial](https://crypto.stanford.edu/~blynn/rop/)
See if you can manage to follow along.  For reference, here is the
[original paper](https://hovav.net/ucsd/dist/geometry.pdf) proposing this
idea.

