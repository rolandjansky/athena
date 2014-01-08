!
!> Tells (quickly) wether point  XX,YY  is inside (Insi=1) ,
!> just at the limit (Insi=0)
!> or outside (Insi=-1)
!> the polygon made of the  NP  points  XP(i),YP(i)
!
 SUBROUTINE Bool_Is_It_In(XX,YY,NP,XP,YP, Insi)
 USE M_MB_General_CONSTANTS, ONLY : EPSBOO
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: NP
 REAL(8), INTENT(IN)  :: XX,YY
 REAL(8), INTENT(IN)  :: XP(*),YP(*)
 INTEGER, INTENT(OUT) :: Insi
 INTEGER :: I1,I2,NINTER
 REAL(8) :: VX,VY,VV,XREL,YREL,YP1,YP2,DYI,EI,XI
 REAL(8), PARAMETER :: ALMOST0=0.000001D0
!
!>> Outside ****
   Insi = -1
!>> ------- ****
   IF( NP <= 2 ) RETURN
!
   DO I1=1,NP
     I2 = MOD(I1,NP) + 1
     VX = XP(I2) - XP(I1)
     VY = YP(I2) - YP(I1)
     VV = SQRT( VX**2 + VY**2 )
     XREL = (   (XX-XP(I1))*VX + (YY-YP(I1))*VY ) / VV
     YREL = ( - (XX-XP(I1))*VY + (YY-YP(I1))*VX ) / VV
     IF(   -EPSBOO <= XREL .AND. XREL < VV+EPSBOO .AND.  &
           -EPSBOO <= YREL .AND. YREL < EPSBOO    ) THEN
!>> At the limit ***
       Insi =  0
!>> ------------ ***
       RETURN
     ENDIF
   ENDDO
!
   NINTER = 0
   DO I1=1,NP
     I2 = MOD(I1,NP) + 1
     IF( XP(I2) < XX .AND. XP(I1) < XX ) CYCLE
     YP1 = YP(I1)
     IF( ABS(YY-YP1) < EPSBOO ) YP1 = YY + EPSBOO
     YP2 = YP(I2)
     IF( ABS(YY-YP2) < EPSBOO ) YP2 = YY + EPSBOO
     DYI = YP2-YP1
     IF( ABS(DYI) < ALMOST0 )            CYCLE
     EI = (YY-YP1) / DYI
     IF( EI < 0.D0 .OR. EI >= 1.D0 )     CYCLE
     XI = XP(I1) + EI*(XP(I2)-XP(I1))
     IF( XI < XX )                       CYCLE
     NINTER = NINTER + 1
   ENDDO
!
!>> -------------------------- Inside ***
   IF( MOD(NINTER,2) /= 0 ) Insi =  1
!>> --------------------------------- ***
!
 END SUBROUTINE Bool_Is_It_In
!
