// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minivm.h"

#define NUM_REGS   (256)
#define NUM_FUNCS  (256)

// Global variable that indicates if the process is running.
static bool is_running = true;

void dsRangeCheck(uint32_t addr) {
    if (addr >= 8192) {
        puts("[Error] Invalid data section address range");
        exit(1);
    }
}

/*
 * [START] functions for each instructions
 */
void opInvalid(__attribute__((__unused__)) struct VMContext *ctx, __attribute__((__unused__)) const uint32_t instr) {
    // This function is for invalid opcode
    puts("[Error] Invalid opcode");
    exit(1);
}

void opHalt(__attribute__((__unused__)) struct VMContext *ctx, __attribute__((__unused__)) const uint32_t instr) {
    // To exit from loop
    is_running = false;
}

void opLoad(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of load
    uint8_t reg1; // 1st arg of load

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);

    // Check the address validity
    dsRangeCheck(ctx->r[reg0].value);
    dsRangeCheck(ctx->r[reg1].value);

    // Move value from memory to register
    ctx->r[reg0].value = 0 | ctx->dataSegment[ctx->r[reg1].value];
}

void opStore(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of store
    uint8_t reg1; // 1st arg of store

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);

    // Check the address validity
    dsRangeCheck(ctx->r[reg0].value);
    dsRangeCheck(ctx->r[reg1].value);

    // Move value from register to memory
    ctx->dataSegment[ctx->r[reg0].value] = 0xff & ctx->r[reg1].value;
}

void opMove(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of move
    uint8_t reg1; // 1st arg of move

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);

    // Move value from register to register
    ctx->r[reg0].value = ctx->r[reg1].value;
}

void opPuti(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of puti
    uint8_t imm1; // 1st arg of puti

    reg0 = EXTRACT_B1(instr);
    imm1 = EXTRACT_B2(instr);

    // Load immediate value to register
    ctx->r[reg0].value = 0 | imm1;
}

void opAdd(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of add
    uint8_t reg1; // 1st arg of add
    uint8_t reg2; // 2nd arg of add

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);
    reg2 = EXTRACT_B3(instr);

    // Add 1st arg and 2nd arg, and store the result to 0th arg
    ctx->r[reg0].value = ctx->r[reg1].value + ctx->r[reg2].value;
}

void opSub(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of sub
    uint8_t reg1; // 1st arg of sub
    uint8_t reg2; // 2nd arg of sub

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);
    reg2 = EXTRACT_B3(instr);

    // Subtract from 1st arg to 2nd arg, and store the result to 0th arg
    ctx->r[reg0].value = ctx->r[reg1].value - ctx->r[reg2].value;
}

void opGt(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of gt
    uint8_t reg1; // 1st arg of gt
    uint8_t reg2; // 2nd arg of gt

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);
    reg2 = EXTRACT_B3(instr);

    // Compare 1st arg and 2nd arg then set the 0th arg to 1 if 1st arg is
    // greater than 2nd arg
    if (ctx->r[reg1].value > ctx->r[reg2].value) {
        ctx->r[reg0].value = 1;
    } else {
        ctx->r[reg0].value = 0;
    }
}

void opGe(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of ge
    uint8_t reg1; // 1st arg of ge
    uint8_t reg2; // 2nd arg of ge

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);
    reg2 = EXTRACT_B3(instr);

    // Compare 1st arg and 2nd arg then set the 0th arg to 1 if 1st arg is
    // greater than or equal to 2nd arg
    if (ctx->r[reg1].value >= ctx->r[reg2].value) {
        ctx->r[reg0].value = 1;
    } else {
        ctx->r[reg0].value = 0;
    }
}

void opEq(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of eq
    uint8_t reg1; // 1st arg of eq
    uint8_t reg2; // 2nd arg of eq

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);
    reg2 = EXTRACT_B3(instr);

    // Compare 1st arg and 2nd arg then set the 0th arg to 1 if 1st arg and 2nd
    // arg are equal
    if (ctx->r[reg1].value == ctx->r[reg2].value) {
        ctx->r[reg0].value = 1;
    } else {
        ctx->r[reg0].value = 0;
    }
}

void opIte(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of ite
    uint8_t imm1; // 1st arg of ite
    uint8_t imm2; // 2nd arg of ite

    reg0 = EXTRACT_B1(instr);
    imm1 = EXTRACT_B2(instr);
    imm2 = EXTRACT_B3(instr);

    // If 0th arg is greater than 0, then jump to 1st arg
    if (ctx->r[reg0].value > 0) {
        ctx->pc = &ctx->codeSegment[imm1 - 1];
    } else {
        ctx->pc = &ctx->codeSegment[imm2 - 1];
    }
}

void opJump(struct VMContext *ctx, const uint32_t instr) {
    uint8_t imm0; // 0th arg of jump

    imm0 = EXTRACT_B1(instr);

    // Change the value of pc
    ctx->pc = &ctx->codeSegment[imm0 - 1];
}

void opPuts(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of puts

    reg0 = EXTRACT_B1(instr);

    printf("%s", (char *) &ctx->dataSegment[ctx->r[reg0].value]);
}

void opGets(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of gets

    reg0 = EXTRACT_B1(instr);

    gets((char *) &ctx->dataSegment[ctx->r[reg0].value]);
}
/*
 * [END] functions for each instructions
 */

void usageExit() {
    // TODO: show usage
    exit(1);
}

void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        f[i] = opInvalid;
    }

    // TODO: initialize function pointers
    f[0x00] = opHalt;
    f[0x10] = opLoad;
    f[0x20] = opStore;
    f[0x30] = opMove;
    f[0x40] = opPuti;
    f[0x50] = opAdd;
    f[0x60] = opSub;
    f[0x70] = opGt;
    f[0x80] = opGe;
    f[0x90] = opEq;
    f[0xa0] = opIte;
    f[0xb0] = opJump;
    f[0xc0] = opPuts;
    f[0xd0] = opGets;
}

void initRegs(Reg *r, uint32_t cnt)
{
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        r[i].type = 0;
        r[i].value = 0;
    }
}

/*
 * This function is for loading bytecodes from file to memory.
 */
uint32_t *loadCodeSegment(char *filename) {
    FILE *fp;
    uint32_t *bytecode;
    long readBytes;

    // Load bytecode file
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("fopen");
        return NULL;
    }

    // Allocate memory for bytecode
    bytecode = calloc(257, 4);

    // Read bytecode from file
    readBytes = fread(bytecode, 4, 256, fp);
    if (readBytes == 0) {
        if (feof(fp) != 0) {
            printf("Input File was empty\n");
            return NULL;
        } else {
            perror("fread");
            return NULL;
        }
    }

    // There is no reason to open file pointer until process is killed
    fclose(fp);

    return bytecode;
}

int main(int argc, char** argv) {
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    uint32_t *codeSegment; // Memory Space that stores pieces of codes
    uint8_t *dataSegment; // Memory Space that stores data

    // There should be at least one argument.
    if (argc < 2) usageExit();

    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Load bytecode from file
    codeSegment = loadCodeSegment(argv[1]);
    if (codeSegment == NULL) {
        return 1;
    }
    // Allocate heap memory
    dataSegment = calloc(8192, 1);
    // Initialize VM context.
    initVMContext(&vm, NUM_REGS, NUM_FUNCS, r, f, codeSegment, dataSegment);

    while (is_running) {
        // TODO: Read 4-byte bytecode, and set the pc accordingly
        stepVMContext(&vm);
    }

    // Zero indicates normal termination.
    return 0;
}
