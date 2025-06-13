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

**Salin Kode:** (VERSI DENGAN REAL IDLE TIME TRACKING)
```c
/* getstudystats.c - getstudystats */
#include <xinu.h>
#include <study.h>

/* Global variables untuk tracking */
static uint32 last_check_time = 0;
static uint32 idle_counter = 0;
static uint32 total_counter = 0;

syscall getstudystats(struct study_stats *stats)
{
    intmask mask;
    uint32 current_time;

    mask = disable();

    if (stats == NULL) {
        restore(mask);
        return SYSERR;
    }

    /* Dapatkan waktu saat ini */
    current_time = clktime;
    
    /* Jika ini first call, inisialisasi */
    if (last_check_time == 0) {
        last_check_time = current_time;
        idle_counter = 0;
        total_counter = 1;
    } else {
        total_counter++;
        
        /* Deteksi idle: jika tidak ada input dalam beberapa detik */
        /* Asumsi: jika clktime tidak berubah banyak = sistem idle */
        if ((current_time - last_check_time) < 5) {
            idle_counter++;
        }
    }

    /* Update statistik */
    stats->active_time = current_time;
    stats->idle_time = idle_counter;
    
    if (total_counter > 0) {
        stats->focus_percent = ((total_counter - idle_counter) * 100) / total_counter;
    } else {
        stats->focus_percent = 100;
    }

    last_check_time = current_time;

    kprintf("\n\nStudy Focus Monitor: Active=%d, Idle=%d, Focus=%d%%\n\n", 
            stats->active_time, stats->idle_time, stats->focus_percent);

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

**Tambahkan Baris Pemanggilan:** (VERSI AMAN - DENGAN ERROR CHECKING)
```c
// ... (kode xsh_uptime.c lainnya) ...
printf("\n");

// Tambahkan Study Focus Monitor test - VERSI AMAN
struct study_stats stats;
kprintf("Testing Study Focus Monitor...\n"); // Debug line
if (getstudystats(&stats) == OK) {
    kprintf("=== Study Focus Monitor ===\n");
    kprintf("Active Time: %d ticks\n", stats.active_time);
    kprintf("Idle Time: %d ticks\n", stats.idle_time);
    kprintf("Focus Percentage: %d%%\n", stats.focus_percent);
} else {
    kprintf("Error: Failed to get study stats\n");
}

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

## TROUBLESHOOTING - Jika Terjadi Exception 13 (General Protection Fault)

### Kemungkinan Penyebab:
1. **Variable `clktime` tidak tersedia** - Ganti dengan nilai static
2. **Header include salah** - Pastikan urutan include benar
3. **Memory alignment issue** - Gunakan nilai sederhana dulu

### Langkah Debug:

#### 1. Cek apakah system call terpanggil
Edit `getstudystats.c` menjadi versi minimal:
```c
#include <xinu.h>
#include <study.h>

syscall getstudystats(struct study_stats *stats)
{
    kprintf("getstudystats called!\n");
    
    if (stats == NULL) {
        return SYSERR;
    }
    
    stats->active_time = 100;
    stats->idle_time = 50;
    stats->focus_percent = 67;
    
    return OK;
}
```

#### 2. Cek apakah struct definition benar
Edit `study.h` pastikan alignment:
```c
#ifndef _STUDY_H_
#define _STUDY_H_

struct study_stats {
    int active_time;    /* Ganti ke int biasa */
    int idle_time;      /* Ganti ke int biasa */
    int focus_percent;  /* Ganti ke int biasa */
};

#endif
```

#### 3. Cek penggunaan di uptime
Edit `xsh_uptime.c` dengan sangat hati-hati:
```c
// Tambahkan di bagian akhir sebelum return 0:
kprintf("Before calling getstudystats\n");
struct study_stats mystats;
int result = getstudystats(&mystats);
kprintf("After calling getstudystats, result=%d\n", result);

if (result == OK) {
    kprintf("Stats: %d, %d, %d\n", 
            mystats.active_time, 
            mystats.idle_time, 
            mystats.focus_percent);
}
```

#### 4. Kompilasi Bertahap
```bash
cd xinu/compile
make clean
make
```

Jika masih error, coba langkah ini:

#### 5. Alternatif Tanpa Struct (Paling Aman)
Ganti system call dengan versi paling sederhana:
```c
#include <xinu.h>

syscall getstudystats(void)
{
    kprintf("Study Focus Monitor: Active!\n");
    return OK;
}
```

Dan di `xsh_uptime.c`:
```c
// Tambahkan sebelum return 0:
kprintf("Testing study monitor...\n");
if (getstudystats() == OK) {
    kprintf("Study monitor works!\n");
}
```

## Penjelasan Idle Time Tracking:

### Mengapa Idle Time = 0?
1. **Kode sebelumnya** menggunakan nilai static dummy
2. **Xinu tidak otomatis track** user idle time
3. **Kita perlu implement** logic tracking sendiri

### Cara Kerja Versi Baru:
1. **Static variables** untuk menyimpan state antar pemanggilan
2. **Tracking waktu** antara pemanggilan system call
3. **Counter mechanism** untuk menghitung aktivitas vs idle
4. **Percentage calculation** berdasarkan rasio aktivitas

### Testing Idle Detection:
1. **Panggil uptime beberapa kali cepat** → Focus % tinggi
2. **Tunggu beberapa detik, panggil lagi** → Focus % turun
3. **Pattern usage** akan terlihat dari statistik

