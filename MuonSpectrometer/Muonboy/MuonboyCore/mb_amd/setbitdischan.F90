!
 SUBROUTINE SetBitDisChan(JTYP,JFF,JZZ,JOB,Nlay,Nchan,Nword,Iword)
 IMPLICIT NONE
!
#include "AmdcStand/amdcsim_com.inc"
!
      INTEGER, INTENT(IN)  :: JTYP, JFF, JZZ, JOB, Nlay, Nchan
      INTEGER, INTENT(OUT) :: Nword, Iword(*)
      INTEGER :: Jbegin, Jend
!
      Jbegin = 1
      Jend   = NBdisaChan
      CALL SetBitChan(JTYP,JFF,JZZ,JOB,Nlay,Nchan,Nword,Iword,Jbegin,Jend)
!
 END SUBROUTINE SetBitDisChan
!
