!
!> Brings angle F (in radian) between -Pi and +Pi
!
 PURE REAL(8) FUNCTION PIMIPI(F)
 USE M_MB_General_CONSTANTS, ONLY : Pi, TwoPi
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: F
!
    PIMIPI = F
!
    DO WHILE( PIMIPI > Pi )
      PIMIPI = PIMIPI - TwoPi
    ENDDO
!
    DO WHILE( PIMIPI < -Pi )
      PIMIPI = PIMIPI + TwoPi
    ENDDO
!
 END FUNCTION PIMIPI
!
