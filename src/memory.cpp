#include "memory.h"
#include <cstring>  // For std::memcpy

Memory::Memory() : mem(SIZE, 0) {}  // Initialize to zeros

Memory::~Memory() {}

void Memory::load(uint32_t addr, const uint8_t* data, size_t size) {
    check_bounds(addr, size);
    std::memcpy(&mem[addr], data, size);
}

uint8_t Memory::read_byte(uint32_t addr) const {
    check_bounds(addr, 1);
    return mem[addr];
}

uint16_t Memory::read_word(uint32_t addr) const {
    check_bounds(addr, 2);
    return (static_cast<uint16_t>(mem[addr + 1]) << 8) | mem[addr];  // Little-endian
}

uint32_t Memory::read_dword(uint32_t addr) const {
    check_bounds(addr, 4);
    return (static_cast<uint32_t>(mem[addr + 3]) << 24) |
           (static_cast<uint32_t>(mem[addr + 2]) << 16) |
           (static_cast<uint32_t>(mem[addr + 1]) << 8) |
           mem[addr];
}

void Memory::write_byte(uint32_t addr, uint8_t val) {
    check_bounds(addr, 1);
    mem[addr] = val;
}

void Memory::write_word(uint32_t addr, uint16_t val) {
    check_bounds(addr, 2);
    mem[addr] = val & 0xFF;
    mem[addr + 1] = (val >> 8) & 0xFF;
}

void Memory::write_dword(uint32_t addr, uint32_t val) {
    check_bounds(addr, 4);
    mem[addr] = val & 0xFF;
    mem[addr + 1] = (val >> 8) & 0xFF;
    mem[addr + 2] = (val >> 16) & 0xFF;
    mem[addr + 3] = (val >> 24) & 0xFF;
}

void Memory::check_bounds(uint32_t addr, size_t size) const {
    if (addr + size > SIZE) {
        throw std::out_of_range("Memory access out of bounds");
    }
}