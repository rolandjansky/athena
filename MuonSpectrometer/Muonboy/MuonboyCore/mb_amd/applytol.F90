!
 SUBROUTINE ApplyTol(ZI,SI,ZL,SL,TOLZ,TOLS, ZF,SF)
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: ZI,SI, ZL(4),SL(4), TOLZ,TOLS
 REAL(8), INTENT(OUT) :: ZF,SF
 REAL(8) :: ZC,SC
! Apply tolerances on cutout : remaining problem still to be solved for the part of the cutout
! at the border of the station when tol>0.
!
    ZC = SUM( ZL ) / 4.d0
    SC = SUM( SL ) / 4.d0
    ZF = ZI - TOLZ
    IF( ZI > ZC ) ZF = ZI + TOLZ
    SF = SI - TOLS
    IF( SI > SC ) SF = SI + TOLS
!
 END SUBROUTINE ApplyTol
!
