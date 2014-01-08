!
! \author J.F.Laporte
!
 INTEGER FUNCTION ISLFLAG(CHDET,CHFLAG)
 IMPLICIT NONE
 CHARACTER(4) :: CHDET, CHFLAG
!
!>> Fake version of the SLUG function ISLFLAG
!>> to be used in standalone mode only
!>> Author  : J.F.Laporte
!
   ISLFLAG=9999
!
 END FUNCTION ISLFLAG
!
