#include "memory.h"

Memory::Memory(uint16_t size) : mem_size(size), ram() {
    ram.fill(0);
}

uint8_t Memory::read_byte(uint16_t addr) {
    if (addr >= mem_size) return 0;  // Out-of-bounds = 0
    return ram[addr];
}

uint16_t Memory::read_word(uint16_t addr) {
    return (read_byte(addr + 1) << 8) | read_byte(addr);  // Little-endian
}

void Memory::write_byte(uint16_t addr, uint8_t val) {
    if (addr < mem_size) ram[addr] = val;
}

void Memory::write_word(uint16_t addr, uint16_t val) {
    write_byte(addr, val & 0xFF);
    write_byte(addr + 1, (val >> 8) & 0xFF);
}

void Memory::load_program(uint16_t start_addr, const std::array<uint8_t, 256>& program, size_t program_size) {
    for (size_t i = 0; i < program_size && (start_addr + i) < mem_size; ++i) {
        ram[start_addr + i] = program[i];
    }
}