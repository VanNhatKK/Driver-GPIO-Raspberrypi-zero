//Thư viện
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>


/* Thông tin driver */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NHAT YOCTO");
MODULE_DESCRIPTION("My gpio");
MODULE_VERSION("V1");

/* Khai báo biến */

static dev_t my_dev_num;
static struct class *my_class;
static struct device *device_file;
static struct cdev my_dev;


/* driver - open*/
static int driver_open(struct inode *inode, struct file *file)
{
	printk("driver open was called\n");
	return 0;
}
/* driver - close*/
static int driver_close(struct inode *inode, struct file *file)
{
	printk("driver close was called\n");
	return 0;
}
/*driver - read */
static ssize_t driver_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	printk("driver read was called\n");
	return 0;
}
/*driver - write */
static ssize_t driver_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	printk("driver write was called\n");
	char data_rev[1024];
	if(copy_from_user(data_rev,buff,len) <0)
	{
		return -EFAULT;
	}
	/* giá trị output */
	switch(data_rev[0])
	{
		case '0':
			gpio_set_value(1,0);
			break;
		case '1':
			gpio_set_value(1,1);
		default:
			printk("Invalid data value:%s\n",data_rev);
			break;
	}
	return len;
}
/* Tạo fops */
static struct file_operations fops = {
	.owner	= THIS_MODULE,
	.open   = driver_open,
	.release= driver_close,
	.read   = driver_read,
	.write  = driver_write
};
/* init */
static int __init DRIVER_GPIO_INIT(void)
{
	printk("Hello,Kernel!\n");
	/* Đăng kí bộ số Major và Minor */
	int retval = alloc_chrdev_region(&my_dev_num,0,1,"name");
	if(retval <0)
	{
		printk(" Register not accept!\n");
		return -1;
	}
	printk(" Register Major: %d and Minor: %d was successfull\n",MAJOR(my_dev_num),MINOR(my_dev_num));
	/* Tạo class */
	my_class = class_create(THIS_MODULE,"classname");
	if(my_class == NULL)
	{
		printk("Class not create!\n");
		goto class_error;
	}
	/* Tạo device file */
	device_file = device_create(my_class,NULL,my_dev_num,NULL,"name");
	if(device_file == NULL)
	{
		printk("Device file not create!\n");
		goto device_error;
	}
	/* Khởi tạo tập tin */
	cdev_init(&my_dev,&fops);

	/* Đăng kí thêm thiết bị vào Kernel */
	int add = cdev_add(&my_dev,my_dev_num,1);
	if(add<0)
	{
		printk("Register device to kernel failed!\n");
		goto add_error;
	}
	/* xu li GPIO4 init */
	gpio_request(1,"rpi-gpio-1");
	/* set GIPO4 direction */
	gpio_direction_output(1,0);

	return 0;

add_error:
	device_destroy(my_class,my_dev_num);
device_error:
	class_destroy(my_class);
class_error:
	unregister_chrdev_region(my_dev_num,1);
	return -1;
}
/* exit */
static void __exit DRIVER_GPIO_EXIT(void)
{
	gpio_set_value(1,0);
	gpio_free(1);
	cdev_del(&my_dev);
	device_destroy(my_class,my_dev_num);
	class_destroy(my_class);
	unregister_chrdev_region(my_dev_num,1);
	printk("Goodbye,Kernel!\n");
}
module_init(DRIVER_GPIO_INIT);
module_exit(DRIVER_GPIO_EXIT);

