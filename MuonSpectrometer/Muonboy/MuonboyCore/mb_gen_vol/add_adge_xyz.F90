!
 SUBROUTINE ADD_ADGE_XYZ(X0,Y0,Z0)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: X0, Y0, Z0
!
   Nadge = Nadge + 1
   XYZadge(1,Nadge) = X0
   XYZadge(2,Nadge) = Y0
   XYZadge(3,Nadge) = Z0
!
 END SUBROUTINE ADD_ADGE_XYZ
!
