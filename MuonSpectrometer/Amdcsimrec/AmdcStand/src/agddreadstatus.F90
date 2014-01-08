!
 SUBROUTINE AgddReadStatus( Iset, Iinout )
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: Iset
 INTEGER, INTENT(INOUT) :: Iinout
 INTEGER, SAVE :: Istatus = 0
!
    IF( Iset > 0 ) THEN
      Istatus = Iinout
    ELSE
      Iinout  = Istatus
    ENDIF
!
 END SUBROUTINE AgddReadStatus
!
