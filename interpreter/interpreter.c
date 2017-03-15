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
    // f[0x00] = halt;
    // f[0x10] = load;
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
char *loadByteCode(char *filename) {
    FILE *fp;
    long originalSize;
    long adjustedSize;
    char *bytecode;
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
    bytecode = calloc(1, adjustedSize);

    // Read bytecode from file
    readBytes = fread(bytecode, originalSize, 1, fp);
    if (readBytes != originalSize) {
        perror("fread");
        return NULL;
    }

    // Mark the end of bytecode
    *(uint32_t *)(bytecode + adjustedSize - 4) = 0xffffffff;

    // There is no reason to open file pointer until process is killed
    fclose(fp);

    return bytecode;
}

int main(int argc, char** argv) {
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    char* bytecode; // Memory Space that stores pieces of codes.
    uint32_t* pc;

    // There should be at least one argument.
    if (argc < 2) usageExit();

    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Initialize VM context.
    initVMContext(&vm, NUM_REGS, NUM_FUNCS, r, f);

    // Load bytecode from file
    bytecode = loadByteCode(argv[1]);
    if (bytecode == NULL) {
        return 1;
    }

    // Set pc to point at the beginning of bytecode
    pc = (uint32_t *)bytecode;

    while (is_running) {
        // TODO: Read 4-byte bytecode, and set the pc accordingly
        stepVMContext(&vm, &pc);
    }

    // Zero indicates normal termination.
    return 0;
}
