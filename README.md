# Simple x86 Instruction Set Simulator

A lightweight C++ simulator for a subset of the x86 instruction set, focusing on the fetch-decode-execute cycle. This project demonstrates computer architecture fundamentals, including register management, memory access, and basic instruction emulation. Ideal for learning/resume-building in systems programming.

## Features
- **Supported Instructions**: NOP (0x90), MOV reg-reg (0x88), ADD reg-reg (0x00), SUB reg-reg (0x28), JMP rel8 (0xEB).
- **Architecture**: 8-bit registers (AX, BX, CX, DX, SI, DI, BP, SP), 256-byte RAM, flags (ZF, CF).
- **Execution Model**: Cycle-accurate simulation with little-endian byte order.
- **Build System**: CMake + Google Test for tests.
- **Use Case**: Simulates a simple program (e.g., load values, add, store to memory, jump).

## Building and Running
### Prerequisites
- C++17 compiler (g++ 7+)
- CMake 3.10+
- Google Test (install: `sudo apt install libgtest-dev` on Ubuntu)

### Build
```bash
mkdir build && cd build
cmake ..
make