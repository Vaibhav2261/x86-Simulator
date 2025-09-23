#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <array>

class Memory {
public:
    Memory(uint16_t size = 256);  // Default 256 bytes
    uint8_t read_byte(uint16_t addr);
    uint16_t read_word(uint16_t addr);  // Little-endian
    void write_byte(uint16_t addr, uint8_t val);
    void write_word(uint16_t addr, uint16_t val);
    void load_program(uint16_t start_addr, const std::array<uint8_t, 256>& program);

private:
    std::array<uint8_t, 256> ram;
    uint16_t mem_size;
};

#endif // MEMORY_H