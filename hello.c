/* I don't know what linux kernel file need include */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
dev_t devno;  
struct cdev hello_cdev;

/* Step 2 works on device number  */
/* Step 3 make file operations work */

unsigned int major = 0; // self define device number
unsigned int minor = 0;
//struct file_operations hello_fops; // file operation points structer

int hello_open(struct inode*, struct file*);
int hello_release(struct inode *, struct file*);
ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t* offset);
ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t*offset);


struct file_operations hello_fops = {
       .open = hello_open,
       .release = hello_release,
       .write = hello_write,
       .read = hello_read,
};

int myadd(int a, int b)
{
  int c = 0;
  c = a+b;
  printk(KERN_EMERG "sum=%d\n", c);
  return c;
}


int hello_open(struct inode* node, struct file* filp)
{
  printk(KERN_EMERG "hello_open!\n");

  myadd(88, 11);
  return 0;
}

int hello_release(struct inode * node, struct file* filp)
{
  printk(KERN_EMERG "hello_release!\n");
  return 0;
}

ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t* offset)
{
   char kbuf[20] = {0};
   int ret = 0;

   //memcpy(kbuf, buf, count);

   if(copy_from_user(kbuf, buf, count)){
     ret = -EFAULT;
   }else{
     ret = count;
   } 

   printk(KERN_EMERG "write %s count=%d, buf=%p\n", kbuf, count, buf );

   return ret;
}


ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t*offset)
{
  int ret = 0;
  char* data = "test_data";
//  memcpy(buf, data, count );
  if(copy_to_user(buf, data, count)){
    ret = -EFAULT;
  }else{
    ret = count;
  }

  printk(KERN_EMERG "read %s count=%d, buf=%p\n", buf, count, buf );
  return ret;
}



//struct class *hello_class = NULL;


int hello_init(void) // kernel initialize function
{
  int i = 0;
  printk(KERN_EMERG "Hello world!\n");
  
  printk(KERN_EMERG "tring to allocate modern device\n");
  if(major)
  {
    devno = MKDEV(major, minor);
    i = register_chrdev_region(devno, 1, "hello_drv_new"); 
    printk(KERN_EMERG "my i =%d major = %d, minor =  %d\n",i, major , minor);

  }
  else
  {
    i = alloc_chrdev_region(&devno, minor,1, "hello_drv");
    major = MAJOR(devno);
    minor = MINOR(devno);
    printk(KERN_EMERG "i =%d major = %d, minor =  %d\n",i, major , minor);
  }
  cdev_init(&hello_cdev, &hello_fops);
  printk(KERN_EMERG "i =%d cdev_initd\n",i);
  
  hello_cdev.owner = THIS_MODULE;
  i = cdev_add(&hello_cdev, devno, 1);
  printk(KERN_EMERG "i =%d cdev_addd \n",i);
#if 0
  hello_class = class_create(THIS_MODULE, "hello_class");
  if(IS_ERR(hello_class))
  {
    printk("failed to create class\n");
    return -1;
  }
 
  device_create(hello_class, NULL, devno, "hello",NULL);
#endif

  return 0;
}

void hello_exit(void) // kernel clean up function
{
  cdev_del(&hello_cdev);  
 // device_destroy(hello_class, devno);
 // class_destroy(hello_class);
  unregister_chrdev_region(devno,1);
  printk("<1>""good bye! and good night!\n");
}

module_init(hello_init); // point to insmod load entry point
module_exit(hello_exit); // point to exit entry point


MODULE_LICENSE("GPL"); //GPL licence

MODULE_AUTHOR("Zhihual"); //Author

MODULE_VERSION("1.0");// First version
