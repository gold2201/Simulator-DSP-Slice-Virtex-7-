#ifndef REGISTER_MODULE_H
#define REGISTER_MODULE_H

#include <stdint.h>

typedef struct {
    int64_t regA;
    int64_t regB;
    int64_t regC;
    int64_t regD;
} DSP_Registers;

void init_registers(DSP_Registers *regs);
void set_regA(DSP_Registers *regs, int64_t value);
void set_regB(DSP_Registers *regs, int64_t value);
void set_regC(DSP_Registers *regs, int64_t value);
void set_regD(DSP_Registers *regs, int64_t value);
int64_t get_regA(const DSP_Registers *regs);
int64_t get_regB(const DSP_Registers *regs);
int64_t get_regC(const DSP_Registers *regs);
int64_t get_regD(const DSP_Registers *regs);
void reset_registers(DSP_Registers *regs);

#endif 