#include <stdio.h>

int flag = 0;

int main() {
  const int n = 3;
	char buffer[n];
	fgets(buffer,n,stdin);
	// printf(buffer);
  int a,b;
  // printf("a has value %p, b has value %p, c is at address: %p\n",&a, &b, &c);
  // printf("a has value %08x, b has value %08x, c is at address: %08x\n",&a, &b, &c);

  // printf ("Mismatch: a has value %d, b has value %d, c is at address: %08x\n",a, b);
  
  // printf ("%08x %08x %08x %08x %08x\n");
  printf(buffer);
  printf("\n");
  printf("a has add %08x, b has add %08x, buff is at address: %08x\n",&a, &b, buffer);
  printf("\n");
  printf("a has value %p, b has value %p, buff is at address: %p\n",&a, &b, &buffer);
  printf ("\x7f\xff\xff\xff\xd9\xd0 %x %s");
  printf("\x7f\xff\xff\xff\xd9\xd0");
  // printf ("%p %p %p %p %p\n");
  // printf ("\x10\x01\x48\x08 %x %x %x %x");
  // printf("\nflag Addr: ");
  // printf("%p\n", &flag);
  // if (flag == 1337) {
  //       puts("Enjoy your shell!");
  //       system("/bin/sh");}
  //   else {
  //       puts("Not allowed");
  //   }
  //   return 0;
	return 0;
}
//gcc -fno-stack-protector -z exestack -o victim problem2.c 