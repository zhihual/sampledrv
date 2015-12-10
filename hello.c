/* I don't know what linux kernel file need include *
#include <linux/moudle.h>
#include <linux/init.h>


static int __init hello_init(void) // kernel initialize function
{
  printk("Hello world!\n");
  return 0;
}

static void __exit hello_exit(void) // kernel clean up function
{
  printk("good bye! and good night!\n");
}

moudle_init(hello_init); // point to insmod load entry point
moudle_exit(hello_exit); // point to exit entry point

MOUDLE_LICENCE("GPL"); //GPL licence

MOUDLE_AUTHOR("Zhihual"); //Author

MOUDLE_VERSION("0.1");// First version
