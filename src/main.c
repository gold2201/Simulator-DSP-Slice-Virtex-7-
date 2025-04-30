#include "dsp_simulator.h"
#include "decoders.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 512
#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BRIGHT_YELLOW  "\x1b[93m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RED     "\x1b[31m"

void parse_key_value(const char *token, const char *key, char *dest, size_t dest_size);
uint64_t bin_to_uint64(const char *bin_str);


void to_binary_str(uint64_t val, int width, char *out) {
    for (int i = width - 1; i >= 0; --i)
        out[width - 1 - i] = ((val >> i) & 1) ? '1' : '0';
    out[width] = '\0';
}

void check_reg_size(char* A_s, char* B_s, char* C_s, char* D_s) {
    if (strlen(A_s) > 30) printf(COLOR_BRIGHT_YELLOW "[WARN] A превышает 30 бит\n" COLOR_RESET);
    if (strlen(B_s) > 18) printf(COLOR_BRIGHT_YELLOW "[WARN] B превышает 18 бит\n" COLOR_RESET);
    if (strlen(C_s) > 48) printf(COLOR_BRIGHT_YELLOW "[WARN] C превышает 48 бит\n" COLOR_RESET);
    if (strlen(D_s) > 25) printf(COLOR_BRIGHT_YELLOW "[WARN] D превышает 25 бит\n" COLOR_RESET);
}

int validate_input(uint8_t opm, uint8_t alm) {
    if(((opm & 0x03) == 0x01) && (((opm & 0x0C) >> 2) != (opm & 0x03))) {
        printf(COLOR_RED "[ERROR] OPMODE[1:0] = 01 должен использоваться вместе с OPMODE[3:2] = 01\n" COLOR_RESET);
        return 1;
    }
    if(((opm & 0x0C) == 0x04) && (((opm & 0x03) << 2) != (opm & 0x0C))) {
        printf(COLOR_RED "[ERROR] OPMODE[3:2] = 01 должен использоваться вместе с OPMODE[1:0] = 01\n" COLOR_RESET);
        return 1;
    }

    if((alm & 0x0C) != 0 && ((opm & 0x04) >> 2) == 1) {
        printf(COLOR_RED "[ERROR] ALUMODE[3:2] != 00 должен использоваться вместе с OPMODE[3:2] = X0\n" COLOR_RESET);
        return 1;
    }

    if((alm & 0x0C) >> 2 == 0x02) {
        printf(COLOR_RED "[ERROR] Комбинация ALUMODE[3:2] == 10 не поддерживается\n" COLOR_RESET);
        return 1;
    }

    return 0;
}   

uint8_t parse_data(char* line, DSP_Registers *Regs, Control *ctrl, INMODE_Decoded *inm_d, OPMODE_Decoded *opm_d, int tick) {
    printf(COLOR_BLUE "\n===== CLOCK CYCLE %d =====\n" COLOR_RESET, tick);

    char A_s[65] = "0", B_s[65] = "0", C_s[65] = "0", D_s[65] = "0";
    char O_s[8] = "0000000", L_s[5] = "0000", CE_s[7] = "000000", RST_s[6] = "00000", IN_s[6] = "00000";

    char* tok = strtok(line, " \n");
    while (tok) {
        parse_key_value(tok, "A", A_s, sizeof(A_s));
        parse_key_value(tok, "B", B_s, sizeof(B_s));
        parse_key_value(tok, "C", C_s, sizeof(C_s));
        parse_key_value(tok, "D", D_s, sizeof(D_s));
        parse_key_value(tok, "OPMODE", O_s, sizeof(O_s));
        parse_key_value(tok, "ALUMODE", L_s, sizeof(L_s));
        parse_key_value(tok, "CE", CE_s, sizeof(CE_s));
        parse_key_value(tok, "RST", RST_s, sizeof(RST_s));
        parse_key_value(tok, "INMODE", IN_s, sizeof(IN_s));
        tok = strtok(NULL, " \n");
    }

    check_reg_size(A_s, B_s, C_s, D_s);

    uint64_t A = bin_to_uint64(A_s) & ((1ULL << 30) - 1);
    uint64_t B = bin_to_uint64(B_s) & ((1ULL << 18) - 1);
    uint64_t C = bin_to_uint64(C_s) & ((1ULL << 48) - 1);
    uint64_t D = bin_to_uint64(D_s) & ((1ULL << 25) - 1);
    uint8_t opm = bin_to_uint64(O_s) & 0x7F;
    uint8_t alm = bin_to_uint64(L_s) & 0x0F;
    uint8_t ce = bin_to_uint64(CE_s) & 0x3F;
    uint8_t rst = bin_to_uint64(RST_s) & 0x1F;
    uint8_t inm = bin_to_uint64(IN_s) & 0x1F;

    if(validate_input(opm, alm)) {
        return 0x10;
    }

    *ctrl = decode_ctrl(ce, rst);
    *inm_d = decode_inmode(inm);
    *opm_d = decode_opmode(opm);

    if (ctrl->RST_A)
        Regs->regA = 0;
    else if (ctrl->CE_A)
        Regs->regA = inm_d->zero_A ? 0 : A;

    if (ctrl->RST_B)
        Regs->regB = 0;
    else if (ctrl->CE_B)
        Regs->regB = B;

    if (ctrl->RST_C)
        Regs->regC = 0;
    else if (ctrl->CE_C)
        Regs->regC = C;

    if (ctrl->RST_D)
        Regs->regD = 0;
    else if (ctrl->CE_D)
        Regs->regD = inm_d->zero_D ? 0 : D;

    return alm;
}

