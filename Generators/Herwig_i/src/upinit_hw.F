      SUBROUTINE UPINIT
c     interface to external pythia processes using Les Houches accord
c     Adapted for pythia 6.203
c     Ian Hinchliffe (may 2002)
c     Adapted for Herwig by Borut Paul Kersevan (February 2003)
c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
#include "GeneratorFortranCommon/heprup.inc"
#include "GeneratorFortranCommon/lesatlas.inc"
#include "GeneratorFortranCommon/genames.inc"

      IF(ATLASCHOICE.EQ.CHARYBDIS) CALL INITCHARYBDIS
      IF(ATLASCHOICE.EQ.ACERMC) THEN
         CALL INITACERMC
         CALL ACERMC_ACSET12_HW(-1)
      ENDIF
      IF(ATLASCHOICE.EQ.ALPGEN) CALL INITALPGEN
      IF(ATLASCHOICE.EQ.MADGRAPH) CALL INITMADGRAPH
      IF(ATLASCHOICE.EQ.MADCUP) CALL INITMADCUP
C      IF(ATLASCHOICE.EQ.TOPREX) CALL INITTOPREX
      IF(ATLASCHOICE.EQ.LHAEXT) CALL INITLHAEXT
      IF(ATLASCHOICE.EQ.MCATNLO) CALL INITMCATNLO
      IF(ATLASCHOICE.EQ.LHEF) CALL INITLHEF
      IF(ATLASCHOICE.EQ.HORACE) CALL INITHORACE
      RETURN
      END
