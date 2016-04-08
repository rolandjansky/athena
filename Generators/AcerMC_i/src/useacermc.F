      SUBROUTINE USEACERMC
C-------------------------------------------------------------C
C    ACERMC 3.x - PYTHIA 6.3 INTERFACE                        C
C    Borut Paul Kersevan (January 2006)                       C
C-------------------------------------------------------------C
c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"

      CHARACTER EVTFIL*140
      COMMON /ACFIL/EVTFIL

CC COMMUNICATE HERWIG STATUS
      COMMON /EVTCON/ ISTG

C UNWEIGHTED EVENTS
      XWGTUP=1.D0

      IDPROC=LPRUP(NPRUP)-600

CC READ EVENT FILE -------------------------------------------
      IUNIT=22
      READ (IUNIT,*,ERR=200,END=100) NUP
      IF (IDPROC.EQ.17.OR.IDPROC.EQ.18
     &     .OR.IDPROC.EQ.20.OR.IDPROC.EQ.103) THEN
         READ(IUNIT,*) XWGTUP
      ENDIF
      READ (IUNIT,*,ERR=200,END=100) SCALUP,AQEDUP,AQCDUP
      READ (IUNIT,*,ERR=200,END=100) (IDUP(I),ISTUP(I),
     &MOTHUP(1,I),ICOLUP(1,I),MOTHUP(2,I),ICOLUP(2,I),
     &(PUP(J,I),J=1,5),VTIMUP(I),SPINUP(I),I=1,NUP)
CC -----------------------------------------------------------


C PROCESS ID FOR EVENT
      IDPRUP=LPRUP(NPRUP)

      ISTG=0
      RETURN

C RETURN WITH ZERO PARTICLES AT THE END OF THE RECORD
 100  NUP=0
      ISTG=1
      RETURN

C ERROR HANDLING
 200  WRITE (*,*) 'USEACERMC: event data file:'
      WRITE (*,*) '  ',EVTFIL
      WRITE (*,*) 'could not be read         !'
      STOP

      END
