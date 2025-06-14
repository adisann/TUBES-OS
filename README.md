# Panduan Ringkas Laporan Modul 9: System Call - Study Focus Monitor

**PENTING:**
- Pastikan Anda berada di direktori yang benar (`/home/xinu/`) sebelum menjalankan perintah-perintah terkait path `xinu/...`
- Gunakan editor teks pilihan Anda (`nano`, `vim`, dll.). Contoh di sini menggunakan `nano`

---

## Tahap 1: Membuat System Call Baru (`getstudystats`)

### 1. Buat File Header Struktur Data
**Perintah:**
```bash
touch xinu/include/study.h
```

**Isi kode:**
```bash
nano xinu/include/study.h
```

**Salin kode:**
```c
#ifndef _STUDY_H_
#define _STUDY_H_

struct study_stats {
    uint32 active_time;
    uint32 idle_time;
    uint32 focus_percent;
};

#endif
```

**Deskripsi Akademis Screenshot:** "Inisialisasi file header `study.h` dalam direktori `/xinu/include/` untuk mendefinisikan struktur data `study_stats` yang akan digunakan oleh system call Study Focus Monitor."

### 2. Buat File System Call Baru
**Perintah:**
```bash
touch xinu/system/getstudystats.c
```

**Deskripsi Akademis Screenshot:** "Inisialisasi file `getstudystats.c` dalam direktori `/xinu/system/` sebagai wadah implementasi system call baru, sesuai struktur direktori kernel Xinu."

### 3. Isi Kode `getstudystats.c`
**Perintah:**
```bash
nano xinu/system/getstudystats.c
```

**Salin Kode:**
```c
/* getstudystats.c - versi simpel dengan debug */
#include <xinu.h>
#include <study.h>

/* Global variables untuk tracking */
static uint32 last_command_time = 0;
static uint32 total_idle_time = 0;
static uint32 total_active_time = 0;
static uint32 system_start_time = 0;
static uint32 last_check_time = 0;

syscall getstudystats(struct study_stats *stats) {
    intmask mask;
    uint32 current_time;
    uint32 time_since_last_check;
    uint32 time_since_last_command;
    
    mask = disable();
    
    if (stats == NULL) {
        restore(mask);
        return SYSERR;
    }
    
    current_time = clktime;
    
    /* First call - initialize */
    if (system_start_time == 0) {
        system_start_time = current_time;
        last_command_time = current_time;
        last_check_time = current_time;
        
        stats->active_time = 0;
        stats->idle_time = 0;
        stats->focus_percent = 100;
        
        restore(mask);
        return OK;
    }
    
    /* Ada command baru = update waktu command terakhir */
    
    /* Hitung waktu yang sudah berlalu sejak check terakhir */
    time_since_last_check = current_time - last_check_time;
    time_since_last_command = current_time - last_command_time;
    
    /* Simpel: jika > 10 detik tidak ada command = idle */
    if (time_since_last_command > 10) {
        total_idle_time += time_since_last_check;
    } else {
        total_active_time += time_since_last_check;
    }
    last_command_time = current_time;
    
    /* Update stats */
    stats->active_time = total_active_time;
    stats->idle_time = total_idle_time;
    
    uint32 total_time = total_active_time + total_idle_time;
    if (total_time > 0) {
        stats->focus_percent = (total_active_time * 100) / total_time;
    } else {
        stats->focus_percent = 100;
    }
    
    last_check_time = current_time;
    
    /* Debug info */
    kprintf("\n=== Study Stats Debug ===\n");
    kprintf("Command detected: YES\n");
    kprintf("Time since last command: %d ticks\n", time_since_last_command);
    kprintf("Time since last check: %d ticks\n", time_since_last_check);
    kprintf("Status: %s\n", (time_since_last_command > 10) ? "IDLE" : "ACTIVE");
    kprintf("Active time: %d, Idle time: %d\n", total_active_time, total_idle_time);
    kprintf("===========================\n\n");
    
    restore(mask);
    return OK;
}


```





