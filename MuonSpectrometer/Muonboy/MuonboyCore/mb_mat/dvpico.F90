!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE DVPICO(F0, NN, ADEB,AFIN, XC1,YC1,ZC1,RO1,RI1,DIO1,  &
                                      XC2,YC2,ZC2,RO2,RI2,DIO2 )
 USE M_MB_General_CONSTANTS, Epsil=>Eps  ! EPS deja utilise
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: NN
 REAL(8) :: F0, ADEB,AFIN, XC1,YC1,ZC1,RO1,RI1,DIO1, XC2,YC2,ZC2,RO2,RI2,DIO2
 INTEGER :: I, L, LP1, IADD(4), NAMATO
 REAL(8) :: PHI0, PHI1, DPHI, ANG, CI, SI
 REAL(8) :: VV(3,5), VEX(3), VEY(3), VEZ(3)
 REAL(8) :: XYZC1(3), XYZC2(3)
 REAL(8), PARAMETER :: EPS=0.000001D0
 REAL(8), DIMENSION(3), PARAMETER :: VOY=(/ Zero, One , Zero /)
 REAL(8), DIMENSION(3), PARAMETER :: VOZ=(/ Zero, Zero,  One /)
!
   IF( RI1 < 0.01D0 .OR. RI2 < 0.01D0 ) RETURN
   PHI0 = PiS180*ADEB
   PHI1 = PiS180*AFIN
   IF( PHI1 < PHI0 ) PHI1 = PHI1 + TwoPi
   DPHI = (PHI1-PHI0)/NN
!
   XYZC1(1:3) = (/ XC1, YC1, ZC1 /)
   XYZC2(1:3) = (/ XC2, YC2, ZC2 /)
   VEZ(1:3) = XYZC2(1:3) - XYZC1(1:3)
   CALL NORM3V(VEZ)
   IF( ABS(DOT_PRODUCT(VEZ,VOZ)) < 0.999D0 ) THEN
     CALL PVEC3V(VEZ,VOZ,VEX)
   ELSE
     CALL PVEC3V(VOY,VEZ,VEX)
   ENDIF
   CALL NORM3V(VEX)
   CALL PVEC3V(VEZ,VEX,VEY)
   NAMATO = NAMATI
!
   DO I=0,NN
     ANG = PHI0 + I*DPHI
     CI = COS(ANG)
     SI = SIN(ANG)
     VV(1:3,1) = XYZC1(1:3) + RI1*CI*VEX(1:3) + RI1*SI*VEY(1:3)
     VV(1:3,2) = XYZC1(1:3) + RO1*CI*VEX(1:3) + RO1*SI*VEY(1:3) + DIO1*VEZ(1:3)
     VV(1:3,3) = XYZC2(1:3) + RO2*CI*VEX(1:3) + RO2*SI*VEY(1:3) + DIO2*VEZ(1:3)
     VV(1:3,4) = XYZC2(1:3) + RI2*CI*VEX(1:3) + RI2*SI*VEY(1:3)
     CALL ADEDMAT( F0, 4, VV(1,1) )
   ENDDO
!
   DO I=1,NN
     DO L=1,4
       LP1 = MOD(L,4) + 1
       IADD(1) = NAMATO + (I-1)*4 + L
       IADD(2) = NAMATO +  I   *4 + L
       IADD(3) = NAMATO +  I   *4 + LP1
       IADD(4) = NAMATO + (I-1)*4 + LP1
       CALL PLAMATII( 4, IADD )
     ENDDO
   ENDDO
   IF( ABS(PHI1-PHI0-TwoPi) > EPS ) THEN
     IADD(1:4) = (/ (NAMATO + L, L=1,4) /)
     CALL PLAMATII( 4, IADD )
     IADD(1:4) = (/ (NAMATO + NN*4 + L, L=1,4) /)
     CALL PLAMATII( 4, IADD )
   ENDIF
!
 END SUBROUTINE DVPICO
!