void process_input_file(const char *in_fn, const char *out_fn) {
    char line[MAX_LINE_LENGTH];
    DSP_Registers Regs;
    Control ctrl;
    INMODE_Decoded inm_d;
    OPMODE_Decoded opm_d;

    FILE *in = fopen(in_fn, "r");
    if (!in)
    {
        perror("Error opening input");
        return;
    }
    FILE *out = fopen(out_fn, "w");
    if (!out)
    {
        perror("Error opening output");
        fclose(in);
        return;
    }

    init_registers(&Regs);

    int tick = 0;
    while(fgets(line, sizeof(line), in)) {
        tick++;

        uint8_t alm = parse_data(line, &Regs, &ctrl, &inm_d, &opm_d, tick);
        if(alm == 0x10) {
            printf(COLOR_RED "[ERROR] parse_data()\n" COLOR_RESET);
            printf("[INFO] Переход на следующий такт\n");
            continue;
        }

        if (ctrl.RST_Mul) {
            Regs.regMUL = 0;
        } else if(ctrl.CE_Mul) {
            Regs.regMUL = dsp_multiply_with_preadd(Regs.regA, Regs.regD, Regs.regB, &inm_d);
        }
        int64_t X = dsp_route_x(&Regs, &opm_d);
        int64_t Y = dsp_route_y(&Regs, &opm_d);
        int64_t Z = dsp_route_z(&Regs, &opm_d);
        DSP_Result pres = dsp_alu(X, Y, Z, alm, &opm_d);
        pattern_detect(&pres, 0, MASK);
        pres.underflow = ((int64_t)pres.result < -(1LL<<47));

        if(ctrl.RST_P) Regs.regP=0;
        else if(ctrl.CE_P) Regs.regP=pres.result;

        char bin_result[49];
        to_binary_str(Regs.regP, 48, bin_result);

        printf(COLOR_YELLOW "[RESULT] P=%s CARRY=%d OVER=%d PATTERN=%d UNDERFLOW=%d\n" COLOR_RESET,
               bin_result, pres.carryout, pres.overflow, pres.pattern_detect, pres.underflow);

        fprintf(out, "P=%s CARRY=%d OVER=%d PAT=%d UNDER=%d\n",
                bin_result, pres.carryout, pres.overflow, pres.pattern_detect, pres.underflow);
    }

    fclose(in);
    fclose(out);
}

int compare_files(const char *f1, const char *f2) {
    FILE *fp1 = fopen(f1, "r");
    FILE *fp2 = fopen(f2, "r");
    if (!fp1 || !fp2) {
        printf(COLOR_RED "[ERROR] Не удалось открыть один из файлов для сравнения\n" COLOR_RESET);
        return 1;
    }

    char line1[MAX_LINE_LENGTH], line2[MAX_LINE_LENGTH];
    int line_num = 0;
    int success = 1;

    while (fgets(line1, sizeof(line1), fp1) && fgets(line2, sizeof(line2), fp2)) {
        line_num++;
        if (strcmp(line1, line2) != 0) {
            printf(COLOR_RED "[ERROR] Несовпадение в строке %d:\n" COLOR_RESET, line_num);
            printf("  output.txt : %s", line1);
            printf("  expected   : %s", line2);
            success = 0;
        }
    }

    if ((fgets(line1, sizeof(line1), fp1) != NULL) || (fgets(line2, sizeof(line2), fp2) != NULL)) {
        printf(COLOR_RED "[ERROR] Файлы имеют разную длину\n" COLOR_RESET);
        success = 0;
    }

    fclose(fp1);
    fclose(fp2);

    if (success)
        printf(COLOR_GREEN "\nФайлы совпадают. Тест пройден!\n" COLOR_RESET);
    else
        printf(COLOR_RED "\nТест не пройден. См. выше различия.\n" COLOR_RESET);

    return !success;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s input.txt output.txt [test_run]\n", argv[0]);
        return 1;
    }

    process_input_file(argv[1], argv[2]);

    // Тестовый режим, если указан 3-й аргумент test_run
    if (argc == 4 && strcmp(argv[3], "test_run") == 0) {
        const char *expected_file = "output_expected.txt";
        return compare_files(argv[2], expected_file);
    }

    return 0;
}





