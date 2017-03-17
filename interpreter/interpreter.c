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
        printf("invalid data section address range\n");
        exit(1);
    }
}

/*
 * [START] functions for each instructions
 */
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
    ctx->dataSegment[ctx->r[reg0].value] = 0xff & ctx->r[reg0].value;
}

void opMove(struct VMContext *ctx, const uint32_t instr) {
    uint8_t reg0; // 0th arg of move
    uint8_t reg1; // 1st arg of move

    reg0 = EXTRACT_B1(instr);
    reg1 = EXTRACT_B2(instr);

    // Move value from register to register
    ctx->r[reg0].value = ctx->r[reg1].value;
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
        f[i] = NULL;
    }

    // TODO: initialize function pointers
    f[0x00] = opHalt;
    f[0x10] = opLoad;
    f[0x20] = opStore;
    f[0x30] = opMove;
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
    long originalSize;
    long adjustedSize;
    uint32_t *bytecode;
    long readBytes;

    // Load bytecode file
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("fopen");
        return NULL;
    }

    // Calculate size of bytecode file
    fseek(fp, 0, SEEK_END);
    originalSize = ftell(fp);
    if (originalSize == -1) {
        perror("ftell");
        return NULL;
    }
    fseek(fp, 0, SEEK_SET);

    // Make size multiple of 4
    if ((originalSize & 0x3) != 0) {
        adjustedSize = ((originalSize >> 2) + 1) << 2;
    } else {
        adjustedSize = originalSize;
    }
    // Add 4 to size to mark the end of bytecode
    adjustedSize += 4;

    // Allocate memory for bytecode
    bytecode = calloc(adjustedSize >> 2, 4);

    // Read bytecode from file
    readBytes = fread(bytecode, 1, originalSize, fp);

    if (readBytes != originalSize) {
        perror("fread");
        return NULL;
    }

    // Mark the end of bytecode
    bytecode[(adjustedSize >> 2) - 1] = 0xffffffff;

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
    uint32_t* pc;

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

    // Set pc to point at the beginning of bytecode
    pc = codeSegment;

    while (is_running) {
        // TODO: Read 4-byte bytecode, and set the pc accordingly
        stepVMContext(&vm, &pc);
    }

    // Zero indicates normal termination.
    return 0;
}
