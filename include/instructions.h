#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>
#include "simulator.h"

// Instruction decoding and execution prototypes
void decode_and_execute(Simulator& cpu, uint32_t& eip);

// Opcodes (subset)
enum Opcode {
    MOV_REG_IMM = 0xB8,  // MOV reg, imm32 (B8 + reg*8)
    MOV_REG_REG = 0x89,  // MOV reg, reg (simplified ModRM)
    ADD_REG_REG = 0x01,  // ADD reg, reg
    SUB_REG_REG = 0x29,  // SUB reg, reg
    CMP_REG_REG = 0x39,  // CMP reg, reg
    JMP_REL32 = 0xE9,    // JMP rel32
    JZ_REL8 = 0x74,      // JZ rel8 (simplified)
    HLT = 0xF4
};

// ModRM simplified (reg1, reg2)
struct ModRM {
    uint8_t mod;
    uint8_t reg1;
    uint8_t reg2;
};

ModRM decode_modrm(uint8_t modrm_byte);

#endif // INSTRUCTIONS_H