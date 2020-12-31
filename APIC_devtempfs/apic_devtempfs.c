/*	Implement char driver that supports open, release and ioctl operations on advanced 
	programmble Interrupt controller(IOAPIC)

	Implement ioctl operation for the following:

	1. APIC_GETID		--> returns identification number of APIC
	2. APIC_GETIRQS		--> returns number of irq pins on APIC
	3. APIC_GETIRQSTATUS	--> returns irq pin status(enable or disable)
	4. APIC_GETIRQTYPE	--> returns irq pin signal type(level or edge trigger)

*/




#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/version.h>
#include <linux/uidgid.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>

#include "apic.h"


#define CHAR_DEV "apic"
#define IOAPIC_BASE  0xFEC00000


static struct cdev *vara_dev;
static struct class *vara_class;
static dev_t mydev;

static int count = 1;

static void *io, *ioregsel, *iowin;

static unsigned int ident, maxirq, irqstat, irqtype;

static  long apic_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	int ioredtlb;

	int irqtlb[] = { 0x10, 0x12, 0x14, 0x16, 0x18, 
			 0x1A, 0x1C, 0x1E, 0x20, 0x22,
			 0x24, 0x26, 0x28, 0x2A, 0x2C,
			 0x2E, 0x30, 0x32, 0x34, 0x36,
			 0x38, 0x3A, 0x3C, 0x3E
		       };

//	ioredtlb = (unsigned int *)arg;

	switch(cmd)
	{

		case APIC_GETID		: iowrite32(0, ioregsel);
				          ident = (ioread32(iowin) >> 24) & 0x0F;
					  pr_info("identity number: %d\n", ident);
			       		  put_user(ident,(unsigned int *)arg);
					  break;

		case APIC_GETIRQS	: iowrite32(1, ioregsel);
                                          maxirq = ((ioread32(iowin) >> 16) & 0x00FF) + 1;
                                          pr_info("Max interrupt pin: %d\n", maxirq);
                                          put_user(maxirq,(unsigned int *)arg);
                                          break;

		case APIC_GETIRQSTATUS	: get_user(ioredtlb,(unsigned int *)arg);
					  iowrite32( irqtlb[ioredtlb] , ioregsel);
					  irqstat = (ioread32(iowin) >> 16) & 0x0001;
					  pr_info("interrupt status: %d\n", irqstat);
                                          put_user(irqstat, (unsigned int *)arg);
                                          break;

		case APIC_GETIRQTYPE	: get_user(ioredtlb,(unsigned int *)arg);
					  iowrite32( irqtlb[ioredtlb] , ioregsel);
                                          irqtype = (ioread32(iowin) >> 15) & 0x0001;
                                          pr_info("interrupt type: %d\n", irqtype);
                                          put_user(irqtype, (unsigned int *)arg);
                                          break;


	}
	return 0;
}


static  int apic_open(struct inode *inode, struct file *file)
{

        pr_info("Open operation invoked\n");

	io = ioremap_nocache(IOAPIC_BASE, PAGE_SIZE);

        ioregsel = (void *)((long)io + 0x00);

        iowin = (void *)((long)io + 0x10);

        pr_info("current uid = %u\n", get_current_user()->uid.val);
        return 0;

}

static int apic_release(struct inode *inode, struct file *file)
{

	pr_info("Release operation invoked\n");
        return 0;

}


static struct file_operations char_dev_fops = {

	.owner 	        = THIS_MODULE,
	.open  	        = apic_open,
	.release        = apic_release,
	.unlocked_ioctl = apic_ioctl
};


static int __init myapic_init(void)
{
	int ret, minorid = 0;

	alloc_chrdev_region(&mydev, minorid, count, CHAR_DEV);

	vara_dev = cdev_alloc();

	cdev_init(vara_dev, &char_dev_fops);

	ret = cdev_add(vara_dev, mydev, count);

	if(ret < 0)
	{
		pr_info("failed to register device driver\n");
		return -1;
	}

	vara_class = class_create(THIS_MODULE, "VIRTUAL");
	device_create(vara_class, NULL, mydev, NULL, "%s", "apic"); 

	pr_info("Device driver registered sucessfully\n");

	pr_info("MAJOR NO: %d and MINOR NO: %d\n", MAJOR(mydev), MINOR(mydev));

	return 0;
}


static void __exit myapic_exit(void)
{

	cdev_del(vara_dev);

	unregister_chrdev_region(mydev, count);

	device_destroy(vara_class, mydev);
	class_destroy(vara_class);

	pr_info("Driver unregistered\n");

}



module_init(myapic_init);
module_exit(myapic_exit);


MODULE_AUTHOR("Ashish Vara");
MODULE_LICENSE("GPL");
