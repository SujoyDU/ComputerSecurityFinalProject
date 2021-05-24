#include <stdio.h>

int flag = 0;

int main() {
  const int n = 128;
	char buffer[n];
	fgets(buffer,n,stdin);
	printf(buffer);
  printf("\nflag Addr: ");
  printf("%p\n", &flag);
  if (flag == 1337) {
        puts("Enjoy your shell!");
        system("/bin/sh");}
    else {
        puts("Not allowed");
    }
    return 0;
	return 0;
}
//gcc -fno-stack-protector -z exestack -o victim problem2.c 