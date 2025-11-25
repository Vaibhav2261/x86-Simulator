#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cctype>
#include <iomanip>
#include <chrono>

class X86Simulator {
private:
    std::map<std::string, uint32_t> registers;
    std::vector<uint8_t> memory;
    
    bool ZF = false, SF = false, CF = false, OF = false, PF = false, AF = false;
    
    std::map<std::string, size_t> labels;
    std::vector<std::string> instructions;
    
    size_t pc = 0;
    bool running = true;
    std::stringstream output;
    size_t instructionCount = 0;
    
public:
    X86Simulator() : memory(1048576, 0) {
        registers["eax"] = 0; registers["ebx"] = 0; registers["ecx"] = 0; registers["edx"] = 0;
        registers["esi"] = 0; registers["edi"] = 0; registers["esp"] = 0xFFFF; registers["ebp"] = 0;
        registers["eip"] = 0;
        
        registers["cs"] = 0; registers["ds"] = 0; registers["ss"] = 0;
        registers["es"] = 0; registers["fs"] = 0; registers["gs"] = 0;
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
    std::string toLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::string removeComment(const std::string& line) {
        size_t pos = line.find(';');
        return (pos != std::string::npos) ? line.substr(0, pos) : line;
    }
    
    bool loadProgram(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = removeComment(line);
            line = trim(line);
            if (line.empty()) continue;
            
            if (line.back() == ':') {
                labels[toLowerCase(line.substr(0, line.length() - 1))] = instructions.size();
            } else {
                instructions.push_back(line);
            }
        }
        return true;
    }
    
    std::vector<std::string> tokenize(const std::string& str) {
        std::vector<std::string> tokens;
        std::string temp = trim(str);
        if (temp.empty()) return tokens;
        
        size_t spacePos = temp.find_first_of(" \t");
        if (spacePos == std::string::npos) {
            tokens.push_back(toLowerCase(temp));
        } else {
            tokens.push_back(toLowerCase(temp.substr(0, spacePos)));
            std::string operands = trim(temp.substr(spacePos));
            
            size_t pos = 0;
            while (pos < operands.length()) {
                while (pos < operands.length() && std::isspace(operands[pos])) pos++;
                if (pos >= operands.length()) break;
                
                size_t comma = operands.find(',', pos);
                if (comma == std::string::npos) {
                    tokens.push_back(toLowerCase(trim(operands.substr(pos))));
                    break;
                } else {
                    tokens.push_back(toLowerCase(trim(operands.substr(pos, comma - pos))));
                    pos = comma + 1;
                }
            }
        }
        return tokens;
    }
    
    bool isRegister(const std::string& str) {
        return registers.find(str) != registers.end() ||
               str == "ax" || str == "bx" || str == "cx" || str == "dx" ||
               str == "al" || str == "ah" || str == "bl" || str == "bh" ||
               str == "cl" || str == "ch" || str == "dl" || str == "dh";
    }
    
    int32_t getValue(const std::string& operand) {
        std::string op = toLowerCase(operand);
        
        if (op == "al") return registers["eax"] & 0xFF;
        if (op == "ah") return (registers["eax"] >> 8) & 0xFF;
        if (op == "bl") return registers["ebx"] & 0xFF;
        if (op == "bh") return (registers["ebx"] >> 8) & 0xFF;
        if (op == "cl") return registers["ecx"] & 0xFF;
        if (op == "ch") return (registers["ecx"] >> 8) & 0xFF;
        if (op == "dl") return registers["edx"] & 0xFF;
        if (op == "dh") return (registers["edx"] >> 8) & 0xFF;
        
        if (op == "ax") return registers["eax"] & 0xFFFF;
        if (op == "bx") return registers["ebx"] & 0xFFFF;
        if (op == "cx") return registers["ecx"] & 0xFFFF;
        if (op == "dx") return registers["edx"] & 0xFFFF;
        
        if (registers.find(op) != registers.end()) return registers[op];
        
        if (op.find("0x") == 0) return std::stoi(op, nullptr, 16);
        if (op.find("0b") == 0) return std::stoi(op.substr(2), nullptr, 2);
        return std::stoi(op);
    }
    
