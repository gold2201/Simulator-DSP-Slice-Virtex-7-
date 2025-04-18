#include "dsp_simulator.h"

void init_registers(DSP_Registers *regs) {
    regs->regA=regs->regB=regs->regC=regs->regD=0;
    regs->regMUL=0;
    regs->regP=0;
}

void set_regA(DSP_Registers *r, int64_t v){ if(r) r->regA=v; }
void set_regB(DSP_Registers *r, int64_t v){ if(r) r->regB=v; }
void set_regC(DSP_Registers *r, int64_t v){ if(r) r->regC=v; }
void set_regD(DSP_Registers *r, int64_t v){ if(r) r->regD=v; }

int64_t get_regA(const DSP_Registers *r){ return r? r->regA:0; }
int64_t get_regB(const DSP_Registers *r){ return r? r->regB:0; }
int64_t get_regC(const DSP_Registers *r){ return r? r->regC:0; }
int64_t get_regD(const DSP_Registers *r){ return r? r->regD:0; }

