      SUBROUTINE UPVETO(IPVETO)
c     interface to external pythia processes using Les Houches accord
c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
  
#include "GeneratorFortranCommon/lesatlas.inc"
#include "GeneratorFortranCommon/genames.inc"

      IF(ATLASCHOICE.EQ.ALPGEN) CALL VETOALPGEN(IPVETO)
      IF(ATLASCHOICE.EQ.MADGRAPH) CALL VETOMADGRAPH(IPVETO)

      RETURN
      END
