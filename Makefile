# Kernel version
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
obj-m := ExpiryWorkBase.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

log:
	dmesg -w

clear:
	dmesg -c


clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean


start:
	make all
	sudo insmod ExpiryWorkBase.ko
	make log
	
stop:
	sudo rmmod ExpiryWorkBase.ko
	make clear
