#ifndef _BM_H
#define _BM_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>


#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof((xs)[0]))
#define BM_STACK_CAPACITY 1024
#define BM_PROGRAM_CAPACITY 1024

typedef enum {
    ERR_OK = 0,
    ERR_STACK_OVERFLOW,
    ERR_STACK_UNDERFLOW,
    ERR_ILLEGAL_INST,
    ERR_ILLEGAL_INST_ACCESS,
    ERR_ILLEGAL_OPERAND,
    ERR_DIV_BY_ZERO,
} Err;


typedef int64_t Word;

typedef enum {
    INST_NOP = 0,
    INST_PUSH,
    INST_DUP,
    INST_PLUS,
    INST_MINUS,
    INST_MULT,
    INST_DIV,
    INST_JMP,
    INST_JMP_IF,
    INST_EQ,
    INST_HALT,
    INST_PRINT_DEBUG,
} Inst_Type;


typedef struct {
    Inst_Type type;
    Word operand;
} Inst;

typedef struct {
    Word stack[BM_STACK_CAPACITY];
    Word stack_size;

    Inst program[BM_PROGRAM_CAPACITY];
    Word program_size;
    Word ip;

    int halt;
} Bm;

typedef struct string
{
    size_t size;
    char* buffer;
}string_t;

Bm bm = { 0 };

#define MAKE_INST_PUSH(value) {.type = INST_PUSH, .operand = (value)}
#define MAKE_INST_PLUS        {.type = INST_PLUS}
#define MAKE_INST_MINUS       {.type = INST_MINUS}
#define MAKE_INST_MULT        {.type = INST_MULT}
#define MAKE_INST_DIV         {.type = INST_DIV}
#define MAKE_INST_JMP(addr)   {.type = INST_JMP, .operand = (addr)}
#define MAKE_INST_DUP(addr)   {.type = INST_DUP, .operand = (addr)}
#define MAKE_INST_HALT        {.type = INST_HALT, .operand = (addr)}
#define MAKE_INST_NOP       {.type = INST_NOP}

const char* err_as_cstr(Err err);
const char* inst_type_as_cstr(Inst_Type type);
Err bm_execute_inst(Bm* bm);
void bm_dump_stack(FILE* stream, const Bm* bm);
void bm_load_program_from_file(Bm* bm, const char* file_path);
void bm_save_program_to_file(Inst* program, size_t program_size, const char* file_path);
void bm_debasm_file(Bm* bm, char const* file_path);
Err bm_execute_inst(Bm* bm);
Err bm_execure_program(Bm* bm, size_t limits);


int sv_to_int(string_t* line);
int cmp_str(string_t str, string_t str2);
Inst get_inst_line(string_t* line);
string_t str_trim_left(string_t source);
string_t chop_line_blank(string_t* source);
string_t line_from_file(string_t* source, char deli);
string_t from_cstr_to_str(char* str);
string_t slurp_file(const char* file_path);
size_t vm_translate_source(string_t source, Inst* program, size_t capacity);

#endif // _BM_H 

#define BM_IMPLEMENTATION
#ifdef BM_IMPLEMENTATION
const char* err_as_cstr(Err err)
{
    switch (err) {
    case ERR_OK:
        return "ERR_OK";
    case ERR_STACK_OVERFLOW:
        return "ERR_STACK_OVERFLOW";
    case ERR_STACK_UNDERFLOW:
        return "ERR_STACK_UNDERFLOW";
    case ERR_ILLEGAL_INST:
        return "ERR_ILLEGAL_INST";
    case ERR_ILLEGAL_OPERAND:
        return "ERR_ILLEGAL_OPERAND";
    case ERR_ILLEGAL_INST_ACCESS:
        return "ERR_ILLEGAL_INST_ACCESS";
    case ERR_DIV_BY_ZERO:
        return "ERR_DIV_BY_ZERO";
    default:
        assert(0 && "err_as_cstr: Unreachable");
    }
}