    void setValue(const std::string& operand, int32_t value) {
        std::string op = toLowerCase(operand);
        
        if (op == "al") registers["eax"] = (registers["eax"] & 0xFFFFFF00) | (value & 0xFF);
        else if (op == "ah") registers["eax"] = (registers["eax"] & 0xFFFF00FF) | ((value & 0xFF) << 8);
        else if (op == "bl") registers["ebx"] = (registers["ebx"] & 0xFFFFFF00) | (value & 0xFF);
        else if (op == "bh") registers["ebx"] = (registers["ebx"] & 0xFFFF00FF) | ((value & 0xFF) << 8);
        else if (op == "cl") registers["ecx"] = (registers["ecx"] & 0xFFFFFF00) | (value & 0xFF);
        else if (op == "ch") registers["ecx"] = (registers["ecx"] & 0xFFFF00FF) | ((value & 0xFF) << 8);
        else if (op == "dl") registers["edx"] = (registers["edx"] & 0xFFFFFF00) | (value & 0xFF);
        else if (op == "dh") registers["edx"] = (registers["edx"] & 0xFFFF00FF) | ((value & 0xFF) << 8);
        else if (op == "ax") registers["eax"] = (registers["eax"] & 0xFFFF0000) | (value & 0xFFFF);
        else if (op == "bx") registers["ebx"] = (registers["ebx"] & 0xFFFF0000) | (value & 0xFFFF);
        else if (op == "cx") registers["ecx"] = (registers["ecx"] & 0xFFFF0000) | (value & 0xFFFF);
        else if (op == "dx") registers["edx"] = (registers["edx"] & 0xFFFF0000) | (value & 0xFFFF);
        else if (registers.find(op) != registers.end()) registers[op] = static_cast<uint32_t>(value);
    }
    
    void updateFlags(int32_t result) {
        ZF = (result == 0);
        SF = (result < 0);
        
        int count = 0;
        uint8_t byte = result & 0xFF;
        for (int i = 0; i < 8; i++) {
            if (byte & (1 << i)) count++;
        }
        PF = (count % 2 == 0);
    }
    
    void executeInstruction(const std::string& instr) {
        auto tokens = tokenize(instr);
        if (tokens.empty()) return;
        
        std::string op = tokens[0];
        instructionCount++;
        
        if (op == "mov" && tokens.size() == 3) {
            setValue(tokens[1], getValue(tokens[2]));
        }
        else if (op == "add" && tokens.size() == 3) {
            int32_t val1 = getValue(tokens[1]);
            int32_t val2 = getValue(tokens[2]);
            int64_t result = static_cast<int64_t>(val1) + val2;
            setValue(tokens[1], static_cast<int32_t>(result));
            updateFlags(static_cast<int32_t>(result));
            CF = (result > INT32_MAX || result < INT32_MIN);
        }
        else if (op == "sub" && tokens.size() == 3) {
            int32_t val1 = getValue(tokens[1]);
            int32_t val2 = getValue(tokens[2]);
            int64_t result = static_cast<int64_t>(val1) - val2;
            setValue(tokens[1], static_cast<int32_t>(result));
            updateFlags(static_cast<int32_t>(result));
            CF = (result < INT32_MIN);
        }
        else if (op == "inc" && tokens.size() == 2) {
            int32_t val = getValue(tokens[1]) + 1;
            setValue(tokens[1], val);
            updateFlags(val);
        }
        else if (op == "dec" && tokens.size() == 2) {
            int32_t val = getValue(tokens[1]) - 1;
            setValue(tokens[1], val);
            updateFlags(val);
        }
        else if (op == "mul" && tokens.size() == 2) {
            int64_t result = static_cast<int64_t>(getValue("eax")) * getValue(tokens[1]);
            registers["eax"] = result & 0xFFFFFFFF;
            registers["edx"] = (result >> 32) & 0xFFFFFFFF;
        }
        else if (op == "imul" && tokens.size() == 3) {
            int64_t result = static_cast<int64_t>(getValue(tokens[1])) * getValue(tokens[2]);
            setValue(tokens[1], static_cast<int32_t>(result));
        }
        else if (op == "div" && tokens.size() == 2) {
            int32_t divisor = getValue(tokens[1]);
            if (divisor == 0) throw std::runtime_error("Division by zero");
            registers["eax"] = getValue("eax") / divisor;
            registers["edx"] = getValue("eax") % divisor;
        }
        else if (op == "and" && tokens.size() == 3) {
            int32_t result = getValue(tokens[1]) & getValue(tokens[2]);
            setValue(tokens[1], result);
            updateFlags(result);
            CF = OF = false;
        }
        else if (op == "or" && tokens.size() == 3) {
            int32_t result = getValue(tokens[1]) | getValue(tokens[2]);
            setValue(tokens[1], result);
            updateFlags(result);
            CF = OF = false;
        }
        else if (op == "xor" && tokens.size() == 3) {
            int32_t result = getValue(tokens[1]) ^ getValue(tokens[2]);
            setValue(tokens[1], result);
            updateFlags(result);
            CF = OF = false;
        }
        else if (op == "not" && tokens.size() == 2) {
            setValue(tokens[1], ~getValue(tokens[1]));
        }
        else if (op == "neg" && tokens.size() == 2) {
            int32_t val = -getValue(tokens[1]);
            setValue(tokens[1], val);
            updateFlags(val);
        }
        else if (op == "shl" && tokens.size() == 3) {
            int32_t result = getValue(tokens[1]) << getValue(tokens[2]);
            setValue(tokens[1], result);
            updateFlags(result);
        }
        else if (op == "shr" && tokens.size() == 3) {
            uint32_t result = static_cast<uint32_t>(getValue(tokens[1])) >> getValue(tokens[2]);
            setValue(tokens[1], result);
            updateFlags(result);
        }
        else if (op == "cmp" && tokens.size() == 3) {
            int32_t result = getValue(tokens[1]) - getValue(tokens[2]);
            updateFlags(result);
            CF = result < 0;
        }
        else if (op == "jmp" && labels.count(tokens[1])) {
            pc = labels[tokens[1]]; return;
        }
        else if (op == "je" || op == "jz") {
            if (ZF && labels.count(tokens[1])) { pc = labels[tokens[1]]; return; }
        }
        else if (op == "jne" || op == "jnz") {
            if (!ZF && labels.count(tokens[1])) { pc = labels[tokens[1]]; return; }
        }
        else if (op == "jg") {
            if (!ZF && !SF && labels.count(tokens[1])) { pc = labels[tokens[1]]; return; }
        }
        else if (op == "jge") {
            if (!SF && labels.count(tokens[1])) { pc = labels[tokens[1]]; return; }
        }
        else if (op == "jl") {
            if (SF && labels.count(tokens[1])) { pc = labels[tokens[1]]; return; }
        }
        else if (op == "jle") {
            if ((SF || ZF) && labels.count(tokens[1])) { pc = labels[tokens[1]]; return; }
        }
        else if (op == "push" && tokens.size() == 2) {
            int32_t val = getValue(tokens[1]);
            registers["esp"] -= 4;
            uint32_t addr = registers["esp"];
            memory[addr] = val & 0xFF;
            memory[addr+1] = (val >> 8) & 0xFF;
            memory[addr+2] = (val >> 16) & 0xFF;
            memory[addr+3] = (val >> 24) & 0xFF;
        }
        else if (op == "pop" && tokens.size() == 2) {
            uint32_t addr = registers["esp"];
            int32_t val = memory[addr] | (memory[addr+1] << 8) | 
                         (memory[addr+2] << 16) | (memory[addr+3] << 24);
            setValue(tokens[1], val);
            registers["esp"] += 4;
        }
        else if (op == "call" && labels.count(tokens[1])) {
            registers["esp"] -= 4;
            uint32_t addr = registers["esp"];
            uint32_t ret = pc + 1;
            memory[addr] = ret & 0xFF;
            memory[addr+1] = (ret >> 8) & 0xFF;
            memory[addr+2] = (ret >> 16) & 0xFF;
            memory[addr+3] = (ret >> 24) & 0xFF;
            pc = labels[tokens[1]];
            return;
        }
        else if (op == "ret") {
            uint32_t addr = registers["esp"];
            uint32_t ret = memory[addr] | (memory[addr+1] << 8) | 
                          (memory[addr+2] << 16) | (memory[addr+3] << 24);
            registers["esp"] += 4;
            pc = ret;
            return;
        }
        else if (op == "nop") {}
        else if (op == "hlt" || op == "halt") {
            running = false; return;
        }
        else if (op == "print" && tokens.size() == 2) {
            int32_t val = getValue(tokens[1]);
            output << val;
            std::cout << val << std::flush;
        }
        else if (op == "printc" && tokens.size() == 2) {
            char ch = static_cast<char>(getValue(tokens[1]));
            output << ch;
            std::cout << ch << std::flush;
        }
        else {
            throw std::runtime_error("Unknown instruction: " + op);
        }
        
        pc++;
    }
    
