// CFromFortran
#include "functions.h"
#include <stdio.h>

// Void, no arguments
extern void print_c()
{
    puts("Hello, from C!\n");
}

// With argument(s)
extern void print_str(char *string)
{
    printf("%s\n", string);
}

// non-string, with a value changed (number)
extern void randomint(int* number, int min, int max)
{
    float value = uniform((float)min, (float)max);
	*number = (int)(value + ((value >= 0) ? 0.5 : -0.5));
}