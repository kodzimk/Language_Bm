#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

typedef struct string
{
	size_t size;
	char* buffer;
}string_t;

char* source_code =
"push 11\n"
"push 2\n"
"dup 1\n";

typedef struct inst {
	char* name;
	int operand;
}Inst;



string_t str_trim_left(string_t source)
{
	size_t i = 0;
	while (i < source.size && isspace(source.buffer[i]))
	{
		i += 1;
	}

	return (string_t) { .size = source.size - i, .buffer = source.buffer };
}

size_t str_trim_right(char* source)
{
	size_t i = 0;
	size_t end = strlen(source);
	while (i < strlen(source) && isspace(source[end - i - 1]))
	{
		i += 1;
	}
	return i;
}

int sv_to_int(string_t* line)
{
	int sum = 0;
	for(size_t i = 0;i < line->size && isdigit(line->buffer[i]);++i)
	{
		sum = sum * 10  + line->buffer[i] - '0';
	}

	return sum;
}

string_t line_from_file(string_t *source, char deli)
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
	/*printf("STRSIZE:%d\n", (int)str.size);
	printf("STR:%s\n",  str.buffer);
	printf("STR2:%s\n", str2.buffer);*/

	for (int i = 0; i < str.size; i++)
	{
		if (str.buffer[i] != str2.buffer[i])
		{
			return 0;
		}
	}
	return 1;
}

string_t line_before_blank(string_t* source)
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


Inst inst_from_line(string_t* line)
{
	string_t inst_name = line_before_blank(line);

	if (cmp_str(inst_name,from_cstr_to_str("push")))
	{
		int operand = sv_to_int(line);
		return (Inst) { .name = "push", .operand = operand };
	}
	else
	{
		int operand = sv_to_int(line);
		return (Inst) { .name = "dup", .operand = operand };
	}

	return (Inst) { .name = "NONE", .operand = 0 };
}


void from_file_to_vm(string_t source)
{
	while (source.size > 0)
	{
		string_t line = str_trim_left(line_from_file(&source,'\n'));
		Inst inst = inst_from_line(&line);
		for (int i = 0; i < strlen(inst.name); ++i)
		{
			printf("%c", inst.name[i]);
		}	
		printf("OPERAND:%d", inst.operand);
		printf("\n");
	}
}


int main()
{
	string_t source = { .size = strlen(source_code),.buffer = source_code };
	from_file_to_vm(source);
	return 0;
}


