!
!> Tells (slowly) whether point XX,YY is inside or outside the
!> polygon made of the NP points XP(i),YP(i)
!
 PURE LOGICAL FUNCTION INSIDE(XX,YY,NP,XP,YP)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NP
 REAL(8), INTENT(IN) :: XX, YY, XP(*), YP(*)
 INTEGER :: I
 REAL(8) :: AA, A0, A1, A2, DA
!
#include "AmdcStand/copipi.inc"
!
    INSIDE = .FALSE.
    IF( NP <= 2 ) RETURN
!
    IF( XX < MINVAL(XP(1:NP)) ) RETURN
    IF( XX > MAXVAL(XP(1:NP)) ) RETURN
    IF( YY < MINVAL(YP(1:NP)) ) RETURN
    IF( YY > MAXVAL(YP(1:NP)) ) RETURN
!
    AA = 0.D0
    A0 = ATAN2( YP(1)-YY , XP(1)-XX )
    A1 = A0
    DO I=2,NP
      A2 = ATAN2( YP(I)-YY , XP(I)-XX )
      DA = A2 - A1
      IF( DA > PiP ) THEN
        DA = DA - TwoPi
      ELSE IF( DA < -PiP ) THEN
        DA = DA + TwoPi
      ELSE IF( DA > PiM .OR. DA < -PiM ) THEN
        RETURN
      ENDIF
      AA = AA + DA
      A1 = A2
    ENDDO
    DA = A0 - A1
    IF( DA > PiP ) THEN
      DA = DA - TwoPi
    ELSE IF( DA < -PiP ) THEN
      DA = DA + TwoPi
    ELSE IF( DA > PiM .OR. DA < -PiM ) THEN
      RETURN
    ENDIF
    AA = AA + DA
!
    AA = ABS(AA)
    IF( ABS(AA-TwoPi) < 0.1D0 ) INSIDE = .TRUE.
!
 END FUNCTION INSIDE
!
!
!>> Tells (quickly) whether point XX,YY is inside or outside the
!>> polygon made of the NP points XP(i),YP(i)
!
 PURE LOGICAL FUNCTION INSIDF(XX,YY,NP,XP,YP)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NP
 REAL(8), INTENT(IN) :: XX, YY, XP(*), YP(*)
 REAL(8), PARAMETER :: EPS=0.000001D0
 INTEGER :: NINTER, I1, I2
 REAL(8) :: DYI, EI, XI
!
    INSIDF = .FALSE.
    IF( NP <= 2 ) RETURN
!
    NINTER = 0
    DO I1=1,NP
      I2 = MOD(I1,NP) + 1
      IF( XP(I2) < XX .AND. XP(I1) < XX ) CYCLE
      DYI = YP(I2)-YP(I1)
      IF( ABS(DYI) < EPS )                CYCLE
      EI = (  YY  -YP(I1)) / DYI
      IF( EI < 0.D0 .OR. EI >= 1.D0 )     CYCLE
      XI = XP(I1) + EI*(XP(I2)-XP(I1))
      IF( XI < XX )                       CYCLE
      NINTER = NINTER + 1
    ENDDO
!
    INSIDF = MOD(NINTER,2) /= 0
!
 END FUNCTION INSIDF
!
