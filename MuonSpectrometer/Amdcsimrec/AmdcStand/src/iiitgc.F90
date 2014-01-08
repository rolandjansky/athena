!
!> Get a flag identifying the TGC chamber type
!> \author  SaMuSoG 4/12/97
!
 SUBROUTINE IIITGC(JTYP,JGEO,JOB, ITGC)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: JTYP, JGEO, JOB
 INTEGER, INTENT(OUT) :: ITGC
 INTEGER :: JTEC, JTGO
!
#include "AmdcStand/amdcsim_com.inc"
!
      ITGC = 0
      IF( JOB < 1 .OR. JOB > NOBJ(JTYP,JGEO) ) RETURN
      JTGO = JTYP_G_O(JTYP,JGEO,JOB)
      JTEC = ITEC_V(JTGO)
      IF( CARTEC(JTEC) /= 'TGC' )              RETURN
      ITGC = NINT( Geo_D2_V(JTGO) )
!
 END SUBROUTINE IIITGC
!
