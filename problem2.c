#include <stdio.h>

int target;
int value;

void vuln(char *string,int k)
{
  printf(string);
  if(target) {
    printf("\nyou have changed the target \n");

  }
  if(k!=10) {
    printf("\nyou have changed K \n");

  }
  else {
    printf("\ntarget not changed\n");
  }
  printf("%d\n",k);
}
int main(int argc, char **argv) {
  int k = 10;
  vuln(argv[1],k);
  
  // const int n = 128;
  // char a[] = 'ABC';
	// char buffer[n];
	// fgets(buffer,n,stdin);
	// printf(buffer);
  // int a,b;
  // printf("a has value %p, b has value %p, c is at address: %p\n",&a, &b, &c);
  // printf("a has value %08x, b has value %08x, c is at address: %08x\n",&a, &b, &c);

  // printf ("Mismatch: a has value %d, b has value %d, c is at address: %08x\n",a, b);

  // printf ("%08x %08x %08x %08x %08x\n");
  // printf(buffer);
  // printf("\n");
  // printf("\n");
  // printf("a has add %08x, b has add %08x, buff is at address: %08x\n",&a, &b, buffer);
  // printf("\n");
  // printf("a has value %p, b has value %p, buff is at address: %p\n",&a, &b, &buffer);
  // printf("\n");
  // printf("%p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p");
  // printf("\n");
  // printf("%6$p");
  //printf("%x %x %x %x %p");
  
  // printf ("\x7f\xff\xff\xff\xd9\xd0 %x %s");
  // printf("\x7f\xff\xff\xff\xd9\xd0");
  // printf ("%p %p %p %p %p\n");ZX
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
// setarch `arch` -R ./victim "`python -c "print '%x '*30"`"
// "`python -c "print '%x '*10"`"

//0000000000004014
//\x14\x40\x00\x00\x00\x00\x00\x00
//python -c "print 'AAAA '+'\x14\x40\x00\x00\x00\x00\x00\x00'+' BBBBBB'+ '%x '*300"
//14 40 00 00
