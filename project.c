#include "spimcore.h"
#include <stdlib.h>
#include <stdio.h>

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero) {
    switch (ALUControl) {
        case 0x0: // ADD
            *ALUresult = A + B;
            break;
        case 0x1: // SUB
            *ALUresult = A - B;
            break;
        case 0x2: // SLT (signed)
            *ALUresult = ((int)A < (int)B) ? 1 : 0;
            break;
        case 0x3: // SLTU (unsigned)
            *ALUresult = (A < B) ? 1 : 0;
            break;
        case 0x4: // AND
            *ALUresult = A & B;
            break;
        case 0x5: // OR
            *ALUresult = A | B;
            break;
        case 0x6: // Shift left by 16
            *ALUresult = B << 16;
            break;
        case 0x7: // NOT A
            *ALUresult = ~A;
            break;
        default:
            *ALUresult = 0;
            break;
    }
    
    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* fetch instruction from memory */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction){

   // fprintf(stdout, "IF: PC=0x%08x\n", PC);
    size_t wordSize = sizeof(unsigned);
    unsigned address = PC / wordSize;

    if (PC % 4 != 0){ //if the address is not word aligned
        //fprintf(stderr, "IF: Error: PC not word-aligned (0x%08x)\n", PC);
        return 1;
    }

    size_t totalMemBytes = 0x10000;
    size_t numWords = totalMemBytes / wordSize;

    if (address >= numWords) {
        //fprintf(stderr, "IF: Error: PC out of bounds (0x%08x, max 0x%08x)\n", PC, totalMemBytes - 4);
        return 1;
    }

    *instruction = Mem[address]; //supposed to get the memory at the address calcualted
    //fprintf(stdout, "IF: Instruction=0x%08x\n", *instruction);
    return 0;
}

/* instruction partition */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec){

    *op = (instruction >> 26) & 0x3F; //extracts opcode bits 31-26
    
    *r1 = (instruction >> 21) & 0x1F;
    *r2 = (instruction >> 16) & 0x1F;
    *r3 = (instruction >> 11) & 0x1F;
    *funct = instruction & 0x3F;
    *offset = instruction & 0xFFFF;
    *jsec = instruction & 0x3FFFFFF;

   // fprintf(stdout, "IP: Instruction=0x%08x, op=0x%02x, r1=%u, r2=%u, r3=%u, funct=0x%02x, offset=0x%04x, jsec=0x%08x\n",
            //instruction, *op, *r1, *r2, *r3, *funct, *offset, *jsec);
}

