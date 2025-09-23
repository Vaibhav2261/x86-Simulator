#include "instructions.h"
#include <iostream>

// Simplified ModRM decoder (assumes no memory mode, mod=3, rm/reg fields)
ModRM decode_modrm(uint8_t modrm_byte) {
    ModRM m;
    m.mod = (modrm_byte >> 6) & 0x3;
    m.reg1 = (modrm_byte >> 3) & 0x7;  // Destination reg
    m.reg2 = modrm_byte & 0x7;         // Source reg (or rm)
    if (m.mod != 3) {
        std::cerr << "Unsupported ModRM mode (memory not implemented)" << std::endl;
    }
    return m;
}

void decode_and_execute(Simulator& cpu, uint32_t& eip) {
    // This is called from run() but delegated to execute_instruction in cpu.cpp
    // For now, it's a placeholder; actual execution is in cpu.cpp
    uint8_t opcode = cpu.memory.read_byte(eip);
    cpu.execute_instruction(opcode, eip);  // Forward to CPU
    eip = cpu.get_eip();
}