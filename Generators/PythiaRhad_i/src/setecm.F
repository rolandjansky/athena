      SUBROUTINE SETECM(WINMATCHIG)
c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
#include "GeneratorFortranCommon/heprup.inc"
      EBMUP(1)=0.5*WINMATCHIG
      EBMUP(2)=0.5*WINMATCHIG

      RETURN
      END
