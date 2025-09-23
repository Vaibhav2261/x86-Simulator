#include <iostream>
#include <iomanip>
#include <array>
#include "simulator.h"
#include "memory.h"

int main() {
    Memory mem(256);
    CPU cpu(mem);

    // Hardcoded test program (x86-like bytes, little-endian)
    // Program: MOV AX, 0x00; MOV BX, 0x05; MOV CX, 0x03; ADD AX, BX; ADD AX, CX; MOV [0x10], AX; JMP +5 (loop-ish); NOP; HALT (simplified)
    std::array<uint8_t, 20> program = {
        0xB8, 0x00,  // MOV AX, imm 0x00 (simplified imm encoding)
        0xBB, 0x05,  // MOV BX, imm 0x05
        0xB9, 0x03,  // MOV CX, imm 0x03
        0x01, 0xC8,  // ADD AX, BX (0x01 = ADD, modrm=0xC8 for AX+BX)
        0x01, 0xC1,  // ADD AX, CX (modrm=0xC1 for AX+CX)
        0xA2, 0x10, 0x00,  // MOV [0x0010], AL (simplified)
        0xEB, 0x05,  // JMP +5 (relative)
        0x90,        // NOP
        0x90,        // NOP (padding)
        0xF4         // HALT (simplified, not executed)
    };
    mem.load_program(0x0000, program);

    std::cout << "Initial Registers: ";
    for (size_t i = 0; i < cpu.registers.size(); ++i) {
        std::cout << "R" << i << "=0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.registers[i]) << " ";
    }
    std::cout << std::dec << "\nPC=0x" << std::hex << cpu.pc << std::dec << "\n";

    cpu.reset();  // PC=0
    uint32_t cycles = 0;
    while (cycles < 30) {  // Run until program end
        cpu.execute_cycle();
        ++cycles;
        if (mem.read_byte(cpu.pc) == 0xF4) break;  // Simulated halt
    }

    std::cout << "After Execution:\n";
    std::cout << "Registers: ";
    for (size_t i = 0; i < cpu.registers.size(); ++i) {
        std::cout << "R" << i << "=0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.registers[i]) << " ";
    }
    std::cout << std::dec << "\n";
    std::cout << "Memory[0x10]: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mem.read_byte(0x10)) << std::dec << "\n";
    std::cout << "Flags: ZF=" << cpu.zf << " CF=" << cpu.cf << "\n";
    std::cout << "PC=0x" << std::hex << cpu.pc << std::dec << "\n";
    std::cout << "Simulation complete. Total cycles: " << cycles << "\n";

    return 0;
}