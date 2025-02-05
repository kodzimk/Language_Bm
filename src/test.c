#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

char* source_code =
"   push 1  a\n"
"push 2\n"
"dup 1\n";

char* get_line_before_chr(char* source,char chr)
{
	size_t i = 0;
	while (source[i] != chr)
	{
		i += 1;
	}
	char* line = (char*)malloc(sizeof(char) * i + 1);

	size_t j = 0;
	while (i > 0 && line != NULL)
	{
		line[j] = source[j];
		j += 1;
		i -= 1;
	}


	return line;
}

size_t remove_space_trim_left(char *source)
{
	size_t i = 0;
	while (i < strlen(source) && isspace(source[i]))
	{
		i += 1;
	}
	return i;
}

size_t remove_space_trim_right(char* source)
{
	size_t i = 0;
	size_t end = strlen(source);
	while (i < strlen(source) && isspace(source[end - i - 1]))
	{
		i += 1;
	}
	return i;
}


int main()
{
	size_t program_size = 0;
	while (program_size < 3)
	{
		char* line = get_line_before_chr(source_code, '\n');
		source_code += strlen(line) + 1;
		line += remove_space_trim_left(line);


		for (size_t i = 0; i < strlen(line); i++)
		{
			printf("%c", line[i]);
		}
		printf("\n");
		free(line);
		program_size++;
	}
	return 0;
}