      SUBROUTINE UPINIT
c     interface to external pythia processes using Les Houches accord
c     Adapted for pythia 6.203
c     Ian Hinchliffe (may 2002)
c 
c #include "GeneratorFortranCommon/GeneratorName.h" 

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

#include "GeneratorFortranCommon/heprup.inc"
#include "GeneratorFortranCommon/lesatlas.inc"
#include "GeneratorFortranCommon/genames.inc"



      IF(ATLASCHOICE.EQ.COMPHEP) CALL INITCOMPHEP
      IF(ATLASCHOICE.EQ.USER) CALL INITUSER
      IF(ATLASCHOICE.EQ.ACERMC) THEN
         CALL INITACERMC
         CALL ACERMC_ACSET12_PY(-1)
      ENDIF         
      IF(ATLASCHOICE.EQ.ALPGEN) CALL INITALPGEN
      IF(ATLASCHOICE.EQ.MADGRAPH) CALL INITMADGRAPH
      IF(ATLASCHOICE.EQ.MADCUP) CALL INITMADCUP
      IF(ATLASCHOICE.EQ.LHAEXT) CALL INITLHAEXT
      IF(ATLASCHOICE.EQ.MATCHIG) CALL INITMATCHIG
      IF(ATLASCHOICE.EQ.HVGEN) CALL INITHVGEN
      IF(ATLASCHOICE.EQ.LHEF) CALL INITLHEF
      IF(ATLASCHOICE.EQ.EXOGRAVITON) CALL INITGRAV
      IF(ATLASCHOICE.EQ.GRAVADD) CALL INITGRAVADD
      RETURN
      END
