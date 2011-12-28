#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

struct proc_mem
{
	unsigned long mem_start;
	unsigned long mem_end;

	unsigned long code_start;
	unsigned long code_end;

	unsigned long data_start;
	unsigned long data_end;

	unsigned long brk_start;
	unsigned long brk_end;
};

static char* pid_file_name = "/var/ptrace_pid";
static char* mem_file_name = "/var/ptrace_mem";
const int long_size = sizeof(long);

void getdata( const pid_t child, const unsigned long addr, char *str, const int len)
{
	char *laddr;
    int i, j;
    union u
    {
      long val;
      char chars[long_size];
    }data;
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j)
    {
        data.val = ptrace( PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        //if( data.val < 0)
        	//printf("data.val: %d error: %s", data.val, strerror(errno));
        memcpy( laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }

    j = len % long_size;
    if(j != 0)
    {
        data.val = ptrace( PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        //if( data.val < 0)
        	//printf("data.val: %d error: %s", data.val, strerror(errno));
        memcpy( laddr, data.chars, j);
    }
    str[len] = '\0';
}

int get_mem_addrs_by_pid( const pid_t pid, struct proc_mem* proc_mem_)
{
	int mem_fd;
	return 0;
}
int search_next_str_by_str_in_mem( const pid_t pid, const struct proc_mem* proc_mem_, const char* needle, const char* next_str)
{
	unsigned char buf[1024];
	unsigned long addr;

	ptrace(PTRACE_ATTACH, pid, NULL, NULL);
	//getdata( pid, addr, buf, 1023);
	ptrace(PTRACE_DETACH, pid, NULL, NULL);
	return 0;
}

int main()
{
	pid_t pid = 1;
	int result;
	struct proc_mem proc_mem_;

	printf("Enter pid: ");
	scanf("%d", &pid);
	int mem_fd;
	if(get_mem_addrs_by_pid( pid, &proc_mem_) < 0)
	{
		printf("Error while getting process mem addresses!\n");
		return 0;
	}

	char needle[1024];
	bzero(needle, 1024);
	printf("Enter str for search: ");
	scanf("%s", needle);

	char* next_str;
	result = search_next_str_by_str_in_mem( pid, &proc_mem_, *needle, next_str);
	if( result < 0)
	{
		printf( "Error while search str!\n");
		return 0;
	}

	if( result == 0)
	{
		printf("Str not found!\n");
		return 0;
	}

	printf("Success str: %s\n", next_str);

	return 0;
}
