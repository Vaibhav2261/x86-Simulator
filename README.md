# x86 Assembly Simulator

A comprehensive x86 instruction set simulator built with React and C++. This project provides both a web-based interactive simulator and a standalone C++ implementation.

## Features

### Web Simulator
-  **Complete x86 ISA Support**: 40+ instructions including arithmetic, logic, control flow, and stack operations
-  **Real-time Visualization**: Live register states, CPU flags, and memory view
-  **Interactive Editor**: Syntax highlighting, code examples, and file import/export
-  **Instant Execution**: Run assembly programs directly in your browser
-  **Responsive Design**: Works on desktop, tablet, and mobile devices

### C++ Simulator
-  **High Performance**: Native C++ implementation for maximum speed
-  **1MB Memory**: Full memory simulation with stack operations
-  **Extended Instructions**: Support for interrupts, system calls, and more
-  **File Input**: Load and execute .asm files

## Supported Instructions

### Data Transfer
- MOV, PUSH, POP

### Arithmetic
- ADD, SUB, INC, DEC, MUL, IMUL, DIV, NEG

### Logic & Bitwise
- AND, OR, XOR, NOT, TEST

### Shift & Rotate
- SHL, SHR, SAR, ROL, ROR

### Control Flow
- JMP, JE/JZ, JNE/JNZ, JG, JGE, JL, JLE, JA, JAE, JB, JBE, JS, JNS
- CALL, RET

### Special
- CMP, NOP, HLT
- PRINT, PRINTC (custom I/O instructions)

## Quick Start

### Web Version (Vercel Deployment)

1. Clone the repository:
```bash
git clone https://github.com/yourusername/x86-simulator.git
cd x86-simulator