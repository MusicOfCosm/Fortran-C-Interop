//Zorin OS <--
//Lubuntu
//Peppermint OS

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#ifdef _MSC_VER
#define _CRT_RAND_S //If using C++, make sure functions.h is before other includes, especially stdlib.h
#endif
#include <stdlib.h>
#include <math.h>
#include <time.h>

// [min, max[
double uniform(float min, float max)
{
    if (!(min < max)) {
        fprintf(stderr, "The first value, %f, must be lesser than the second one, %f!", min, max);
        exit(EXIT_FAILURE);
    } else {
#ifdef _MSC_VER
        unsigned int number;
        errno_t err = rand_s(&number);
        if (err != 0) {
            printf_s("The rand_s function failed!\n");
            exit(EXIT_FAILURE);
        }
        return ( (double)number / ((double)UINT_MAX + 1) * (max-min) ) + min;
#else
        unsigned int t;
        srand((&t));
        int number = rand();
        return ( (double)number / ((double)RAND_MAX + 1) * (max-min) ) + min;
#endif
    }
}

// [min, max]
int randint(int min, int max)
{
    float value = uniform((float)min, (float)max);
	return (int)(value + ((value >= 0) ? 0.5 : -0.5));
}

#ifdef __cplusplus
}
#endif