    void run() {
        auto start = std::chrono::high_resolution_clock::now();
        
        while (running && pc < instructions.size()) {
            if (instructionCount > 1000000) {
                throw std::runtime_error("Maximum instruction count exceeded (infinite loop?)");
            }
            executeInstruction(instructions[pc]);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "\n\n=== Execution Statistics ===" << std::endl;
        std::cout << "Instructions executed: " << instructionCount << std::endl;
        std::cout << "Execution time: " << duration.count() / 1000.0 << " ms" << std::endl;
    }
    
    void printRegisters() {
        std::cout << "\n=== 32-bit Registers ===" << std::endl;
        for (const auto& reg : {"eax", "ebx", "ecx", "edx", "esi", "edi", "esp", "ebp"}) {
            std::cout << std::setw(4) << std::left << reg << ": 0x" 
                     << std::hex << std::setw(8) << std::setfill('0') << registers[reg]
                     << " (" << std::dec << static_cast<int32_t>(registers[reg]) << ")" << std::endl;
        }
        
        std::cout << "\n=== Flags ===" << std::endl;
        std::cout << "ZF=" << ZF << " SF=" << SF << " CF=" << CF 
                  << " OF=" << OF << " PF=" << PF << " AF=" << AF << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <assembly_file>" << std::endl;
        std::cerr << "Example: " << argv[0] << " test.asm" << std::endl;
        return 1;
    }
    
    try {
        X86Simulator sim;
        
        if (!sim.loadProgram(argv[1])) {
            return 1;
        }
        
        std::cout << "=== x86 Simulator ===" << std::endl;
        std::cout << "Loading: " << argv[1] << std::endl;
        std::cout << "Starting execution...\n" << std::endl;
        
        sim.run();
        sim.printRegisters();
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}