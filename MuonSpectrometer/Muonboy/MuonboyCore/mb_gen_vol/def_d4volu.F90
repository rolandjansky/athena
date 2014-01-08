!
 SUBROUTINE DEF_D4Volu(D1,D2,D3,D4)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: D1,D2,D3,D4
!
   D4Volu(  1 ,Nvolu) = D1
   IF( M4 < 2 ) RETURN
   D4Volu(  2 ,Nvolu) = D2
   IF( M4 < 3 ) RETURN
   D4Volu(  3 ,Nvolu) = D3
   IF( M4 < 4 ) RETURN
   D4Volu(4:M4,Nvolu) = D4
!
 END SUBROUTINE DEF_D4Volu
!
