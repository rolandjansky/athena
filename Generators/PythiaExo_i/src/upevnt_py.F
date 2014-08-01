      SUBROUTINE UPEVNT
c     routine for Les Houches interface to external pythia routines
c     adapted from base pythia code. 
c     Ian Hinchliffe (May 2002)
c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/lesatlas.inc"
#include "GeneratorFortranCommon/genames.inc"
      IF(ATLASCHOICE.EQ.COMPHEP) CALL USECOMPHEP
      IF(ATLASCHOICE.EQ.USER) CALL USEUSER
      IF(ATLASCHOICE.EQ.ACERMC) THEN
         CALL USEACERMC
         CALL ACERMC_ACSET12_PY(0)
      ENDIF
      IF(ATLASCHOICE.EQ.ALPGEN) CALL USEALPGEN
      IF(ATLASCHOICE.EQ.MADGRAPH) CALL USEMADGRAPH
      IF(ATLASCHOICE.EQ.MADCUP) CALL USEMADCUP
      IF(ATLASCHOICE.EQ.LHAEXT) CALL USELHAEXT
      IF(ATLASCHOICE.EQ.MATCHIG) CALL USEMATCHIG
      IF(ATLASCHOICE.EQ.HVGEN) CALL USEHVGEN
      IF(ATLASCHOICE.EQ.LHEF) CALL USELHEF
      IF(ATLASCHOICE.EQ.EXOGRAVITON) CALL USEGRAV
      IF(ATLASCHOICE.EQ.GRAVADD) CALL USEGRAVADD
      RETURN
      END
