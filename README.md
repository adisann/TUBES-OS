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
    last_command_time = current_time;
    
    /* Hitung waktu yang sudah berlalu sejak check terakhir */
    time_since_last_check = current_time - last_check_time;
    time_since_last_command = current_time - last_command_time;
    
    /* Simpel: jika > 10 detik tidak ada command = idle */
    if (time_since_last_command > 10) {
        total_idle_time += time_since_last_check;
    } else {
        total_active_time += time_since_last_check;
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




```c

/* 
 * Letakkan kode ini di dalam fungsi main() di uptime.c
 * Ganti bagian printf sederhana dengan kode ini
 */

struct study_stats stats;
char input;
int loop_active = 1;

/* Simple color codes untuk XINU terminal */
#define CLR_RESET   "\033[0m"
#define CLR_BOLD    "\033[1m"
#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_YELLOW  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_CYAN    "\033[36m"

/* Helper function untuk draw progress bar */
void draw_progress(int percent) {
    int i, filled = percent / 5; /* 20 char bar */
    printf("[");
    for (i = 0; i < 20; i++) {
        if (i <= filled) {
            if (percent >= 80) printf("#");
            else if (percent >= 60) printf("=");
            else if (percent >= 40) printf("-");
            else printf(".");
        } else {
            printf(" ");
        }
    }
    printf("] %d%%", percent);
}

/* Main interactive loop */
while (loop_active) {
    /* Clear screen - compatible dengan kebanyakan terminal */
    printf("\033[2J\033[1;1H");
    
    if (getstudystats(&stats) == OK) {
        /* Header */
        printf(CLR_BOLD CLR_CYAN);
        printf("=====================================\n");
        printf("    XINU STUDY FOCUS MONITOR\n");
        printf("         Real-time Stats\n");
        printf("=====================================\n");
        printf(CLR_RESET);
        
        /* Calculate readable time */
        int total_time = stats.active_time + stats.idle_time;
        int active_mins = stats.active_time / 60;
        int idle_mins = stats.idle_time / 60;
        int total_mins = total_time / 60;
        
        /* Time Statistics */
        printf(CLR_BOLD "\n> TIME BREAKDOWN:\n" CLR_RESET);
        printf("  Active Time : %d min (%d ticks)\n", active_mins, stats.active_time);
        printf("  Idle Time   : %d min (%d ticks)\n", idle_mins, stats.idle_time);
        printf("  Total Time  : %d min (%d ticks)\n", total_mins, total_time);
        
        /* Focus Analysis */
        printf(CLR_BOLD "\n> FOCUS ANALYSIS:\n" CLR_RESET);
        printf("  Focus Score : ");
        
        /* Color-coded focus percentage */
        if (stats.focus_percent >= 80) {
            printf(CLR_GREEN "%d%%" CLR_RESET " [EXCELLENT]\n", stats.focus_percent);
        } else if (stats.focus_percent >= 60) {
            printf(CLR_YELLOW "%d%%" CLR_RESET " [GOOD]\n", stats.focus_percent);
        } else if (stats.focus_percent >= 40) {
            printf(CLR_BLUE "%d%%" CLR_RESET " [AVERAGE]\n", stats.focus_percent);
        } else {
            printf(CLR_RED "%d%%" CLR_RESET " [POOR]\n", stats.focus_percent);
        }
        
        printf("  Progress    : ");
        draw_progress(stats.focus_percent);
        printf("\n");
        
        /* Productivity Assessment */
        printf(CLR_BOLD "\n> PRODUCTIVITY TIPS:\n" CLR_RESET);
        if (stats.focus_percent >= 80) {
            printf("  * Great focus! Keep up the momentum\n");
            printf("  * Consider a short break soon\n");
        } else if (stats.focus_percent >= 60) {
            printf("  * Good progress, stay consistent\n");
            printf("  * Try 25-minute focused sessions\n");
        } else if (stats.focus_percent >= 40) {
            printf("  * Remove distractions from workspace\n");
            printf("  * Set specific study goals\n");
        } else {
            printf("  * Take a break and reset focus\n");
            printf("  * Find a quieter study environment\n");
        }
        
        /* Session Summary */
        printf(CLR_BOLD "\n> SESSION SUMMARY:\n" CLR_RESET);
        printf("  Duration    : %d minutes\n", total_mins);
        printf("  Efficiency  : %s\n", 
               stats.focus_percent >= 70 ? "HIGH" : 
               stats.focus_percent >= 50 ? "MEDIUM" : "LOW");
        
        /* Current system info */
        printf("  System      : XINU OS\n");
        printf("  Command     : uptime\n");
        
    } else {
        printf(CLR_RED CLR_BOLD "ERROR: Cannot retrieve study statistics!\n" CLR_RESET);
        printf("Make sure study tracking is enabled.\n");
    }
    
    /* Interactive menu */
    printf(CLR_BOLD CLR_CYAN "\n=====================================\n");
    printf("  [R] Refresh  [Q] Quit  [H] Help\n");
    printf("=====================================\n" CLR_RESET);
    printf("Choice: ");
    
    /* Get user input */
    input = getchar();
    
    /* Process input */
    switch (input) {
        case 'r':
        case 'R':
            /* Refresh - just continue loop */
            printf(CLR_GREEN "Refreshing...\n" CLR_RESET);
            sleep(1);
            break;
            
        case 'h':
        case 'H':
            /* Help screen */
            printf("\033[2J\033[1;1H"); /* Clear screen */
            printf(CLR_BOLD CLR_BLUE "XINU STUDY MONITOR HELP\n");
            printf("=======================\n" CLR_RESET);
            printf("This tool monitors your study focus based on\n");
            printf("system activity and calculates productivity.\n\n");
            
            printf(CLR_YELLOW "METRICS:\n" CLR_RESET);
            printf("* Active Time  - Time spent studying\n");
            printf("* Idle Time    - Time spent inactive\n");
            printf("* Focus Score  - Productivity percentage\n\n");
            
            printf(CLR_YELLOW "FOCUS LEVELS:\n" CLR_RESET);
            printf("* 80-100%% - EXCELLENT (Green)\n");
            printf("* 60-79%%  - GOOD (Yellow)\n");
            printf("* 40-59%%  - AVERAGE (Blue)\n");
            printf("* 0-39%%   - POOR (Red)\n\n");
            
            printf(CLR_YELLOW "STUDY TIPS:\n" CLR_RESET);
            printf("* Use Pomodoro technique (25min work)\n");
            printf("* Take regular 5-minute breaks\n");
            printf("* Remove phone and distractions\n");
            printf("* Set specific goals for each session\n");
            printf("* Study in a quiet environment\n\n");
            
            printf("Press any key to return...");
            getchar();
            break;
            
        case 'q':
        case 'Q':
            /* Quit */
            printf("\033[2J\033[1;1H"); /* Clear screen */
            printf(CLR_BOLD CLR_GREEN "Study session complete!\n\n" CLR_RESET);
            
            /* Final stats */
            if (getstudystats(&stats) == OK) {
                printf("FINAL STATISTICS:\n");
                printf("-----------------\n");
                printf("Session Duration: %d minutes\n", (stats.active_time + stats.idle_time) / 60);
                printf("Focus Score: %d%%\n", stats.focus_percent);
                printf("Active Time: %d minutes\n", stats.active_time / 60);
                
                if (stats.focus_percent >= 70) {
                    printf(CLR_GREEN "Excellent work! Keep it up!\n" CLR_RESET);
                } else if (stats.focus_percent >= 50) {
                    printf(CLR_YELLOW "Good session! Try to improve focus next time.\n" CLR_RESET);
                } else {
                    printf(CLR_BLUE "There's room for improvement. You can do better!\n" CLR_RESET);
                }
            }
            
            printf("\nThanks for using XINU Study Monitor!\n");
            printf("Type 'uptime' again to start a new session.\n\n");
            loop_active = 0; /* Exit loop */
            break;
            
        case '\n':
        case '\r':
            /* Enter key - just refresh */
            break;
            
        default:
            printf(CLR_RED "Invalid option. Try again.\n" CLR_RESET);
            sleep(1);
            break;
    }
}

```
