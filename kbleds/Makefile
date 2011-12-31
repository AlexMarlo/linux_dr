obj-m := kbleds.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insmod:
#	rmmod kbleds
	insmod kbleds.ko
	lsmod | grep kbleds

rmmod:
	rmmod kbleds

show:
	cat /var/log/syslog | grep kbleds
