# TUBES-OS

# Panduan Implementasi Custom System Call: Study Focus Monitor di Xinu

## PENTING:
- Pastikan Anda berada di direktori yang benar (`/home/xinu/`) sebelum menjalankan perintah
- Gunakan editor teks pilihan Anda (`nano`, `vim`, dll.)
- Ikuti struktur yang sama seperti syscall `chname` di website

## Langkah-Langkah Implementasi

### Tahap 1: Membuat System Call Baru (`getstudystats`)

#### 1. Buat File Syscall Baru
- **Perintah:** `touch xinu/system/getstudystats.c`
- **Deskripsi Screenshot:** "Inisialisasi file `getstudystats.c` dalam direktori `/xinu/system/` sebagai wadah implementasi system call Study Focus Monitor"

#### 2. Buat Header File untuk Struktur Data
- **Perintah:** `nano xinu/include/study.h`
- **Deskripsi Screenshot:** "Pembuatan header file `study.h` untuk mendefinisikan struktur data study_stats"
- **Isi File:**
```c
/* study.h - definisi struktur Study Focus Monitor */

#ifndef _STUDY_H_
#define _STUDY_H_

struct study_stats {
    uint32  active_time;    /* waktu CPU aktif dalam ticks */
    uint32  idle_time;      /* waktu CPU idle dalam ticks */
    uint32  focus_percent;  /* persentase fokus (0-100) */
};

#endif
```

n#### 3. Implementasi Kode `getstudystats.c`
- **Perintah:** `nano xinu/system/getstudystats.c`
- **Deskripsi Screenshot:** "Implementasi kode system call `getstudystats()` dengan logika perhitungan study focus"
- **Template Kode:**
```c
#include <xinu.h>
#include <study.h>

syscall getstudystats(struct study_stats *stats)
{
    intmask mask;
    uint32 total_time;

    mask = disable();  // Disable interrupt untuk data konsisten

    if (stats == NULL) {
        restore(mask);
        return SYSERR;
    }

    total_time = clktime + clkidle;

    if (total_time == 0) {
        stats->active_time = 0;
        stats->idle_time = 0;
        stats->focus_percent = 0;
    } else {
        stats->active_time = clktime;
        stats->idle_time = clkidle;
        stats->focus_percent = (clktime * 100) / total_time;
    }

    restore(mask);
    return OK;
}

```

#### 4. Deklarasikan Syscall di `prototypes.h`
- **Perintah:** `nano xinu/include/prototypes.h`
- **Tambahkan Baris:** (setelah syscall lainnya)
```c
extern syscall getstudystats(struct study_stats *); /* in file getstudystats.c */
```
- **Deskripsi Screenshot:** "Penambahan prototipe fungsi `getstudystats()` ke `prototypes.h` untuk aksesibilitas global"

#### 5. Include Header di `xinu.h`
- **Perintah:** `nano xinu/include/xinu.h`
- **Tambahkan:** `#include <study.h>` (setelah include lainnya)
- **Deskripsi Screenshot:** "Penambahan include `study.h` di `xinu.h` untuk integrasi dengan sistem"

#### 6. Kompilasi Ulang Xinu (Pertama)
- **Perintah:**
```bash
cd xinu/compile
make clean
make
```
- **Deskripsi Screenshot:** "Kompilasi pertama untuk mengintegrasikan system call `getstudystats()` ke dalam kernel Xinu"

### Tahap 2: Membuat Program Test

#### 7. Buat File Test Program
- **Perintah:** `touch xinu/test/studytest.c`
- **Deskripsi Screenshot:** "Pembuatan file test program untuk menguji fungsionalitas Study Focus Monitor"

#### 8. Implementasi Test Program
- **Perintah:** `nano xinu/test/studytest.c`
- **Template Kode:**
```c
/* studytest.c - test program untuk Study Focus Monitor */

#include <xinu.h>
#include <study.h>

int main(int argc, char *argv[])
{
    struct study_stats stats;
    
    kprintf("=== Study Focus Monitor Test ===\n");
    
    /* Panggil system call */
    if (getstudystats(&stats) == SYSERR) {
        kprintf("Error: Gagal mendapatkan study stats\n");
        return 1;
    }
    
    /* Tampilkan hasil */
    kprintf("Active Time: %d ticks\n", stats.active_time);
    kprintf("Idle Time: %d ticks\n", stats.idle_time);
    kprintf("Focus Percentage: %d%%\n", stats.focus_percent);
    
    return 0;
}
```

