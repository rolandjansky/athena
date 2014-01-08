!
!> \author SamuSog
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE MULT3M3M(ATTA,ATTB, ATTC)
!
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: ATTA(3,3), ATTB(3,3)
 REAL(8), INTENT(OUT) :: ATTC(3,3)
!
 INTEGER :: I
!
    DO I=1,3
      ATTC(I,1:3) = ATTA(1,1:3)*ATTB(I,1) + ATTA(2,1:3)*ATTB(I,2) + ATTA(3,1:3)*ATTB(I,3)
    ENDDO
!
 END
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE MULT3M3V(ATT,VVA, VVB)
!
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: ATT(3,3), VVA(3)
 REAL(8), INTENT(OUT) :: VVB(3)
!
    VVB(1:3) = ATT(1,1:3)*VVA(1) + ATT(2,1:3)*VVA(2) + ATT(3,1:3)*VVA(3)
!
 END
!
