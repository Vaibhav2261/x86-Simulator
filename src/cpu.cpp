#include "simulator.h"
#include "instructions.h"
#include <iostream>

Simulator::Simulator() {
    reset();
}

Simulator::~Simulator() {}

void Simulator::reset() {
    for (int i = 0; i < 9; ++i) {
        registers[i] = 0;
    }
    zero_flag = false;
    carry_flag = false;
    memory = Memory();  // Reset memory to zeros
}

void Simulator::run() {
    uint32_t eip = get_eip();
    while (true) {
        if (eip >= Memory::SIZE) {
            std::cerr << "EIP out of bounds!" << std::endl;
            break;
        }
        uint8_t opcode = memory.read_byte(eip);
        if (opcode == HLT) {
            break;
        }
        execute_instruction(opcode, eip);
        eip = get_eip();  // Updated by instructions
    }
    set_eip(eip);
}

void Simulator::execute_instruction(uint8_t opcode, uint32_t eip) {
    uint32_t new_eip = eip + 1;  // Default advance by 1

    switch (opcode) {
        case MOV_REG_IMM: {
            int reg = (memory.read_byte(eip + 1) - 0xB8) / 8;  // e.g., B8=0, B9=1, etc.
            uint32_t imm = memory.read_dword(eip + 2);
            set_reg(reg, imm);
            new_eip += 5;  // opcode + reg_byte + imm32
            break;
        }
        case MOV_REG_REG:
        case ADD_REG_REG:
        case SUB_REG_REG:
        case CMP_REG_REG: {
            uint8_t modrm = memory.read_byte(eip + 1);
            ModRM m = decode_modrm(modrm);
            uint32_t src = get_reg(m.reg2);
            uint32_t dst = get_reg(m.reg1);
            if (opcode == MOV_REG_REG) {
                set_reg(m.reg1, src);
            } else if (opcode == ADD_REG_REG) {
                uint32_t result = dst + src;
                set_reg(m.reg1, result);
                update_flags(result, dst, src);
            } else if (opcode == SUB_REG_REG) {
                uint32_t result = dst - src;
                set_reg(m.reg1, result);
                update_flags(result, dst, src, true);
            } else if (opcode == CMP_REG_REG) {
                uint32_t result = dst - src;
                update_flags(result, dst, src, true);
            }
            new_eip += 2;  // opcode + modrm
            break;
        }
        case JMP_REL32: {
            int32_t rel = static_cast<int32_t>(memory.read_dword(eip + 1));
            set_eip(eip + 5 + rel);
            return;  // Don't advance eip here
        }
        case JZ_REL8: {
            int8_t rel = static_cast<int8_t>(memory.read_byte(eip + 1));
            if (zero_flag) {
                set_eip(eip + 2 + rel);
            } else {
                new_eip += 2;
            }
            break;
        }
        case HLT:
            set_eip(eip);
            return;
        default:
            std::cerr << "Unknown opcode 0x" << std::hex << static_cast<int>(opcode) << " at 0x" << eip << std::endl;
            set_eip(eip + 1);
            return;
    }
    set_eip(new_eip);
}

void Simulator::update_flags(uint32_t result, uint32_t op1, uint32_t op2, bool is_sub) {
    zero_flag = (result == 0);
    carry_flag = is_sub ? (op1 < op2) : (result < op1 || result < op2);  // Simplified CF
    // Other flags omitted for simplicity
}