#include <linux/study_focus_info.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/time.h>

// Modern file reading tanpa set_fs()
static unsigned long get_system_idle_time(void) {
    struct file *file;
    char *buffer;
    char *line_start, *token;
    unsigned long idle_time = 0;
    loff_t pos = 0;
    ssize_t bytes_read;
    
    buffer = kmalloc(1024, GFP_KERNEL);
    if (!buffer) return 0;
    
    file = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(file)) {
        kfree(buffer);
        return 0;
    }
    
    bytes_read = kernel_read(file, buffer, 1023, &pos);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        line_start = buffer;
        if (strncmp(line_start, "cpu ", 4) == 0) {
            token = line_start + 4;
            // Skip user, nice, system
            for (int i = 0; i < 3; i++) {
                while (*token == ' ') token++;
                while (*token != ' ' && *token != '\0') token++;
            }
            // Parse idle time
            while (*token == ' ') token++;
            idle_time = simple_strtoul(token, NULL, 10) / HZ;
        }
    }
    
    filp_close(file, NULL);
    kfree(buffer);
    return idle_time;
}

// Implementasi syscall yang benar
asmlinkage long sys_get_study_focus_info(struct study_focus_info __user *info) {
    struct study_focus_info kernel_info;
    unsigned long uptime_sec = ktime_get_seconds();
    
    if (!info) return -EINVAL;
    
    kernel_info.idle_time_sec = get_system_idle_time();
    kernel_info.active_time_sec = (uptime_sec > kernel_info.idle_time_sec) ? 
                                (uptime_sec - kernel_info.idle_time_sec) : 
                                (uptime_sec / 4);
    
    unsigned long total_time = kernel_info.active_time_sec + kernel_info.idle_time_sec;
    kernel_info.focus_percent = total_time ? 
                              (kernel_info.active_time_sec * 100) / total_time : 0;
    
    if (kernel_info.focus_percent > 100) kernel_info.focus_percent = 100;
    
    if (copy_to_user(info, &kernel_info, sizeof(kernel_info)))
        return -EFAULT;
        
    printk(KERN_INFO "study_focus: Active=%lus, Idle=%lus, Focus=%u%%\n",
           kernel_info.active_time_sec, kernel_info.idle_time_sec, kernel_info.focus_percent);
    
    return 0;
}
EXPORT_SYMBOL(sys_get_study_focus_info);
