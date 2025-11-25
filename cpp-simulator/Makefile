CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
TARGET = x86sim
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
    rm -f $(TARGET)

run: $(TARGET)
    ./$(TARGET) examples/test.asm

.PHONY: all clean run