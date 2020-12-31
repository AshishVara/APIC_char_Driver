obj-m += apic_devtempfs.o
obj-m += apic_sysfs.o

KDIR= /lib/modules/$(shell uname -r)/build

all:
	make -C  $(KDIR) SUBDIRS=$(PWD) modules

clean:
	@rm -rf a.out
	make -C $(KDIR) SUBDIRS=$(PWD) clean
