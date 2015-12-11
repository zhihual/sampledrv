/* I don't know what linux kernel file need include */
#include <linux/module.h>
#include <linux/init.h>

int myadd(int a, int b)
{
  int c = 0;
  c = a+b;
  printk(KERN_EMERG "sum=%d\n", c);
  return c;
}

int hello_init(void) // kernel initialize function
{
  int i=0;
  printk(KERN_EMERG "Hello world!\n");
 // while(1)
  {
    i++;
  };

  myadd(1,2);
  return 0;
}

void hello_exit(void) // kernel clean up function
{
  printk("<1>""good bye! and good night!\n");
}

module_init(hello_init); // point to insmod load entry point
module_exit(hello_exit); // point to exit entry point


MODULE_LICENSE("GPL"); //GPL licence

MODULE_AUTHOR("Zhihual"); //Author

MODULE_VERSION("1.0");// First version
