#include "lc3_config.h"

uint16_t sign_extend(uint16_t x, int bit_count) {
	if ((x >> (bit_count - 1)) & 1) {
		x |= (0xFFFF << bit_count);
	}
	return x; 
}



void update_flags(uint16_t r) {	
	if (reg[r] == 0)
	{
		reg[R_COND] = FL_ZRO
	}
	else if (reg[r] >> 15)
	{
		reg[R_COND] = FL_NEG;
	}
	else
	{
		reg[R_COND] = FL_POS; 
	}
}

