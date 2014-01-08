!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_SPHERE_RN1N2( Name, Jcolor, Radius,N1,N2 )
 IMPLICIT NONE
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Jcolor,N1,N2
 REAL(8),           INTENT(IN) :: Radius
!
 REAL(8) :: RadXYZ(3)
!
   RadXYZ(1:3) = Radius
   CALL DEF_ELIPSO_RXYZN1N2( Name, Jcolor, RadXYZ,N1,N2 )
!
 END
