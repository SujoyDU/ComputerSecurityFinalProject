#include <stdio.h>

int main()
{
	const int n = 12;
	char buffer[n];
	fgets(buffer,n,stdin);
	printf(buffer);
	return 0;
}