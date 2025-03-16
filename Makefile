# Kernel version
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
obj-m := ExpiryWorkBase.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean


start:
	make all
	sudo insmod ExpiryWorkBase.ko

	
stop:
	sudo rmmod ExpiryWorkBase.ko
	make clean
pull:
	git pull origin $(BRANCH) --rebase
