/* I don't know what linux kernel file need include */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>


//dev_t devno;  
//struct cdev hello_cdev;

/* Step 2 works on device number  */
/* Step 3 make file operations work */

//unsigned int major = 0; // self define device number
//unsigned int minor = 0;
//struct file_operations hello_fops; // file operation points structer

#define DEV_SIZE 100
struct _hello_dev_t{
   char kbuf[DEV_SIZE];
   unsigned int major;
   unsigned int minor;
   unsigned int cur_size;
   dev_t devno;
   struct cdev hello_cdev;
   wait_queue_head_t test_queue; 
};

//struct _hello_dev_t my_dev;

int hello_open(struct inode*, struct file*);
int hello_release(struct inode *, struct file*);
ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t* offset);
ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t*offset);
loff_t hello_llseek(struct file *filp, loff_t offset, int whence);
long hello_ioctl(struct file* filp, unsigned int cmd, unsigned long arg);

/* These macros are for out of kernel modules to test that
 * the kernel supports the unlocked_ioctl and compat_ioctl
 * fields in struct file_operations. */
#if 0
#define HAVE_COMPAT_IOCTL 1
#define HAVE_UNLOCKED_IOCTL 1

struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	int (*readdir) (struct file *, void *, filldir_t);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*aio_fsync) (struct kiocb *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	int (*show_fdinfo)(struct seq_file *m, struct file *f);
};
#endif

struct file_operations hello_fops = {
       .open = hello_open,
       .release = hello_release,
       .write = hello_write,
       .read = hello_read,
       .llseek = hello_llseek,
       .unlocked_ioctl= hello_ioctl,
};

#define HELLO_MAGIC 'x'
#define HELLO_MAX_NR 3


/*
  32 bits linux ioctl: 
type : 8bit
number: 8bit
direction: 2bit
size: 14bit

 _IO(type, nr)
 _IOR(type, nr, size)
 _IOW(type, nr, size)
 _IOWR(type, nr, size)

 _IOC_DIR(cmd)
 _IOC_TYPE(cmd)
 _IOC_NR(cmd)
 _IOC_SIZE(cmd)
*/

struct ioctl_data{
  unsigned int size;
  char buf[100];
};

#define HELLO_CLEAR _IO(HELLO_MAGIC, 1)
#define HELLO_OFFSET _IO(HELLO_MAGIC, 2)
#define HELLO_KBUF _IOW(HELLO_MAGIC, 3, struct ioctl_data )





int myadd(int a, int b)
{
  int c = 0;
  c = a+b;
  printk(KERN_EMERG "sum=%d\n", c);
  return c;
}


int hello_open(struct inode* node, struct file* filp)
{
  struct _hello_dev_t *dev = NULL;

  printk(KERN_EMERG "hello_open!\n");
  dev = container_of(node->i_cdev, struct _hello_dev_t, hello_cdev);

  filp->private_data = dev; 
   
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
   //char kbuf[20] = {0};
   int ret = 0;
   struct _hello_dev_t *dev = filp->private_data;

   if(*offset >= DEV_SIZE){
      return count ? -ENXIO: 0 ;
   }

   if(*offset+count > DEV_SIZE){
      count = DEV_SIZE-*offset;
   }


   if(copy_from_user(dev->kbuf+*offset, buf, count)){
     ret = -EFAULT;
   }else{
     ret = count;
     dev->cur_size += count;
     *offset += count;
     printk(KERN_EMERG "write %d bytes, cur_size:[%d]\n", count, dev->cur_size);
     printk(KERN_EMERG "kbuf is [%s]\n", dev->kbuf);
     wake_up_interruptible(&dev->test_queue);
   } 

   return ret;
}


ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t*offset)
{
  int ret = 0;
  struct _hello_dev_t *dev = filp->private_data;


   if(*offset >= DEV_SIZE){
      return count ? -ENXIO: 0 ;
   }
   printk("read 0x%x\n", filp->f_flags);
#if 0
   if (filp->f_flags & O_NONBLOCK)
   {
      printk("can't read");
      return -EAGAIN;
   }
#endif
   printk("%x read data sleep\n", filp->f_flags);
   
   if (wait_event_interruptible(dev->test_queue, dev->cur_size>0))
       return -EFAULT;

   if(*offset+count > DEV_SIZE){
      count = DEV_SIZE-*offset;
   }
  if(copy_to_user(buf, dev->kbuf+ *offset, count)){
    ret = -EFAULT;
  }else{
    ret = count;
    dev->cur_size -= count;
    *offset += count;
    printk(KERN_EMERG "read %d bytes, cur_size:[%d]\n", count, dev->cur_size);
  }
  return ret;
}





