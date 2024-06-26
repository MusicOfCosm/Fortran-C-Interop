program CFROMFORTRAN
    use iso_c_binding, only: null_char=>c_null_char
    implicit none
    
    interface with_function_C
        subroutine print_c() bind(C)
        end subroutine
        
        subroutine print_str(string) bind(C)
            use iso_c_binding, only: char=>c_char
            character(char), intent(in) :: string(*)
        end subroutine

        !takes number and assign it an integer between min and max
        subroutine randomint(number, min, max) bind(C)
            use iso_c_binding, only: int=>c_int
            integer(int), intent(inout) :: number
            integer(int), value, intent(in) :: min, max
        end subroutine
    end interface with_function_c

    integer :: ran

    call print_c()

    call print_str('Using a C function from Fortran!'//null_char)
    
    call randomint(ran, 2, 6)
    print *, 'random integer between 2 and 6: ', ran
end program CFROMFORTRAN