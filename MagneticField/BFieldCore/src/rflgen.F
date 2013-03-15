*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
************************************************************************
      SUBROUTINE RFLGEN(NVERSION,BINNER,RMAINN,ZMAINN,
     +                  RTORI,ZTORI,RTORO,ZTORO,EPS0)
*
*>> Initialise general field parameters
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 19/01/98
*>> NB: preliminary version
*
      IMPLICIT NONE
*
*
      INTEGER NVERSION
      REAL    BINNER,RMAINN,ZMAINN,RTORI,ZTORI,RTORO,ZTORO,EPS0
*
      INTEGER NVERSION_I
      REAL    BINNER_I,RMAINN_I,ZMAINN_I
      REAL    RTORI_I,ZTORI_I,RTORO_I,ZTORO_I,EPS0_I
*
      DATA NVERSION_I        /  3 /
      DATA BINNER_I          /  20. /
      DATA RMAINN_I,ZMAINN_I /  105.,320. /
      DATA EPS0_I            /  5. /
      DATA RTORI_I,ZTORI_I   /  423.0,  681.5 /
******DATA RTORO_I,ZTORO_I   / 1250.0, 1500.0 /
      DATA RTORO_I,ZTORO_I   / 1500.0, 2300.0 /
*
      NVERSION = NVERSION_I
      BINNER   = BINNER_I
      RMAINN   = RMAINN_I
      ZMAINN   = ZMAINN_I
      EPS0     = EPS0_I
      RTORI    = RTORI_I
      ZTORI    = ZTORI_I
      RTORO    = RTORO_I
      ZTORO    = ZTORO_I
*
*
      RETURN
      END
 
