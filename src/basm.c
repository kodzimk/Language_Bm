#define BM_IMPLEMENTATION
#include"bm.h"

char* shift(int *argc, char*** argv)
{
    assert(*argc > 0);

    char* result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

int main(int argc, char** argv)
{
    char* program_name = shift(&argc, &argv);
    if (argc  == 0) {
        fprintf(stderr, "Usage: %s <input.ebasm> <output.bm>\n",program_name);
        fprintf(stdout, "ERROR: expected input and output\n");
        exit(1);
    }

    const char* input_file_path = shift(&argc,&argv);

    if (argc == 0) {
        fprintf(stderr, "Usage: %s <input.ebasm> <output.bm>\n", program_name);
        fprintf(stdout, "ERROR: expected input and output\n");
        exit(1);
    }

    const char* output_file_path = shift(&argc, &argv);

    string_t source = slurp_file(input_file_path);

    bm.program_size = vm_translate_source(source,
        bm.program,
        BM_PROGRAM_CAPACITY);

    free(source.buffer);

    Err err = bm_execure_program(&bm, 69);
    bm_save_program_to_file(bm.program, bm.program_size, output_file_path);

    if (err != ERR_OK) {
        return 1;
    }

    return 0;
}