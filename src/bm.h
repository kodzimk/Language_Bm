#ifndef _BM_H
#define _BM_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>

#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof((xs)[0]))
#define BM_STACK_CAPACITY 1024
#define BM_PROGRAM_CAPACITY 1024
#define BM_LABEL_CAPACITY 8
#define BM_LABEL_SIZE_CAPACITY 32

typedef uint64_t Inst_Addr;

typedef enum {
    ERR_OK = 0,
    ERR_STACK_OVERFLOW,
    ERR_STACK_UNDERFLOW,
    ERR_ILLEGAL_INST,
    ERR_ILLEGAL_INST_ACCESS,
    ERR_ILLEGAL_OPERAND,
    ERR_DIV_BY_ZERO,
    ERR_ILLEGAL_OPERAND_TYPE,
} Err;

typedef enum {
    INST_NOP = 0,
    INST_PUSH,
    INST_DUP,
    INST_SWAP,
    INST_PLUSI,
    INST_MINUSI,
    INST_MULTI,
    INST_DIVI,
    INST_PLUSF,
    INST_MINUSF,
    INST_MULTF,
    INST_DIVF,
    INST_JMP,
    INST_JMP_IF,
    INST_EQ,
    INST_HALT,
    INST_NOT,
    INST_GEF,
    INST_PRINT_DEBUG,
    AMOUNT_OF_INSTS
} Inst_Type;

typedef struct string
{
    size_t size;
    char* buffer;
}string_t;

typedef struct
{
    Inst_Addr addr;
    string_t name;
}Label;

typedef struct
{
    Inst_Addr addr;
    string_t name;
}Unresolved_jmp;

typedef struct {
    Label label[BM_LABEL_CAPACITY];
    size_t label_size;

    Unresolved_jmp unresolved_jmp[BM_LABEL_CAPACITY];
    size_t unresolved_jmp_size;
}table_label;
table_label table = { 0 };

typedef union {
    uint64_t as_u64;
    int64_t as_i64;
    double as_f64;
    void* as_ptr;
}Word;

static_assert(sizeof(Word) == 8, "The Bm word is expected to be 64 bits");

typedef struct {
    Inst_Type type;
    Word operand;
} Inst;

typedef struct {
    Word stack[BM_STACK_CAPACITY];
    Inst_Addr stack_size;

    Inst program[BM_PROGRAM_CAPACITY];
    Inst_Addr program_size;
    Inst_Addr ip;

    int halt;
} Bm;

Bm bm = { 0 };

#define MAKE_INST_PUSH(value) {.type = INST_PUSH, .operand = (value)}
#define MAKE_INST_PLUSI       {.type = INST_PLUSI}
#define MAKE_INST_MINUSI       {.type = INST_MINUSI}
#define MAKE_INST_MULTI        {.type = INST_MULTI}
#define MAKE_INST_DIVI         {.type = INST_DIVI}
#define MAKE_INST_JMP(addr)   {.type = INST_JMP, .operand = (addr)}
#define MAKE_INST_DUP(addr)   {.type = INST_DUP, .operand = (addr)}
#define MAKE_INST_HALT        {.type = INST_HALT, .operand = (addr)}
#define MAKE_INST_NOP         {.type = INST_NOP}
#define MAKE_INST_RET         {.type = INST_RET}

const char* err_as_cstr(Err err);
const char* inst_names(Inst_Type type);
int inst_has_operand(Inst_Type type);

void bm_dump_stack(FILE* stream, const Bm* bm);
void bm_load_program_from_file(Bm* bm, const char* file_path);
void bm_save_program_to_file(Inst* program, size_t program_size, const char* file_path);
void bm_debasm_file(Bm* bm, char const* file_path);

Err bm_execute_inst(Bm* bm);
Err bm_execute_program(Bm* bm, int limits);

Inst_Addr table_label_find(const table_label* table, string_t name);
void table_label_push(table_label* table, string_t name, Inst_Addr addr);
void table_label_unresolved_push(table_label* table, string_t name, Inst_Addr addr);

Word number_literal_as_word(string_t sv);

int sv_to_int(string_t* line);
int cmp_str(string_t str, string_t str2);
string_t str_trim_left(string_t source);
string_t chop_line_blank(string_t* source);
string_t chop_line(string_t* source, char deli);
string_t from_cstr_to_str(const char* str);
string_t slurp_file(const char* file_path);

