obj-m := nothing.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -rf *.o
	rm -rf .*.cmd
	rm -rf *.ko 
	rm -rf modules.order 
	rm -rf Module.symvers
	rm -rf .tmp_versions
	rm -rf *.mod.c

insmod:
	insmod nothing.ko
	lsmod | grep nothing

rmmod:
	rmmod nothing

show:
	cat /var/log/syslog
