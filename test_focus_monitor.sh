#!/bin/bash

# Test Script untuk Study Focus Monitor
# Tugas Sistem Operasi - Custom System Call

# Warna untuk output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Header script
echo -e "${BLUE}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║              🧪 STUDY FOCUS MONITOR - TEST SUITE             ║${NC}"
echo -e "${BLUE}║                    Testing System Call                      ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Fungsi untuk menampilkan status
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✅ $2${NC}"
    else
        echo -e "${RED}❌ $2${NC}"
    fi
}

# Fungsi untuk menampilkan info
print_info() {
    echo -e "${YELLOW}ℹ️  $1${NC}"
}

# Test 1: Cek apakah program sudah dikompilasi
echo -e "${BLUE}[TEST 1]${NC} Checking if program is compiled..."
if [ -f "study_focus_monitor" ]; then
    print_status 0 "Program executable found"
else
    print_info "Program not found, compiling..."
    make all
    if [ $? -eq 0 ]; then
        print_status 0 "Compilation successful"
    else
        print_status 1 "Compilation failed"
        exit 1
    fi
fi

echo ""

# Test 2: Test basic functionality
echo -e "${BLUE}[TEST 2]${NC} Testing basic functionality..."
./study_focus_monitor > test_output.txt 2>&1
if [ $? -eq 0 ]; then
    print_status 0 "Basic execution successful"
    print_info "Program output saved to test_output.txt"
else
    print_status 1 "Basic execution failed"
    print_info "Check if system call is properly installed in kernel"
fi

echo ""

# Test 3: Test with tips option
echo -e "${BLUE}[TEST 3]${NC} Testing --tips option..."
./study_focus_monitor --tips > test_tips_output.txt 2>&1
if [ $? -eq 0 ]; then
    print_status 0 "Tips option works"
else
    print_status 1 "Tips option failed"
fi

echo ""

# Test 4: Test with log option
echo -e "${BLUE}[TEST 4]${NC} Testing --log option..."
./study_focus_monitor --log > test_log_output.txt 2>&1
if [ $? -eq 0 ]; then
    print_status 0 "Log option works"
    if [ -f "study_focus_log.txt" ]; then
        print_status 0 "Log file created successfully"
        print_info "Log file contents:"
        echo -e "${YELLOW}$(tail -1 study_focus_log.txt)${NC}"
    else
        print_status 1 "Log file not created"
    fi
else
    print_status 1 "Log option failed"
fi

echo ""

# Test 5: Test help option
echo -e "${BLUE}[TEST 5]${NC} Testing --help option..."
./study_focus_monitor --help > test_help_output.txt 2>&1
if [ $? -eq 0 ]; then
    print_status 0 "Help option works"
else
    print_status 1 "Help option failed"
fi

echo ""

# Test 6: Test multiple options
echo -e "${BLUE}[TEST 6]${NC} Testing multiple options..."
./study_focus_monitor --tips --log > test_multi_output.txt 2>&1
if [ $? -eq 0 ]; then
    print_status 0 "Multiple options work"
else
    print_status 1 "Multiple options failed"
fi

echo ""

# Test 7: Check system call availability
echo -e "${BLUE}[TEST 7]${NC} Checking system call availability..."
if grep -q "get_study_focus_info" /proc/kallsyms 2>/dev/null; then
    print_status 0 "System call symbol found in kernel"
else
    print_status 1 "System call not found in kernel"
    print_info "This is expected if system call is not installed"
fi

echo ""

# Test 8: Performance test
echo -e "${BLUE}[TEST 8]${NC} Performance test (multiple calls)..."
start_time=$(date +%s.%N)
for i in {1..5}; do
    ./study_focus_monitor > /dev/null 2>&1
done
end_time=$(date +%s.%N)
duration=$(echo "$end_time - $start_time" | bc 2>/dev/null || echo "N/A")
print_info "5 calls completed in: ${duration}s"

echo ""

# Summary
echo -e "${BLUE}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                        📊 TEST SUMMARY                       ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════════╝${NC}"

echo -e "${YELLOW}Generated test files:${NC}"
echo "  📄 test_output.txt - Basic output"
echo "  📄 test_tips_output.txt - Tips output"  
echo "  📄 test_log_output.txt - Log output"
echo "  📄 test_help_output.txt - Help output"
echo "  📄 test_multi_output.txt - Multiple options output"
if [ -f "study_focus_log.txt" ]; then
    echo "  📄 study_focus_log.txt - Activity log"
fi

echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "1. Review test outputs for any errors"
echo "2. If system call fails, install it in kernel"
echo "3. Use 'make clean' to cleanup test files"
echo "4. Use 'make install' to install system-wide"

echo ""
echo -e "${GREEN}🎉 Testing completed! Check the outputs above.${NC}"

# Cleanup option
echo ""
read -p "Do you want to cleanup test files? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    rm -f test_*.txt
    print_status 0 "Test files cleaned up"
fi
