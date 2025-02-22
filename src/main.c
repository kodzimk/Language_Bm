#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

typedef union {
	uint64_t as_u64;
	int64_t as_i64;
	double as_f64;
	void *as_ptr;
}Frame;