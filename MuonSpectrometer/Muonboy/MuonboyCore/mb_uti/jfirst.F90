!
 PURE INTEGER FUNCTION JFIRST(J)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: J
!
!>> Gets the two first decimal digits of integer J
!
    JFIRST = J
    DO WHILE ( JFIRST > 100 )
      JFIRST = JFIRST / 100
    ENDDO
!
 END FUNCTION JFIRST
!
