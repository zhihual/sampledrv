#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>

unsigned int flag;

void sig_handler(int sig)
{
	printf("<app>%s\n", __FUNCTION__);
	flag = 1;
}

int main(void)
{
	char buf[20];
	int fd;	
	int f_flags;
	flag = 0;

	fd = open("/dev/test", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}
	
	signal(SIGIO, sig_handler);
	fcntl(fd, F_SETOWN, getpid());
	f_flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, FASYNC | f_flags);

	while(1)
	{
		printf("waiting \n");
		sleep(4);
		if(flag)
			break;
	}

	read(fd, buf, 10);
	printf("finish: read[%s]\n", buf);

	close(fd);
	return 0;
}