Inst get_inst_line(string_t* line, table_label* lt, Bm* bm);
void vm_translate_source(string_t source, Bm* bm, table_label* lt);


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
        return "ERR_ILLEGAL_INST_ACCESS";
    case ERR_ILLEGAL_OPERAND_TYPE:
        return "ERR_ILLEGAL_OPERAND_TYPE";
    default:
        assert(0 && "err_as_cstr: Unreachable");
    }
}
const char* inst_names(Inst_Type type)
{
    switch (type) {
    case INST_NOP:         return "nop";
    case INST_PUSH:        return "push";
    case INST_PLUSI:       return "plusi";
    case INST_MINUSI:      return "minusi";
    case INST_MULTI:       return "multi";
    case INST_DIVI:        return "divi";
    case INST_PLUSF:       return "plusf";
    case INST_MINUSF:      return "munisf";
    case INST_MULTF:       return "multf";
    case INST_DIVF:        return "divf";
    case INST_SWAP:        return "swap";
    case INST_JMP:         return "jmp";
    case INST_HALT:        return "halt";
    case INST_JMP_IF:      return "jmp_if";
    case INST_EQ:          return "eq";
    case INST_NOT:          return "not";
    case INST_GEF:          return "gef";
    case INST_PRINT_DEBUG: return "print_debug";
    case INST_DUP:         return "dup";
    case AMOUNT_OF_INSTS:
    default: assert(0 && "inst_type_as_cstr: unreachable");
    }
}

int inst_has_operand(Inst_Type type)
{
    switch (type){
    case INST_NOP:         return 0;
    case INST_PUSH:        return 1;
    case INST_DUP:         return 1;
    case INST_SWAP:        return 1;
    case INST_PLUSI:       return 0;
    case INST_MINUSI:      return 0;
    case INST_MULTI:       return 0;
    case INST_DIVI:        return 0;
    case INST_PLUSF:       return 0;
    case INST_MINUSF:      return 0;
    case INST_MULTF:       return 0;
    case INST_DIVF:        return 0;
    case INST_JMP:         return 1;
    case INST_HALT:        return 0;
    case INST_JMP_IF:      return 1;
    case INST_EQ:          return 0; 
    case INST_GEF:         return 0;
    case INST_NOT:         return 0;
    case INST_PRINT_DEBUG: return 0;
    case AMOUNT_OF_INSTS:
    default: assert(0 && "inst_type_as_cstr: unreachable");
        
    }
}

Err bm_execute_program(Bm* bm, int limit)
{
    while (limit != 0 && !bm->halt) {
        Err err = bm_execute_inst(bm);
        if (err != ERR_OK) {
            return err;
        }
        if (limit > 0) {
            --limit;
        }
    }

    return ERR_OK;
}

