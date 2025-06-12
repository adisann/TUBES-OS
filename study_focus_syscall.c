#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>  // Add this

// System call number - harus disesuaikan dengan arsitektur
#define __NR_get_study_focus_info 548

// Fungsi untuk membaca idle time dari /proc/stat

static unsigned long get_system_idle_time(void)
{
    struct file *file;
    char *buffer;
    char *line_start, *token;
    unsigned long idle_time = 0;
    loff_t pos = 0;
    ssize_t bytes_read;
    
    buffer = kmalloc(1024, GFP_KERNEL);
    if (!buffer) {
        printk(KERN_ERR "study_focus: Failed to allocate buffer\n");
        return 0;
    }
    
    file = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "study_focus: Cannot open /proc/stat\n");
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
            // Get idle time
            while (*token == ' ') token++;
            idle_time = simple_strtoul(token, NULL, 10) / HZ;
        }
    }
    
    filp_close(file, NULL);
    kfree(buffer);
    return idle_time;
}

// Implementasi system call
SYSCALL_DEFINE1(get_study_focus_info, struct study_focus_info __user *, info)
{
    struct study_focus_info kernel_info;
    unsigned long uptime_sec;
    unsigned long total_time;
    
    // Validasi parameter
    if (!info) {
        printk(KERN_ERR "study_focus: Invalid parameter\n");
        return -EINVAL;
    }
    
    // Dapatkan uptime sistem dalam detik
    uptime_sec = ktime_get_seconds();
    
    // Dapatkan idle time
    kernel_info.idle_time_sec = get_system_idle_time();
    
    // Hitung active time (uptime - idle time)
    if (uptime_sec > kernel_info.idle_time_sec) {
        kernel_info.active_time_sec = uptime_sec - kernel_info.idle_time_sec;
    } else {
        kernel_info.active_time_sec = uptime_sec / 4; // Estimasi sederhana
    }
    
    // Hitung total time dan focus percentage
    total_time = kernel_info.active_time_sec + kernel_info.idle_time_sec;
    
    if (total_time > 0) {
        kernel_info.focus_percent = (kernel_info.active_time_sec * 100) / total_time;
    } else {
        kernel_info.focus_percent = 0;
    }
    
    // Pastikan persentase tidak melebihi 100%
    if (kernel_info.focus_percent > 100) {
        kernel_info.focus_percent = 100;
    }
    
    // Copy data ke user space
    if (copy_to_user(info, &kernel_info, sizeof(kernel_info))) {
        printk(KERN_ERR "study_focus: Failed to copy data to user space\n");
        return -EFAULT;
    }
    
    // Log untuk debugging
    printk(KERN_INFO "study_focus: Active=%lus, Idle=%lus, Focus=%u%%\n", 
           kernel_info.active_time_sec, 
           kernel_info.idle_time_sec, 
           kernel_info.focus_percent);
    
    return 0;
}

// Export symbol untuk user space
EXPORT_SYMBOL(sys_get_study_focus_info);