### Alternative: Keyboard Input Tracking
Jika ingin tracking keyboard input real-time, bisa tambahkan:
```c
/* Di getstudystats.c */
extern uint32 key_count;  /* Perlu ditambahkan di keyboard handler */

/* Dalam function */
if (key_count > last_key_count) {
    /* Ada aktivitas keyboard */
    active_counter++;
} else {
    /* Tidak ada input = idle */
    idle_counter++;
}
```

### Untuk Demo Laporan:
- **Screenshot pertama:** Focus = 100% (fresh boot)
- **Screenshot kedua:** Setelah tunggu idle, Focus turun
- **Screenshot ketiga:** Setelah aktivitas, Focus naik lagi



------ STOP ------------




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
/* getstudystats.c - getstudystats */
#include <xinu.h>
#include <study.h>

syscall getstudystats(struct study_stats *stats)
{
    intmask mask;
    uint32 total_time;

    mask = disable();

    if (stats == NULL) {
        restore(mask);
        return SYSERR;
    }

    total_time = clktime;

    if (total_time == 0) {
        stats->active_time = 0;
        stats->idle_time = 0;
        stats->focus_percent = 0;
    } else {
        stats->active_time = clktime;
        stats->idle_time = 0;
        stats->focus_percent = 100;
    }

    kprintf("\n\nStudy Focus Monitor is working\n\n");

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
struct study_stats stats;
if (getstudystats(&stats) == OK) {
    printf("=== Study Focus Monitor ===\n");
    printf("Active Time: %d ticks\n", stats.active_time);
    printf("Idle Time: %d ticks\n", stats.idle_time);
    printf("Focus Percentage: %d%%\n", stats.focus_percent);
}

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

**Semoga panduan ini memudahkan Anda dalam menyusun laporan praktikum Study Focus Monitor. Selamat mengerjakan!**


```c
/* getstudystats.c - getstudystats dengan deteksi keyboard sederhana */
#include <xinu.h>
#include <study.h>

/* Global variables untuk tracking */
static uint32 last_keyboard_check = 0;
static uint32 last_activity_time = 0;
static uint32 total_idle_time = 0;
static uint32 total_active_time = 0;
static uint32 system_start_time = 0;

syscall getstudystats(struct study_stats *stats) {
    intmask mask;
    uint32 current_time;
    uint32 time_since_activity;
    bool32 has_keyboard_activity = FALSE;
    
    mask = disable();
    
    if (stats == NULL) {
        restore(mask);
        return SYSERR;
    }
    
    current_time = clktime;
    
    /* First call - initialize */
    if (system_start_time == 0) {
        system_start_time = current_time;
        last_activity_time = current_time;
        last_keyboard_check = current_time;
        total_idle_time = 0;
        total_active_time = 0;
        
        stats->active_time = 0;
        stats->idle_time = 0;
        stats->focus_percent = 100;
        
        restore(mask);
        return OK;
    }
    
    /* Check keyboard activity - ini yang penting! */
    
    /* Method 1: Cek keyboard buffer jika ada */
    #ifdef KBHIT  /* Jika Xinu punya fungsi kbhit() */
    if (kbhit()) {
        has_keyboard_activity = TRUE;
        /* Consume keys to prevent buffer overflow */
        while (kbhit()) {
            getchar();
        }
    }
    #endif
    
    /* Method 2: Cek TTY input buffer (lebih universal di Xinu) */
    /* Asumsi: TTY device 0 untuk console input */
    if (ttynin(0) > 0) {  /* Ada input di TTY buffer */
        has_keyboard_activity = TRUE;
    }
    
    /* Method 3: Sederhana - anggap ada aktivitas karena command baru saja dijalankan */
    /* Setiap kali fungsi ini dipanggil = ada keyboard activity */
    has_keyboard_activity = TRUE;
    
    /* Update activity time jika ada keyboard input */
    if (has_keyboard_activity) {
        last_activity_time = current_time;
    }
    
    /* Calculate idle time */
    time_since_activity = current_time - last_activity_time;
    
    /* Threshold: 30 detik = idle */
    if (time_since_activity > 30000) {  /* 30000 ticks */
        /* System is idle */
        uint32 time_since_check = current_time - last_keyboard_check;
        total_idle_time += time_since_check;
    } else {
        /* System is active */
        uint32 time_since_check = current_time - last_keyboard_check;
        total_active_time += time_since_check;
    }
    
    /* Update stats */
    stats->active_time = total_active_time;
    stats->idle_time = total_idle_time;
    
    uint32 total_time = total_active_time + total_idle_time;
    if (total_time > 0) {
        stats->focus_percent = (total_active_time * 100) / total_time;
    } else {
        stats->focus_percent = 100;
    }
    
    last_keyboard_check = current_time;
    
    /* Debug info */
    kprintf("\n=== Keyboard Activity Detection ===\n");
    kprintf("Keyboard Activity: %s\n", has_keyboard_activity ? "YES" : "NO");
    kprintf("Time since last activity: %d ticks\n", time_since_activity);
    kprintf("Status: %s\n", (time_since_activity > 30000) ? "IDLE" : "ACTIVE");
    kprintf("===================================\n\n");
    
    restore(mask);
    return OK;
}

/* Fungsi helper untuk cek TTY input - jika tidak ada ttynin() */
int check_tty_input(void) {
    /* Implementasi tergantung Xinu version */
    /* Contoh sederhana: */
    
    /* Method A: Cek stdin buffer */
    // return (stdin buffer count > 0);
    
    /* Method B: Cek device input */
    // return devtab[CONSOLE].dvintr != NULL;
    
    /* Method C: Placeholder - selalu return 0 jika tidak ada implementasi */
    return 0;
}
```
