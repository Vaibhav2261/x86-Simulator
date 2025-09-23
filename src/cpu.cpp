#include "simulator.h"
#include <iostream>

CPU::CPU(Memory& m) : memory(m), decoder() {
    reset();
}

void CPU::reset() {
    registers.fill(0);
    pc = 0x0000;
    zf = false;
    cf = false;
}

uint8_t CPU::fetch_byte() {
    uint8_t byte = memory.read_byte(pc);
    ++pc;
    return byte;
}

void CPU::execute_cycle() {
    uint8_t opcode = fetch_byte();
    uint8_t modrm = 0;
    if ((opcode & 0xF8) == 0x00 || opcode == 0x88) {  // For ADD/MOV, fetch modrm
        modrm = fetch_byte();
    }
    decode_and_execute(opcode, modrm);
}

void CPU::decode_and_execute(uint8_t opcode, uint8_t modrm) {
    decoder.decode(opcode, modrm, *this);
}

void CPU::run(uint32_t max_cycles) {
    for (uint32_t i = 0; i < max_cycles; ++i) {
        execute_cycle();
        // Check for halt if needed
    }
}