!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE TRANSLAT_VOL_XYZ( Name, X,Y,Z )
 IMPLICIT NONE
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 CHARACTER (len=*), INTENT(IN) :: Name
 REAL(8),           INTENT(IN) :: X,Y,Z
 REAL(8) :: VV(3)
!
   VV(1:3) = (/ X, Y, Z /)
   CALL TRANSLAT_VOL( Name, VV )
!
 END
