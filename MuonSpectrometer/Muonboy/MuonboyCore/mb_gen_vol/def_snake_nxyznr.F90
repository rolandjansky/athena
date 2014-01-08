!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_SNAKE_NXYZNR( Name, Jcolor, Npt,Xpt,Ypt,Zpt,Nr,Rr )
 IMPLICIT NONE
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Jcolor,Npt,Nr
 REAL(8),           INTENT(IN) :: Xpt(*),Ypt(*),Zpt(*),Rr
!
 REAL(8) :: Ff
!
   Ff = 0.D0
   CALL DEF_SNAKE_NXYZNRF( Name, Jcolor, Npt,Xpt,Ypt,Zpt,Nr,Rr,Ff )
!
 END
