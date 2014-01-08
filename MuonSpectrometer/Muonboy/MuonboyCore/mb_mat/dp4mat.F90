!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE DP4MAT(F0, IXYZ, X0,Y0,Z0, DX,DY,DZ)
 IMPLICIT NONE
 INTEGER :: IXYZ
 REAL(8) :: F0, X0, Y0, Z0, DX, DY, DZ
 REAL(8) :: VV(12)
!
   IF( IXYZ == 1 ) THEN
      VV( 1: 3) = (/ X0+DX, Y0+DY, Z0+DZ /)
      VV( 4: 6) = (/ X0+DX, Y0+DY, Z0-DZ /)
      VV( 7: 9) = (/ X0+DX, Y0-DY, Z0-DZ /)
      VV(10:12) = (/ X0+DX, Y0-DY, Z0+DZ /)
   ELSEIF( IXYZ == 2 ) THEN
      VV( 1: 3) = (/ X0+DX, Y0+DY, Z0+DZ /)
      VV( 4: 6) = (/ X0+DX, Y0+DY, Z0-DZ /)
      VV( 7: 9) = (/ X0-DX, Y0+DY, Z0-DZ /)
      VV(10:12) = (/ X0-DX, Y0+DY, Z0+DZ /)
   ELSEIF( IXYZ == 3 ) THEN
      VV( 1: 3) = (/ X0+DX, Y0+DY, Z0+DZ /)
      VV( 4: 6) = (/ X0+DX, Y0-DY, Z0+DZ /)
      VV( 7: 9) = (/ X0-DX, Y0-DY, Z0+DZ /)
      VV(10:12) = (/ X0-DX, Y0+DY, Z0+DZ /)
   ENDIF
   CALL PLAMATXX( F0, 4, VV )
!
 END
