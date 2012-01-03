obj-m := print.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insmod:
#	rmmod print
	insmod print.ko
	lsmod | grep print

rmmod:
	rmmod print

show:
	cat /var/log/syslog | grep print
