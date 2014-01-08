!
!> \author M.Virchaux
!
 PURE REAL(8) FUNCTION FACPHI( Y , X )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: X, Y
 REAL(8) :: F, RF
 INTEGER :: NF
!
   F = ATAN2( Y , X ) - Pi0Sec
   IF(F < 0.D0) F = F + TwoPi
   NF = F / PiSect + 0.5D0
   RF = 2.D0 * ABS(F-NF*PiSect) / PiSect
   FACPHI = 0.90D0 + 0.20D0 * RF
!
 END FUNCTION FACPHI
!
