#ifndef DSP_SIMULATOR_H
#define DSP_SIMULATOR_H
#include <stdint.h>
#include "decoders.h"

#define RESULT_MASK ((1LL<<48)-1)
#define CARRYOUT_MASK (1LL<<48)

#define MASK ((1LL << 46))

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
int64_t dsp_multiply(int64_t A, int64_t B, int64_t D, INMODE_Decoded* inm_d);
DSP_Result dsp_alu(int64_t X, int64_t Y, int64_t Z, uint8_t alm, OPMODE_Decoded* opm_d);
int64_t dsp_route_x(DSP_Registers *Regs, OPMODE_Decoded *opm_d);
int64_t dsp_route_y(DSP_Registers *Regs, OPMODE_Decoded *opm_d);
int64_t dsp_route_z(DSP_Registers *Regs, OPMODE_Decoded *opm_d);
void pattern_detect(DSP_Result* result, int64_t pattern, int64_t mask);

int64_t mask64_to_48(int64_t num);

#endif
