MODNAME := test
obj-m := $(MODNAME).o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
ins:
	insmod $(MODNAME).ko
rm:
	rmmod $(MODNAME)