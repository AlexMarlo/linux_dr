#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static char* str1 = "Started\0";
static char* str2 = "Stoped\0";
char ch;

int main()
{
  int pid = (int)getpid();
  printf("str1: %d \n", str1);
  printf("str2: %d \n", str2);

  printf("%s pid: %d \n", str1, pid);

  scanf("%c", &ch);

  printf("%s \n", str2);

  return 0;
}
