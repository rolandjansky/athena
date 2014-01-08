!
 SUBROUTINE SetBitDedChan(JTYP,JFF,JZZ,JOB,Nlay,Nchan,Nword,Iword)
 IMPLICIT NONE
!
#include "AmdcStand/amdcsim_com.inc"
!
      INTEGER, INTENT(IN)  :: JTYP, JFF, JZZ, JOB, Nlay, Nchan
      INTEGER, INTENT(OUT) :: Nword, Iword(*)
      INTEGER :: Jbegin, Jend
!
      Jbegin = NBdisaChan + 1
      Jend   = NBdeadChan
      CALL SetBitChan(JTYP,JFF,JZZ,JOB,Nlay,Nchan,Nword,Iword,Jbegin,Jend)
!
 END SUBROUTINE SetBitDedChan
!
