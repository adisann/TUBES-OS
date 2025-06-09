#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <time.h>

// Definisi nomor system call (sesuaikan dengan kernel)
#define __NR_get_study_focus_info 548

// Struktur data yang sama dengan kernel
struct study_focus_info {
    unsigned long active_time_sec;
    unsigned long idle_time_sec;
    unsigned int focus_percent;
};

// Fungsi untuk menampilkan header yang menarik
void print_header() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    📚 STUDY FOCUS MONITOR 📚                 ║\n");
    printf("║                   Pantau Fokus Belajar Anda!                ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// Fungsi untuk konversi detik ke format jam:menit:detik
void format_time(unsigned long seconds, char *buffer) {
    unsigned long hours = seconds / 3600;
    unsigned long minutes = (seconds % 3600) / 60;
    unsigned long secs = seconds % 60;
    
    if (hours > 0) {
        sprintf(buffer, "%luj %lum %lud", hours, minutes, secs);
    } else if (minutes > 0) {
        sprintf(buffer, "%lum %lud", minutes, secs);
    } else {
        sprintf(buffer, "%lud", secs);
    }
}

// Fungsi untuk menampilkan bar progress fokus
void print_focus_bar(unsigned int focus_percent) {
    int bar_length = 30;
    int filled = (focus_percent * bar_length) / 100;
    
    printf("   Fokus: [");
    
    // Bagian yang terisi (hijau)
    for (int i = 0; i < filled; i++) {
        printf("█");
    }
    
    // Bagian yang kosong
    for (int i = filled; i < bar_length; i++) {
        printf("░");
    }
    
    printf("] %u%%\n", focus_percent);
}

// Fungsi untuk memberikan motivasi berdasarkan tingkat fokus
void give_motivation(unsigned int focus_percent) {
    printf("\n   💬 Motivasi: ");
    
    if (focus_percent >= 80) {
        printf("Luar biasa! 🌟 Fokus Anda sangat tinggi!");
    } else if (focus_percent >= 60) {
        printf("Bagus! 👍 Pertahankan konsistensi belajar Anda!");
    } else if (focus_percent >= 40) {
        printf("Cukup baik! 📈 Masih bisa ditingkatkan lagi!");
    } else if (focus_percent >= 20) {
        printf("Perlu improvement! 💪 Kurangi distraksi ya!");
    } else {
        printf("Focus time! ⏰ Saatnya lebih serius belajar!");
    }
    printf("\n");
}

// Fungsi untuk menampilkan tips produktivitas
void show_productivity_tips() {
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                    💡 TIPS PRODUKTIVITAS                    │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ • Gunakan teknik Pomodoro (25 menit fokus, 5 menit break)  │\n");
    printf("│ • Matikan notifikasi media sosial saat belajar             │\n");
    printf("│ • Siapkan cemilan dan minuman sebelum mulai belajar        │\n");
    printf("│ • Buat to-do list untuk melacak progress harian            │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

// Fungsi untuk menyimpan log harian
void save_daily_log(struct study_focus_info *info) {
    FILE *log_file;
    time_t now;
    struct tm *local_time;
    char timestamp[64];
    
    // Dapatkan timestamp
    time(&now);
    local_time = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);
    
    // Buka file log (append mode)
    log_file = fopen("study_focus_log.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "%s | Active: %lus | Idle: %lus | Focus: %u%%\n",
                timestamp, info->active_time_sec, info->idle_time_sec, info->focus_percent);
        fclose(log_file);
        printf("\n   📝 Log tersimpan di: study_focus_log.txt\n");
    }
}

int main(int argc, char *argv[]) {
    struct study_focus_info info;
    char active_time_str[50], idle_time_str[50];
    int show_tips = 0;
    int save_log = 0;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tips") == 0) {
            show_tips = 1;
        } else if (strcmp(argv[i], "--log") == 0) {
            save_log = 1;
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [--tips] [--log] [--help]\n", argv[0]);
            printf("  --tips  : Tampilkan tips produktivitas\n");
            printf("  --log   : Simpan hasil ke file log\n");
            printf("  --help  : Tampilkan bantuan ini\n");
            return 0;
        }
    }
    
    print_header();
    
    // Panggil system call
    long result = syscall(__NR_get_study_focus_info, &info);
    
    if (result == -1) {
        printf("❌ Error: Gagal mengakses system call\n");
        printf("   Pastikan kernel sudah dikompilasi dengan system call ini!\n");
        printf("   Error detail: %s\n", strerror(errno));
        return 1;
    }
    
    // Format waktu untuk display
    format_time(info.active_time_sec, active_time_str);
    format_time(info.idle_time_sec, idle_time_str);
    
    // Tampilkan hasil dengan format menarik
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                     📊 LAPORAN FOKUS                        │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│                                                             │\n");
    printf("   ⏱️  Waktu Aktif  : %s\n", active_time_str);
    printf("   😴 Waktu Idle   : %s\n", idle_time_str);
    printf("│                                                             │\n");
    
    print_focus_bar(info.focus_percent);
    
    printf("│                                                             │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    // Berikan motivasi
    give_motivation(info.focus_percent);
    
    // Tampilkan tips jika diminta
    if (show_tips) {
        show_productivity_tips();
    }
    
    // Simpan log jika diminta  
    if (save_log) {
        save_daily_log(&info);
    }
    
    printf("\n   🎯 Terus semangat belajar! Sukses menanti! 🎓\n\n");
    
    return 0;
}
