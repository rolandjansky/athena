!
 SUBROUTINE SWAP_ZT( Z , T )
 IMPLICIT NONE
 REAL(8), INTENT(INOUT) :: Z,T
 REAL(8) :: Zbuf
!
    Zbuf = Z
    Z    = T
    T    = Zbuf
!
 END SUBROUTINE SWAP_ZT
!
