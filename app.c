/*This is test app for hello drv*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
  char buf[20] = {0};
  int fd = 0;

  fd = open("/dev/hello", O_RDWR);

  if (fd < 0 )
  {
     perror("open");
     return -1;
  }

  close(fd);
  return 0;
}




