/* I don't know what linux kernel file need include */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

dev_t devno;  
struct cdev hello_cdev;

/* Step 2 works on device number  */

unsigned int major = 253; // self define device number
unsigned int minor = 0;
struct file_operations hello_fops; // file operation points structer

int myadd(int a, int b)
{
  int c = 0;
  c = a+b;
  printk(KERN_EMERG "sum=%d\n", c);
  return c;
}

int hello_init(void) // kernel initialize function
{
  int i = 0;
  printk(KERN_EMERG "Hello world!\n");
  
  printk(KERN_EMERG "tring to allocate modern device\n");
  i = alloc_chrdev_region(&devno, minor,1, "hello_drv");
  major = MAJOR(devno);
  minor = MINOR(devno);
  printk(KERN_EMERG "i =%d major = 0x%d, minor =  0x%d\n",i, major , minor);

  cdev_init(&hello_cdev, &hello_fops);
  printk(KERN_EMERG "i =%d cdev_initd\n",i);
  
  hello_cdev.owner = THIS_MODULE;
  i = cdev_add(&hello_cdev, devno, 1);
  printk(KERN_EMERG "i =%d cdev_addd \n",i);

  return 0;
}

void hello_exit(void) // kernel clean up function
{
  
  unregister_chrdev_region(devno,1);
  printk("<1>""good bye! and good night!\n");
}

module_init(hello_init); // point to insmod load entry point
module_exit(hello_exit); // point to exit entry point


MODULE_LICENSE("GPL"); //GPL licence

MODULE_AUTHOR("Zhihual"); //Author

MODULE_VERSION("1.0");// First version
