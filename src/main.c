#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

#define VM_STACK_CAP 128

typedef enum {
	TRAP_OKAY = 0,
	TRAP_STACK_OVERFLOW,
	TRAP_STACK_UNDERFLOW,
	TRAP_INVALID_INST,
}TRAP;

typedef enum {
	PUSH = 0,
	POP,
	PLUS,
	MINUS,
}INST_TYPE;

typedef int Value;

typedef struct {
	INST_TYPE type;
	Value operand;
}INST;

typedef struct {
	Value stack[VM_STACK_CAP];
	size_t stack_size;
}VM;

#define MAKE_INST_PUSH(value) ((INST){.type = PUSH ,.operand = (value)})
#define MAKE_INST_POP ((INST){.type = POP})
#define MAKE_INST_PLUS ((INST){.type = PLUS})
#define MAKE_INST_MINUS ((INST){.type = MINUS})


TRAP vm_execute(VM* vm, INST inst)
{
	switch (inst.type)
	{
	case PUSH:
		if (vm->stack_size >= VM_STACK_CAP)
			return TRAP_STACK_OVERFLOW;

		vm->stack[vm->stack_size] = inst.operand;
		vm->stack_size += 1;
		break;
	case POP:
		if (vm->stack_size == 0)
			return TRAP_STACK_UNDERFLOW;

         vm->stack_size -= 1;
		break;
	case PLUS:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] += vm->stack[vm->stack_size - 1];
		vm->stack_size--;
		break;
	case MINUS:
		if (vm->stack_size < 2)
			return TRAP_STACK_UNDERFLOW;

		vm->stack[vm->stack_size - 2] -= vm->stack[vm->stack_size - 1];
		vm->stack_size--;
		break;
	default:
		return TRAP_INVALID_INST;
		break;
	}
	return TRAP_OKAY;
}

void vm_print(VM* vm)
{
	printf("Stack: \n");
	for (size_t i = 0; i < vm->stack_size; i++)
	{
		fprintf(stdout, "%d\n", vm->stack[i]);
	}
}

VM vm = { 0 };

int main()
{
	char arr[5] = "hello";
	arr[0] = 's';
	//[H e l l o]
	char* p1 = (char*)malloc(sizeof(char) * 6);
	//[H][e][l][l][o]\0

	strcpy(p1, "hello");
    (p1+1);

	p1[0] = 'A';
	puts(p1);
	free(p1);

	return 0;
}
