#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>
#include <stdexcept>

class Memory {
public:
    static const uint32_t SIZE = 64 * 1024; // 64KB

    Memory();
    ~Memory();

    // Load data into memory at address
    void load(uint32_t addr, const uint8_t* data, size_t size);

    // Read/Write bytes, words (16-bit), dwords (32-bit) - little-endian
    uint8_t read_byte(uint32_t addr) const;
    uint16_t read_word(uint32_t addr) const;
    uint32_t read_dword(uint32_t addr) const;

    void write_byte(uint32_t addr, uint8_t val);
    void write_word(uint32_t addr, uint16_t val);
    void write_dword(uint32_t addr, uint32_t val);

private:
    std::vector<uint8_t> mem;
    void check_bounds(uint32_t addr, size_t size) const;
};

#endif // MEMORY_H