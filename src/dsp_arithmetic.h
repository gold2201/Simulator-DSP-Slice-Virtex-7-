#ifndef DSP_ARITHMETIC_H
#define DSP_ARITHMETIC_H

#include <stdint.h>

typedef struct {
    int64_t result;
    int overflow;
    int carryout;
} DSP_Result;

DSP_Result dsp_multiply(int32_t a, int32_t b);
DSP_Result dsp_mac(int32_t a, int32_t b, int64_t acc);
DSP_Result dsp_add(int64_t a, int64_t b);
DSP_Result dsp_subtract(int64_t a, int64_t b);

#endif 