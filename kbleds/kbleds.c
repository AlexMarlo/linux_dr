#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/tty.h>
#include <linux/vt_kern.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>

MODULE_DESCRIPTION("Keyboard leds flashing!");
MODULE_AUTHOR("Mitroshkin Alexander aka xanm!");
MODULE_LICENSE("Dual BSD/GPL");

struct timer_list my_timer;
struct tty_struct *my_tty;
struct tty_driver *my_driver;
char kbledstatus = 0;
int result;

//void * ioctl;

#define BLINK_DELAY HZ/5
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xFF

static void my_timer_func(unsigned long ptr)
{
	int *pstatus =(int *)ptr;
	printk(KERN_INFO "kbleds: my_timer_func(%i) \n", *pstatus);

	if(*pstatus == ALL_LEDS_ON)
		*pstatus = RESTORE_LEDS;
	else
		*pstatus = ALL_LEDS_ON;

	printk(KERN_INFO "kbleds: ioctl %i \n", my_driver->ops->ioctl);
return;	
	//(my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, NULL, KDSETLED, *pstatus);
	//(my_driver->ops->ioctl) (my_tty, NULL, KDSETLED, RESTORE_LEDS);

	my_timer.expires = jiffies + BLINK_DELAY;
	add_timer(&my_timer);
}

static int kbleds_init( void)
{
	int i;

	printk(KERN_INFO "kbleds: loading\n");
	printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);

	for(i=0; i < MAX_NR_CONSOLES; i++){
		if(!vc_cons[i].d)
			break;
		printk(KERN_INFO "kbleds: poet_atkm: console[%i/%i] #%i, tty %lx\n", i, MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
			(unsigned long)vc_cons[i].d->port.tty);
	}
	printk(KERN_INFO "kbleds: finishing scan consoles!\n");
	
	my_tty = vc_cons[fg_console].d->port.tty;
	my_driver = my_tty->driver;
	printk(KERN_INFO "kbleds: tty driver magic %x\n", my_driver->magic);

	init_timer(&my_timer);
	my_timer.function = my_timer_func;
	my_timer.data = (unsigned long)&kbledstatus;
	my_timer.expires = jiffies + BLINK_DELAY;
	add_timer(&my_timer);
	return 0;
}

static void __exit kbleds_cleanup(void)
{
	printk(KERN_INFO  "kbleds: unloading!\n");

	del_timer(&my_timer);
	(my_driver->ops->ioctl) (my_tty, NULL, KDSETLED, RESTORE_LEDS);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);
