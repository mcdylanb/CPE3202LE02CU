//GOAL: 
//1) fetch instruction & 
//2) execute instruction (instruction cycle).
#include <stdio.h>
#include <stdint.h>

#define MEMORY_SIZE 2048    // 2^11 cells
#define DATA_MEMORY_START 0x400  // 1024 in decimal
#define IO_BUFFER_SIZE 32   // 2^5 cells

// Memory system - 8-bit wide cells
static uint8_t memory[MEMORY_SIZE];
// I/O buffer - 8-bit wide cells, 32 cells (2^5)
static uint8_t io_buffer[IO_BUFFER_SIZE];

// Memory access functions
static uint8_t read_memory(uint16_t address) {
    if (address < MEMORY_SIZE) {
        return memory[address];
    }
    return 0; // Return 0 for invalid address
}

static void write_memory(uint16_t address, uint8_t data) {
    if (address < MEMORY_SIZE) {
        memory[address] = data;
    }
}

// I/O buffer access functions
static uint8_t read_io_buffer(uint8_t address) {
    if (address < IO_BUFFER_SIZE) {
        return io_buffer[address];
    }
    return 0; // Return 0 for invalid address
}

static void write_io_buffer(uint8_t address, uint8_t data) {
    if (address < IO_BUFFER_SIZE) {
        io_buffer[address] = data;
    }
}

int CU(void)
{
    int PC, IR, MAR, MBR, IO, BR = 0;
    
    // CU operations will go here
    
    return 1;
}

int main() {
    for(int i = 0; i < 10; i++) {
        printf("Hello World\n");
    }
    return 0;
}