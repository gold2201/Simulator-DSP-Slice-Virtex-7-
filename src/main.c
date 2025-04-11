#include "dsp_simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 256

void parse_key_value(const char *token, const char *key, char *dest, size_t dest_size);
uint64_t bin_to_uint64(const char *bin_str);

// Структура для декодированного OPMODE
typedef struct {
    int use_preadd;
    int routeToX;
    int routeToY;
    int y_const;
    int z_enable;
} OPMODE_Decoded;

OPMODE_Decoded decode_opmode(uint8_t opmode) {
    OPMODE_Decoded decoded;
    decoded.use_preadd = (opmode & (1 << 6)) != 0;
    decoded.routeToX  = (opmode & (1 << 5)) != 0;
    decoded.routeToY  = (opmode & (1 << 4)) != 0;
    decoded.y_const   = (opmode & (1 << 3)) != 0;
    decoded.z_enable  = (opmode & (1 << 2)) != 0;
    return decoded;
}

void process_input_file(const char *input_filename, const char *output_filename) {
    FILE *inFile = fopen(input_filename, "r");
    if (!inFile) {
        perror("Ошибка при открытии входного файла");
        return;
    }
    FILE *outFile = fopen(output_filename, "w");
    if (!outFile) {
        perror("Ошибка при открытии выходного файла");
        fclose(inFile);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int clockCycle = 0;
    DSP_Registers regs;
    init_registers(&regs);

    while (fgets(line, sizeof(line), inFile)) {
        clockCycle++;
        char A_str[35] = "0", B_str[20] = "0", C_str[60] = "0", D_str[30] = "0";
        char OPMODE_str[10] = "0000000";
        char ALUMODE_str[10] = "0000";
        char USEMUL_str[10] = "000";

        char *token = strtok(line, " \n");
        while (token != NULL) {
            parse_key_value(token, "A", A_str, sizeof(A_str));
            parse_key_value(token, "B", B_str, sizeof(B_str));
            parse_key_value(token, "C", C_str, sizeof(C_str));
            parse_key_value(token, "D", D_str, sizeof(D_str));
            parse_key_value(token, "OPMODE", OPMODE_str, sizeof(OPMODE_str));
            parse_key_value(token, "ALUMODE", ALUMODE_str, sizeof(ALUMODE_str));
            parse_key_value(token, "USEMUL", USEMUL_str, sizeof(USEMUL_str));
            token = strtok(NULL, " \n");
        }

        uint64_t A_val = bin_to_uint64(A_str);
        uint64_t B_val = bin_to_uint64(B_str);
        uint64_t C_val = bin_to_uint64(C_str);
        uint64_t D_val = bin_to_uint64(D_str);
        uint8_t alumode = (uint8_t)bin_to_uint64(ALUMODE_str);
        uint8_t opmode = (uint8_t)bin_to_uint64(OPMODE_str);
        uint8_t usemul_code = (uint8_t)bin_to_uint64(USEMUL_str);

        set_regA(&regs, A_val);
        set_regB(&regs, B_val);
        set_regC(&regs, C_val);
        set_regD(&regs, D_val);

        OPMODE_Decoded mode = decode_opmode(opmode);

        DSP_Result mulRes;
        if (mode.use_preadd) {
            mulRes = dsp_multiply_with_preadd(get_regA(&regs), get_regD(&regs), get_regB(&regs));
        } else {
            mulRes = dsp_multiply(get_regA(&regs), get_regB(&regs));
        }

        regs.regMUL = mulRes.result;

        // Обработка USEMUL через бинарный код
        switch (usemul_code) {
            case 0b001: set_regA(&regs, regs.regMUL); break;
            case 0b010: set_regB(&regs, regs.regMUL); break;
            case 0b011: set_regC(&regs, regs.regMUL); break;
            case 0b100: set_regD(&regs, regs.regMUL); break;
            default: break; // 000 или другое — не использовать
        }

        int64_t X_val = mode.routeToX ? regs.regMUL : get_regA(&regs);
        int64_t Y_val = mode.routeToY ? regs.regMUL : (mode.y_const ? 1 : 0);
        int64_t Z_val = mode.z_enable ? get_regC(&regs) : 0;

        DSP_Result result = dsp_alu(X_val, Y_val, Z_val, alumode);

        fprintf(outFile, "P=%048ld CARRYOUT=%d OVERFLOW=%d\n", result.result, result.carryout, result.overflow);
    }

    fclose(inFile);
    fclose(outFile);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input.txt output.txt\n", argv[0]);
        return 1;
    }

    process_input_file(argv[1], argv[2]);
    return 0;
}



