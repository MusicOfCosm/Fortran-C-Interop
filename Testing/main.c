// FortranFromC
#include "functions.h"

#include <stdio.h>
#include <stdlib.h>

extern void pass_ptr(float** arr);
extern void arr_op();

float* arr; // Global dynamic array

int size = 10;

int main() {
    arr = (float*)malloc(size * sizeof(float)); // Allocate memory for the array
    for(int i = 0; i < size; i++) {
        arr[i] = i + 1.0; // Initialize the array
    }
    for(int i = 0; i < size; i++) {
        printf("%f\n", arr[i]);
    }
    printf("\nYay!\n\n");

    pass_ptr(&arr); // Pass the array to Fortran
    arr_op(); // Call the Fortran subroutine

    for(int i = 0; i < size; i++) {
        printf("%f\n", arr[i]);
    }

    free(arr); // Don't forget to free the memory when you're done

    return 0;
}
