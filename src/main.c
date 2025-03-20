#include "dsp_io.h"

int main(void) {
    const char *input_filename = "input.txt";
    const char *output_filename = "output.txt";

    process_input_file(input_filename, output_filename);
    
    return 0;
}