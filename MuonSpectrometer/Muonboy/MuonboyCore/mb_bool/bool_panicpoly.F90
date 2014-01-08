!
!> \author SaMuSoG
!
 SUBROUTINE Bool_PanicPoly(AA,II,NN,JJT,XXT,YYT)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN) :: AA
 INTEGER     , INTENT(IN) :: II,NN
 INTEGER     , INTENT(IN) :: JJT(*)
 REAL(8)     , INTENT(IN) :: XXT(*),YYT(*)
!
   WRITE(*,'(1X,A1,I4,I10)') AA,II,NN
   WRITE(*,'(12I10)')        JJT(1:NN)
   WRITE(*,'(12F10.2)')      XXT(1:NN)
   WRITE(*,'(12F10.2)')      YYT(1:NN) 
!
 END SUBROUTINE Bool_PanicPoly
!
