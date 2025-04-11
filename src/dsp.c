#include "dsp_simulator.h"
#include <stdio.h>

// Умножение с предсложением: (A + D) * B
DSP_Result dsp_multiply_with_preadd(int64_t A, int64_t D, int64_t B) {
    DSP_Result res;
    int64_t mult = (A + D) * B;
    res.result = mult & RESULT_MASK;
    res.carryout = (mult >> 48) & 1;
    res.overflow = 0;
    return res;
}

// Простое умножение A * B
DSP_Result dsp_multiply(int64_t A, int64_t B) {
    DSP_Result res;
    int64_t mult = A * B;
    res.result = mult & RESULT_MASK;
    res.carryout = (mult >> 48) & 1;
    res.overflow = 0;
    return res;
}

// ALU с арифметикой и логикой
DSP_Result dsp_alu(int64_t X, int64_t Y, int64_t Z, uint8_t alumode) {
    DSP_Result res;
    int64_t alu;

    switch (alumode) {
        case 0x0: alu = X + Y + Z; break;
        case 0x1: alu = Z - (X + Y); break;
        case 0x2: alu = -Z + (X + Y) - 1; break;
        case 0x3: alu = ~(Z + X + Y); break;
        case 0x4: alu = X & Z; break;  // логическое И
        case 0x5: alu = X | Z; break;  // логическое ИЛИ
        case 0x6: alu = X ^ Z; break;  // логическое XOR
        case 0x7: alu = ~X; break;     // побитовое НЕ
        default: alu = X + Y + Z; break;
    }

    res.result = alu & RESULT_MASK;
    res.carryout = (alu >> 48) & 1;
    res.overflow = 0;
    return res;
}



