import React, { useState, useRef } from 'react';
import { Play, Download, Upload, Trash2, AlertCircle, CheckCircle, Code, Cpu } from 'lucide-react';

const X86Simulator = () => {
  const [code, setCode] = useState(`; Fibonacci sequence calculator
mov ecx, 10      ; Calculate 10 numbers
mov eax, 0       ; First number
mov ebx, 1       ; Second number

loop_start:
print eax        ; Print current number
mov al, 10       ; Newline
printc al

mov edx, eax     ; Save current
add eax, ebx     ; Next = current + previous
mov ebx, edx     ; Update previous

dec ecx
cmp ecx, 0
jg loop_start

hlt`);
  
  const [output, setOutput] = useState('');
  const [registers, setRegisters] = useState(null);
  const [flags, setFlags] = useState(null);
  const [memory, setMemory] = useState(null);
  const [isRunning, setIsRunning] = useState(false);
  const [error, setError] = useState('');
  const [executionTime, setExecutionTime] = useState(0);
  const [instructionCount, setInstructionCount] = useState(0);
  const fileInputRef = useRef(null);

  const examples = {
    fibonacci: `; Fibonacci sequence calculator
mov ecx, 10      ; Calculate 10 numbers
mov eax, 0       ; First number
mov ebx, 1       ; Second number

loop_start:
print eax        ; Print current number
mov al, 10       ; Newline
printc al

mov edx, eax     ; Save current
add eax, ebx     ; Next = current + previous
mov ebx, edx     ; Update previous

dec ecx
cmp ecx, 0
jg loop_start

hlt`,
    factorial: `; Calculate factorial of 6
mov eax, 6       ; Number to calculate
mov ebx, 1       ; Result

factorial_loop:
imul ebx, eax    ; result *= n
dec eax          ; n--
cmp eax, 1
jg factorial_loop

print ebx        ; Print result (720)
hlt`,
    hello: `; Print "Hello, World!"
mov al, 72       ; 'H'
printc al
mov al, 101      ; 'e'
printc al
mov al, 108      ; 'l'
printc al
printc al
mov al, 111      ; 'o'
printc al
mov al, 44       ; ','
printc al
mov al, 32       ; ' '
printc al
mov al, 87       ; 'W'
printc al
mov al, 111      ; 'o'
printc al
mov al, 114      ; 'r'
printc al
mov al, 108      ; 'l'
printc al
mov al, 100      ; 'd'
printc al
mov al, 33       ; '!'
printc al
mov al, 10       ; newline
printc al
hlt`,
    sum: `; Sum numbers from 1 to 100
mov ecx, 100     ; Counter
mov eax, 0       ; Sum

sum_loop:
add eax, ecx
dec ecx
cmp ecx, 0
jg sum_loop

print eax        ; Print result (5050)
hlt`,
    stack: `; Stack operations demo
mov eax, 10
mov ebx, 20
mov ecx, 30

push eax         ; Push values
push ebx
push ecx

pop edx          ; Pop in reverse
pop esi
pop edi

print edx        ; Should print 30
mov al, 32
printc al
print esi        ; Should print 20
mov al, 32
printc al
print edi        ; Should print 10
hlt`,
    bitwise: `; Bitwise operations demo
mov eax, 0xFF    ; 255
mov ebx, 0x0F    ; 15

and eax, ebx     ; 15 & 255 = 15
print eax
mov al, 10
printc al

mov eax, 0x0F
mov ebx, 0xF0
or eax, ebx      ; 15 | 240 = 255
print eax
mov al, 10
printc al

mov eax, 0xFF
xor eax, eax     ; XOR with itself = 0
print eax
hlt`,
    subroutine: `; Subroutine example
mov eax, 5
call square      ; Call square function
print eax        ; Print 25
hlt

square:
imul eax, eax    ; Square the value
ret`
  };

  class X86SimulatorEngine {
    constructor() {
      this.registers = {
        eax: 0, ebx: 0, ecx: 0, edx: 0,
        esi: 0, edi: 0, esp: 0xFFFF, ebp: 0, eip: 0
      };
      this.memory = new Uint8Array(1048576);
      this.flags = { ZF: false, SF: false, CF: false, OF: false, PF: false, AF: false };
      this.labels = {};
      this.instructions = [];
      this.pc = 0;
      this.running = true;
      this.output = '';
      this.instructionCount = 0;
      this.maxInstructions = 100000;
    }

    getValue(operand) {
      const op = operand.toLowerCase().trim();
      
      if (this.registers.hasOwnProperty(op)) {
        return this.registers[op];
      }
      
      if (op === 'ax') return this.registers.eax & 0xFFFF;
      if (op === 'bx') return this.registers.ebx & 0xFFFF;
      if (op === 'cx') return this.registers.ecx & 0xFFFF;
      if (op === 'dx') return this.registers.edx & 0xFFFF;
      if (op === 'si') return this.registers.esi & 0xFFFF;
      if (op === 'di') return this.registers.edi & 0xFFFF;
      if (op === 'sp') return this.registers.esp & 0xFFFF;
      if (op === 'bp') return this.registers.ebp & 0xFFFF;
      
      if (op === 'al') return this.registers.eax & 0xFF;
      if (op === 'ah') return (this.registers.eax >> 8) & 0xFF;
      if (op === 'bl') return this.registers.ebx & 0xFF;
      if (op === 'bh') return (this.registers.ebx >> 8) & 0xFF;
      if (op === 'cl') return this.registers.ecx & 0xFF;
      if (op === 'ch') return (this.registers.ecx >> 8) & 0xFF;
      if (op === 'dl') return this.registers.edx & 0xFF;
      if (op === 'dh') return (this.registers.edx >> 8) & 0xFF;
      
      if (op.startsWith('0x')) {
        return parseInt(op, 16);
      }
      if (op.startsWith('0b')) {
        return parseInt(op.substring(2), 2);
      }
      return parseInt(op, 10);
    }

    setValue(operand, value) {
      const op = operand.toLowerCase().trim();
      value = value | 0;
      
      if (op === 'al') {
        this.registers.eax = (this.registers.eax & 0xFFFFFF00) | (value & 0xFF);
      } else if (op === 'ah') {
        this.registers.eax = (this.registers.eax & 0xFFFF00FF) | ((value & 0xFF) << 8);
      } else if (op === 'bl') {
        this.registers.ebx = (this.registers.ebx & 0xFFFFFF00) | (value & 0xFF);
      } else if (op === 'bh') {
        this.registers.ebx = (this.registers.ebx & 0xFFFF00FF) | ((value & 0xFF) << 8);
      } else if (op === 'cl') {
        this.registers.ecx = (this.registers.ecx & 0xFFFFFF00) | (value & 0xFF);
      } else if (op === 'ch') {
        this.registers.ecx = (this.registers.ecx & 0xFFFF00FF) | ((value & 0xFF) << 8);
      } else if (op === 'dl') {
        this.registers.edx = (this.registers.edx & 0xFFFFFF00) | (value & 0xFF);
      } else if (op === 'dh') {
        this.registers.edx = (this.registers.edx & 0xFFFF00FF) | ((value & 0xFF) << 8);
      } else if (op === 'ax') {
        this.registers.eax = (this.registers.eax & 0xFFFF0000) | (value & 0xFFFF);
      } else if (op === 'bx') {
        this.registers.ebx = (this.registers.ebx & 0xFFFF0000) | (value & 0xFFFF);
      } else if (op === 'cx') {
        this.registers.ecx = (this.registers.ecx & 0xFFFF0000) | (value & 0xFFFF);
      } else if (op === 'dx') {
        this.registers.edx = (this.registers.edx & 0xFFFF0000) | (value & 0xFFFF);
      } else if (op === 'si') {
        this.registers.esi = (this.registers.esi & 0xFFFF0000) | (value & 0xFFFF);
      } else if (op === 'di') {
        this.registers.edi = (this.registers.edi & 0xFFFF0000) | (value & 0xFFFF);
      } else if (op === 'sp') {
        this.registers.esp = (this.registers.esp & 0xFFFF0000) | (value & 0xFFFF);
      } else if (op === 'bp') {
        this.registers.ebp = (this.registers.ebp & 0xFFFF0000) | (value & 0xFFFF);
      } else if (this.registers.hasOwnProperty(op)) {
        this.registers[op] = value >>> 0;
      }
    }

    updateFlags(result, isArithmetic = true) {
      result = result | 0;
      this.flags.ZF = result === 0;
      this.flags.SF = result < 0;
      
      if (isArithmetic) {
        let count = 0;
        let temp = result & 0xFF;
        while (temp) {
          count += temp & 1;
          temp >>= 1;
        }
        this.flags.PF = (count % 2) === 0;
      }
    }

    tokenize(line) {
      line = line.split(';')[0].trim();
      if (!line) return [];
      
      const parts = line.split(/\s+/);
      const op = parts[0].toLowerCase();
      const operands = parts.slice(1).join(' ').split(',').map(s => s.trim()).filter(s => s);
      return [op, ...operands];
    }

    loadProgram(code) {
      const lines = code.split('\n');
      this.instructions = [];
      this.labels = {};
      
      lines.forEach(line => {
        line = line.split(';')[0].trim();
        if (!line) return;
        
        if (line.endsWith(':')) {
          const label = line.slice(0, -1).toLowerCase();
          this.labels[label] = this.instructions.length;
        } else {
          this.instructions.push(line);
        }
      });
    }

    executeInstruction(instr) {
      const tokens = this.tokenize(instr);
      if (!tokens.length) return;
      
      const op = tokens[0];
      
      if (op === 'mov' && tokens.length === 3) {
        this.setValue(tokens[1], this.getValue(tokens[2]));
      }
      else if (op === 'add' && tokens.length === 3) {
        const val1 = this.getValue(tokens[1]);
        const val2 = this.getValue(tokens[2]);
        const result = val1 + val2;
        this.setValue(tokens[1], result);
        this.updateFlags(result);
        this.flags.CF = result > 0xFFFFFFFF || result < -0x80000000;
      }
      else if (op === 'sub' && tokens.length === 3) {
        const val1 = this.getValue(tokens[1]);
        const val2 = this.getValue(tokens[2]);
        const result = val1 - val2;
        this.setValue(tokens[1], result);
        this.updateFlags(result);
        this.flags.CF = result < 0;
      }
      else if (op === 'inc' && tokens.length === 2) {
        const val = this.getValue(tokens[1]) + 1;
        this.setValue(tokens[1], val);
        this.updateFlags(val);
      }
      else if (op === 'dec' && tokens.length === 2) {
        const val = this.getValue(tokens[1]) - 1;
        this.setValue(tokens[1], val);
        this.updateFlags(val);
      }
      else if (op === 'mul' && tokens.length === 2) {
        const val1 = this.getValue('eax');
        const val2 = this.getValue(tokens[1]);
        const result = val1 * val2;
        this.registers.eax = (result & 0xFFFFFFFF) >>> 0;
        this.registers.edx = ((result / 0x100000000) | 0) >>> 0;
      }
      else if (op === 'imul' && tokens.length === 3) {
        const val1 = this.getValue(tokens[1]);
        const val2 = this.getValue(tokens[2]);
        const result = val1 * val2;
        this.setValue(tokens[1], result);
      }
      else if (op === 'div' && tokens.length === 2) {
        const divisor = this.getValue(tokens[1]);
        if (divisor === 0) throw new Error('Division by zero');
        const dividend = this.getValue('eax');
        this.registers.eax = Math.floor(dividend / divisor);
        this.registers.edx = dividend % divisor;
      }
      else if (op === 'and' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) & this.getValue(tokens[2]);
        this.setValue(tokens[1], result);
        this.updateFlags(result, false);
        this.flags.CF = false;
        this.flags.OF = false;
      }
      else if (op === 'or' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) | this.getValue(tokens[2]);
        this.setValue(tokens[1], result);
        this.updateFlags(result, false);
        this.flags.CF = false;
        this.flags.OF = false;
      }
      else if (op === 'xor' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) ^ this.getValue(tokens[2]);
        this.setValue(tokens[1], result);
        this.updateFlags(result, false);
        this.flags.CF = false;
        this.flags.OF = false;
      }
      else if (op === 'not' && tokens.length === 2) {
        this.setValue(tokens[1], ~this.getValue(tokens[1]));
      }
      else if (op === 'neg' && tokens.length === 2) {
        const val = -this.getValue(tokens[1]);
        this.setValue(tokens[1], val);
        this.updateFlags(val);
      }
      else if (op === 'shl' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) << this.getValue(tokens[2]);
        this.setValue(tokens[1], result);
        this.updateFlags(result);
      }
      else if (op === 'shr' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) >>> this.getValue(tokens[2]);
        this.setValue(tokens[1], result);
        this.updateFlags(result);
      }
      else if (op === 'sar' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) >> this.getValue(tokens[2]);
        this.setValue(tokens[1], result);
        this.updateFlags(result);
      }
      else if (op === 'rol' && tokens.length === 3) {
        const val = this.getValue(tokens[1]);
        const shift = this.getValue(tokens[2]) % 32;
        const result = (val << shift) | (val >>> (32 - shift));
        this.setValue(tokens[1], result);
      }
      else if (op === 'ror' && tokens.length === 3) {
        const val = this.getValue(tokens[1]);
        const shift = this.getValue(tokens[2]) % 32;
        const result = (val >>> shift) | (val << (32 - shift));
        this.setValue(tokens[1], result);
      }
      else if (op === 'cmp' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) - this.getValue(tokens[2]);
        this.updateFlags(result);
        this.flags.CF = result < 0;
      }
      else if (op === 'test' && tokens.length === 3) {
        const result = this.getValue(tokens[1]) & this.getValue(tokens[2]);
        this.updateFlags(result, false);
        this.flags.CF = false;
        this.flags.OF = false;
      }
      else if (op === 'jmp' && tokens.length === 2) {
        if (this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'je' || op === 'jz') && tokens.length === 2) {
        if (this.flags.ZF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jne' || op === 'jnz') && tokens.length === 2) {
        if (!this.flags.ZF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jg' || op === 'jnle') && tokens.length === 2) {
        if (!this.flags.ZF && !this.flags.SF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jge' || op === 'jnl') && tokens.length === 2) {
        if (!this.flags.SF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jl' || op === 'jnge') && tokens.length === 2) {
        if (this.flags.SF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jle' || op === 'jng') && tokens.length === 2) {
        if ((this.flags.SF || this.flags.ZF) && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'ja' || op === 'jnbe') && tokens.length === 2) {
        if (!this.flags.CF && !this.flags.ZF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jae' || op === 'jnb') && tokens.length === 2) {
        if (!this.flags.CF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jb' || op === 'jnae') && tokens.length === 2) {
        if (this.flags.CF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if ((op === 'jbe' || op === 'jna') && tokens.length === 2) {
        if ((this.flags.CF || this.flags.ZF) && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if (op === 'js' && tokens.length === 2) {
        if (this.flags.SF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if (op === 'jns' && tokens.length === 2) {
        if (!this.flags.SF && this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if (op === 'push' && tokens.length === 2) {
        const val = this.getValue(tokens[1]);
        this.registers.esp -= 4;
        const addr = this.registers.esp;
        this.memory[addr] = val & 0xFF;
        this.memory[addr + 1] = (val >> 8) & 0xFF;
        this.memory[addr + 2] = (val >> 16) & 0xFF;
        this.memory[addr + 3] = (val >> 24) & 0xFF;
      }
      else if (op === 'pop' && tokens.length === 2) {
        const addr = this.registers.esp;
        const val = this.memory[addr] | (this.memory[addr + 1] << 8) | 
                   (this.memory[addr + 2] << 16) | (this.memory[addr + 3] << 24);
        this.setValue(tokens[1], val);
        this.registers.esp += 4;
      }
      else if (op === 'call' && tokens.length === 2) {
        this.registers.esp -= 4;
        const addr = this.registers.esp;
        const retAddr = this.pc + 1;
        this.memory[addr] = retAddr & 0xFF;
        this.memory[addr + 1] = (retAddr >> 8) & 0xFF;
        this.memory[addr + 2] = (retAddr >> 16) & 0xFF;
        this.memory[addr + 3] = (retAddr >> 24) & 0xFF;
        if (this.labels[tokens[1]] !== undefined) {
          this.pc = this.labels[tokens[1]];
          return;
        }
      }
      else if (op === 'ret') {
        const addr = this.registers.esp;
        const retAddr = this.memory[addr] | (this.memory[addr + 1] << 8) | 
                       (this.memory[addr + 2] << 16) | (this.memory[addr + 3] << 24);
        this.registers.esp += 4;
        this.pc = retAddr;
        return;
      }
      else if (op === 'nop') {
        // Do nothing
      }
      else if (op === 'hlt' || op === 'halt') {
        this.running = false;
        return;
      }
      else if (op === 'print' && tokens.length === 2) {
        this.output += this.getValue(tokens[1]);
      }
      else if (op === 'printc' && tokens.length === 2) {
        this.output += String.fromCharCode(this.getValue(tokens[1]));
      }
      else {
        throw new Error('Unknown instruction: ' + op);
      }
      
      this.pc++;
    }

    run() {
      const startTime = performance.now();
      while (this.running && this.pc < this.instructions.length) {
        if (this.instructionCount++ > this.maxInstructions) {
          throw new Error('Maximum instruction count exceeded (infinite loop?)');
        }
        this.executeInstruction(this.instructions[this.pc]);
      }
      return performance.now() - startTime;
    }
  }

  const runSimulator = () => {
    setIsRunning(true);
    setError('');
    setOutput('');
    setRegisters(null);
    setFlags(null);
    setMemory(null);
    setInstructionCount(0);
    
    try {
      const sim = new X86SimulatorEngine();
      sim.loadProgram(code);
      const execTime = sim.run();
      
      setOutput(sim.output);
      setRegisters(sim.registers);
      setFlags(sim.flags);
      setMemory(sim.memory.slice(0xFFE0, 0x10000));
      setExecutionTime(execTime);
      setInstructionCount(sim.instructionCount);
    } catch (err) {
      setError(err.message);
    } finally {
      setIsRunning(false);
    }
  };

  const loadExample = (exampleName) => {
    setCode(examples[exampleName]);
    setOutput('');
    setError('');
    setRegisters(null);
    setFlags(null);
  };

  const downloadCode = () => {
    const blob = new Blob([code], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'program.asm';
    a.click();
    URL.revokeObjectURL(url);
  };

  const uploadFile = (e) => {
    const file = e.target.files[0];
    if (file) {
      const reader = new FileReader();
      reader.onload = (e) => setCode(e.target.result);
      reader.readAsText(file);
    }
  };

  const supportedInstructions = ['MOV', 'ADD', 'SUB', 'INC', 'DEC', 'MUL', 'IMUL', 'DIV', 'AND', 'OR', 'XOR', 'NOT', 'NEG', 
    'SHL', 'SHR', 'SAR', 'ROL', 'ROR', 'CMP', 'TEST', 'JMP', 'JE', 'JNE', 'JG', 'JGE', 'JL', 
    'JLE', 'JA', 'JAE', 'JB', 'JBE', 'JS', 'JNS', 'PUSH', 'POP', 'CALL', 'RET', 'NOP', 'HLT',
    'PRINT', 'PRINTC'];

  const registerList = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'esp', 'ebp', 'eip'];
  const regGroups = [
    { e: 'eax', x: 'ax', l: 'al', h: 'ah' },
    { e: 'ebx', x: 'bx', l: 'bl', h: 'bh' },
    { e: 'ecx', x: 'cx', l: 'cl', h: 'ch' },
    { e: 'edx', x: 'dx', l: 'dl', h: 'dh' }
  ];

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-blue-900 to-gray-900 text-white p-4">
      <div className="max-w-7xl mx-auto">
        <header className="text-center mb-8">
          <div className="flex items-center justify-center gap-3 mb-2">
            <Cpu className="w-12 h-12 text-blue-400" />
            <h1 className="text-4xl font-bold bg-gradient-to-r from-blue-400 to-cyan-400 bg-clip-text text-transparent">
              x86 Assembly Simulator
            </h1>
          </div>
          <p className="text-gray-400">A complete x86 instruction set simulator with support for arithmetic, logic, control flow, and stack operations</p>
        </header>

        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mb-6">
          <div className="lg:col-span-2 bg-gray-800 rounded-lg shadow-2xl overflow-hidden border border-gray-700">
            <div className="bg-gray-900 p-4 border-b border-gray-700 flex items-center justify-between">
              <div className="flex items-center gap-2">
                <Code className="w-5 h-5 text-blue-400" />
                <h2 className="text-xl font-semibold">Assembly Editor</h2>
              </div>
              <div className="flex gap-2">
                <button
                  onClick={() => fileInputRef.current.click()}
                  className="px-3 py-1 bg-gray-700 hover:bg-gray-600 rounded flex items-center gap-2 transition-colors text-sm"
                >
                  <Upload className="w-4 h-4" />
                  Upload
                </button>
                <input
                  ref={fileInputRef}
                  type="file"
                  accept=".asm,.txt"
                  onChange={uploadFile}
                  className="hidden"
                />
                <button
                  onClick={downloadCode}
                  className="px-3 py-1 bg-gray-700 hover:bg-gray-600 rounded flex items-center gap-2 transition-colors text-sm"
                >
                  <Download className="w-4 h-4" />
                  Download
                </button>
                <button
                  onClick={() => setCode('')}
                  className="px-3 py-1 bg-red-600 hover:bg-red-700 rounded flex items-center gap-2 transition-colors text-sm"
                >
                  <Trash2 className="w-4 h-4" />
                  Clear
                </button>
              </div>
            </div>
            <textarea
              value={code}
              onChange={(e) => setCode(e.target.value)}
              className="w-full h-96 p-4 bg-gray-950 text-green-400 font-mono text-sm focus:outline-none resize-none"
              placeholder="Enter x86 assembly code here..."
              spellCheck={false}
            />
            <div className="p-4 border-t border-gray-700 flex gap-2 flex-wrap">
              <button
                onClick={runSimulator}
                disabled={isRunning}
                className="px-6 py-2 bg-gradient-to-r from-blue-600 to-cyan-600 hover:from-blue-700 hover:to-cyan-700 rounded-lg flex items-center gap-2 transition-all font-semibold disabled:opacity-50 disabled:cursor-not-allowed shadow-lg"
              >
                <Play className="w-5 h-5" />
                {isRunning ? 'Running...' : 'Run Program'}
              </button>
              <select
                onChange={(e) => e.target.value && loadExample(e.target.value)}
                className="px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg transition-colors"
                value=""
              >
                <option value="">Load Example...</option>
                <option value="fibonacci">Fibonacci Sequence</option>
                <option value="factorial">Factorial Calculator</option>
                <option value="hello">Hello World</option>
                <option value="sum">Sum 1-100</option>
                <option value="stack">Stack Operations</option>
                <option value="bitwise">Bitwise Operations</option>
                <option value="subroutine">Subroutine Call</option>
              </select>
            </div>
          </div>

          <div className="space-y-6">
            <div className="bg-gray-800 rounded-lg shadow-2xl overflow-hidden border border-gray-700">
              <div className="bg-gray-900 p-4 border-b border-gray-700">
                <h2 className="text-xl font-semibold">Program Output</h2>
              </div>
              <div className="p-4 bg-gray-950 h-48 overflow-auto">
                {error && (
                  <div className="flex items-start gap-2 text-red-400 mb-2">
                    <AlertCircle className="w-5 h-5 flex-shrink-0 mt-1" />
                    <span className="font-mono text-sm">{error}</span>
                  </div>
                )}
                {output && (
                  <div className="flex items-start gap-2 text-green-400 mb-2">
                    <CheckCircle className="w-5 h-5 flex-shrink-0 mt-1" />
                    <pre className="font-mono text-sm whitespace-pre-wrap">{output}</pre>
                  </div>
                )}
                {!error && !output && (
                  <p className="text-gray-500 text-sm">Output will appear here after execution...</p>
                )}
              </div>
              {(executionTime > 0 || instructionCount > 0) && (
                <div className="p-3 bg-gray-900 border-t border-gray-700 text-xs text-gray-400 flex gap-4">
                  <span>⚡ Execution Time: {executionTime.toFixed(2)}ms</span>
                  <span>📊 Instructions: {instructionCount}</span>
                </div>
              )}
            </div>

            <div className="bg-gray-800 rounded-lg shadow-2xl overflow-hidden border border-gray-700">
              <div className="bg-gray-900 p-4 border-b border-gray-700">
                <h2 className="text-xl font-semibold">CPU Flags</h2>
              </div>
              <div className="p-4 bg-gray-950">
                {flags ? (
                  <div className="grid grid-cols-3 gap-3">
                    {Object.entries(flags).map(([flag, value]) => (
                      <div key={flag} className="flex items-center justify-between bg-gray-800 rounded px-3 py-2">
                        <span className="font-mono text-sm text-gray-400">{flag}</span>
                        <span className={value ? 'text-green-400 font-bold' : 'text-red-400 font-bold'}>
                          {value ? '1' : '0'}
                        </span>
                      </div>
                    ))}
                  </div>
                ) : (
                  <p className="text-gray-500 text-sm">Flags will appear here after execution...</p>
                )}
              </div>
            </div>
          </div>
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <div className="bg-gray-800 rounded-lg shadow-2xl overflow-hidden border border-gray-700">
            <div className="bg-gray-900 p-4 border-b border-gray-700">
              <h2 className="text-xl font-semibold">32-bit Registers</h2>
            </div>
            <div className="p-4 bg-gray-950 max-h-96 overflow-auto">
              {registers ? (
                <div className="space-y-2">
                  {registerList.map(reg => (
                    <div key={reg} className="flex items-center justify-between bg-gray-800 rounded px-4 py-2">
                      <span className="font-mono font-bold text-blue-400 uppercase">{reg}</span>
                      <div className="flex gap-3 items-center">
                        <span className="font-mono text-sm text-gray-400">
                          0x{registers[reg].toString(16).padStart(8, '0').toUpperCase()}
                        </span>
                        <span className="font-mono text-sm text-green-400">
                          {registers[reg] | 0}
                        </span>
                      </div>
                    </div>
                  ))}
                </div>
              ) : (
                <p className="text-gray-500 text-sm">Registers will appear here after execution...</p>
              )}
            </div>
          </div>

          <div className="bg-gray-800 rounded-lg shadow-2xl overflow-hidden border border-gray-700">
            <div className="bg-gray-900 p-4 border-b border-gray-700">
              <h2 className="text-xl font-semibold">16-bit & 8-bit Registers</h2>
            </div>
            <div className="p-4 bg-gray-950 max-h-96 overflow-auto">
              {registers ? (
                <div className="space-y-4">
                  {regGroups.map(({ e, x, l, h }) => (
                    <div key={e} className="bg-gray-800 rounded p-3">
                      <div className="grid grid-cols-3 gap-2 text-xs">
                        <div className="bg-gray-900 rounded px-2 py-1">
                          <div className="text-blue-400 font-bold uppercase">{x}</div>
                          <div className="text-green-400 font-mono">
                            {(registers[e] & 0xFFFF).toString(16).padStart(4, '0')}
                          </div>
                        </div>
                        <div className="bg-gray-900 rounded px-2 py-1">
                          <div className="text-blue-400 font-bold uppercase">{l}</div>
                          <div className="text-green-400 font-mono">
                            {(registers[e] & 0xFF).toString(16).padStart(2, '0')}
                          </div>
                        </div>
                        <div className="bg-gray-900 rounded px-2 py-1">
                          <div className="text-blue-400 font-bold uppercase">{h}</div>
                          <div className="text-green-400 font-mono">
                            {((registers[e] >> 8) & 0xFF).toString(16).padStart(2, '0')}
                          </div>
                        </div>
                      </div>
                    </div>
                  ))}
                </div>
              ) : (
                <p className="text-gray-500 text-sm">Registers will appear here after execution...</p>
              )}
            </div>
          </div>
        </div>

        <footer className="mt-8 text-center text-gray-500 text-sm">
          <div className="mb-2">
            <h3 className="text-white font-semibold mb-2">Supported Instructions</h3>
            <div className="flex flex-wrap justify-center gap-2 text-xs">
              {supportedInstructions.map(inst => (
                <span key={inst} className="bg-gray-800 px-2 py-1 rounded">{inst}</span>
              ))}
            </div>
          </div>
          <p className="mt-4">Built with React • Enhanced x86 Architecture Simulator</p>
        </footer>
      </div>
    </div>
  );
};

export default X86Simulator;