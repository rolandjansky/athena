!
!> Parametrization of the radius dependance of the resolution function in cm
!
 REAL(8) FUNCTION fredig(rr)
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: rr                 !< radius in cm
 INTEGER, PARAMETER :: n  = 4
!REAL(8), PARAMETER :: c0 = 0.0080         !< constant term, not used
 REAL(8), PARAMETER :: c1 = 1.3d0          !< radius where resolution minimale
 REAL(8), PARAMETER :: c2 = 0.0180 / c1**n !< additionnal contribution at RRR=0
   fredig = c2 * (rr-c1)**n
 END FUNCTION fredig
!