Err bm_execute_inst(Bm* bm)
{
    if (bm->ip >= bm->program_size) {
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

    case INST_PLUSI:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2].as_u64 += bm->stack[bm->stack_size - 1].as_u64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_MINUSI:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2].as_u64 -= bm->stack[bm->stack_size - 1].as_u64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_MINUSF:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2].as_f64 -= bm->stack[bm->stack_size - 1].as_f64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;
    case INST_MULTF:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2].as_f64 *= bm->stack[bm->stack_size - 1].as_f64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_DIVF:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }

        bm->stack[bm->stack_size - 2].as_f64 /= bm->stack[bm->stack_size - 1].as_f64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_MULTI:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2].as_u64 *= bm->stack[bm->stack_size - 1].as_u64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_DIVI:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }

        if (bm->stack[bm->stack_size - 1].as_u64 == 0) {
            return ERR_DIV_BY_ZERO;
        }

        bm->stack[bm->stack_size - 2].as_u64 /= bm->stack[bm->stack_size - 1].as_u64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_PLUSF:
        if (bm->stack_size > BM_STACK_CAPACITY)
        {
            return ERR_STACK_OVERFLOW;
        }

        bm->stack[bm->stack_size - 2].as_f64 += bm->stack[bm->stack_size - 1].as_f64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_JMP:
        bm->ip = inst.operand.as_u64;
        break;

    case INST_HALT:
        bm->halt = 1;
        break;

    case INST_EQ:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }

        bm->stack[bm->stack_size - 2].as_u64 = bm->stack[bm->stack_size - 1].as_u64 == bm->stack[bm->stack_size - 2].as_u64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_JMP_IF:
        if (bm->stack_size < 1) {
            return ERR_STACK_UNDERFLOW;
        }

        if (bm->stack[bm->stack_size - 1].as_u64) {
            bm->ip = inst.operand.as_u64;
        }
        else {
            bm->ip += 1;
        }
            bm->stack_size -= 1;
        break;

    case INST_PRINT_DEBUG:
        if (bm->stack_size < 1) {
            return ERR_STACK_UNDERFLOW;
        }
        fprintf(stdout, "  u64: %lu, i64: %ld, f64: %lf, ptr: %p\n",
            bm->stack[bm->stack_size - 1].as_u64,
            bm->stack[bm->stack_size - 1].as_i64,
            bm->stack[bm->stack_size - 1].as_f64,
            bm->stack[bm->stack_size - 1].as_ptr);
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case INST_DUP:
        if (bm->stack_size >= BM_STACK_CAPACITY) {
            return ERR_STACK_OVERFLOW;
        }

        if (bm->stack_size - inst.operand.as_u64 <= 0) {
            return ERR_STACK_UNDERFLOW;
        }

        bm->stack[bm->stack_size] = bm->stack[bm->stack_size - 1 - inst.operand.as_u64];
        bm->stack_size += 1;
        bm->ip += 1;
        break;

    case INST_SWAP:
        if (inst.operand.as_u64 >= bm->stack_size) {
            return ERR_STACK_UNDERFLOW;
        }

        const uint64_t a = bm->stack_size - 1;
        const uint64_t b = bm->stack_size - 1 - inst.operand.as_u64;

        Word t = bm->stack[a];
        bm->stack[a] = bm->stack[b];
        bm->stack[b] = t;
        bm->ip += 1;

        break;
    case INST_NOT:
        if (bm->stack_size < 1) {
            return ERR_STACK_UNDERFLOW;
        }

        bm->stack[bm->stack_size - 1].as_u64 = !bm->stack[bm->stack_size - 1].as_u64;
        bm->ip += 1;
        break;

    case INST_GEF:
        if (bm->stack_size < 2) {
            return ERR_STACK_UNDERFLOW;
        }
        bm->stack[bm->stack_size - 2].as_u64 = bm->stack[bm->stack_size - 1].as_f64 >=
            bm->stack[bm->stack_size - 2].as_f64;
        bm->stack_size -= 1;
        bm->ip += 1;
        break;

    case AMOUNT_OF_INSTS:
    default:
        return ERR_ILLEGAL_INST;
    }

    return ERR_OK;
}

void bm_dump_stack(FILE* stream, const Bm* bm)
{
    fprintf(stream, "Stack:\n");
    if (bm->stack_size > 0) {
        for (Inst_Addr i = 0; i < bm->stack_size; ++i) {
            fprintf(stream, "u64:%lu, i64:%ld, f64:%lf, ptr:%p\n", 
                      bm->stack[i].as_u64,
                      bm->stack[i].as_i64,
                      bm->stack[i].as_f64,
                      bm->stack[i].as_ptr);
        }
    }
    else {
        fprintf(stream, "  [empty]\n");
    }
}

