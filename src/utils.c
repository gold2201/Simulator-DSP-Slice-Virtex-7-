#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void parse_key_value(const char *token, const char *key, char *dest, size_t dest_size) {
    if (strncmp(token, key, strlen(key)) == 0 && token[strlen(key)] == '=') {
        strncpy(dest, token + strlen(key) + 1, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

uint64_t bin_to_uint64(const char *bin_str) {
    return strtoull(bin_str, NULL, 2);
}