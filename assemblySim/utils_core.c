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
		reg[R_COND] = FL_ZRO;
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

uint16_t swap16(uint16_t x)
{
	return (x << 8) | (x >> 8);
}

void read_image_file(FILE* file) 
{
	/* the origin tells us where in memory to place the image */
	uint16_t origin;
	fread(&origin, sizeof(origin), 1, file);
	origin = swap16(origin);

	/* we know the maximum file size so we only need one fread */
	uint16_t max_read = MEMORY_MAX - origin;
	uint16_t* p = memory + origin;
	size_t read = fread(p, sizeof(uint16_t), max_read, file);

	/* swap to little endian */
	while (read-- > 0)
	{
		*p = swap16(*p);
		++p;
	}

}

int read_image(const char* image_path)
{
	FILE* file = fopen(image_path, "rb");
	if (!file) { return 0; };
	read_image_file(file);
	fclose(file);
	return 1;
}

void mem_write(uint16_t address, uint16_t val)
{
	memory[address] = val;
}

uint16_t mem_read(uint16_t address)
{
	if (address == MR_KBSR)
	{
		if (check_key())
		{
			memory[MR_KBSR] = (1 << 15);
			memory[MR_KBDR] = getchar();
		}
		else
		{
			memory[MR_KBSR] = 0;
		}
	}
	return memory[address];
}

HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;

void disable_input_buffering()
{
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hStdin, &fdwOldMode); /* save old mode */
	fdwMode = fdwOldMode
		^ ENABLE_ECHO_INPUT  /* no input echo */
		^ ENABLE_LINE_INPUT; /* return when one or
								more characters are available */
	SetConsoleMode(hStdin, fdwMode); /* set new mode */
	FlushConsoleInputBuffer(hStdin); /* clear buffer */
}

void restore_input_buffering()
{
	SetConsoleMode(hStdin, fdwOldMode);
}

uint16_t check_key()
{
	return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}

void handle_interrupt(int signal)
{
	restore_input_buffering();
	printf("\n");
	exit(-2);
}
