!
!> \author SaMuSoG
!
 SUBROUTINE Bool_Coo_Edg_Sys( Ndim, Nedge,Ptxyz, Sys, Pcoo )
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ndim, Nedge
 REAL(8), INTENT(IN) :: Ptxyz(3,Ndim)
 REAL(8), INTENT(IN) :: Sys(3,0:3)
 REAL(8) :: Pcoo(Ndim,3)
 INTEGER :: IE
 REAL(8) :: VV(3)
!
   DO IE=1,Nedge
     VV(1:3) = Ptxyz(1:3,IE) - Sys(1:3,0)
     Pcoo(IE,1) = SUM( VV(1:3)*Sys(1:3,1) )
     Pcoo(IE,2) = SUM( VV(1:3)*Sys(1:3,2) )
     Pcoo(IE,3) = SUM( VV(1:3)*Sys(1:3,3) )
   ENDDO
!
 END SUBROUTINE Bool_Coo_Edg_Sys
!
