#ifndef DSP_SIMULATOR_H
#define DSP_SIMULATOR_H

#include <stdint.h>

#define RESULT_MASK ((1LL << 48) - 1)
#define CARRYOUT_MASK (1LL << 48)

typedef struct {
    int64_t result;
    int overflow;
    int carryout;
} DSP_Result;

typedef struct {
    int64_t regA, regB, regC, regD;
    int64_t regX, regY, regZ;
    int64_t regMUL; // временный регистр для хранения результата умножения
} DSP_Registers;

// Функции управления регистрами
void init_registers(DSP_Registers *regs);
void set_regA(DSP_Registers *regs, int64_t value);
void set_regB(DSP_Registers *regs, int64_t value);
void set_regC(DSP_Registers *regs, int64_t value);
void set_regD(DSP_Registers *regs, int64_t value);
int64_t get_regA(const DSP_Registers *regs);
int64_t get_regB(const DSP_Registers *regs);
int64_t get_regC(const DSP_Registers *regs);
int64_t get_regD(const DSP_Registers *regs);

// DSP операции
DSP_Result dsp_multiply_with_preadd(int64_t A, int64_t D, int64_t B);
DSP_Result dsp_multiply(int64_t A, int64_t B);
DSP_Result dsp_alu(int64_t X, int64_t Y, int64_t Z, uint8_t alumode);

#endif
