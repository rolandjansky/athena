C-----------------------------------------------------------------------
      SUBROUTINE HWUGUP
C-----------------------------------------------------------------------
C     Subroutine to handle termination of HERWIG if reaches end of event
C     file
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
C--reset the number of events to the correct value
      NEVHEP = NEVHEP-1
C--output information on the events
      CALL HWEFIN
C--run users end code
      CALL HWAEND
C ATLAS/JMB Argh athena doesn't like to STOP!
C These lines should force HWEPRO to stop trying to generate and event
C and terminate.
      IERROR=-1
      GENEV=.TRUE.
C      STOP
      END

