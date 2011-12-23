#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>	///task_struct
#include <linux/types.h>	///pid_t
#include <linux/mm.h>		///vm_area_struct

static int pid = 1;

module_param(pid, int, 0000);
MODULE_PARM_DESC(pid, "Process ID");

static int __init taskinfo_init(void)
{
	struct task_struct *task;
	struct mm_struct *a_mm;	///адресное пространство
	struct vm_area_struct *vma;	///области памяти
	u32 mem_start;
	u32 mem_len;

	//task = find_task_by_pid((pid_t)pid);
	task = pid_task(find_vpid((pid_t)pid), PIDTYPE_PID);

	if (!task) {
		printk(KERN_INFO "taskinfo: No find process with pid %d\n", pid);
		return 0;
	}

	a_mm = task->active_mm;
	vma = task->active_mm->mmap;

	mem_start = (u32)vma->vm_start;
	mem_len = (u32)(vma->vm_end - vma->vm_start);

	
	///информация из дескриптора
	printk(KERN_INFO "taskinfo: Process %d information:\n", task->pid);
	printk(KERN_INFO "taskinfo: NAME - %s\n", task->comm);
	printk(KERN_INFO "taskinfo: STATE - %u\n", (u32)task->state);

	///области памяти
	printk(KERN_INFO "taskinfo: START MEMORY AREA ADDRESS - 0x%08X\n", mem_start);
	printk(KERN_INFO "taskinfo:   END MEMORY AREA ADDRESS - 0x%08X\n", (u32)vma->vm_end);
	printk(KERN_INFO "taskinfo: MEMORY AREA LENGTH - 0x%08X\n", mem_len);

	///адреса сегментов
	printk(KERN_INFO "taskinfo: START CODE SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->start_code);
	printk(KERN_INFO "taskinfo:   END CODE SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->end_code);
	printk(KERN_INFO "taskinfo: CODE SEGMENT LENGTH - 0x%08X\n", (u32)(a_mm->end_code - a_mm->start_code));
	printk(KERN_INFO "taskinfo: START DATA SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->start_data);
	printk(KERN_INFO "taskinfo:   END DATA SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->end_data);
	printk(KERN_INFO "taskinfo: DATA SEGMENT LENGTH - 0x%08X\n", (u32)(a_mm->end_data - a_mm->start_data));
	printk(KERN_INFO "taskinfo: START BRK SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->start_brk);
	printk(KERN_INFO "taskinfo:   END BRK SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->brk);
	printk(KERN_INFO "taskinfo: BRK SEGMENT LENGTH - 0x%08X\n", (u32)(a_mm->brk - a_mm->start_brk));

	return 0;
}

static void __exit taskinfo_exit(void)
{
	return;
}

module_init(taskinfo_init);
module_exit(taskinfo_exit);

MODULE_LICENSE("GPL");
