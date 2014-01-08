!
!> \author SamuSog
!
!--------------------------------------------------------------------
!> Get Range: Jtyp
!--------------------------------------------------------------------
 SUBROUTINE FStationTypeRange( JtypBeg, JtypEnd )
!
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: JtypBeg, JtypEnd
!
#include "AmdcStand/amdcsim_param.inc"
!
    JtypBeg = 1
    JtypEnd = MTYP
!
 END SUBROUTINE FStationTypeRange
!
!--------------------------------------------------------------------
!> Get Range: Jgeo
!--------------------------------------------------------------------
 SUBROUTINE FStationGeomRange( JgeoBeg, JgeoEnd )
!
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: JgeoBeg,JgeoEnd
!
#include "AmdcStand/amdcsim_param.inc"
!
    JgeoBeg = 1
    JgeoEnd = MGEO
!
 END SUBROUTINE FStationGeomRange
!
!--------------------------------------------------------------------
!> Get Range: Jzz
!--------------------------------------------------------------------
 SUBROUTINE FPositionZRange( JzzBeg, JzzEnd )
!
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: JzzBeg, JzzEnd
!
#include "AmdcStand/amdcsim_param.inc"
!
    JzzBeg = - MZZ
    JzzEnd =   MZZ
!
 END SUBROUTINE FPositionZRange
!
!--------------------------------------------------------------------
!> Get Range: Jff
!--------------------------------------------------------------------
 SUBROUTINE FPositionPhiRange( JffBeg, JffEnd )
!
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: JffBeg,JffEnd
!
    JffBeg = 1
    JffEnd = 8
!
 END SUBROUTINE FPositionPhiRange
!
!--------------------------------------------------------------------
!> Get Range: Jcut
!--------------------------------------------------------------------
 SUBROUTINE FCutOutRange( JcutBeg, JcutEnd )
!
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: JcutBeg, JcutEnd
!
    JcutBeg = 1
    JcutEnd = 4
!
 END
!
