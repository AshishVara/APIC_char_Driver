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
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/uidgid.h>
#include <linux/cred.h>
#include <linux/kobject.h>
#include <linux/sched.h>
#include <asm/io.h>


#define IOAPIC_BASE  0xFEC00000


static struct kobject *myapic;

static int irqno_type, irqno_stat;

static void *io, *ioregsel, *iowin;

static unsigned int ident, maxirq, irqstat, irqtype;

static int irqtlb[] = { 0x10, 0x12, 0x14, 0x16, 0x18, 
		 0x1A, 0x1C, 0x1E, 0x20, 0x22,
		 0x24, 0x26, 0x28, 0x2A, 0x2C,
		 0x2E, 0x30, 0x32, 0x34, 0x36,
		 0x38, 0x3A, 0x3C, 0x3E
	       };

static ssize_t apic_id_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	iowrite32(0, ioregsel);
        ident = (ioread32(iowin) >> 24) & 0x0F;
        pr_info("identity number: %d\n", ident);
        return sprintf(buf, "APIC id no: %d\n", ident);
}


static ssize_t apic_irqno_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	iowrite32(1, ioregsel);
        maxirq = ((ioread32(iowin) >> 16) & 0x00FF) + 1;
        pr_info("Max interrupt pin: %d\n", maxirq);
        return sprintf(buf, "Number of interrupt lines: %d\n", maxirq);
}


static ssize_t apic_irqstat_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

        sscanf(buf, "%du", &irqno_stat);
	iowrite32( irqtlb[irqno_stat] , ioregsel);
        irqstat = (ioread32(iowin) >> 16) & 0x0001;
	return count;
}

static ssize_t apic_irqstat_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{

	if(irqstat == 1)
        	return sprintf(buf, "Irq pin %d: disable\n", irqno_stat);
	else
                return sprintf(buf, "Irq pin %d: enable\n", irqno_stat);

}


static ssize_t apic_irqtype_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%du", &irqno_type);
        iowrite32( irqtlb[irqno_type] , ioregsel);
        irqtype = (ioread32(iowin) >> 15) & 0x0001;
        return count;
}


static ssize_t apic_irqtype_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        if(irqtype == 1)
                return sprintf(buf, "Irq pin %d: Level Sensitive\n", irqno_type);
        else
                return sprintf(buf, "Irq pin %d: Edge Sensitive\n", irqno_type);

}

static struct kobj_attribute apic_id_attr = __ATTR(APIC_GETID, 0660, apic_id_show, NULL);
static struct kobj_attribute apic_irqno_attr = __ATTR(APIC_GETIRQS, 0660, apic_irqno_show, NULL);
static struct kobj_attribute apic_irqstat_attr = __ATTR(APIC_GETIRQSTATUS, 0660, apic_irqstat_show, apic_irqstat_store);
static struct kobj_attribute apic_irqtype_attr = __ATTR(APIC_GETIRQTYPE, 0660, apic_irqtype_show, apic_irqtype_store);


static struct attribute *attrs[] = {

        &apic_id_attr.attr,
	&apic_irqno_attr.attr,
	&apic_irqstat_attr.attr,
	&apic_irqtype_attr.attr,
        NULL,
};


static struct attribute_group attr_group = {

        .attrs = attrs,
};


static int __init myapic_init(void)
{
	int ret;

	myapic = kobject_create_and_add("apic", kernel_kobj);
	if(myapic < 0)
	{
		pr_info("failed to create object\n");
		return -1;
	}

	ret = sysfs_create_group(myapic, &attr_group);
	if(ret)
        {
                kobject_put(myapic);
                pr_err("kobject files not created\n");

        }

        pr_info("kobject files created\n");

        io = ioremap_nocache(IOAPIC_BASE, PAGE_SIZE);

        ioregsel = (void *)((long)io + 0x00);

        iowin = (void *)((long)io + 0x10);

	return 0;
}


static void __exit myapic_exit(void)
{

	kobject_put(myapic);
	pr_info("Driver unloading\n");
}



module_init(myapic_init);
module_exit(myapic_exit);


MODULE_AUTHOR("Ashish Vara");
MODULE_LICENSE("GPL");
