*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
************************************************************************
      SUBROUTINE BPREPA(XYZ, RFZSC)
*
*>> Author : Saclay Muon Software Group (SaMuSoG)
*
      IMPLICIT NONE
*
*
      REAL XYZ(3), RFZSC(6)
      REAL R, F
*
      R = SQRT( XYZ(1)**2 + XYZ(2)**2 )
      IF( R.GT.0.000001 ) THEN
         F = ATAN2(XYZ(2),XYZ(1))
         IF(F.LT.0.) F = F + 6.283185308
      ELSE
         F = 0.000000
         R = 0.000001
      ENDIF
*
      RFZSC(1) = R
      RFZSC(2) = F
      RFZSC(3) = XYZ(3)
************************
      RFZSC(4) = XYZ(2)/R
      RFZSC(5) = XYZ(1)/R
***** RFZSC(4) = SIN(F)
***** RFZSC(5) = COS(F)
      RFZSC(6) = ABS(XYZ(3))
************************
*
*
      RETURN
      END