**Deskripsi Akademis Screenshot:** "Implementasi kode system call `getstudystats()` yang mengikuti template standar Xinu, mencakup penanganan interupsi dan penambahan `kprintf()` untuk verifikasi eksekusi Study Focus Monitor."

### 4. Deklarasikan System Call di `prototypes.h`
**Perintah:**
```bash
nano xinu/include/prototypes.h
```

**Tambahkan Baris:** (di bagian akhir sebelum `#endif`)
```c
extern syscall getstudystats(struct study_stats *); /* in file getstudystats.c */
```

**Deskripsi Akademis Screenshot:** "Penambahan prototipe fungsi `getstudystats()` ke `prototypes.h` agar system call dapat dikenali dan diakses secara global oleh komponen kernel Xinu lainnya."

### 5. Tambahkan Include di `xinu.h`
**Perintah:**
```bash
nano xinu/include/xinu.h
```

**Tambahkan Baris:** (setelah include lainnya)
```c
#include <study.h>
```

**Deskripsi Akademis Screenshot:** "Penambahan include `study.h` ke file header utama `xinu.h` untuk memastikan struktur data Study Focus Monitor dapat diakses di seluruh sistem."

### 6. Kompilasi Ulang Xinu (Pertama)
**Perintah:**
```bash
cd xinu/compile
make clean
make
```

**Deskripsi Akademis Screenshot:** "Kompilasi ulang kernel Xinu menggunakan `make clean` dan `make` untuk mengintegrasikan system call `getstudystats()` yang baru ke dalam image sistem operasi Xinu."

---

## Tahap 2: Menggunakan System Call `getstudystats` pada Perintah `uptime`

### 7. Edit File Perintah `uptime`
**Perintah:**
```bash
nano xinu/shell/xsh_uptime.c
```

**Tambahkan Baris Pemanggilan:** (sebelum `return 0;`)
```c
// ... (kode xsh_uptime.c lainnya) ...
printf("\n");

// Tambahkan Study Focus Monitor test
/* Simple interactive version untuk uptime.c */

struct study_stats stats;
char choice;

do {
    /* Clear screen sederhana */
    printf("\n\n\n");
    
    if (getstudystats(&stats) == OK) {
        printf("=== STUDY FOCUS MONITOR ===\n");
        printf("Active Time    : %d ticks (%d min)\n", 
               stats.active_time, stats.active_time/60);
        printf("Idle Time      : %d ticks (%d min)\n", 
               stats.idle_time, stats.idle_time/60);
        printf("Focus Score    : %d%%\n", stats.focus_percent);
        
        /* Simple visual indicator */
        printf("Focus Level    : ");
        if (stats.focus_percent >= 80) {
            printf("EXCELLENT! ****\n");
        } else if (stats.focus_percent >= 60) {
            printf("GOOD ***\n");
        } else if (stats.focus_percent >= 40) {
            printf("OK **\n");
        } else {
            printf("POOR *\n");
        }
        
        /* Simple progress bar */
        printf("Progress       : [");
        int i, bars = stats.focus_percent / 10;
        for (i = 0; i < 10; i++) {
            if (i < bars) printf("#");
            else printf("-");
        }
        printf("]\n");
        
        /* Quick tip */
        printf("Tip            : ");
        if (stats.focus_percent >= 80) {
            printf("Keep it up!\n");
        } else if (stats.focus_percent >= 60) {
            printf("Stay focused!\n");
        } else {
            printf("Remove distractions!\n");
        }
        
    } else {
        printf("ERROR: Cannot get study stats!\n");
    }
    
    printf("\n[R]efresh [Q]uit: ");
    choice = getchar();
    
    if (choice == 'r' || choice == 'R') {
        printf("Refreshing...\n");
        sleep(1);
    }
    
} while (choice != 'q' && choice != 'Q');

printf("\nSession complete! Thanks for studying.\n");
return 0;

```

