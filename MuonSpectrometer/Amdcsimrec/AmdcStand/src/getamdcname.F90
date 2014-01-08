!
!> \author SaMuSoG
!
      SUBROUTINE GetAmdcName(FileName)
      IMPLICIT NONE
      CHARACTER(16), INTENT(OUT) :: FileName
!
#include "AmdcStand/amdcsim_com.inc"
!
      FileName = C_AMDC_NAME
!
      END SUBROUTINE GetAmdcName
!      
