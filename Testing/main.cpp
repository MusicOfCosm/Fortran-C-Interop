// FortranFromCPP
#include "functions.hpp"

#include <iostream>

// #include <glm/glm.hpp>

#include <vector>

struct vec3 {
    float x;
    float y;
    float z;
};

extern "C" {
    void pass_ptr(void * arr);
    void arr_op();

    std::vector<vec3> test_array(1);

    // std::vector<float> test_array = {
    //     0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f
    // };

    vec3 *arr_ptr = &test_array.front();

    int size = test_array.size();
}

int main()
{
	for (vec3 element : test_array)
        printf("%f, %f, %f\n", element.x, element.y, element.z);

    // for (float element : test_array)
    //     printf("%f ", element);

    pass_ptr(&arr_ptr);
    // pass_ptr(&(&test_array[0].x));
    
    arr_op();

    test_array = {
        vec3{1.f, 2.f, 3.f},
        vec3{2.f, 4.f, 6.f},
        vec3{3.f, 6.f, 9.f}
    };

    for (vec3 element : test_array)
        printf("%f, %f, %f\n", element.x, element.y, element.z);

    test_array = {
    };
    size = test_array.size();
    // arr_ptr = &test_array.front();
    pass_ptr(&arr_ptr);

    arr_op();

    for (vec3 element : test_array)
        printf("%f, %f, %f\n", element.x, element.y, element.z);
    
    // for (float element : test_array)
    //     printf("%f ", element);
    test_array = {
        vec3{1.f, 2.f, 3.f},
        vec3{2.f, 4.f, 6.f},
        vec3{3.f, 6.f, 9.f},
        vec3{4.f, 8.f, 12.f}
    };
    test_array.resize(1080*1920);
    size = test_array.size();
    arr_ptr = &test_array.front();
    pass_ptr(&arr_ptr);
    arr_op();

    // for (vec3 element : test_array)
    //     printf("%f, %f, %f\n", element.x, element.y, element.z);
    puts("Yep");
    
    return 0;
}