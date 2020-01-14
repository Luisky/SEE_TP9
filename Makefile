obj-m += m2lse_module.o

# KDIR is the location of the kernel source.  The current standard is
# to link to the associated source tree from the directory containing
# the compiled modules.
KDIR  := /lib/modules/$(shell uname -r)/build

# PWD is the current working directory and the location of our module
# source files.
PWD   := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
	rm -rf *.o *mod.c modules.order Module.symvers

.PHONY: clean
clean:
	make -C $(KDIR) M=$(PWD) clean
