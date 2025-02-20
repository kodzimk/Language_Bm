#include <float.h>
#include <math.h>
#include <stdio.h>
#include<stdlib.h>
#include<stdint.h>

void print_bits(uint8_t* bytes, size_t byte_size)
{
	for (int i = (int)byte_size - 1; i >= 0; --i)
	{
		uint8_t byte = bytes[i];
		for (int j = 7; j >= 0; --j) {
			printf("%d", !!(byte & (1 << j)));
		}
		printf(" ");
	}
	printf("\n");
}


#define EXP_MASK (((1LL << 11LL) - 1LL) << 52LL)
#define FRACTION_MASK ((1LL << 52LL) - 1LL)
#define SIGN_MASK (1LL << 63LL)
#define TYPE_MASK (((1LL << 4LL) - 1LL) << 48LL)
#define VALUE_MASK ((1LL << 48LL) - 1LL)
#define TYPE(n) ((1LL << 3LL) + n)

double mk_inf(void)
{
	uint64_t y = EXP_MASK;
	return *(double*)&y;
}

inline uint64_t set_type(double x, uint64_t type)
{
	uint64_t y = *(uint64_t*)&x;
	y &= (((TYPE_MASK >> 48LL) & type) << 48LL);

	return *(double*)&y;
} 

inline uint64_t get_type(double x)
{
	uint64_t y = (*(uint64_t*)&x);
	return (y & TYPE_MASK) >> 48LL;
}

double set_value(double x, uint64_t value)
{
	uint64_t y = *(uint64_t*)&x;
	y &= (value & VALUE_MASK);
	return *(double*)&y;
}

inline uint64_t get_value(double x)
{
	uint64_t y = (*(uint64_t*)&x);
	return (y & VALUE_MASK);
}

inline static int is_nan(double x)
{
	uint64_t y = (*(uint64_t*)&x);
	return ((y & EXP_MASK) == EXP_MASK) && ((y & FRACTION_MASK) != 0);
}

inline int is_inf(double x)
{
	uint64_t y = (*(uint64_t*)&x);
	return ((y & EXP_MASK) == EXP_MASK) && ((y & FRACTION_MASK) == 0);
}

#define INSPECT_VALUE(type,value,label)        \
         {                                \
             type name = (value);        \
             printf("%s\n    ",label);     \
             print_bits((uint8_t*)&name,sizeof(name)); \
             printf("    is_nan = %d\n",is_nan(name)); \
             printf("    isnan = %d\n",isnan(name)); \
         }     

int main(void)
{
	INSPECT_VALUE(double,3.141592653569, "3.141592653569");
	INSPECT_VALUE(double, nan("0"), "nan(0)");
	INSPECT_VALUE(double, nan("10"), "nan(10)");
	//INSPECT_VALUE(uint64_t, NAN_MASK);

	return 0;
}