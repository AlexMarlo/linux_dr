obj-m += taskinfo.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insmod:
#	rmmod taskinfo
	insmod taskinfo.ko
	lsmod | grep taskinfo

rmmod:
	rmmod taskinfo

show:
	cat /var/log/syslog | grep taskinfo:
