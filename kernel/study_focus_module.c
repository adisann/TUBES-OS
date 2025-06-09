#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>
#include <linux/smp.h>

// Struktur data yang sama dengan user-space
struct study_focus_info {
    unsigned long active_time_sec;
    unsigned long idle_time_sec;
    unsigned int focus_percent;
};

static unsigned long prev_idle, prev_total;

static void read_jiffies_vals(unsigned long *idle, unsigned long *total) {
    int cpu = 0;
    u64 user, nice, system, idle_time, iowait, irq, softirq, steal;
    
    user = kcpustat_cpu(cpu).cpustat[CPUTIME_USER];
    nice = kcpustat_cpu(cpu).cpustat[CPUTIME_NICE];
    system = kcpustat_cpu(cpu).cpustat[CPUTIME_SYSTEM];
    idle_time = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
    iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
    irq = kcpustat_cpu(cpu).cpustat[CPUTIME_IRQ];
    softirq = kcpustat_cpu(cpu).cpustat[CPUTIME_SOFTIRQ];
    steal = kcpustat_cpu(cpu).cpustat[CPUTIME_STEAL];
    
    *idle = idle_time + iowait;
    *total = user + nice + system + idle_time + iowait + irq + softirq + steal;
}

// System call implementation
SYSCALL_DEFINE1(get_study_focus_info, struct study_focus_info __user *, info) {
    struct study_focus_info kinfo;
    unsigned long idle, total, d_idle, d_total;
    
    if (!info)
        return -EINVAL;
    
    read_jiffies_vals(&idle, &total);
    
    if (prev_total == 0) {
        // First call - initialize
        prev_idle = idle;
        prev_total = total;
        kinfo.active_time_sec = 0;
        kinfo.idle_time_sec = 0;
        kinfo.focus_percent = 0;
    } else {
        d_idle = idle - prev_idle;
        d_total = total - prev_total;
        
        prev_idle = idle;
        prev_total = total;
        
        if (d_total == 0) {
            kinfo.active_time_sec = 0;
            kinfo.idle_time_sec = 0;
            kinfo.focus_percent = 0;
        } else {
            kinfo.idle_time_sec = d_idle / HZ;
            kinfo.active_time_sec = (d_total - d_idle) / HZ;
            
            if ((kinfo.idle_time_sec + kinfo.active_time_sec) == 0) {
                kinfo.focus_percent = 0;
            } else {
                kinfo.focus_percent = (kinfo.active_time_sec * 100) / 
                                    (kinfo.idle_time_sec + kinfo.active_time_sec);
            }
        }
    }
    
    if (copy_to_user(info, &kinfo, sizeof(kinfo)))
        return -EFAULT;
    
    return 0;
}

static int __init sf_init(void) {
    pr_info("study_focus system call module loaded\n");
    return 0;
}

static void __exit sf_exit(void) {
    pr_info("study_focus system call module unloaded\n");
}

module_init(sf_init);
module_exit(sf_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Study Focus via /proc/study_focus");
MODULE_AUTHOR("Kelompok 2");

