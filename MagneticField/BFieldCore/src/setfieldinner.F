*
*
*
*

* Author : SaMuSoG
************************************************************************
      SUBROUTINE SETFIELDINNER(BINNER_I,RMAINN_I,ZMAINN_I,
     +RTORI_I,ZTORI_I,RTORO_I,ZTORO_I,EPS0_I,NVERSION_I)
*
      IMPLICIT NONE
*
#include "BFieldCore/cflgen.inc"
*
      INTEGER NVERSION_I
      REAL    BINNER_I,RMAINN_I,ZMAINN_I
      REAL    RTORI_I,ZTORI_I,RTORO_I,ZTORO_I, EPS0_I
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
      RETURN
      END
