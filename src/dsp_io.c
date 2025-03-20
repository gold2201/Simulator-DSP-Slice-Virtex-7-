#include "dsp_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define RESULT_MASK ((1LL << 48) - 1)
#define CARRYOUT_MASK (1LL << 48)

// Функция для преобразования двоичной строки в число
uint64_t bin_to_uint64(const char *bin_str) {
    uint64_t result = 0;
    while (*bin_str) {
        result = (result << 1) | (*bin_str - '0');
        bin_str++;
    }
    return result;
}

// Функция для разбора ключ-значение (например, "A=1010")
void parse_key_value(const char *token, const char *key, char *value, size_t max_len) {
    size_t key_len = strlen(key);
    if (strncmp(token, key, key_len) == 0 && token[key_len] == '=') {
        strncpy(value, token + key_len + 1, max_len - 1);
        value[max_len - 1] = '\0';
    }
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

        char *token = strtok(line, " \n");
        while (token != NULL) {
            parse_key_value(token, "A", A_str, sizeof(A_str));
            parse_key_value(token, "B", B_str, sizeof(B_str));
            parse_key_value(token, "C", C_str, sizeof(C_str));
            parse_key_value(token, "D", D_str, sizeof(D_str));
            parse_key_value(token, "OPMODE", OPMODE_str, sizeof(OPMODE_str));
            token = strtok(NULL, " \n");
        }

        uint64_t A_val = bin_to_uint64(A_str);
        uint64_t B_val = bin_to_uint64(B_str);
        uint64_t C_val = bin_to_uint64(C_str);
        uint64_t D_val = bin_to_uint64(D_str);

        set_regA(&regs, A_val);
        set_regB(&regs, B_val);
        set_regC(&regs, C_val);
        set_regD(&regs, D_val);

        DSP_Result opRes;
        if (strcmp(OPMODE_str, "0000000") == 0) {
            opRes = dsp_multiply((int32_t)get_regA(&regs), (int32_t)get_regB(&regs));
        } else if (strcmp(OPMODE_str, "0000001") == 0) {
            opRes = dsp_mac((int32_t)get_regA(&regs), (int32_t)get_regB(&regs), get_regC(&regs));
        } else if (strcmp(OPMODE_str, "0000010") == 0) {
            opRes = dsp_add(get_regC(&regs), get_regD(&regs));
        } else if (strcmp(OPMODE_str, "0000011") == 0) {
            opRes = dsp_subtract(get_regC(&regs), get_regD(&regs));
        } else {
            fprintf(outFile, "Тактовый цикл %d: Неизвестный OPMODE=%s\n", clockCycle, OPMODE_str);
            continue;
        }
        
        // Вывод результата, переполнения и переноса из структуры DSP_Result
        fprintf(outFile, "P=%048lld CARRYOUT=%d OVERFLOW=%d\n", 
                (unsigned long long)opRes.result, opRes.carryout, opRes.overflow);
    }

    fclose(inFile);
    fclose(outFile);
}

