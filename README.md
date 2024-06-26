# Fortran C Interop

There comes a question for every C/C++ programmer of what compiler to use. gcc? clang? msvc? In April 2023, I looked up if there were other compilers, and I found that [Intel had their own](https://www.intel.com/content/www/us/en/developer/tools/oneapi/dpc-compiler.html), which interested me since my computer's CPU and GPU are from them. I also saw and downloaded their [Fortran compiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/fortran-compiler.html) since it seemed interesting, though I actually didn't do anything with it.

Fast forward to December, and after updating both compilers, I decided to look more into Fortran. It was interesting and all, but I couldn't think of a project than I couldn't do in C/C++ instead. However, I knew that basically every compiled language can use C code, and it turns out, C can use Fortran code as well!


## Testing

This folder contains my initial testing for interoperability between C and Fortran. I decided to clean it up less that I would usually, only enough so that you can run it.

Most of my time was spent trying to pass a pointer from C/C++ to Fortran and modify it. It took a lot of time, but I figured out that you needed to declare the array as a pointer to a pointer  instead of a pointer on the C/C++ side (`extern void pass_ptr(float** arr);` instead of `extern void pass_ptr(float* arr);`.


## Slime_mold

*vcpkg and Fortran don't mix well; for the sake of putting this on GitHub, I changed the project from how it was and used git submodules for the first time*

In order to see if it was viable to use Fortran C interop in an actual project, I copied my sources files from a slime mold simulation that I did using OpenGL, remaking the function that updates the agents' attributes in Fortran.

What made it much more interesting for me was that ifx (Intel's Fortran compiler) can use `do concurrent` loops, which parallelizes the process (if possible). With it, the program runs way faster than if it was a normal `do` loop, but it crashes after ten to thirty seconds without explanation. 
Looking with [AppCrashView](https://www.nirsoft.net/utils/app_crash_view.html), the program "Stopped working" with an "Access Violation" Exception Code.

Another thing is that, since I didn't want the ImGui window to mess with the simulation, I used the *docking* branch that can use a floating ImGui window instead.


## Link to ressources

Documentation in Fortran is not the best (it's still better than say, CMake's though), and when looking at answers to questions that have already been asked on forums and communities, the explanation mostly go over your head as a beginner.

Here are some ressources that helped me learn.


### [Parallel Programming in Fortran Cheat sheets](https://github.com/sueskind/ur-parallel-programming)

This repository has cheat sheets on base Fortran and on parallel programming. I didn't really understand the MPI one, and OpenMP even less, but I think that the one for learning base Fortran is even better than the [tutorials at fortran-lang.org](https://fortran-lang.org/learn/quickstart/), even if it is incomplete.


### [Fortran C Interop](https://github.com/aerosayan/fortran-c-interop)

This repository was particularly helpful in getting started with the projects here, as it provided examples and explanations.


### Others

[Interoperability with C (GNU Fortran docs)](https://gcc.gnu.org/onlinedocs/gfortran/Interoperability-with-C.html)

[Using IBM XL C with Fortran (docs)](https://www.ibm.com/docs/en/xl-c-aix/13.1.3?topic=guide-using-xl-fortran)