#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>
#include "simulator.h"

class InstructionDecoder {
public:
    void decode(uint8_t opcode, uint8_t modrm, CPU& cpu);
    // Handlers for each instruction
    void nop(CPU& cpu);
    void mov_reg_reg(CPU& cpu, uint8_t src_reg, uint8_t dst_reg);
    void add_reg_reg(CPU& cpu, uint8_t src_reg, uint8_t dst_reg);
    void sub_reg_reg(CPU& cpu, uint8_t src_reg, uint8_t dst_reg);
    void jmp_rel8(CPU& cpu, int8_t offset);
};

#endif // INSTRUCTIONS_H