void bm_debasm_file(Bm* bm, char const* file_path) 
{
    bm_load_program_from_file(bm, file_path);

    for (Inst_Addr i = 0; i < bm->program_size; ++i)
    {
        printf("%s", inst_names(bm->program[i].type));
        if (inst_has_operand(bm->program[i].operand.as_i64)){
            printf(" %ld", bm->program[i].operand.as_i64);
        }
        printf("\n");
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

string_t str_trim_right(string_t source)
{
    size_t i = 0;
    size_t end = source.size;
    while (i < source.size && isspace(source.buffer[end - 1 - i]))
    {
        i += 1;
    }

    return (string_t) { .size = source.size - i, .buffer = source.buffer };
}

int sv_to_int(string_t* line)
{
    if (!isdigit(*line->buffer))
        return -1;

    int sum = 0;
    for (size_t i = 0; i < line->size && isdigit(line->buffer[i]); ++i)
    {
        sum = sum * 10 + line->buffer[i] - '0';
    }

    return sum;
}

Inst_Addr table_label_find(const table_label* lt, string_t name) {
    for (size_t i = 0; i < lt->label_size; ++i) {
        if (cmp_str(lt->label[i].name, name)) {
            return lt->label[i].addr;
        }
    }

    fprintf(stderr, "ERROR: label `%.*s` does not exist\n",
        (int)name.size, name.buffer);
    exit(1);

    return -1;
}

void table_label_unresolved_push(table_label* table, string_t name, Inst_Addr addr) {
    table->unresolved_jmp[table->unresolved_jmp_size++] = (Unresolved_jmp){ .name = name,.addr = addr };
}

void table_label_push(table_label* table, string_t name, Inst_Addr addr) {
    table->label[table->label_size++] = (Label){ .name = name , .addr = addr };
}

string_t chop_line(string_t* source, char deli)
{
    size_t i = 0;

    while (source->size > i && source->buffer[i] != deli && source->buffer[i] != '#')
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

string_t from_cstr_to_str(const char* str)
{
    return (string_t) { .size = strlen(str), .buffer = str };
}

int cmp_str(string_t str, string_t str2)
{
    if (str.size != str2.size) {
        return 0;
    }
    else {
        return memcmp(str.buffer, str2.buffer, str.size) == 0;
    }
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

Word number_literal_as_word(string_t sv)
{
    assert(sv.size < 1024);
    char cstr[sv.size + 1];
    char* endptr = 0;

    memcpy(cstr, sv.buffer, sv.size);
    cstr[sv.size] = '\0';

    Word result = { 0 };


    result.as_u64 = strtoull(cstr, &endptr, 10);
    printf("LLU: %llu\n", result.as_u64);
    if ((size_t)(endptr - cstr) != sv.size) {
        result.as_f64= strtod(cstr, &endptr);
        printf("LD: %f\n", result.as_f64);
    /*    if ((size_t)(endptr - cstr) != sv.size) {
            fprintf(stderr, "ERROR: %s is not a number literla\n", cstr);
            exit(1);
        }*/
    }

    return result;
}

Inst get_inst_line(string_t* line,table_label  *lt,Bm *bm)
{
    string_t inst_name = chop_line_blank(line);
    *line = str_trim_left(*line);
    if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_PUSH))))
    {
        return (Inst) { .type = INST_PUSH, .operand = number_literal_as_word(*line) };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_JMP))))
    {    
        string_t operand = str_trim_right(chop_line(line, '\n'));
        table_label_unresolved_push(lt, operand, bm->program_size);

         return (Inst) { .type = INST_JMP, .operand.as_i64 = 0 };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_JMP_IF))))
    {
        string_t operand = str_trim_right(chop_line(line, '\n'));
        table_label_unresolved_push(lt, operand, bm->program_size);

        return (Inst) { .type = INST_JMP_IF, .operand.as_i64 = 0 };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_PLUSI))))
    {
        return (Inst) { .type = INST_PLUSI };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_PLUSF))))
    {
        return (Inst) { .type = INST_PLUSF };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_MINUSF))))
    {
        return (Inst) { .type = INST_MINUSF };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_MULTF))))
    {
        return (Inst) { .type = INST_MULTF  };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_DIVF))))
    {
        return (Inst) { .type = INST_DIVF
        };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_DUP))))
    {
        return (Inst) { .type = INST_DUP, .operand.as_i64 = sv_to_int(line) };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_SWAP))))
    {
        return (Inst) { .type = INST_SWAP, .operand.as_i64 = sv_to_int(line) };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_EQ))))
    {
        return (Inst) { .type = INST_EQ };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_NOT))))
    {
        return (Inst) { .type = INST_NOT };
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_GEF))))
    {
        return (Inst) { .type = INST_GEF};
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_HALT))))
    {
        return (Inst) { .type = INST_HALT};
    }
    else if (cmp_str(inst_name, from_cstr_to_str(inst_names(INST_PRINT_DEBUG))))
    {
        return (Inst) { .type = INST_PRINT_DEBUG};
    }


    return(Inst) { .type = INST_NOP };
}

void vm_translate_source(string_t source,Bm *bm,table_label* lt)
{
    while (source.size > 0)
    {
        assert(bm->program_size < BM_PROGRAM_CAPACITY);
        string_t line = str_trim_left(chop_line(&source, '\n'));
        
         if (line.size > 0 && *line.buffer != '#' && !isspace(*line.buffer)) {

             if (line.size > 0 && line.buffer[line.size - 2] == ':')
             {
                 string_t name = {
                     .size = line.size - 2,
                     .buffer = line.buffer,
                 };
            
                 table_label_push(lt, name, bm->program_size);
             }
             else
             {
                 bm->program[bm->program_size++] = get_inst_line(&line, lt, bm);
             }
         }
    }

    for (size_t i = 0; i < lt->unresolved_jmp_size; i++)
    {
        Inst_Addr addr = table_label_find(lt, lt->unresolved_jmp[i].name);
        bm->program[lt->unresolved_jmp[i].addr - 1].operand.as_u64 = addr;
    }
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