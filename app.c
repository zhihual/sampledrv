/*This is test app for hello drv*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

struct ioctl_data{
  unsigned int size;
  char buf[100];
};

#define HELLO_MAGIC 'x'
#define HELLO_MAX_NR 3

#define HELLO_CLEAR _IO(HELLO_MAGIC, 1)
#define HELLO_OFFSET _IO(HELLO_MAGIC, 2)
#define HELLO_KBUF _IOW(HELLO_MAGIC, 3, struct ioctl_data )


int main(void)
{
  char buf[20] = {0};
  int fd = 0;
  int count = 0;
  int ret = 0;

  struct ioctl_data my_data = {
      .size = 10,
      .buf = "123456789"
  };

  fd = open("/dev/hello", O_RDWR);

  if (fd < 0 )
  {
     perror("open");
     return -1;
  }

  write(fd, "hehe linux",11 );
  ret = lseek(fd, 0, SEEK_SET);
  
  ret = ioctl(fd,  HELLO_KBUF, (void*)&my_data);


  read(fd, buf, 11);

  printf("<app>buf is [%s]\n", buf);

  

  close(fd);
  return 0;
}




