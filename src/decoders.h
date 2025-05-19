#ifndef DECODERS_H
#define DECODERS_H

#include <stdint.h>

typedef struct {
    int routeX, routeY, routeZ; 
} OPMODE_Decoded;

typedef struct {
    int zero_A, zero_D, add_sub; 
} INMODE_Decoded;

typedef struct {
    int CE_A, CE_B, CE_C, CE_D, CE_P, CE_Mul; 
    int RST_A, RST_B, RST_C, RST_D, RST_P, RST_Mul; 
} Control;

OPMODE_Decoded decode_opmode(uint8_t op);
INMODE_Decoded decode_inmode(uint8_t in);
Control decode_ctrl(uint8_t ce, uint8_t rst);

#endif