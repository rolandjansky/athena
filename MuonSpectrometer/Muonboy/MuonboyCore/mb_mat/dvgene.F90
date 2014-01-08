!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE DVGENE(F0, NN, VV, IXYZ, A1,A2)
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: NN, IXYZ
 REAL(8) :: F0, A1, A2
 REAL(8) :: VV(3*NN)
 INTEGER :: IXYZ2, IXYZ3, NADD, I, JA
 INTEGER :: IADD(NN)
 REAL(8) :: DIFF
 REAL(8) :: VVA(3*NN)
 REAL(8), PARAMETER :: EPS = 0.000001D0
!
   IXYZ2 = MOD(IXYZ  ,3) + 1
   IXYZ3 = MOD(IXYZ+1,3) + 1
!
   NADD = 0
   DO I=1,NN
     DO JA=1,NADD
       DIFF = ( VV(3*I-3+IXYZ2) - VVA(3*JA-3+IXYZ2) )**2  &
            + ( VV(3*I-3+IXYZ3) - VVA(3*JA-3+IXYZ3) )**2
       IF( DIFF <  EPS ) GO TO 100
     ENDDO
     NADD = NADD + 1
     VVA(3*NADD-3+IXYZ ) = A1
     VVA(3*NADD-3+IXYZ2) = VV(3*I-3+IXYZ2)
     VVA(3*NADD-3+IXYZ3) = VV(3*I-3+IXYZ3)
     JA = NADD
100  IADD(I) = NAMATI + JA
   ENDDO
   CALL ADEDMAT( F0, NADD, VVA )
   CALL PLAMATII( NN, IADD )
!
   DO JA=1,NADD
     VVA(3*JA-3+IXYZ ) = A2
   ENDDO
   IADD(1:NN) = IADD(1:NN) + NADD
   CALL ADEDMAT( F0, NADD, VVA )
   CALL PLAMATII( NN, IADD )
!
   CALL CLOMAT
!
 END
