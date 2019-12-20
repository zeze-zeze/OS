PWD := $(shell pwd) 
KVERSION := $(shell uname -r)
KERNEL_DIR   = /usr/src/linux-headers-$(KVERSION)/
MODULE_NAME = os_hw4_driver
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	gcc main.c -o main

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf main

install:
	insmod $(MODULE_NAME).ko
	mknod /dev/os c 200 0

remove:
	rmmod -f $(MODULE_NAME).ko
	rm -rf /dev/os