/* instruction decode */
int instruction_decode(unsigned op,struct_controls *controls){

    //fprintf(stdout, "ID: Decoding opcode: 0x%02x\n", op);
    controls -> MemRead = 0;
    controls -> MemWrite = 0;
    controls -> MemtoReg = 0;
    controls -> RegWrite = 0;
    controls -> RegDst = 0;
    controls -> Jump = 0;
    controls -> Branch = 0;
    controls -> ALUSrc = 0;
    controls -> ALUOp = 0; // Initialize ALUOp

    switch (op){
        case 0x04: // beq
            controls -> ALUOp = 1;
            controls -> Branch = 1;
            controls -> ALUSrc = 0;
            break;
        case 0x0a: // slti
            controls -> ALUOp = 2;
            controls -> RegWrite = 1;
            controls -> ALUSrc = 1;
            break;
        case 0x0b: // sltiu
            controls -> ALUOp = 3;
            controls -> RegWrite = 1;
            controls -> ALUSrc = 1;
            break;
        case 0x0c: // andi
            controls -> ALUOp = 4;
            controls -> RegWrite = 1;
            controls -> ALUSrc = 1;
            break;
        case 0x0d: // ori
            controls -> ALUOp = 5;
            controls -> RegWrite = 1;
            controls -> ALUSrc = 1;
            break;
        case 0x0f: // lui
            controls -> ALUOp = 6;
            controls -> RegWrite = 1;
            controls -> ALUSrc = 1;
            break;
        case 0x00: // R-type
            controls -> ALUOp = 7;
            controls -> RegWrite = 1;
            controls -> RegDst = 1;
            controls -> ALUSrc = 0;
            break;
        case 0x08: // addi
        case 0x09: // addiu (assuming it behaves like addi for ALU)
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            controls -> ALUOp = 0;
            break;
        case 0x02: // j (jump)
            controls -> Jump = 1;
            break;
        case 0x23: // lw
            controls -> ALUSrc = 1;
            controls -> MemRead = 1;
            controls -> ALUOp = 0;
            controls -> MemtoReg = 1;
            controls -> RegWrite = 1;
            break;
        case 0x2b: // sw
            controls -> ALUSrc = 1;
            controls -> MemWrite = 1;
            controls -> ALUOp = 0;
            break;
        default: // Unknown opcode, halt
            fprintf(stderr, "ID: Error: Unknown opcode 0x%02x\n", op);
            return 1;
    }
       // fprintf(stdout, "ID: Controls: MemRead=%d, MemWrite=%d, MemtoReg=%d, RegWrite=%d, RegDst=%d, Jump=%d, Branch=%d, ALUSrc=%d, ALUOp=%d\n",
            //controls -> MemRead, controls -> MemWrite, controls -> MemtoReg, controls -> RegWrite, controls -> RegDst, controls->Jump, controls->Branch, controls->ALUSrc, controls->ALUOp);
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2){

    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value) {
    // Check if the 16th bit is set (negative number)
    if (offset & 0x8000) {
        *extended_value = offset | 0xFFFF0000; // Sign extend
    } else {
        *extended_value = offset; // No sign extension needed
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero){

    unsigned B = ALUSrc ? extended_value : data2;
    char ALUControl;
    
    // Determine ALUControl based on ALUOp and funct
    if (ALUOp == 0x7) { // R-type instruction
        switch (funct) {
            case 0x20: // add
                ALUControl = 0x0;
                break;
            case 0x22: // sub
                ALUControl = 0x1;
                break;
            case 0x24: // and
                ALUControl = 0x4;
                break;
            case 0x25: // or
                ALUControl = 0x5;
                break;
            case 0x2A: // slt
                ALUControl = 0x2;
                break;
            case 0x2B: // sltu
                ALUControl = 0x3;
                break;
            default:
                return 1; // Illegal instruction (halt)
        }
    } else {
        ALUControl = ALUOp; // For I-type instructions, use ALUOp directly
    }
    
    // Call ALU function
    ALU(data1, B, ALUControl, ALUresult, Zero);
    
    return 0; // No halt
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem){

    size_t wordSize = sizeof(unsigned);

    unsigned address = ALUresult / wordSize;

    size_t totalMemBytes = 0x10000; // 64kB
    size_t numWords = (totalMemBytes / wordSize);

    // Check alignment and bounds on any memory access
    if ((MemRead || MemWrite) && (ALUresult % wordSize != 0)){
        //fprintf(stderr, "MEM: Error: Address not word-aligned (0x%08x)\n", ALUresult);
        return 1;
    }
    if ((MemRead || MemWrite) && address >= numWords){
        //fprintf(stderr, "MEM: Error: Address out of bounds (0x%08x)\n", ALUresult);
        return 1;
    }

    // Perform read or write
    if (MemRead) {
        *memdata = Mem[address];
        //fprintf(stdout, "MEM: Read mem[0x%08x] = 0x%08x\n", ALUresult, *memdata);
    }
    if (MemWrite) {
        Mem[address] = data2;
        //fprintf(stdout, "MEM: Write mem[0x%08x] = 0x%08x\n", ALUresult, data2);
    }

    return 0;
}
// Implementation notes:
// 1. If reading, load from Mem[ALUresult/4] into *memdata.
// 2. If writing, store data2 into that same word address.
// 3. Return 1 on mis‑aligned or out‑of‑bounds access; otherwise 0. :contentReference[oaicite:0]{index=0}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg) {

    if (!RegWrite){
        return;
    }

    // Choose destination register: rt (r2) if RegDst=0, rd (r3) if RegDst=1
    unsigned dest = RegDst ? r3 : r2;
    // Choose write data: memory data if MemtoReg=1, else ALU result
    unsigned value = MemtoReg ? memdata : ALUresult;

    // Enforce $zero remains zero
    if (dest != 0) {
        Reg[dest] = value;
        //fprintf(stdout, "WR: Write Reg[%u] = 0x%08x\n", dest, value);
    }
}
// Implementation notes:
// 1. Only write if RegWrite=1.
// 2. Select register number by RegDst, and data by MemtoReg.
// 3. $0 is read‑only (remains 0). :contentReference[oaicite:1]{index=1}


/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC) {

    unsigned pc_old    = *PC;
    unsigned pc_next4  = pc_old + 4;
    unsigned newPC     = pc_next4;

    if (Jump){
        // Jump target: top 4 bits of (PC+4) concatenated with (jsec << 2)
        newPC = (pc_next4 & 0xF0000000) | (jsec << 2);
    } else if (Branch && Zero) {
        // Branch target: (PC+4) + (sign-extended immediate << 2)
        newPC = pc_next4 + (extended_value << 2);
    }

    *PC = newPC;
    //fprintf(stdout, "PC: Updated from 0x%08x to 0x%08x\n", pc_old, newPC);
}
// Implementation notes:
// 1. Default PC = PC + 4.
// 2. If Branch=1 and Zero=1, PC = PC+4 + (imm << 2).
// 3. Else if Jump=1, PC = {high 4 bits of (PC+4)} ∥ (jsec << 2). :contentReference[oaicite:2]{index=2}&#8203;:contentReference[oaicite:3]{index=3}