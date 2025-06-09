#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>

#define PROC_NAME "study_focus"

static unsigned long prev_idle, prev_total;

static void read_jiffies_vals(unsigned long *idle, unsigned long *total) {
    struct kernel_cpustat *st = &kcpustat_cpu(0);
    unsigned long iowait = st->cpustat[CPUTIME_IOWAIT];

    *idle = st->cpustat[CPUTIME_IDLE] + iowait;
    *total = st->cpustat[CPUTIME_USER]
           + st->cpustat[CPUTIME_NICE]
           + st->cpustat[CPUTIME_SYSTEM]
           + *idle
           + st->cpustat[CPUTIME_IRQ]
           + st->cpustat[CPUTIME_SOFTIRQ]
           + st->cpustat[CPUTIME_STEAL];
}

static int proc_show(struct seq_file *m, void *v) {
    unsigned long idle, total, d_idle, d_total;
    unsigned long idle_sec, active_sec;
    unsigned int focus;

    read_jiffies_vals(&idle, &total);
    if (prev_total == 0) {
        prev_idle  = idle;
        prev_total = total;
        seq_printf(m, "initializing...\n");
        return 0;
    }
    d_idle  = idle  - prev_idle;
    d_total = total - prev_total;
    prev_idle  = idle;
    prev_total = total;

    idle_sec   = d_idle  / HZ;
    active_sec = (d_total - d_idle) / HZ;
    focus      = (active_sec * 100) / (idle_sec + active_sec);

    seq_printf(m,
       "active_time_sec=%lu\nidle_time_sec=%lu\nfocus_percent=%u\n",
       active_sec, idle_sec, focus);
    return 0;
}

static int proc_open_cb(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct file_operations proc_fops = {
    .owner   = THIS_MODULE,
    .open    = proc_open_cb,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};

static int __init sf_init(void) {
    proc_create(PROC_NAME, 0444, NULL, &proc_fops);
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
