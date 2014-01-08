!
! Author : M.Virchaux
!
 SUBROUTINE DVRRZZ( F1,F2,NF, NN, RR,ZZ )
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NF, NN
 REAL(8), INTENT(IN) :: F1, F2
 REAL(8), INTENT(IN) :: RR(*), ZZ(*)
 INTEGER :: LF, I, J, IP1, NAMATO
 INTEGER :: IADD(NN+2)
 REAL(8) :: DF, FF
 REAL(8) :: UU(3,NN)
!
   IF( NF <= 1 .OR. NN <= 2 ) RETURN
!
   UU(1,1:NN) = RR(1:NN)
   UU(2,1:NN) = Zero
   UU(3,1:NN) = ZZ(1:NN)
!
   DF = F2 - F1
   IF( DF < 0.D0 ) DF = DF + TwoPi
   DF = DF / NF
   NAMATO = NAMATI
!
   DO LF=0,NF
     FF = F1 + LF*DF
     CALL ADEDMAT( FF, NN, UU )
   ENDDO
!
   IADD(1:NN) = (/ (NAMATO + I, I=1,NN) /)
   CALL PLAMATII( NN, IADD )
   IADD(1:NN) = (/ (NAMATO + NF*NN + I, I=1,NN) /)
   CALL PLAMATII( NN, IADD )
   DO J=1,NF
     DO I=1,NN
       IP1 = MOD(I,NN) + 1
       IADD(1) = NAMATO + (J-1)*NN + I
       IADD(2) = NAMATO +  J   *NN + I
       IADD(3) = NAMATO +  J   *NN + IP1
       IADD(4) = NAMATO + (J-1)*NN + IP1
       CALL PLAMATII( 4, IADD )
     ENDDO
   ENDDO
!
 END SUBROUTINE DVRRZZ
!
