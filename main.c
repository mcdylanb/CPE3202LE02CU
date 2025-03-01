//GOAL: 
//1) fetch instruction & 
//2) execute instruction (instruction cycle).
#include <stdio.h>
#include <stdint.h>

#define MEMORY_SIZE 2048    // 2^11 cells
#define DATA_MEMORY_START 0x400  // 1024 in decimal
#define IO_BUFFER_SIZE 32   // 2^5 cells

// Instruction Opcodes (5 bits)
#define OPCODE_WM  0x01  // 00001 - Write to memory
#define OPCODE_RM  0x02  // 00010 - Read from memory
#define OPCODE_BR  0x03  // 00011 - Branch
#define OPCODE_RIO 0x04  // 00100 - Read from IO buffer
#define OPCODE_WIO 0x05  // 00101 - Write to IO buffer
#define OPCODE_WB  0x06  // 00110 - Write data to MBR
#define OPCODE_WIB 0x07  // 00111 - Write data to IOBR
#define OPCODE_EOP 0x1F  // 11111 - End of program

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

// Function to initialize memory with instructions and data
void initMemory(void) {
    // Instructions from Table 2
    // WB 0xFF - Write 0xFF to MBR
    memory[0x000] = 0x30;
    memory[0x001] = 0xFF;
    
    // WM 0x400 - Write data from MBR to memory address 0x400
    memory[0x002] = 0x0C;
    memory[0x003] = 0x00;
    
    // RM 0x400 - Read data from memory address 0x400 to MBR
    memory[0x004] = 0x14;
    memory[0x005] = 0x00;
    
    // BR 0x12A - Branch to memory address 0x12A
    memory[0x006] = 0x19;
    memory[0x007] = 0x2A;
    
    // ... (skipping to 0x12A)
    
    // WIB 0x05 - Write 0x05 to IOBR
    memory[0x12A] = 0x38;
    memory[0x12B] = 0x05;
    
    // WIO 0x0A - Write data from IOBR to IO buffer address 0x0A
    memory[0x12C] = 0x28;
    memory[0x12D] = 0x0A;
    
    // EOP - End of program
    memory[0x12E] = 0xF8;
    memory[0x12F] = 0x00;
}

int CU(void) {
    // Registers
    uint16_t PC = 0x000;    // Program Counter (11-bit)
    uint16_t IR = 0;        // Instruction Register (16-bit)
    uint16_t MAR = 0;       // Memory Address Register (11-bit)
    uint8_t MBR = 0;        // Memory Buffer Register (8-bit)
    uint16_t IOAR = 0;      // I/O Address Register (11-bit)
    uint8_t IOBR = 0;       // I/O Buffer Register (8-bit)
    
    // Instruction decoding variables
    uint8_t inst_code = 0;  // 5-bit instruction code
    uint16_t operand = 0;   // 11-bit operand
    
    printf("Starting Control Unit execution...\n");
    
    // Instruction cycle (fetch-decode-execute)
    while (1) {
        // FETCH CYCLE
        printf("\nFetch Cycle - PC: 0x%03X\n", PC);
        
        // Fetching upper byte
        IR = memory[PC];
        printf("Fetched upper byte: 0x%02X\n", IR);
        IR = IR << 8;  // Move the byte to the correct position
        PC++;  // Point to the address of the lower byte
        
        // Fetching lower byte
        IR = IR | memory[PC];
        printf("Fetched lower byte: 0x%02X\n", memory[PC]);
        printf("Complete instruction: 0x%04X\n", IR);
        PC++;  // Points to the next instruction
        
        // DECODE CYCLE
        inst_code = IR >> 11;  // Get the 5-bit instruction code
        operand = IR & 0x07FF;  // Get the 11-bit operand
        
        printf("Decode Cycle - Opcode: 0x%02X, Operand: 0x%03X\n", inst_code, operand);
        
        // EXECUTE CYCLE
        printf("Execute Cycle - ");
        
        switch (inst_code) {
            case OPCODE_WM:  // Write to memory
                printf("WM instruction - Write to memory address 0x%03X\n", operand);
                MAR = operand;
                memory[MAR] = MBR;
                printf("Wrote value 0x%02X to memory address 0x%03X\n", MBR, MAR);
                break;
                
            case OPCODE_RM:  // Read from memory
                printf("RM instruction - Read from memory address 0x%03X\n", operand);
                MAR = operand;
                MBR = memory[MAR];
                printf("Read value 0x%02X from memory address 0x%03X\n", MBR, MAR);
                break;
                
            case OPCODE_BR:  // Branch
                printf("BR instruction - Branch to address 0x%03X\n", operand);
                PC = operand;
                printf("PC updated to 0x%03X\n", PC);
                break;
                
            case OPCODE_RIO:  // Read from IO buffer
                printf("RIO instruction - Read from IO buffer address 0x%03X\n", operand);
                IOAR = operand;
                IOBR = read_io_buffer(IOAR);
                printf("Read value 0x%02X from IO buffer address 0x%03X\n", IOBR, IOAR);
                break;
                
            case OPCODE_WIO:  // Write to IO buffer
                printf("WIO instruction - Write to IO buffer address 0x%03X\n", operand);
                IOAR = operand;
                write_io_buffer(IOAR, IOBR);
                printf("Wrote value 0x%02X to IO buffer address 0x%03X\n", IOBR, IOAR);
                break;
                
            case OPCODE_WB:  // Write data to MBR
                printf("WB instruction - Write data 0x%02X to MBR\n", operand & 0xFF);
                MBR = operand & 0xFF;
                printf("MBR updated to 0x%02X\n", MBR);
                break;
                
            case OPCODE_WIB:  // Write data to IOBR
                printf("WIB instruction - Write data 0x%02X to IOBR\n", operand & 0xFF);
                IOBR = operand & 0xFF;
                printf("IOBR updated to 0x%02X\n", IOBR);
                break;
                
            case OPCODE_EOP:  // End of program
                printf("EOP instruction - End of program\n");
                return 1;  // Return 1 for successful completion
                
            default:  // Unknown instruction
                printf("Unknown instruction code: 0x%02X\n", inst_code);
                return 0;  // Return 0 for error
        }
    }
    
    return 0;  // Should never reach here
}

int main() {
    initMemory();  // Initialize memory with instructions and data
    
    if (CU() == 1) {
        printf("\nProgram run successfully!\n");
    } else {
        printf("\nError encountered, program terminated!\n");
    }
    
    return 0;
}