const char* inst_type_as_cstr(Inst_Type type)
{
    switch (type) {
    case INST_NOP:         return "INST_NOP";
    case INST_PUSH:        return "INST_PUSH";
    case INST_PLUS:        return "INST_PLUS";
    case INST_MINUS:       return "INST_MINUS";
    case INST_MULT:        return "INST_MULT";
    case INST_DIV:         return "INST_DIV";
    case INST_JMP:         return "INST_JMP";
    case INST_HALT:        return "INST_HALT";
    case INST_JMP_IF:      return "INST_JMP_IF";
    case INST_EQ:          return "INST_EQ";
    case INST_PRINT_DEBUG: return "INST_PRINT_DEBUG";
    case INST_DUP:         return "INST_DUP";
    default: assert(0 && "inst_type_as_cstr: unreachable");
    }
}
Err bm_execute_inst(Bm* bm)
{
    if (bm->ip < 0 || bm->ip >= bm->program_size) {
        return ERR_ILLEGAL_INST_ACCESS;
    }

    Inst inst = bm->program[bm->ip];

    switch (inst.type) {
    case INST_NOP:
        bm->ip += 1;
        break;

    case INST_PUSH:
        if (bm->stack_size >= BM_STACK_CAPACITY) {
            return ERR_STACK_OVERFLOW;
        }
        bm->stack[bm->stack_size++] = inst.operand;
        bm->ip += 1;
        break;

    case INST_PLUS:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2] += bm->stack[bm->stack_size - 1];
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_MINUS:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2] -= bm->stack[bm->stack_size - 1];
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_MULT:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2] *= bm->stack[bm->stack_size - 1];
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_DIV:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }

        if (bm->stack[bm->stack_size - 1] == 0) {
            return ERR_DIV_BY_ZERO;
        }

        bm->stack[bm->stack_size - 2] /= bm->stack[bm->stack_size - 1];
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_JMP:
        bm->ip = inst.operand;
        break;

    case INST_HALT:
        bm->halt = 1;
        break;

    case INST_EQ:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }

        bm->stack[bm->stack_size - 2] = bm->stack[bm->stack_size - 1] == bm->stack[bm->stack_size - 2];
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_JMP_IF:
        if (bm->stack_size < 1) {
            return ERR_STACK_UNDERFLOW;
        }

        if (bm->stack[bm->stack_size - 1]) {
            bm->stack_size -= 1;
            bm->ip = inst.operand;
        }
        else {
            bm->ip += 1;
        }
        break;

    case INST_PRINT_DEBUG:
        if (bm->stack_size < 1) {
            return ERR_STACK_UNDERFLOW;
        }
        printf("%ld\n", bm->stack[bm->stack_size - 1]);
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_DUP:
        if (bm->stack_size >= BM_STACK_CAPACITY) {
            return ERR_STACK_OVERFLOW;
        }

        if (bm->stack_size - inst.operand <= 0) {
            return ERR_STACK_UNDERFLOW;
        }

        if (inst.operand < 0) {
            return ERR_ILLEGAL_OPERAND;
        }

        bm->stack[bm->stack_size] = bm->stack[bm->stack_size - 1 - inst.operand];
        bm->stack_size += 1;
        bm->ip += 1;
        break;

    default:
        return ERR_ILLEGAL_INST;
    }

    return ERR_OK;
}

Err bm_execure_program(Bm* bm, size_t limits) {
    for (; limits != 0 && !bm->halt && bm->ip < bm->program_size;)
    {
        Err error = bm_execute_inst(bm);
        if (error != ERR_OK)
        {
            fprintf(stderr, "%s\n", err_as_cstr(error));
            return error;
        }
        bm_dump_stack(stdout, bm);

        if (limits > 0) {
            limits -= 1;
        }
    }

    return ERR_OK;
}

void bm_dump_stack(FILE* stream, const Bm* bm)
{
    fprintf(stream, "Stack:\n");
    if (bm->stack_size > 0) {
        for (Word i = 0; i < bm->stack_size; ++i) {
            fprintf(stream, "  %ld\n", bm->stack[i]);
        }
    }
    else {
        fprintf(stream, "  [empty]\n");
    }
}

void bm_debasm_file(Bm* bm, char const* file_path)
{
    bm_load_program_from_file(bm, file_path);

    for (size_t i = 0; i < (size_t)bm->program_size; ++i)
    {
        switch (bm->program[i].type)
        {
        case INST_PUSH:
            printf("push:%d\n", (int)bm->program[i].operand);
            break;
        case INST_DUP:
            printf("dup:%d\n", (int)bm->program[i].operand);
            break;
        case INST_PLUS:
            printf("plus\n");
            break;
        case INST_MINUS:
            printf("minus\n");
            break;
        case INST_MULT:
            printf("mult\n");
            break;
        case INST_DIV:
            printf("div\n");
            break;
        case INST_JMP:
            printf("jmp:%d\n", (int)bm->program[i].operand);
            break;
        case INST_JMP_IF:
            printf("jmp_if\n");
            break;
        case INST_EQ:
            printf("eq\n");
            break;
        case INST_HALT:
            printf("halt\n");
            break;
        case INST_NOP:
            printf("nop\n");
            break;
        case INST_PRINT_DEBUG:
            printf("print\n");
            break;
        }
    }
}

