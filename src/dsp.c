#include "dsp_simulator.h"
#include <stdio.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_BLUE    "\x1b[34m"

DSP_Result dsp_multiply_with_preadd(int64_t A, int64_t D, int64_t B) {
    DSP_Result res;
    int64_t sum = A + D;
    int64_t mult = sum * B;
    printf(COLOR_CYAN "\n--- [DSP MULTIPLY WITH PREADD] ---\n" COLOR_RESET);
    printf(COLOR_GREEN "  A = %ld\n  D = %ld\n  A + D = %ld\n  B = %ld\n  (A + D) * B = %ld\n" COLOR_RESET,
           A, D, sum, B, mult);
    res.result = mult & RESULT_MASK;
    res.carryout = (mult >> 48) & 1;
    res.overflow = 0;
    return res;
}

DSP_Result dsp_multiply(int64_t A, int64_t B) {
    DSP_Result res;
    int64_t mult = A * B;
    printf(COLOR_CYAN "\n--- [DSP MULTIPLY] ---\n" COLOR_RESET);
    printf(COLOR_GREEN "  A = %ld\n  B = %ld\n  A * B = %ld\n" COLOR_RESET,
           A, B, mult);
    res.result = mult & RESULT_MASK;
    res.carryout = (mult >> 48) & 1;
    res.overflow = 0;
    return res;
}

DSP_Result dsp_alu(int64_t X, int64_t Y, int64_t Z, uint8_t a) {
    DSP_Result res;
    int64_t v;
    const char *opName = "";

    printf(COLOR_CYAN "\n--- [DSP ALU] ---\n" COLOR_RESET);
    printf(COLOR_MAGENTA "  Inputs: X = %ld, Y = %ld, Z = %ld, ALUMODE = %u\n" COLOR_RESET, X, Y, Z, a);

    switch(a) {
      case 0: v = X + Y + Z; opName = "Z + X + Y"; break;
      case 1: v = Z - (X + Y); opName = "Z - (X + Y)"; break;
      case 2: v = -Z + (X + Y) - 1; opName = "-Z + (X + Y) - 1"; break;
      case 3: v = ~(Z + X + Y); opName = "~(Z + X + Y)"; break;
      case 4: v = X & Z; opName = "X & Z"; break;
      case 5: v = X | Z; opName = "X | Z"; break;
      case 6: v = X ^ Z; opName = "X ^ Z"; break;
      case 7: v = ~X; opName = "~X"; break;
      default: v = X + Y + Z; opName = "Default: Z + X + Y"; break;
    }

    printf(COLOR_YELLOW "  Operation: %s\n  Raw Result: %ld\n" COLOR_RESET, opName, v);

    res.result = v & RESULT_MASK;
    res.carryout = (v >> 48) & 1;
    res.overflow = 0;
    return res;
}



