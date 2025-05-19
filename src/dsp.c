#include "dsp_simulator.h"
#include "decoders.h"
#include <stdio.h>
#include <string.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_BLUE    "\x1b[34m"

#define MAX_48 ((1LL << 47) - 1)
#define MIN_48 (-(1LL << 47))


int64_t dsp_preadd(int64_t A, int64_t D, INMODE_Decoded* inm_d) {
    int64_t sum;
    if(!inm_d->add_sub) {
        sum = D + A;
        printf(COLOR_GREEN " D + A = %ld\n (D + A) " COLOR_RESET, sum);
    } else {
        sum = D - A;
        printf(COLOR_GREEN " D - A = %ld\n (D - A) " COLOR_RESET, sum);
    }
    return sum;
}

int64_t dsp_multiply(int64_t A, int64_t B, int64_t D, INMODE_Decoded* inm_d) {
    printf(COLOR_CYAN "\n--- [DSP MULTIPLY WITH PREADD] ---\n" COLOR_RESET);
    printf(COLOR_GREEN " A = %ld\n B = %ld\n D = %ld\n" COLOR_RESET, A, B, D);

    int64_t sum = dsp_preadd(A, D, inm_d);
    int64_t mult = sum * B;

    printf(COLOR_GREEN "* B = %ld\n" COLOR_RESET, mult);

    return mult;
}

DSP_Result dsp_alu(int64_t X, int64_t Y, int64_t Z, uint8_t alm, OPMODE_Decoded* opm_d) {
    DSP_Result res;
    int64_t v;
    const char *opName = "";

    printf(COLOR_CYAN "\n--- [DSP ALU] ---\n" COLOR_RESET);
    printf(COLOR_MAGENTA "  Inputs: X = %ld, Y = %ld, Z = %ld, ALUMODE = %u\n" COLOR_RESET, X, Y, Z, alm);

    //Сравнить младшие два бита ALUMODE с помощью XOR и убедиться что старший бит = 0
    if (alm < 8 && alm > 4) {
        if (((alm & 1) ^ ((alm & 2) >> 1)) != opm_d->routeY >> 1) {
            v = X ^ Z; opName = "X ^ Z";
        } else if ((!((alm & 1) ^ ((alm & 2) >> 1))) != opm_d->routeY >> 1) {
            v = ~(X ^ Z); opName = "!(X ^ Z)";
        }
    }

    if (!strcmp(opName, ""))
    {
        switch (alm)
        {
        case 0: v = X + Y + Z; opName = "Z + X + Y"; break;
        case 1: v = -Z + (X + Y) - 1; opName = "-Z + (X + Y) - 1"; break;
        case 2: v = ~(Z + X + Y); opName = "!(Z + X + Y)"; break;
        case 3: v = Z - (X + Y); opName = "Z - (X + Y)"; break;
        case 12: v = X & Z; opName = "X & Z"; break;
        case 13: v = X & ~Z; opName = "X & !Z"; break;
        case 14: v = ~(X & Z); opName = "~(X & Z)"; break;
        case 15: v = ~X | Z; opName = "~X | Z"; break;
        default:
            v = X + Y + Z;
            opName = "Default: Z + X + Y";
            break;
        }
    }

    printf(COLOR_YELLOW "  Operation: %s\n  Raw Result: %ld\n" COLOR_RESET, opName, v);

    res.result = mask64_to_48(v);
    res.overflow = (v > MAX_48 || v < MIN_48);
    res.underflow = (v < MIN_48);
    res.carryout = (v >> 48) & 1;
    return res;
}

int64_t dsp_route_x(DSP_Registers *Regs, OPMODE_Decoded *opm_d) {

    switch (opm_d->routeX)
    {
    case 0: return 0; break;
    case 1: return Regs->regMUL; break;
    case 2: return Regs->regP; break;
    case 3: return (Regs->regA << 18) | Regs->regB; break;
    default:
        return 0; break;
    }

}

int64_t dsp_route_y(DSP_Registers *Regs, OPMODE_Decoded *opm_d) {

    switch (opm_d->routeY)
    {
    case 0: return 0; break;
    case 1: return Regs->regMUL; break;
    case 2: return 0xFFFFFFFFFFFF; break;
    case 3: return Regs->regC; break;
    default:
        return 0; break;
    }

}

int64_t dsp_route_z(DSP_Registers *Regs, OPMODE_Decoded *opm_d) {

    switch (opm_d->routeZ)
    {
    case 0: return 0; break;
    case 1: return 0; break;
    case 2: return Regs->regP; break;
    case 3: return Regs->regC; break;
    case 4: return Regs->regP; break;
    case 5: return 0; break;
    case 6: return (Regs->regP << 17); break;
    case 7: return 0; break;
    default:
        return 0; 
    }

}

void pattern_detect(DSP_Result* result, int64_t pattern, int64_t mask) {

    int64_t res = result->result;
    mask = ~mask;
    res = mask & res;
    result->pattern_detect = res == pattern;

}