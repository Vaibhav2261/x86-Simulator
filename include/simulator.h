#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <cstdint>
#include <vector>
#include "memory.h"
#include "instructions.h"

class Simulator {
public:
    Simulator();
    ~Simulator();

    // Registers (0: EAX, 1: ECX, 2: EDX, 3: EBX, 4: ESP, 5: EBP, 6: ESI, 7: EDI, 8: EIP)
    // Note: Standard x86 order: EAX=0, ECX=1, EDX=2, EBX=3, ESP=4, EBP=5, ESI=6, EDI=7
    uint32_t registers[9];

    // Flags (simplified: only ZF and CF)
    bool zero_flag;
    bool carry_flag;

    // Memory
    Memory memory;

    // Run the simulator until HLT
    void run();

    // Reset CPU state
    void reset();

    // Get register by index (for convenience, 0-7 only)
    uint32_t get_reg(int idx) const { return (idx >= 0 && idx < 8) ? registers[idx] : 0; }
    void set_reg(int idx, uint32_t val) { if (idx >= 0 && idx < 8) registers[idx] = val; }

    // EIP access
    uint32_t get_eip() const { return registers[8]; }
    void set_eip(uint32_t eip) { registers[8] = eip; }

    // Execute single instruction (public for testing)
    void execute_instruction(uint8_t opcode, uint32_t current_eip);

private:
    void update_flags(uint32_t result, uint32_t op1, uint32_t op2, bool is_sub = false);
};

#endif // SIMULATOR_H