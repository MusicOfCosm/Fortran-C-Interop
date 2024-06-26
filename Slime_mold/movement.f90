module Slime
    use, intrinsic :: iso_c_binding, float=>c_float, int=>c_int, bool=>c_bool
    implicit none

    ! Global variables
    integer(int),  bind(C, name="WIDTH")  :: WIDTH
    integer(int),  bind(C, name="HEIGHT") :: HEIGHT

    real(float),   bind(C, name="SPEED") :: SPEED
    integer(int),  bind(C, name="SENSOR_RANGE") :: SENSOR_RANGE
    integer(int),  bind(C, name="SENSOR_SIZE") :: SENSOR_SIZE
    real(float),   bind(C, name="SA") :: SA
    real(float),   bind(C, name="RA") :: RA
    logical(bool), bind(C, name="BOUNDARY") :: BOUNDARY

    integer(int), bind(C, name="instances") :: instances
    integer(int), bind(C, name="canvas_size") :: canvas_size
    real(float), pointer :: canvas(:)

    double precision, parameter :: PI = 3.14159265358979323846

    type, bind(C) :: vec3
        real(float) :: x
        real(float) :: y
        real(float) :: z
    end type vec3

    
contains
    subroutine pass_ptr(array_ptr) bind(C)
        type(c_ptr), intent(in) :: array_ptr

        call c_f_pointer(array_ptr, canvas, [canvas_size])
        ! if(.not. C_associated(array_ptr)) error stop "input pointer not C associated"
        ! if(.not. associated(canvas)) error stop "data array pointer not associated"
    end subroutine pass_ptr

    real(float) function uniform(min, max)
        real, intent(in) :: min, max
        real :: random
        
        call random_number(random) ! Real number between 0 and 1

        uniform = (random * (max-min)) + min
    end function uniform
    
    ! subroutine boundaries(x, y)
    !     real x, y


    ! end subroutine boundaries

    subroutine movement_f(agents) bind(C, name="movement_f")
        type(vec3), intent(inout) :: agents(*)

        ! print *, "SPEED", SPEED
        ! print *, "SENSOR_RANGE", SENSOR_RANGE
        ! print *, "SENSOR_SIZE", SENSOR_SIZE
        ! print *, "SA", SA
        ! print *, "RA", RA
        ! print *, "BOUNDARY", BOUNDARY

        ! print *, agents(1)%x, agents(1)%y, agents(1)%z
        real(float) :: forward, left, right, rand
        
        integer i
        rand = uniform(-RA, RA)
        do concurrent (i = 1:instances)
            forward = sense_f(0.0, agents(i))
            left = sense_f(SA, agents(i))
            right = sense_f(-SA, agents(i))
    

            ! if (forward >= left .and. forward >= right) Do nothing
    
            if (forward < left .and. forward < right) then
                agents(i)%z = agents(i)%z + (rand)
    
            else if (left > right) then
                agents(i)%z = agents(i)%z + (RA)
    
            else if (left < right) then
                agents(i)%z = agents(i)%z + (-RA)
            end if
            
            !moving
            agents(i)%x = agents(i)%x + (SPEED * cos(agents(i)%z))
            agents(i)%y = agents(i)%y + (SPEED * sin(agents(i)%z))
        end do

        do i = 1, instances
            if (agents(i)%x < 0 .and. agents(i)%x > WIDTH - 1 .and. agents(i)%y < 0 .and. agents(i)%y > HEIGHT - 1) then
                if (BOUNDARY .eqv. .true.) then
                    agents(i)%z = uniform(real(-PI), real(PI))
                end if
                call RealBoundaries(agents(i)%x, agents(i)%y)
            end if
        end do

    contains
        pure subroutine RealBoundaries(x, y)
            real(float), intent(inout) :: x, y
            if (BOUNDARY .eqv. .true.) then
                if (x > WIDTH) then
                    x = real(WIDTH)
                else if (x < 0) then
                    x = 0.0
                endif
                if (y > HEIGHT) then
                    y = real(HEIGHT)
                else if (y < 0) then
                    y = 0.0
                endif
            else
                if (x > WIDTH) then
                    x = x - WIDTH
                else if (x < 0) then
                    x = x + WIDTH
                endif

                if (y > HEIGHT) then
                    y = y - HEIGHT
                else if (y < 0) then
                    y = y + HEIGHT
                endif
            endif
        end subroutine RealBoundaries
        pure subroutine IntBoundaries(x, y)
            integer, intent(inout) :: x, y
            if (BOUNDARY .eqv. .true.) then
                if (x > WIDTH) then
                    x = WIDTH
                else if (x < 0) then
                    x = 0.0
                endif
                if (y > HEIGHT) then
                    y = HEIGHT
                else if (y < 0) then
                    y = 0.0
                endif
            else
                if (x > WIDTH) then
                    x = x - WIDTH
                else if (x < 0) then
                    x = x + WIDTH
                endif

                if (y > HEIGHT) then
                    y = y - HEIGHT
                else if (y < 0) then
                    y = y + HEIGHT
                endif
            endif
        end subroutine IntBoundaries

        pure real function sense_f(sensorAngleOffset, agent) result(summation)
            real, intent(in) :: sensorAngleOffset
            type(vec3), intent(in) :: agent
        
            real :: sensorAngle, sensorDirX, sensorDirY
            integer :: sensorCentreX, sensorCentreY
            integer :: offsetX, offsetY
            integer :: posX, posY, pos
            
            sensorAngle = agent%z + sensorAngleOffset
            sensorDirX = cos(sensorAngle)
            sensorDirY = sin(sensorAngle)
            sensorCentreX = nint(agent%x + sensorDirX * SENSOR_RANGE)
            sensorCentreY = nint(agent%y + sensorDirY * SENSOR_RANGE)
            summation = 0.0

            do offsetX = -SENSOR_SIZE, SENSOR_SIZE !Goes from -SENSOR_SIZE to SENSOR_SIZE
                do offsetY = -SENSOR_SIZE, SENSOR_SIZE
                    posX = sensorCentreX + offsetX
                    posY = sensorCentreY + offsetY
                    call IntBoundaries(posX, posY) !resetting posX and posY to be inside the bounds
                    
                    pos = (posY) * (WIDTH) + (posX) !Convert from 2D array to 1D array
                    if (pos >= WIDTH * HEIGHT) pos = pos - (WIDTH+1)
                    !if (pos >= WIDTH * HEIGHT .and. pos <= 0) printf("posX: %d, posY: %d, pos: %d\n%d\n\n", posX, posY, pos, WIDTH*HEIGHT)

                    summation = summation + canvas(pos+1)
                end do
            end do

        end function sense_f
    end subroutine movement_f
end module Slime