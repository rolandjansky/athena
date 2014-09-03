      SUBROUTINE UPEVNT
c     routine for Les Houches interface to external pythir routines
c     adapted from base pythia code. 
c     Ian Hinchliffe (May 2002)
c     Adapted for Herwig by Borut Paul Kersevan (February 2003)

c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/lesatlas.inc"
#include "GeneratorFortranCommon/genames.inc"

      IF(ATLASCHOICE.EQ.CHARYBDIS) CALL USECHARYBDIS
      IF(ATLASCHOICE.EQ.ACERMC) THEN
         CALL USEACERMC
         CALL ACERMC_ACSET12_HW(0)
      ENDIF
      IF(ATLASCHOICE.EQ.ALPGEN) CALL USEALPGEN
      IF(ATLASCHOICE.EQ.MADGRAPH) CALL USEMADGRAPH
      IF(ATLASCHOICE.EQ.MADCUP) CALL USEMADCUP
C      IF(ATLASCHOICE.EQ.TOPREX) CALL USETOPREX
      IF(ATLASCHOICE.EQ.LHAEXT) CALL USELHAEXT
      IF(ATLASCHOICE.EQ.MCATNLO) CALL USEMCATNLO
      IF(ATLASCHOICE.EQ.LHEF) CALL USELHEF
      IF(ATLASCHOICE.EQ.HORACE) CALL USEHORACE
      RETURN
      END
