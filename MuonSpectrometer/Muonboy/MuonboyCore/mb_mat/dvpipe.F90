!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE DVPIPE(F0, NN, ADEB,AFIN, XC1,YC1,ZC1,RO1,RI1, XC2,YC2,ZC2,RO2,RI2 )
 USE M_MB_General_CONSTANTS, Epsil=>Eps  ! EPS deja utilise
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: NN
 REAL(8) :: F0, ADEB,AFIN, XC1,YC1,ZC1,RO1,RI1, XC2,YC2,ZC2,RO2,RI2
 INTEGER :: ICLOSE, J, I, L
 INTEGER :: IADD(2*NN+2)
 REAL(8) :: PHI0, PHI1, DPHI, ANG, RCOSA, RSINA
 REAL(8) :: VV(6*NN+6)
 REAL(8) :: VEX(3),VEY(3),VEZ(3)
 REAL(8) :: XYZC(3,2),RO(2),RI(2)
 REAL(8), DIMENSION(3), PARAMETER :: VOY=(/ Zero, One , Zero /)
 REAL(8), DIMENSION(3), PARAMETER :: VOZ=(/ Zero, Zero,  One /)
 REAL(8), PARAMETER :: EPS=0.0001D0
!
   IF( RI1 >= RO1 .OR. RI2 >= RO2 ) RETURN
!
   PHI0 = PiS180*ADEB
   PHI1 = PiS180*AFIN
   IF( PHI1 < PHI0 ) PHI1 = PHI1 + TwoPi
   DPHI = (PHI1-PHI0)/NN
   IF( PHI1-PHI0 > 0.9999D0*TwoPi ) THEN
     IF( RI1 < EPS .AND. RI2 < EPS ) THEN
       ICLOSE = 2
     ELSEIF( RI1 < EPS .OR. RI2 < EPS ) THEN
       RETURN
     ELSE
       ICLOSE = 1
     ENDIF
   ELSE
     IF( RI1 < EPS .OR. RI2 < EPS ) RETURN
     ICLOSE = 0
   ENDIF
!
   XYZC(1:3,1) = (/ XC1, YC1, ZC1 /)
   XYZC(1:3,2) = (/ XC2, YC2, ZC2 /)
   RO(1:2)     = (/ RO1, RO2 /)
   RI(1:2)     = (/ RI1, RI2 /)
   VEZ(1:3) =  XYZC(1:3,2) -XYZC(1:3,1)
   CALL NORM3V(VEZ)
   IF( ABS(DOT_PRODUCT(VEZ,VOZ)) < 0.999D0 ) THEN
     CALL PVEC3V(VEZ,VOZ,VEX)
   ELSE
     CALL PVEC3V(VOY,VEZ,VEX)
   ENDIF
   CALL NORM3V(VEX)
   CALL PVEC3V(VEZ,VEX,VEY)
!
!-----------------------------------------------------
   DO J=1,2
!
     DO I=1,NN+1
       ANG = PHI0 + (I-1.D0)*DPHI
       RCOSA = RO(J)*COS(ANG)
       RSINA = RO(J)*SIN(ANG)
       DO L=1,3
         VV(3*I-3+L) = XYZC(L,J) + RCOSA*VEX(L) + RSINA*VEY(L)
       ENDDO
       IADD(I) = NAMATI + I
     ENDDO
     IF( ICLOSE >= 1 ) THEN
       IADD(NN+1) = NAMATI + 1
       CALL ADEDMAT( F0, NN, VV )
     ELSE
       CALL ADEDMAT( F0, NN+1, VV)
     ENDIF
!
     IF( ICLOSE == 2 ) THEN
       CALL PLAMATII( NN, IADD )
     ELSE
       DO I=1,NN+1
         ANG = PHI1 - (I-1.D0)*DPHI
         RCOSA = RI(J)*COS(ANG)
         RSINA = RI(J)*SIN(ANG)
         DO L=1,3
           VV(3*I-3+L) = XYZC(L,J) + RCOSA*VEX(L) + RSINA*VEY(L)
         ENDDO
         IADD(NN+1+I) = NAMATI + I
       ENDDO
       IF( ICLOSE >= 1 ) THEN
         IADD(2*NN+2) = NAMATI + 1
         CALL ADEDMAT( F0, NN, VV )
       ELSE
         CALL ADEDMAT( F0, NN+1, VV)
       ENDIF
       CALL PLAMATII( 2*NN+2, IADD )
     ENDIF
!
   ENDDO
!-----------------------------------------------------
!
   CALL CLOMAT
!
  END
