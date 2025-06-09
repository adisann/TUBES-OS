# Makefile untuk Study Focus System Call Project
# Tugas Sistem Operasi - Custom System Call

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = study_focus_monitor
SOURCE = user_get_focus_info.c

# Warna untuk output yang menarik
GREEN = \033[0;32m
YELLOW = \033[1;33m
RED = \033[0;31m
NC = \033[0m # No Color

.PHONY: all clean install test help

all: $(TARGET)
	@echo "$(GREEN)✅ Build berhasil!$(NC)"
	@echo "$(YELLOW)📋 Gunakan 'make help' untuk melihat opsi lainnya$(NC)"

$(TARGET): $(SOURCE)
	@echo "$(YELLOW)🔨 Kompilasi user program...$(NC)"
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	@echo "$(YELLOW)🧹 Membersihkan file build...$(NC)"
	rm -f $(TARGET)
	rm -f study_focus_log.txt
	@echo "$(GREEN)✅ Clean berhasil!$(NC)"

install: $(TARGET)
	@echo "$(YELLOW)📦 Installing ke /usr/local/bin...$(NC)"
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@echo "$(GREEN)✅ Install berhasil! Gunakan 'study_focus_monitor' dari mana saja$(NC)"

test: $(TARGET)
	@echo "$(YELLOW)🧪 Menjalankan test program...$(NC)"
	./$(TARGET) --tips --log
	@echo ""
	@echo "$(GREEN)✅ Test selesai! Check file study_focus_log.txt$(NC)"

# Target untuk demo dengan berbagai opsi
demo: $(TARGET)
	@echo "$(YELLOW)🎬 Demo 1: Basic usage$(NC)"
	./$(TARGET)
	@echo ""
	@echo "$(YELLOW)🎬 Demo 2: Dengan tips$(NC)"
	./$(TARGET) --tips
	@echo ""
	@echo "$(YELLOW)🎬 Demo 3: Dengan logging$(NC)"
	./$(TARGET) --log

# Build kernel module (jika diperlukan)
kernel:
	@echo "$(YELLOW)⚙️  Catatan: Untuk mengaktifkan system call di kernel:$(NC)"
	@echo "1. Tambahkan kode system call ke kernel source"
	@echo "2. Update system call table"
	@echo "3. Recompile dan install kernel"
	@echo "4. Reboot sistem"
	@echo "$(RED)⚠️  Proses ini memerlukan root access dan sangat teknis!$(NC)"

help:
	@echo "$(GREEN)📚 Study Focus System Call - Build Help$(NC)"
	@echo ""
	@echo "$(YELLOW)Available targets:$(NC)"
	@echo "  all      - Build user program (default)"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to system (requires sudo)"
	@echo "  test     - Run test with all options"
	@echo "  demo     - Show demo with different options"
	@echo "  kernel   - Show kernel installation notes"
	@echo "  help     - Show this help"
	@echo ""
	@echo "$(YELLOW)Usage examples:$(NC)"
	@echo "  make                    # Build program"
	@echo "  make test              # Build and test"
	@echo "  ./study_focus_monitor  # Run basic"
	@echo "  ./study_focus_monitor --tips --log  # With options"
	@echo ""
	@echo "$(YELLOW)Program options:$(NC)"
	@echo "  --tips   Show productivity tips"
	@echo "  --log    Save results to file"
	@echo "  --help   Show program help"

# Target untuk membuat dokumentasi
docs:
	@echo "$(YELLOW)📋 Membuat dokumentasi...$(NC)"
	@echo "# Study Focus System Call Documentation" > README.md
	@echo "" >> README.md
	@echo "## Build Instructions" >> README.md
	@echo "\`\`\`bash" >> README.md
	@echo "make all     # Build program" >> README.md
	@echo "make test    # Build and test" >> README.md
	@echo "make install # Install system-wide" >> README.md
	@echo "\`\`\`" >> README.md
	@echo "$(GREEN)✅ README.md telah dibuat!$(NC)"
