# Problem II

This writeup describes solve problem 2.

Mainly used printf() format string vunerability to exploit the problem.


# Problem 1: Overwrite a local variable.

The main idea behind using printf() exploit is that, printf() is defined as a function of variable length arguments.

we can use ```'%x'``` to print the contents of the stack immediately on top printf(). We can also use ```'%n'``` to write the number of bytes into a stack.

If we want to overwrite a local variable, we would need to find the address of that local variable. Then we can use a python input like,
```
setarch `arch` -R ./victim "`python -c "print 'AAAA '+'%x '*100"`"

```
Using this input format, we can see where in the stack the values ```41414141 25207825 20782520 78252078 25207825 20782520 78252078 25207825``` are. Here ```0x41414141``` represents ```AAAA``` and ```0x782520``` repeating pattern represents ```'%x '```.

With that information in mind, we need to figure out the necessary padding. Because lengths of 64bit memeory address are 64bit long. Although the variable memory address might be like ```0x7fffffffd9e4```. Here the memory address is missing 2 leading 0s. We use little-endian format to pad the memory address.
```\xe4\d9\xff\xff\xff\xff\x00\x00```.

We can use
``` 
setarch `arch` -R ./victim "`python -c "print 'AAAA '+'\xe4\d9\xff\xff\xff\xff\x00\x00'+'%x '*100+'%n '"`"

```
to set the number of bytes and modify the value.
