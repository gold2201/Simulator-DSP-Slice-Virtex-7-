#include "dsp_simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 512
#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_BLUE    "\x1b[34m"

void parse_key_value(const char *token, const char *key, char *dest, size_t dest_size);
uint64_t bin_to_uint64(const char *bin_str);

typedef struct { int use_preadd, routeX, routeY, y_const, z_enable; } OPMODE_Decoded;
OPMODE_Decoded decode_opmode(uint8_t op) {
    return (OPMODE_Decoded){ (op >> 6) & 1, (op >> 5) & 1, (op >> 4) & 1, (op >> 3) & 1, (op >> 2) & 1 };
}

typedef struct { int CE_A, CE_B, CE_C, CE_D, CE_P; int RST_A, RST_B, RST_C, RST_D, RST_P; } Control;
Control decode_ctrl(uint8_t ce, uint8_t rst) {
    return (Control){ (ce>>4)&1, (ce>>3)&1, (ce>>2)&1, (ce>>1)&1, ce&1, (rst>>4)&1, (rst>>3)&1, (rst>>2)&1, (rst>>1)&1, rst&1 };
}

void process_input_file(const char *in_fn, const char *out_fn) {
    FILE *in = fopen(in_fn, "r"); if(!in){ perror("Error opening input"); return; }
    FILE *out = fopen(out_fn, "w"); if(!out){ perror("Error opening output"); fclose(in); return; }

    char line[MAX_LINE_LENGTH];
    DSP_Registers R; init_registers(&R);
    int tick = 0;

    while(fgets(line, sizeof(line), in)) {
        tick++;
        printf(COLOR_BLUE "\n===== CLOCK CYCLE %d =====\n" COLOR_RESET, tick);

        char A_s[31]="0", B_s[19]="0", C_s[49]="0", D_s[26]="0";
        char O_s[8]="0000000", L_s[5]="0000", CE_s[6]="00000", RST_s[6]="00000";

        char *tok = strtok(line, " \n");
        while(tok) {
            parse_key_value(tok, "A", A_s, sizeof(A_s));
            parse_key_value(tok, "B", B_s, sizeof(B_s));
            parse_key_value(tok, "C", C_s, sizeof(C_s));
            parse_key_value(tok, "D", D_s, sizeof(D_s));
            parse_key_value(tok, "OPMODE", O_s, sizeof(O_s));
            parse_key_value(tok, "ALUMODE", L_s, sizeof(L_s));
            parse_key_value(tok, "CE", CE_s, sizeof(CE_s));
            parse_key_value(tok, "RST", RST_s, sizeof(RST_s));
            tok = strtok(NULL, " \n");
        }

        uint64_t A = bin_to_uint64(A_s) & ((1ULL<<30)-1);
        uint64_t B = bin_to_uint64(B_s) & ((1ULL<<18)-1);
        uint64_t C = bin_to_uint64(C_s) & ((1ULL<<48)-1);
        uint64_t D = bin_to_uint64(D_s) & ((1ULL<<25)-1);
        uint8_t opm  = bin_to_uint64(O_s) & 0x7F;
        uint8_t alm  = bin_to_uint64(L_s) & 0x0F;
        uint8_t ce   = bin_to_uint64(CE_s) & 0x1F;
        uint8_t rst  = bin_to_uint64(RST_s) & 0x1F;

        printf(COLOR_GREEN "[INPUT] A=%s, B=%s, C=%s, D=%s\n        OPMODE=%s, ALUMODE=%s, CE=%s, RST=%s\n" COLOR_RESET,
            A_s, B_s, C_s, D_s, O_s, L_s, CE_s, RST_s);

        Control ctrl = decode_ctrl(ce, rst);
        if(ctrl.RST_A) R.regA=0; else if(ctrl.CE_A) R.regA=A;
        if(ctrl.RST_B) R.regB=0; else if(ctrl.CE_B) R.regB=B;
        if(ctrl.RST_C) R.regC=0; else if(ctrl.CE_C) R.regC=C;
        if(ctrl.RST_D) R.regD=0; else if(ctrl.CE_D) R.regD=D;

        OPMODE_Decoded m = decode_opmode(opm);
        DSP_Result mres = m.use_preadd ?
            dsp_multiply_with_preadd(R.regA, R.regD, R.regB) :
            dsp_multiply(R.regA, R.regB);
        R.regMUL = mres.result;

        int64_t X = m.routeX ? R.regMUL : R.regA;
        int64_t Y = m.routeY ? R.regMUL : (m.y_const ? 1 : 0);
        int64_t Z = m.z_enable ? R.regC : 0;
        DSP_Result pres = dsp_alu(X, Y, Z, alm);
        pres.pattern_detect = (pres.result == ((1ULL<<48)-1));
        pres.underflow = ((int64_t)pres.result < -(1LL<<47));

        if(ctrl.RST_P) R.regP=0;
        else if(ctrl.CE_P) R.regP=pres.result;

        printf(COLOR_YELLOW "[RESULT] P=0x%012lx  CARRY=%d  OVER=%d  PATTERN=%d  UNDERFLOW=%d\n" COLOR_RESET,
               R.regP, pres.carryout, pres.overflow, pres.pattern_detect, pres.underflow);

        fprintf(out, "P=%012lx CARRY=%d OVER=%d PAT=%d UNDER=%d\n",
                R.regP, pres.carryout, pres.overflow, pres.pattern_detect, pres.underflow);
    }

    fclose(in);
    fclose(out);
}

int main(int argc, char *argv[]) {
    if(argc!=3) { printf("Usage: %s <in> <out>\n", argv[0]); return 1; }
    process_input_file(argv[1], argv[2]);
    return 0;
}




