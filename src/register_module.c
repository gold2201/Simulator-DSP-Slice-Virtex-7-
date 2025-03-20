#include "register_module.h"

void init_registers(DSP_Registers *regs) {
    regs->regA = 0;
    regs->regB = 0;
    regs->regC = 0;
    regs->regD = 0;
}

void set_regA(DSP_Registers *regs, int64_t value) {
    regs->regA = value;
}

void set_regB(DSP_Registers *regs, int64_t value) {
    regs->regB = value;
}

void set_regC(DSP_Registers *regs, int64_t value) {
    regs->regC = value;
}

void set_regD(DSP_Registers *regs, int64_t value) {
    regs->regD = value;
}

int64_t get_regA(const DSP_Registers *regs) {
    return regs->regA;
}

int64_t get_regB(const DSP_Registers *regs) {
    return regs->regB;
}

int64_t get_regC(const DSP_Registers *regs) {
    return regs->regC;
}

int64_t get_regD(const DSP_Registers *regs) {
    return regs->regD;
}

void reset_registers(DSP_Registers *regs) {
    regs->regA = 0;
    regs->regB = 0;
    regs->regC = 0;
    regs->regD = 0;
}