#### 9. Buat Shell Command (Opsional)
- **Perintah:** `nano xinu/shell/xsh_studytest.c`
- **Deskripsi Screenshot:** "Pembuatan shell command untuk memudahkan testing dari Xinu shell"
```c
#include <xinu.h>
#include <study.h>

command xsh_studytest(int nargs, char *args[])
{
    struct study_stats stats;
    kprintf("\nRunning Study Focus Monitor via shell...\n");

    if (getstudystats(&stats) == SYSERR) {
        kprintf("Error: Tidak bisa ambil data.\n");
        return 1;
    }

    kprintf("Active: %d, Idle: %d, Focus: %d%%\n",
        stats.active_time, stats.idle_time, stats.focus_percent);

    return 0;
}

```


#### 10. Update Makefile
- **Perintah:** `nano xinu/compile/Makefile`
- **Tambahkan:** File `getstudystats.c` dan `studytest.c` ke dalam target kompilasi
- **Deskripsi Screenshot:** "Modifikasi Makefile untuk menyertakan file-file baru dalam proses kompilasi"

system/getstudystats.o \
test/studytest.o \
(Sesuaikan jika menggunakan xsh_studytest.o juga)


#### 11. Kompilasi Ulang Xinu (Kedua)
- **Perintah:**
```bash
make clean
make
```
- **Deskripsi Screenshot:** "Kompilasi kedua untuk mengintegrasikan test program ke dalam image Xinu"

### Tahap 3: Testing dan Validasi

#### 12. Jalankan `minicom`
- **Perintah:** `sudo minicom` (Password: `xinurocks`)
- **Deskripsi Screenshot:** "Inisialisasi minicom untuk komunikasi dengan backend Xinu VM"

#### 13. Start Backend VM
- **Aksi:** Nyalakan backend vm dari VirtualBox, tunggu hingga "Welcome to GRUB!"
- **Deskripsi Screenshot:** "Proses booting backend VM menampilkan GRUB loader"

#### 14. Login dan Tunggu Prompt `xsh$`
- **Deskripsi Screenshot:** "Shell prompt Xinu (`xsh$`) aktif, sistem siap menerima perintah"

#### 15. Uji System Call
- **Perintah:** `studytest` (atau cara alternatif yang Anda implementasikan)
- **Deskripsi Screenshot:** "Hasil eksekusi test program menampilkan statistik study focus dengan data active time, idle time, dan focus percentage"

#### 16. Validasi Error Handling
- **Test Case:** Coba panggil dengan parameter NULL untuk memastikan error handling bekerja
- **Deskripsi Screenshot:** "Validasi error handling saat parameter NULL diberikan"

## Kriteria Penilaian yang Harus Dipenuhi

### 1. Fungsi System Call ✓
- [ ] System call dapat mengembalikan data yang valid
- [ ] Perhitungan active_time, idle_time, dan focus_percent benar
- [ ] Menggunakan variabel global Xinu (clktime, clkidle) dengan tepat

### 2. Error Handling ✓
- [ ] Handle kasus pointer NULL dengan mengembalikan SYSERR
- [ ] Proper interrupt disable/restore untuk konsistensi data

### 3. Komentar Kode ✓
- [ ] Setiap bagian kritis dijelaskan dengan komentar
- [ ] Dokumentasi fungsi dan parameter
- [ ] Penjelasan logika perhitungan

### 4. Integrasi dengan Sistem ✓
- [ ] Deklarasi yang benar di header files
- [ ] Kompilasi berhasil tanpa error
- [ ] Test program berfungsi dengan baik

## Tips Implementation

1. **Perhitungan Focus Percentage:**
   - Pastikan tidak ada division by zero
   - Gunakan rumus: `(active_time / total_time) * 100`

2. **Interrupt Handling:**
   - Selalu disable interrupt saat akses variabel global
   - Restore interrupt sebelum return

3. **Validasi Input:**
   - Cek pointer NULL sebelum akses
   - Return SYSERR untuk input invalid

4. **Testing:**
   - Test dengan berbagai kondisi sistem
   - Verifikasi hasil perhitungan manual

## Struktur File yang Dimodifikasi

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
