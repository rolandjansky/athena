*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      SUBROUTINE GEADFI(TTT,NT,ITLMA, DTL,TLMI,JTL)
*
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 24/09/97
*
      IMPLICIT NONE
*
*
      INTEGER NT, ITLMA, JTL(*)
      REAL    TTT(*), DTL, TLMI
      INTEGER IT, ITL
      REAL    TEST
*
      IF( NT.LE.0 ) RETURN
*
      DTL  = (TTT(NT)-TTT(1)) / (ITLMA-0.01)
      TLMI = TTT(1) - DTL
*
      IT = 2
      DO 100 ITL=1,ITLMA
        TEST = TLMI + (ITL+1.)*DTL
        IF( TEST.GT.TTT(IT) ) THEN
          IF( IT.GE.NT ) THEN
            JTL(ITL) = NT
          ELSE
            JTL(ITL) = - IT
10          IT = IT + 1
            IF( TEST.GT.TTT(IT) .AND. IT.LT.NT ) GO TO 10
          ENDIF
        ELSE
          JTL(ITL) = IT
        ENDIF
100   CONTINUE
*
*
      RETURN
      END
