#include "decoders.h"

#define OPMODE_01 0x03
#define OPMODE_23 0x0C
#define OPMODE_46 0x70

OPMODE_Decoded decode_opmode(uint8_t op)
{
    return (OPMODE_Decoded){(op & OPMODE_01), (op & OPMODE_23) >> 2, (op & OPMODE_46) >> 4};
}

Control decode_ctrl(uint8_t ce, uint8_t rst)
{
    return (Control) {
        (ce >> 5) & 1,
        (ce >> 4) & 1,
        (ce >> 3) & 1,
        (ce >> 2) & 1,
        (ce >> 1) & 1,
        ce & 1,
        (rst >> 5) & 1,
        (rst >> 4) & 1,
        (rst >> 3) & 1,
        (rst >> 2) & 1,
        (rst >> 1) & 1,
        rst & 1
    };
}