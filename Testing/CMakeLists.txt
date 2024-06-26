cmake_minimum_required(VERSION 3.8)

enable_language(Fortran)

# Intel compilers
# set(CMAKE_C_COMPILER icx)
# set(CMAKE_CXX_COMPILER icx)
# set(CMAKE_F_COMPILER ifx)

# GNU compilers
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_F_COMPILER gfortran)

if (CMAKE_C_COMPILER MATCHES icx)
    set(CMAKE_C_FLAGS "/Qstd=c17")
    set(CMAKE_CXX_FLAGS "/Qstd=c++17")
    set(FFLAGS "/Qsave-temps-")
endif()

project(CFromFortran)
set(CMAKE_Fortran_MODULE_DIRECTORY CMakeFiles/CFromFortran.dir)
add_executable(CFromFortran main.f90 functions.c)
# add_library(c_functions STATIC functions.c)
# add_executable(CFromFortran main.f90)
# target_link_libraries(CFromFortran c_functions)



project(FortranFromC)
set(CMAKE_Fortran_MODULE_DIRECTORY CMakeFiles/FortranFromC.dir)
add_executable(FortranFromC main.c functions.f90)
# add_library(f_functions STATIC functions.f90)
# add_executable(FortranFromC main.cpp)
# target_link_libraries(FortranFromC f_functions)


project(FortranFromCPP)
set(CMAKE_Fortran_MODULE_DIRECTORY CMakeFiles/FortranFromCPP.dir)
add_executable(FortranFromCPP main.cpp functions.f90)


# https://enccs.github.io/cmake-workshop/cxx-fortran/