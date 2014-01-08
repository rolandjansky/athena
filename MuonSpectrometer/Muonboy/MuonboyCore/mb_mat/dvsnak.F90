!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE DVSNAK( NN, XX,YY,ZZ, NR,RR )
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NN, NR
 REAL(8), INTENT(IN) :: XX(NN), YY(NN), ZZ(NN)
 REAL(8), INTENT(IN) :: RR
 INTEGER :: J, NAMATO, I, IP1, IADD(NR+2)
 REAL(8) :: DPHI, ANG, RCOSA, RSINA, AMBDA
 REAL(8) :: XYZ(3,NN)
 REAL(8) :: VEX(3), VEY(3), VEZ(3), VFZ(3), VGZ(3)
 REAL(8) :: VE0(3,NR), VVE(3,NR), VVF(3,NR)
 REAL(8), DIMENSION(3), PARAMETER :: VOY=(/ Zero, One , Zero /)
 REAL(8), DIMENSION(3), PARAMETER :: VOZ=(/ Zero, Zero,  One /)
!
   IF( NN <= 1 .OR. NR <= 2 ) RETURN
   DPHI = TwoPi/NR
   DO J=1,NN
     XYZ(1:3,J) = (/ XX(J), YY(J), ZZ(J) /)
   ENDDO
   NAMATO = NAMATI
!
   VEZ(1:3) = XYZ(1:3,2) - XYZ(1:3,1)
   CALL NORM3V(VEZ)
   IF( ABS(DOT_PRODUCT(VEZ,VOZ)) < 0.999D0 ) THEN
     CALL PVEC3V(VEZ,VOZ, VEX)
     CALL NORM3V(VEX)
   ELSE
     CALL PVEC3V(VOY,VEZ, VEX)
     CALL NORM3V(VEX)
   ENDIF
   CALL PVEC3V(VEZ,VEX, VEY)
   DO I=1,NR
     ANG = (I-1.D0)*DPHI
     RCOSA = RR*COS(ANG)
     RSINA = RR*SIN(ANG)
     VE0(1:3,I) = RCOSA*VEX(1:3) + RSINA*VEY(1:3)
     VVE(1:3,I) = XYZ(1:3,1) + VE0(1:3,I)
   ENDDO
   CALL ADEDMAT( Zero, NR, VVE(1,1) )
!
   DO J=3,NN+1
     IF( J <= NN ) THEN
       VFZ(1:3) = XYZ(1:3,J) - XYZ(1:3,J-1)
       CALL NORM3V(VFZ)
       VGZ(1:3) = VEZ(1:3) + VFZ(1:3)
     ELSE
       VGZ = VEZ
     ENDIF
     DO I=1,NR
       AMBDA = -DOT_PRODUCT(VE0(1:3,I),VGZ) / DOT_PRODUCT(VEZ,VGZ)
       VE0(1:3,I) = VE0(1:3,I)   + AMBDA*VEZ(1:3)
       VVF(1:3,I) = XYZ(1:3,J-1) + VE0(1:3,I)
     ENDDO
     CALL ADEDMAT( Zero, NR, VVF(1,1) )
     VEZ = VFZ
     VVE = VVF
   ENDDO
!
   IADD(1:NR) = (/ (NAMATO + I, I=1,NR) /)
   CALL PLAMATII( NR, IADD )
   DO J=2,NN
     DO I=1,NR
       IP1 = MOD(I,NR) + 1
       IADD(1) = NAMATO + (J-2)*NR + I
       IADD(2) = NAMATO + (J-2)*NR + IP1
       IADD(3) = NAMATO + (J-1)*NR + IP1
       IADD(4) = NAMATO + (J-1)*NR + I
       CALL PLAMATII( 4, IADD )
     ENDDO
   ENDDO
   IADD(1:NR) = (/ (NAMATO + (NN-1)*NR + I, I=1,NR) /)
   CALL PLAMATII( NR, IADD )
!
 END
