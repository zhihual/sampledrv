/*This is test app for hello drv*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
  char buf[20] = {0};
  int fd = 0;
  int count = 0;
  fd = open("/dev/hello", O_RDWR);

  if (fd < 0 )
  {
     perror("open");
     return -1;
  }

  read(fd, buf, 20);
  printf("file read %s\n", buf);

//  write(fd, buf, 20);  
  count = write(fd, (const void*)(0), 20);

  printf("write count=%d\n", count);

  close(fd);
  return 0;
}




