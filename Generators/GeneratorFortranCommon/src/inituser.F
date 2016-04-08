      SUBROUTINE INITUSER
c     interface to external pythia processes using Les Houches accord
c     Adapted for pythia 6.203
c     Ian Hinchliffe (may 2002)
c
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

#include "GeneratorFortranCommon/heprup.inc"

      print *, "THE DUMMY GeneratorFortranCommon/inituser routine called"

      RETURN
      END
