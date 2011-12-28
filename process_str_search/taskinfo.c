#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/sched.h> ///task_struct
#include <linux/types.h> ///pid_t
#include <linux/mm.h> ///vm_area_struct

#include <linux/fs.h>      // Needed by filp
#include <asm/uaccess.h>   // Needed by segment descriptors

MODULE_LICENSE("GPL");

struct proc_mem
{
  unsigned long mem_start;
  unsigned long mem_end;

  unsigned long code_start;
  unsigned long code_end;

  unsigned long data_start;
  unsigned long data_end;

  unsigned long brk_start;
  unsigned long brk_end;
};

static int pid = 1;
static char* pid_file_name = "/var/ptrace_pid";
static char* mem_file_name = "/var/ptrace_mem";
struct proc_mem proc_mem_;
int result;

//module_param(pid, int, 0000);
//MODULE_PARM_DESC(pid, "Process ID");

static int taskinfo_get_pid(const char* file_name)
{
  struct file *f;
  char buf[16];
  mm_segment_t fs;
  int result;

  memset( buf, 0 , sizeof(buf));

  f = filp_open(file_name, O_RDONLY, 0);
  if(f == NULL){
    printk(KERN_INFO "filp_open error!!.\n");
    return -1;
  }
  else{
    fs = get_fs();
    set_fs(get_ds());
    f->f_op->read(f, buf, 16, &f->f_pos);
    set_fs(fs);
  }
  filp_close(f,NULL);

  result = simple_strtol(buf, NULL, 10);
  if( result == LONG_MAX || result == LONG_MIN){
    printk(KERN_INFO "simple_strtol error!!.\n");
    return -1;
  }

  return result;
}

int write_mem_addresses(const char* file_name, const struct proc_mem* proc_mem_)
{
  struct file *f;
  char buf[256];
  mm_segment_t fs;
  int result;

  memset( buf, 0 , sizeof(buf));

  f = filp_open(file_name, O_CREAT | O_WRONLY, S_IRWXU);
  if(f == NULL){
    printk(KERN_ALERT "filp_open error!\n");
    return -1;
  }
  else{
    if (!f->f_op->write) { 
      //fput (f); 
      printk(KERN_ALERT "write error!\n");
      return -1; 
    }

    fs = get_fs();
    set_fs(get_ds());

    result = sprintf(buf, "%lu\n%lu\n%lu\n%lu\n%lu\n%lu\n%lu\n%lu\n",
                     proc_mem_->mem_start,
                     proc_mem_->mem_end,
                     proc_mem_->code_start,
                     proc_mem_->code_end,
                     proc_mem_->data_start,
                     proc_mem_->data_end,
                     proc_mem_->brk_start,
                     proc_mem_->brk_end
                     );
    if( result < 0)
      return -1;

    f->f_op->write(f, buf, strlen(buf), &f->f_pos);
    set_fs(fs);
  }
  filp_close(f,NULL);

  return 0;
}

static int __init taskinfo_init(void)
{
  struct task_struct *task;
  struct mm_struct *a_mm;	///адресное пространство
  struct vm_area_struct *vma;	///области памяти
  u32 mem_start;
  u32 mem_len;

  pid = taskinfo_get_pid( pid_file_name);

  if( pid < 1){
    printk(KERN_INFO " Error while getting pid: %d \n", pid);
    return 0;
  }

  //task = find_task_by_pid((pid_t)pid);
  task = pid_task(find_vpid((pid_t)pid), PIDTYPE_PID);

  if (!task) {
    printk(KERN_INFO "taskinfo: No find process with pid %d\n", pid);
    return 0;
  }

  a_mm = task->active_mm;
  vma = task->active_mm->mmap;

  ///области памяти
  proc_mem_.mem_start = vma->vm_start;
  proc_mem_.mem_end = vma->vm_end;

  ///адреса сегментов
  proc_mem_.code_start = a_mm->start_code;
  proc_mem_.code_end = a_mm->end_code;
  proc_mem_.data_start = a_mm->start_data;
  proc_mem_.data_end = a_mm->end_data;
  proc_mem_.brk_start = a_mm->start_brk;
  proc_mem_.brk_end = a_mm->brk;

  result = write_mem_addresses( mem_file_name, &proc_mem_);
  
  if( result < 0)
    printk(KERN_INFO "Error while wrtite mem addresses!\n");

  mem_start = (u32)vma->vm_start;
  mem_len = (u32)(vma->vm_end - vma->vm_start);
  
  ///информация из дескриптора
  printk(KERN_INFO "taskinfo: Process %d information:\n", task->pid);
  printk(KERN_INFO "taskinfo: NAME - %s\n", task->comm);
  printk(KERN_INFO "taskinfo: STATE - %u\n", (u32)task->state);

  ///области памяти
  printk(KERN_INFO "taskinfo: START MEMORY AREA ADDRESS - 0x%08X\n", mem_start);
  printk(KERN_INFO "taskinfo:  END MEMORY AREA ADDRESS - 0x%08X\n", (u32)vma->vm_end);
  printk(KERN_INFO "taskinfo: MEMORY AREA LENGTH - 0x%08X\n", mem_len);

  ///адреса сегментов
  printk(KERN_INFO "taskinfo: START CODE SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->start_code);
  printk(KERN_INFO "taskinfo:  END CODE SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->end_code);
  printk(KERN_INFO "taskinfo: CODE SEGMENT LENGTH - 0x%08X\n", (u32)(a_mm->end_code - a_mm->start_code));
  printk(KERN_INFO "taskinfo: START DATA SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->start_data);
  printk(KERN_INFO "taskinfo:  END DATA SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->end_data);
  printk(KERN_INFO "taskinfo: DATA SEGMENT LENGTH - 0x%08X\n", (u32)(a_mm->end_data - a_mm->start_data));
  printk(KERN_INFO "taskinfo: START BRK SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->start_brk);
  printk(KERN_INFO "taskinfo:  END BRK SEGMENT ADDRESS - 0x%08X\n", (u32)a_mm->brk);
  printk(KERN_INFO "taskinfo: BRK SEGMENT LENGTH - 0x%08X\n", (u32)(a_mm->brk - a_mm->start_brk));

  return 0;
}

static void __exit taskinfo_exit(void)
{
  return;
}

module_init(taskinfo_init);
module_exit(taskinfo_exit);