loff_t hello_llseek(struct file *filp, loff_t offset, int whence)
{
  loff_t new_pos;
  loff_t old_pos;

  new_pos = 0;
  old_pos = filp->f_pos;
 
  switch(whence)
  {
    case SEEK_SET:
         new_pos = offset;
         printk("SEEK_SET\n");
         break;
    case SEEK_CUR:
         new_pos = old_pos + offset;
         break;
    case SEEK_END:
         new_pos = DEV_SIZE + offset;
         break;
    default:
         printk("unkown whence\n");
         return -EINVAL;
  }

  if (new_pos < 0 || new_pos > DEV_SIZE)
  {
     printk("f_pos failed\n");
     return -EINVAL;
  }

  filp->f_pos = new_pos;

  return new_pos;
}


long hello_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
{
  long ret = 0;
  struct _hello_dev_t *dev = filp->private_data;

  if(_IOC_TYPE(cmd) != HELLO_MAGIC) return -EINVAL;
  if(_IOC_NR(cmd) > HELLO_MAX_NR) return -EINVAL;

  printk("%d........\n", _IOC_SIZE(cmd));

  if(_IOC_DIR(cmd)&_IOC_READ)
    ret = !access_ok(VERIFY_WRITE, (void __user*)arg, _IOC_SIZE(cmd));
  else if(_IOC_DIR(cmd) & _IOC_WRITE)
    ret = !access_ok(VERIFY_READ, (void __user*)arg, _IOC_SIZE(cmd));

  if(ret) return -EFAULT;

  switch(cmd){
    case HELLO_CLEAR:
         memset(dev->kbuf, 0, DEV_SIZE);
         dev->cur_size = 0;
         filp->f_pos = 0;
         break;
    case HELLO_OFFSET:
         filp->f_pos += (int)arg;
         printk("change  offset!\n");
         break;
    case HELLO_KBUF:
         memset(dev->kbuf, 0, DEV_SIZE);
         memcpy(dev->kbuf, ((struct ioctl_data*)arg)->buf, ((struct ioctl_data*)arg)->size);
         filp->f_pos = 0;
         dev->cur_size = ((struct ioctl_data *)arg)->size;
         break;
    default:
         printk("error cmd!\n");
         ret = -EINVAL;
         break;
  }

  return ret;
}

//struct class *hello_class = NULL;


struct _hello_dev_t my_dev;
int hello_init(void) // kernel initialize function
{
  int i = 0;
  printk(KERN_EMERG "Hello world!\n");
  
  printk(KERN_EMERG "tring to allocate modern device\n");
  
  my_dev.cur_size = 0;
  my_dev.major = 0;
  my_dev.minor = 0;


  if(my_dev.major)
  {
    my_dev.devno = MKDEV(my_dev.major, my_dev.minor);
    i = register_chrdev_region(my_dev.devno, 1, "hello_drv_new"); 
    printk(KERN_EMERG "my i =%d major = %d, minor =  %d\n",i, my_dev.major ,my_dev.minor);

  }
  else
  {
    i = alloc_chrdev_region(&my_dev.devno, my_dev.minor,1, "hello_drv");
    my_dev.major = MAJOR(my_dev.devno);
    my_dev.minor = MINOR(my_dev.devno);
    printk(KERN_EMERG "i =%d major = %d, minor =  %d\n",i, my_dev.major , my_dev.minor);
  }

  cdev_init(&my_dev.hello_cdev, &hello_fops);
  printk(KERN_EMERG "i =%d cdev_initd\n",i);
  
  my_dev.hello_cdev.owner = THIS_MODULE;
 
  init_waitqueue_head(&my_dev.test_queue);
  i = cdev_add(&my_dev.hello_cdev, my_dev.devno, 1);
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
  cdev_del(&my_dev.hello_cdev);  
 // device_destroy(hello_class, devno);
 // class_destroy(hello_class);
  unregister_chrdev_region(my_dev.devno,1);
  printk("<1>""good bye! and good night!\n");
}

module_init(hello_init); // point to insmod load entry point
module_exit(hello_exit); // point to exit entry point


MODULE_LICENSE("GPL"); //GPL licence

MODULE_AUTHOR("Zhihual"); //Author

MODULE_VERSION("1.0");// First version
