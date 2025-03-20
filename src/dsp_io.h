#ifndef DSP_IO_H
#define DSP_IO_H

#include "dsp_arithmetic.h"
#include "register_module.h"

#define MAX_LINE_LENGTH 512

void process_input_file(const char *input_filename, const char *output_filename);

#endif 