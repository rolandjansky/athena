      SUBROUTINE EXTPROC(IPROC)
c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
#include "GeneratorFortranCommon/lesatlas.inc"
      ATLASCHOICE = IPROC
      RETURN
      END
