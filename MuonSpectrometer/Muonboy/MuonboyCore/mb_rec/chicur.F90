!
!> \author M.Virchaux
!
 PURE REAL(8) FUNCTION CHICUR(X,C)
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: X, C
!
   IF(X <= C) THEN
     CHICUR = X
   ELSE
     CHICUR = 2.D0*C - C**2 / X
   ENDIF
!
 END FUNCTION CHICUR
!
