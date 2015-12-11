obj-m += hello.o
KERNELDIR := $(shell uname -r)
PWD := $(shell pwd)

all:
	$(MAKE) -C /lib/modules/$(KERNELDIR)/build M=$(PWD) modules

clean :
	$(MAKE) -C /lib/modules/$(KERNELDIR)/build M=$(PWD) modules clean
	rm -rf modules.order


	
