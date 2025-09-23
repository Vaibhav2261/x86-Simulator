#include <iostream>
#include <iomanip>
#include "simulator.h"

int main() {
    Simulator sim;

    // Hardcoded program: MOV EAX, 5; MOV EBX, 3; ADD EAX, EBX; HLT
    // Bytes (little-endian):
    // MOV EAX,5: B8 05 00 00 00
    // MOV EBX,3: BB 03 00 00 00
    // ADD EAX,EBX: 01 D8 (01 /r ADD r32, r/m32; here EBX to EAX)
    // HLT: F4
    uint8_t program[] = {
        0xB8, 0x05, 0x00, 0x00, 0x00,  // MOV EAX, 5
        0xBB, 0x03, 0x00, 0x00, 0x00,  // MOV EBX, 3
        0x01, 0xD8,                     // ADD EAX, EBX
        0xF4                            // HLT
    };
    sim.memory.load(0x1000, program, sizeof(program));  // Load at address 0x1000
    sim.set_eip(0x1000);

    std::cout << "Starting simulation..." << std::endl;
    sim.run();

    std::cout << "Simulation halted." << std::endl;
    std::cout << "EAX = " << std::hex << std::setw(8) << std::setfill('0') << sim.get_reg(0) << std::endl;
    // Expected: 8 (0x00000008)

    return 0;
}