      SUBROUTINE TOPDEC

#include "HERWIG65.INC"

      INTEGER I,J,IBT,IQK,IQB
      DOUBLE PRECISION BRCON

      print *, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

C--code to change the top decay modes
      BRCON = 0.5D0*(1.0D0-0.333D0)
C--find the CKM unsurpressed modes and reset the branching ratios
      DO I=1,NDKYS
C--top decay modes
         IF(IDK(I).EQ.6.AND.IDKPRD(4,I).EQ.0) THEN
            IBT = 0
            IQK = 0
            IQB = 0
            DO J=1,3
               IF(IDKPRD(J,I).EQ.5) THEN
                  IBT = IDKPRD(J,I)
               ELSEIF(IDKPRD(J,I).LE.4) THEN
                  IQK = IDKPRD(J,I)
               ELSEIF(IDKPRD(J,I).LE.10) THEN
                  IQB = IDKPRD(J,I)
               ENDIF
            ENDDO
C--reset the branching ratios
            IF(IBT.NE.0.AND.IQK.NE.0.AND.IQB.NE.0) THEN
c--u d mode
               IF(IQK.EQ.2.AND.IQB.EQ.7) THEN
                  BRFRAC(I) = BRCON*VCKM(1,1)
c--c s mode
               ELSEIF(IQK.EQ.4.AND.IQB.EQ.9) THEN
                  BRFRAC(I) = BRCON*VCKM(2,2)
               ENDIF
            ENDIF
C--antitop decay modes
         ELSE IF(IDK(I).EQ.12.AND.IDKPRD(4,I).EQ.0) THEN
            IBT = 0
            IQK = 0
            IQB = 0
            DO J=1,3
               IF(IDKPRD(J,I).EQ.11) THEN
                  IBT = IDKPRD(J,I)
               ELSEIF(IDKPRD(J,I).LE.4) THEN
                  IQK = IDKPRD(J,I)
               ELSEIF(IDKPRD(J,I).LE.10) THEN
                  IQB = IDKPRD(J,I)
               ENDIF
            ENDDO
C--reset the branching ratios
            IF(IBT.NE.0.AND.IQK.NE.0.AND.IQB.NE.0) THEN
c--u d mode
               IF(IQB.EQ.8.AND.IQK.EQ.1) THEN
                  BRFRAC(I) = BRCON*VCKM(1,1)
c--c s mode
               ELSEIF(IQB.EQ.10.AND.IQK.EQ.3) THEN
                  BRFRAC(I) = BRCON*VCKM(2,2)
               ENDIF
            ENDIF
         ENDIF
      ENDDO
C--add the CKM supressed top decay modes
      DO I=1,2
         DO J=1,3
            IF(I.NE.J) THEN
               CALL HWMODK( 6,BRCON*VCKM(I,J),100, 2*I,(-2)*J+1, 5,0,0)
               CALL HWMODK(-6,BRCON*VCKM(I,J),100,(-2)*I, 2*J-1,-5,0,0)
            ENDIF
         ENDDO
      ENDDO

      RETURN
      END
