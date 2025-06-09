#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>
#include <linux/smp.h>

#define PROC_NAME "study_focus"

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

static int proc_show(struct seq_file *m, void *v) {
    unsigned long idle, total, d_idle, d_total;
    unsigned long idle_sec, active_sec;
    unsigned int focus;
    
    read_jiffies_vals(&idle, &total);
    
    if (prev_total == 0) {
        prev_idle = idle;
        prev_total = total;
        seq_printf(m, "initializing...\n");
        return 0;
    }
    
    d_idle = idle - prev_idle;
    d_total = total - prev_total;
    
    prev_idle = idle;
    prev_total = total;
    
    // Avoid division by zero
    if (d_total == 0) {
        seq_printf(m, "no activity detected\n");
        return 0;
    }
    
    idle_sec = d_idle / HZ;
    active_sec = (d_total - d_idle) / HZ;
    
    // Avoid division by zero
    if ((idle_sec + active_sec) == 0) {
        focus = 0;
    } else {
        focus = (active_sec * 100) / (idle_sec + active_sec);
    }
    
    seq_printf(m,
        "active_time_sec=%lu\nidle_time_sec=%lu\nfocus_percent=%u\n",
        active_sec, idle_sec, focus);
    
    return 0;
}

static int proc_open_cb(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open_cb,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init sf_init(void) {
    struct proc_dir_entry *entry;
    
    entry = proc_create(PROC_NAME, 0444, NULL, &proc_fops);
    if (!entry) {
        pr_err("Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }
    
    pr_info("study_focus module loaded\n");
    return 0;
}

static void __exit sf_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    pr_info("study_focus module unloaded\n");
}

module_init(sf_init);
module_exit(sf_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Study Focus via /proc/study_focus");
MODULE_AUTHOR("Kelompok 2");
