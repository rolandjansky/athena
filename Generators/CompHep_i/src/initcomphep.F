      SUBROUTINE INITCOMPHEP
c     interface to external pythia processes using Les Houches accord 
c hep-ph/0109068

c     Adapted for pythia 6.203
c     Ian Hinchliffe (may 2002)
c      Based on code by
c*************************************************************************
c*                          UPINIT routine                               *
c*          for CompHEP 4.4.3  -  PYTHIA 6.2 interface                   *
c*************************************************************************
c*  25.11.2001         Ilyin V.A.                                        *
c*  27.03.2006 Modified for version 4.4.3 by Vikas Bansal                *
c
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
C...User process initialization commonblock.
#include "GeneratorFortranCommon/heprup.inc"
*...CompHEP-PYTHIA interface
      CHARACTER BEAM*16
*      COMMON/BEAMS/SQRTS,RPDTY,KFB(2),BMASS(2),BEAM(2)
      COMMON/BEAMS/BENERGY(2),KFB(2),BMASS(2),BEAM(2)
      COMMON/SUBPROC/NUP(100),KFP(100,20),SIGMAERR(100),IDPR(100),
     .        NCUT(100),NEV(100),XM(100,20,3),SIGMA(100),NPROC
      RPDTY = 0d0
      SQRTS = BENERGY(1) + BENERGY(2)

*...PDG codes for beams
      IDBMUP(1)=KFB(1)     
      IDBMUP(2)=KFB(2)     
*...energies of beams (in PYTHI beams are massless !)
      EBMUP(1) = SQRTS/2d0*(COSH(RPDTY)+SINH(RPDTY))
      EBMUP(2) = SQRTS/2d0*(COSH(RPDTY)-SINH(RPDTY))
*...PDFLIB group and set of the parton distributions
***     already done
*...events weighting
      IDWTUP = 3     !unweighted input
*...number of subprocesses in input
      NPRUP = nproc
*...cross section for each subprocess
      DO 100 i=1,NPRUP
       XSECUP(i) = SIGMA(i)
100   CONTINUE
*...statistcal error on the cross sections
      DO 110 i=1,NPRUP
       XERRUP(i) = SIGMAERR(i)
110   CONTINUE
*...maximum event weight
      DO 120 i=1,NPRUP
       XMAXUP(i) = 1.d0
120   CONTINUE
*...unique integer identifier for each subprocess
      DO 130 i=1,NPRUP
       LPRUP(i) = IDPR(i)
130   CONTINUE
 
      RETURN
      END
