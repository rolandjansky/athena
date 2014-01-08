!
! Author : SaMuSoG  17/06/98
!
 SUBROUTINE REMAMA(LUN,III)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: LUN
 INTEGER, INTENT(OUT) :: III
 INTEGER :: IAIII
!
#include "AmdcStand/comama.inc"
!
    READ(LUN,'(I4)') III
    IAIII = IABS(III)
    IF( IAIII < 1 .OR. IAIII > MAMAMA ) THEN
      PRINT 7000,III
7000  FORMAT(' In REMAMA : Matter identifier',I5,' is out of range  =====>  STOP !!!')
      STOP
    ENDIF
!
 END SUBROUTINE REMAMA
!
