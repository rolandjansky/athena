      SUBROUTINE INITACERMC
C-------------------------------------------------------------C
C    ACERMC 3.x - PYTHIA 6.3 INTERFACE                        C
C    Borut Paul Kersevan (January 2006)                       C
C-------------------------------------------------------------C

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
#include "GeneratorFortranCommon/heprup.inc"

      CHARACTER DUMM*200,EVTFIL*140
      COMMON /ACFIL/EVTFIL

CC OPEN HEADER FILE ------------------------------------------

      OPEN (21,FILE='inparmAcerMC.dat',STATUS='OLD',ERR=100)
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(60x,6x,I4)') IDPROC ! TEXT 
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(A200)') DUMM ! TEXT 
      READ(21,FMT='(30x,I6,I6)') IDBMUP(1),IDBMUP(2) ! BEAM CODE
      READ(21,FMT='(30x,F12.2)') EBMUP(1) ! BEAM ENERGY
      EBMUP(2)=EBMUP(1)
      READ(21,FMT='(30x,I6,I6)')PDFGUP(1),PDFSUP(1) ! PDF SET
      PDFGUP(2)=PDFGUP(1)
      PDFSUP(2)=PDFSUP(1)
      READ(21,FMT='(30x,A140)') EVTFIL ! EVENT FILE NAME
      READ(21,FMT='(A200)') DUMM ! NO OF STORED EVENTS - TREATED AS DUMMY
      READ(21,FMT='(30x,D15.6,5x,D15.6)') XSECUP(1),XERRUP(1) ! X-SEC INFO
      CLOSE(21)

C Fix pdf in HERWIG
      IF (PDFSUP(1).GT.100) THEN
         PDFGUP(1)=-1
         PDFGUP(2)=-1
      ENDIF

C DECIDE ON WEIGHTING STRATEGY: UNWEIGHTED ON INPUT +/-1 WEIGHT
      IDWTUP=-3

C PROCESS TAGS
      NPRUP=1
      LPRUP(NPRUP)=600+IDPROC
      XMAXUP(NPRUP)=1.D0

CC -----------------------------------------------------------


CC OPEN EVENT FILE -------------------------------------------
      IUNIT=22
      OPEN(IUNIT,FILE=EVTFIL,STATUS='OLD',ERR=200)
CC -----------------------------------------------------------

      RETURN

C ERROR HANDLING
 100  WRITE (*,*)
     #     'INITACERMC: inparmAcerMC.dat file could not be opened!'
      WRITE (*,*)'Please link an *.inparm file to inparmAcerMC.dat!'
      STOP
C ERROR HANDLING
 200  WRITE (*,*) 'INITCERMC: Missing event data file:',
     # ' ',EVTFIL
      STOP

      END