**Deskripsi Akademis Screenshot:** "Modifikasi file `xsh_uptime.c` dengan menambahkan pemanggilan ke system call `getstudystats()` dan menampilkan statistik Study Focus Monitor saat perintah `uptime` dieksekusi."

### 8. Kompilasi Ulang Xinu (Kedua)
**Perintah:** (Pastikan masih di `xinu/compile`)
```bash
make clean
make
```

**Deskripsi Akademis Screenshot:** "Kompilasi ulang kernel Xinu kedua kalinya untuk memasukkan perubahan pada perintah `uptime` ke dalam image sistem operasi Xinu yang final."

---

## Tahap 3: Menjalankan dan Menguji System Call

### 9. Jalankan `minicom` (di development-system vm)
**Perintah:**
```bash
sudo minicom
```
(Password: `xinurocks`)

**Deskripsi Akademis Screenshot (Terminal `minicom` awal):** "Inisialisasi `minicom` pada development-system vm sebagai antarmuka komunikasi serial untuk berinteraksi dengan backend vm Xinu."

### 10. Start Backend VM (di VirtualBox)
**Aksi:** Nyalakan backend vm dari Oracle VM VirtualBox Manager. Tunggu hingga "Welcome to GRUB!".

**Deskripsi Akademis Screenshot (Backend vm di GRUB):** "Proses booting backend vm, menampilkan layar GRUB sebagai tahap awal sebelum memuat kernel Xinu yang telah dimodifikasi dengan Study Focus Monitor."

### 11. Login dan Tunggu Prompt `xsh$` (di `minicom`)
**Aksi:** Kembali ke jendela `minicom`. Tunggu output Xinu muncul dan diakhiri dengan prompt `xsh$`.

**Deskripsi Akademis Screenshot (`minicom` menampilkan `xsh$`):** "Shell prompt Xinu (`xsh$`) telah aktif pada `minicom`, menandakan sistem operasi Xinu berhasil dijalankan pada backend vm dan siap menerima perintah."

### 12. Uji Perintah `uptime` (di `minicom`)
**Perintah:**
```bash
uptime
```

**Deskripsi Akademis Screenshot (Output `uptime` dengan Study Focus Monitor):** "Hasil eksekusi perintah `uptime`. Selain informasi waktu operasional, tampil juga pesan 'Study Focus Monitor is working' dan statistik focus monitoring, yang memvalidasi keberhasilan pemanggilan system call `getstudystats()` dari dalam perintah `uptime`."

---

## File yang Dimodifikasi/Dibuat:

```
xinu/
├── include/
│   ├── study.h              (BARU)
│   ├── prototypes.h         (DIMODIFIKASI)
│   └── xinu.h              (DIMODIFIKASI)
├── system/
│   └── getstudystats.c     (BARU)
├── shell/
│   └── xsh_uptime.c        (DIMODIFIKASI)
└── compile/
    └── Makefile            (OTOMATIS - tidak perlu diubah)
```

---

## Screenshot yang Diperlukan untuk Laporan:

1. ✅ Pembuatan file `study.h`
2. ✅ Isi kode `study.h`
3. ✅ Pembuatan file `getstudystats.c`
4. ✅ Isi kode `getstudystats.c`
5. ✅ Modifikasi `prototypes.h`
6. ✅ Modifikasi `xinu.h`
7. ✅ Kompilasi pertama (`make clean` dan `make`)
8. ✅ Modifikasi `xsh_uptime.c`
9. ✅ Kompilasi kedua (`make clean` dan `make`)
10. ✅ Jalankan `minicom`
11. ✅ Boot backend VM (GRUB)
12. ✅ Prompt `xsh$` di `minicom`
13. ✅ Hasil perintah `uptime` dengan Study Focus Monitor

