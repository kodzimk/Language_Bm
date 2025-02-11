#define BM_IMPLEMENTATION
#include"bm.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./bm <output.bm>\n");
        fprintf(stdout, "ERROR: expected input\n");
        exit(1);
    }

  
    const char* input_file_path = argv[1];
    if (input_file_path[strlen(input_file_path) - 1] != 'm' && input_file_path[strlen(input_file_path) - 2] != 'b' &&
        input_file_path[strlen(input_file_path) - 3] != '.')
    {
        fprintf(stdout, "ERROR: incorrect format of file\n");
        exit(1);
    }

    bm_debasm_file(&bm, input_file_path);
    


    return 0;
}