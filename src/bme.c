#define BM_IMPLEMENTATION
#include"bm.h"

char* shift(int* argc, char*** argv)
{
    assert(*argc > 0);

    char* result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

int main(int argc, char** argv)
{
    shift(&argc, &argv);

    const char* input_file = NULL;
    int limit = -1;

    while (argc > 0)
    {
        const char* flag = shift(&argc, &argv);
        if (strcmp(flag, "-i") == 0)
        {
            if (argc == 0)
            {
                fprintf(stdout, "ERROR: n oargument for input file!!! \n");
                exit(1);
            }
            
            input_file = shift(&argc, &argv);
        }
        else if (strcmp(flag, "-l") == 0)
        {
            if (argc == 0)
            {
                fprintf(stdout, "ERROR: expected value of limit of the program!!! \n");
                exit(1);
            }

            limit = atoi(shift(&argc, &argv));
        }
        else
        {
            if (argc == 0)
            {
                fprintf(stdout, "ERROR: Unkown flag!!!\n");
                exit(1);
            }
        }
    }


    bm_load_program_from_file(&bm, input_file);

    Err err = bm_execure_program(&bm, limit);

    if (err != ERR_OK) {
        return 1;
    }

    return 0;
}