#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

char* str1 = "Started\0";
char* str2 = "Stoped\0";
char ch;

int main()
{
	int pid = (int)getpid();
	printf("%s pid: %d \n", str1, pid);

	scanf("%c", &ch);

	printf("%s \n", str2);

	return 0;
}
