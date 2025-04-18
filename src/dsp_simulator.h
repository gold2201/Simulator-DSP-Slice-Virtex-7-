#ifndef DSP_SIMULATOR_H
#define DSP_SIMULATOR_H
#include <stdint.h>
#define RESULT_MASK ((1LL<<48)-1)
#define CARRYOUT_MASK (1LL<<48)

typedef struct {
    int64_t result;
    int overflow;
    int carryout;
    int pattern_detect;
    int underflow;
} DSP_Result;

typedef struct {
    int64_t regA, regB, regC, regD;
    int64_t regMUL;
    int64_t regP;
} DSP_Registers;

// регистры
void init_registers(DSP_Registers *regs);
void set_regA(DSP_Registers*, int64_t);
void set_regB(DSP_Registers*, int64_t);
void set_regC(DSP_Registers*, int64_t);
void set_regD(DSP_Registers*, int64_t);
int64_t get_regA(const DSP_Registers*);
int64_t get_regB(const DSP_Registers*);
int64_t get_regC(const DSP_Registers*);
int64_t get_regD(const DSP_Registers*);

// операции
DSP_Result dsp_multiply_with_preadd(int64_t,int64_t,int64_t);
DSP_Result dsp_multiply(int64_t,int64_t);
DSP_Result dsp_alu(int64_t,int64_t,int64_t,uint8_t);

#endif
