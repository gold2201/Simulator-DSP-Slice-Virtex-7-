#include "dsp_arithmetic.h"

#define RESULT_MASK ((1LL << 48) - 1)
#define CARRYOUT_MASK (1LL << 48)

DSP_Result dsp_multiply(int32_t a, int32_t b) {
    DSP_Result res;
    int64_t raw_result = (int64_t)a * (int64_t)b;
    res.carryout = (raw_result & CARRYOUT_MASK) ? 1 : 0;
    res.overflow = (raw_result > (int64_t)RESULT_MASK || raw_result < -(int64_t)RESULT_MASK) ? 1 : 0;
    res.result = raw_result & RESULT_MASK;
    return res;
}

DSP_Result dsp_mac(int32_t a, int32_t b, int64_t acc) {
    DSP_Result res;
    int64_t raw_result = (int64_t)a * (int64_t)b + acc;
    res.carryout = (raw_result & CARRYOUT_MASK) ? 1 : 0;
    res.overflow = (raw_result > (int64_t)RESULT_MASK || raw_result < -(int64_t)RESULT_MASK) ? 1 : 0;
    res.result = raw_result & RESULT_MASK;
    return res;
}

DSP_Result dsp_add(int64_t a, int64_t b) {
    DSP_Result res;
    int64_t raw_result = a + b;
    res.carryout = (raw_result & CARRYOUT_MASK) ? 1 : 0;
    res.overflow = (raw_result > (int64_t)RESULT_MASK || raw_result < -(int64_t)RESULT_MASK) ? 1 : 0;
    res.result = raw_result & RESULT_MASK;
    return res;
}

DSP_Result dsp_subtract(int64_t a, int64_t b) {
    DSP_Result res;
    int64_t raw_result = a - b;
    res.carryout = (raw_result & CARRYOUT_MASK) ? 1 : 0;
    res.overflow = (raw_result > (int64_t)RESULT_MASK || raw_result < -(int64_t)RESULT_MASK) ? 1 : 0;
    res.result = raw_result & RESULT_MASK;
    return res;
}