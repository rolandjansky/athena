!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE DPCMAT(F0, IXYZ, X10,Y10,Z10, DX1,DY1,DZ1, X20,Y20,Z20, DX2,DY2,DZ2 )
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: IXYZ
 REAL(8) :: F0, X10,Y10,Z10, DX1,DY1,DZ1, X20,Y20,Z20, DX2,DY2,DZ2
 INTEGER :: LL3, NN3, NADD, I3, JA, JJ
 INTEGER :: JJ3(20), IADD(20)
 INTEGER :: NN3DIM(2)
 REAL(8) :: DIFF
 REAL(8) :: VVV(3,20)
 REAL(8) :: UU1(4),UU2(4),UU3(20)
 REAL(8) :: VV1(4),VV2(4),VV3(20)
 INTEGER, DIMENSION(4), PARAMETER :: JJ1 = (/ 1, 1, 1, 1 /), JJ2 = (/ 1, 1, 1, 1 /)
 REAL(8), PARAMETER :: EPS = 0.000001D0
!
   IF( IXYZ == 1 ) THEN
      UU1(1:4) = (/ Y10+DY1, Y10+DY1, Y10-DY1, Y10-DY1 /)
      VV1(1:4) = (/ Z10+DZ1, Z10-DZ1, Z10-DZ1, Z10+DZ1 /)
      UU2(1:4) = (/ Y20+DY2, Y20+DY2, Y20-DY2, Y20-DY2 /)
      VV2(1:4) = (/ Z20+DZ2, Z20-DZ2, Z20-DZ2, Z20+DZ2 /)
   ELSEIF( IXYZ == 2 ) THEN
      UU1(1:4) = (/ X10+DX1, X10+DX1, X10-DX1, X10-DX1 /)
      VV1(1:4) = (/ Z10+DZ1, Z10-DZ1, Z10-DZ1, Z10+DZ1 /)
      UU2(1:4) = (/ X20+DX2, X20+DX2, X20-DX2, X20-DX2 /)
      VV2(1:4) = (/ Z20+DZ2, Z20-DZ2, Z20-DZ2, Z20+DZ2 /)
   ELSEIF( IXYZ == 3 ) THEN
      UU1(1:4) = (/ X10+DX1, X10+DX1, X10-DX1, X10-DX1 /)
      VV1(1:4) = (/ Y10+DY1, Y10-DY1, Y10-DY1, Y10+DY1 /)
      UU2(1:4) = (/ X20+DX2, X20+DX2, X20-DX2, X20-DX2 /)
      VV2(1:4) = (/ Y20+DY2, Y20-DY2, Y20-DY2, Y20+DY2 /)
   ENDIF
!
   CALL CUTOUT( 4,JJ1,UU1,VV1, 4,JJ2,UU2,VV2, 20, 1, LL3,NN3DIM,JJ3,UU3,VV3 )
   NN3 = NN3DIM(1)
!
   NADD = 0
   DO I3=1,NN3
    DO JA=1,NADD
     DIFF = (UU3(I3)-UU3(JA))**2 + (VV3(I3)-VV3(JA))**2
     IF( DIFF < EPS ) GO TO 40
    ENDDO
       NADD = NADD + 1
       JA = NADD
40  UU3(JA) = UU3(I3)
    VV3(JA) = VV3(I3)
    IADD(I3) = JA + NAMATI
   ENDDO
!
   IF( IXYZ == 1 ) THEN
      DO JJ=1,NADD
       VVV(1:3,JJ) = (/ X10+DX1, UU3(JJ), VV3(JJ) /)
      ENDDO
   ELSEIF( IXYZ == 2 ) THEN
      DO JJ=1,NADD
       VVV(1:3,JJ) = (/ UU3(JJ), Y10+DY1, VV3(JJ) /)
      ENDDO
   ELSEIF( IXYZ == 3 ) THEN
      DO JJ=1,NADD
       VVV(1:3,JJ) = (/ UU3(JJ), VV3(JJ), Z10+DZ1 /)
      ENDDO
   ENDIF
!
   CALL ADEDMAT( F0, NADD, VVV(1,1) )
   CALL PLAMATII( NN3, IADD )
!
 END