void bm_load_program_from_file(Bm* bm, const char* file_path)
{
    FILE* f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Could not open file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    long m = ftell(f);
    if (m < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    assert(m % sizeof(bm->program[0]) == 0);
    assert((size_t)m <= BM_PROGRAM_CAPACITY * sizeof(bm->program[0]));

    if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    bm->program_size = fread(bm->program, sizeof(bm->program[0]), m / sizeof(bm->program[0]), f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    fclose(f);
}

void bm_save_program_to_file(Inst* program, size_t program_size,
    const char* file_path)
{
    FILE* f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Could not open file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    fwrite(program, sizeof(program[0]), program_size, f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Could not write to file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    fclose(f);
}

string_t str_trim_left(string_t source)
{
    size_t i = 0;
    while (i < source.size && isspace(source.buffer[i]))
    {
        i += 1;
    }

    return (string_t) { .size = source.size - i, .buffer = source.buffer + i };
}

int sv_to_int(string_t* line)
{
    int sum = 0;
    for (size_t i = 0; i < line->size && isdigit(line->buffer[i]); ++i)
    {
        sum = sum * 10 + line->buffer[i] - '0';
    }

    return sum;
}

string_t line_from_file(string_t* source, char deli)
{
    size_t i = 0;
    while (source->size > i && source->buffer[i] != deli)
    {
        i++;
    }

    string_t line = { .size = i,.buffer = source->buffer };

    if (source->size == i)
    {
        source->size -= i;
        source->buffer += i;
    }
    else
    {
        source->size -= i + 1;
        source->buffer += i + 1;
    }

    return line;
}

string_t from_cstr_to_str(char* str)
{
    return (string_t) { .size = strlen(str), .buffer = str };
}

int cmp_str(string_t str, string_t str2)
{
    for (size_t i = 0; i < str.size; i++)
    {
        if (str.buffer[i] != str2.buffer[i])
        {
            return 0;
        }
    }
    return 1;
}

string_t chop_line_blank(string_t* source)
{
    size_t i = 0;
    while (source->size > i && !isspace(source->buffer[i]))
    {
        i++;
    }

    string_t line = { .size = i,.buffer = source->buffer };

    if (source->size == i)
    {
        source->size -= i;
        source->buffer += i;
    }
    else
    {
        source->size -= i + 1;
        source->buffer += i + 1;
    }

    return line;
}



Inst get_inst_line(string_t* line)
{
    string_t inst_name = chop_line_blank(line);
    *line = str_trim_left(*line);
    if (cmp_str(inst_name, from_cstr_to_str("push")))
    {
        int operand = sv_to_int(line);
        return (Inst) { .type = INST_PUSH, .operand = (operand) };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("min")))
    {
        int operand = sv_to_int(line);
        return (Inst) { .type = INST_MINUS, .operand = (operand) };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("div")))
    {
        int operand = sv_to_int(line);
        return (Inst) { .type = INST_DIV, .operand = (operand) };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("mul")))
    {
        int operand = sv_to_int(line);
        return (Inst) { .type = INST_MULT, .operand = (operand) };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("eq")))
    {
        return (Inst) { .type = INST_EQ };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("jmp")))
    {
        int operand = sv_to_int(line);
        return (Inst) { .type = INST_JMP, .operand = operand };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("jmp_if")))
    {
        return (Inst) { .type = INST_JMP_IF };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("hart")))
    {
        return (Inst) { .type = INST_HALT };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("plus")))
    {
        return (Inst) { .type = INST_PLUS };
    }
    else if (cmp_str(inst_name, from_cstr_to_str("dup")))
    {
        int operand = sv_to_int(line);
        return (Inst) { .type = INST_DUP, .operand = (operand) };
    }

    return(Inst) { .type = INST_NOP };
}

int check_empty_line(string_t line)
{
    for (size_t i = 0; i < line.size; ++i)
    {
        if (!isspace(line.buffer[i]))
            return 1;
    }

    return 0;
}


size_t vm_translate_source(string_t source, Inst* program, size_t capacity)
{
    size_t program_size = 0;
    while (source.size > 0)
    {
        assert(program_size < capacity);
        string_t line = str_trim_left(line_from_file(&source, '\n'));
        for (size_t i = 0; i < strlen(line.buffer); i++)
        {
            printf("%c", line.buffer[i]);
        }
        printf("\n\n");
        if (check_empty_line(line) && *line.buffer != '#') {
            program[program_size] = get_inst_line(&line);
            program_size++;
        }
    }

    return program_size;
}

string_t slurp_file(const char* file_path)
{
    FILE* f = fopen(file_path, "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    long m = ftell(f);
    if (m < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    char* buffer = (char*)malloc(m);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: Could not allocate memory for file: %s\n",
            strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    size_t n = fread(buffer, 1, m, f);
    if (ferror(f)) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
        exit(1);
    }

    fclose(f);

    return (string_t) {
        .size = n,
            .buffer = buffer,
    };
}


#endif