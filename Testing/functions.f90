!  FortranFromC(PP)
module m
    use, intrinsic :: iso_c_binding, float=>c_float, int=>c_int
    implicit none
    
    type, bind(C) :: vec3
        real(float) :: x
        real(float) :: y
        real(float) :: z
    end type vec3

    integer(int), bind(C, name="size") :: arr_size
    ! These don't work
    ! type(vec3), bind(C, name="array") :: array(3)
    ! type(vec3) :: array(3)
    ! type(vec3), target, bind(C, name="array") :: array(3)
    ! type(c_ptr), pointer :: array_ptr ! void pointer (void *)
    ! type(vec3), target, dimension(:), allocatable :: f_array

    ! type(vec3), pointer :: array(:)
    ! type(vec3), pointer, bind(C, name="test_array") :: array(:)
    ! real(float), pointer :: array(:)
    type(vec3), pointer :: array(:)

    
contains
    subroutine pass_ptr(array_ptr) bind(C)
        type(c_ptr), intent(in) :: array_ptr
        ! type(vec3), allocatable :: temp(:)

        ! if (allocated(array)) then
        !     print *, "deallocating"
        !     deallocate(array)
        !     print *, "deallocated"
        ! endif
        call c_f_pointer(array_ptr, array, [arr_size])
        ! call c_f_pointer(array_ptr, temp, [arr_size])

        ! print *, size(array)
        

        ! if(.not. C_associated(array_ptr)) error stop "input pointer not C associated"
        ! if(.not. associated(array)) error stop "data array pointer not associated"

    end subroutine pass_ptr
    
    subroutine arr_op() BIND(C)
        integer i
        call print_size()

        ! array_ptr = c_loc(array(1)) ! array_ptr is now a C pointer to array
        ! call c_f_pointer(array_ptr, f_array, [size])
        
        ! integer :: test_int
        ! if(.not. C_associated(array_ptr)) error stop "input pointer not C associated"
        ! if(.not. associated(f_array)) error stop "data array pointer not associated"

        ! type(vec3), intent(inout) :: array(*)
        ! print *, size
        
        ! do i = 1, arr_size
        !     array(i)%x = array(i)%x * 5
        !     array(i)%y = array(i)%y - 2
        !     array(i)%z = array(i)%z / 3
        !     print *, array(i)%x, array(i)%y, array(i)%z
        ! end do

        ! do i = 1, size
        !     array(i) = array(i) * 5
        ! end do

        ! test_int = 0
        ! do i = -test_int, test_int
        !     print *, i
        ! end do

        contains
        subroutine print_size()
            print *, 'array size', size(array)
        end subroutine
    end subroutine arr_op 
end module m