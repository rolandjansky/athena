!
!> Brings angle F (in radian) between 0 and 2.Pi
!
 PURE REAL(8) FUNCTION PITOPI(F)
 USE M_MB_General_CONSTANTS, ONLY : TwoPi
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: F
!
    PITOPI = F
!
    DO WHILE( PITOPI > TwoPi )
      PITOPI = PITOPI - TwoPi
    ENDDO
!
    DO WHILE( PITOPI < 0.D0 )
      PITOPI = PITOPI + TwoPi
    ENDDO
!
 END FUNCTION PITOPI
!
