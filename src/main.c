#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<assert.h>

#define VM_STACK_CAP 128
#define VM_PROGRAM_CAP 128

typedef enum {
	TRAP_OKAY = 0,
	TRAP_STACK_OVERFLOW,
	TRAP_STACK_UNDERFLOW,
	TRAP_INVALID_INST,
	TRAP_INVALID_ADDR,
	TRAP_DIVISION_BY_ZERO,
}TRAP;

typedef enum {
	PUSH = 0,
	POP,
	PLUS,
	MINUS,
	DIV,
	MUL,
	HALT,
	JMP,
	EQ,
	GT,
	LS,
	JMP_IF,
}INST_TYPE;

typedef int Value;

typedef struct {
	INST_TYPE type;
	Value operand;
}INST;

typedef struct {
	Value stack[VM_STACK_CAP];
	size_t stack_size;
	int sp;
	int halt;
	int program_size;
	INST program[VM_PROGRAM_CAP];
}VM;

#define MAKE_INST_PUSH(value) ((INST){.type = PUSH ,.operand = (value)})
#define MAKE_INST_POP ((INST){.type = POP})
#define MAKE_INST_PLUS ((INST){.type = PLUS})
#define MAKE_INST_MINUS ((INST){.type = MINUS})
#define MAKE_INST_MUL ((INST){.type = MUL})
#define MAKE_INST_DIV ((INST){.type = DIV})
#define MAKE_INST_HALT ((INST){.type = HALT})
#define MAKE_INST_EQ ((INST){.type = EQ})
#define MAKE_INST_GT ((INST){.type = GT})
#define MAKE_INST_LS ((INST){.type = LS})
#define MAKE_INST_JMP(value) ((INST){.type = JMP ,.operand = (value)})
#define MAKE_INST_JMP_IF(value) ((INST){.type = JMP_IF ,.operand = (value)})
#define ARRAY_SIZE(arr) (sizeof(arr)) / (sizeof(arr[0]))

//why i am creating marcos not functions!!!

const char* convert_trap_to_str(TRAP trap)
{
	switch (trap)
	{
	case TRAP_STACK_OVERFLOW:
		return "TRAP_STACK_OVERFLOW";
	case TRAP_STACK_UNDERFLOW:
		return "TRAP_STACK_UNDERFLOW";
	case TRAP_INVALID_INST:
		return "TRAP_INVALID_INST";
	case TRAP_INVALID_ADDR:
		return "TRAP_INVALID_ADDR";
	case TRAP_DIVISION_BY_ZERO:
		return "TRAP_DIVISION_BY_ZERO";
	default:
		return "TRAP_OKAY";
	}
}


TRAP vm_execute(VM* vm)
{
	if (vm->sp >= vm->program_size)
		return TRAP_STACK_OVERFLOW;

	INST inst = vm->program[vm->sp];

	switch (inst.type)
	{
	case PUSH:
		if (vm->stack_size >= VM_STACK_CAP)
			return TRAP_STACK_OVERFLOW;

		vm->stack[vm->stack_size] = inst.operand;
		vm->stack_size += 1;
		vm->sp++;
		break;
	case POP:
		if (vm->stack_size == 0)
			return TRAP_STACK_UNDERFLOW;
         vm->stack_size -= 1;
		 vm->sp++;
		break;
	case PLUS:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] += vm->stack[vm->stack_size - 1];
		vm->stack_size -= 1;
		vm->sp++;
		break;
	case MINUS:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] -= vm->stack[vm->stack_size - 1];
		vm->stack_size -= 1;
		vm->sp++;
		break;
	case MUL:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] *= vm->stack[vm->stack_size - 1];
		vm->stack_size -= 1;
		vm->sp++;
		break;
	case DIV:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		if (vm->stack[vm->stack_size - 1] == 0)
			return TRAP_DIVISION_BY_ZERO;

		vm->stack[vm->stack_size - 2] /= vm->stack[vm->stack_size - 1];
		vm->stack_size -= 1;
		vm->sp++;
		break;
	case JMP:
		if (inst.operand >= VM_STACK_CAP || inst.operand < 0)
			return TRAP_INVALID_ADDR;

		vm->sp = inst.operand;
		break;
	case HALT:
		vm->halt = 1;
		break;
	case EQ:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] = vm->stack[vm->stack_size - 1] == vm->stack[vm->stack_size - 2];
		vm->stack_size--;
		vm->sp++;
		break;
	case GT:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] = vm->stack[vm->stack_size - 1] > vm->stack[vm->stack_size - 2];
		vm->stack_size--;
		vm->sp++;
		break;
	case LS:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] = vm->stack[vm->stack_size - 1] < vm->stack[vm->stack_size - 2];
		vm->stack_size--;
		vm->sp++;
		break;
	case JMP_IF:
		if (vm->stack_size < 1)
			return TRAP_STACK_UNDERFLOW;

		if (vm->stack[vm->stack_size - 1])
		{
			vm->stack_size -= 1;
			vm->sp = inst.operand;
		}
		else
			vm->sp += 1;
		break;
	default:
		return TRAP_INVALID_INST;
		break;
	}
	return TRAP_OKAY;
}


VM vm = { 0 };

void vm_copy_from_memory(VM* vm, INST *program,size_t program_size)
{
	assert("", program_size <  VM_PROGRAM_CAP);
	memcpy(vm->program, program, sizeof(program[0]) * program_size);
	vm->program_size = program_size;
}

void vm_debug(VM* vm)
{
	fprintf(stdout, "%s\n","Stack:");
	for (int i = 0; i < vm->stack_size; ++i)
	{
		fprintf(stdout, "%d\n", vm->stack[i]);
	}
}


int main()
{
	INST program[] = {
	MAKE_INST_PUSH(2),
	MAKE_INST_PUSH(1),
	MAKE_INST_LS,
	MAKE_INST_JMP_IF(0),
	MAKE_INST_HALT,
	};

	vm_copy_from_memory(&vm, program, ARRAY_SIZE(program));
	for (int i = 0;!vm.halt && i < 69;++i)
	{
		TRAP trap = vm_execute(&vm);
		if (trap != TRAP_OKAY)
		{
			fprintf(stderr, "%s\n", convert_trap_to_str(trap));
			exit(1);
		}
		vm_debug(&vm);
	}

	return 0;
}
