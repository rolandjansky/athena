CDECK  ID>, CIRCES.
*CMZ :-        -03/07/01  17.07.47  by  Bryan Webber
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CIRCES (XX1M, XX2M, XROOTS, XACC, XVER, XREV, XCHAT)
C-----------------------------------------------------------------------
C     DUMMY SUBROUTINE: DELETE AND SET CIRCOP NON-ZERO
C     IN MAIN PROGRAM IF YOU USE CIRCE BEAM SPECTRUM PACKAGE
C-----------------------------------------------------------------------
      DOUBLE PRECISION XX1M, XX2M, XROOTS
      INTEGER XACC, XVER, XREV, XCHAT
      WRITE (6,10)
   10 FORMAT(/10X,'CIRCES CALLED BUT NOT LINKED')
      STOP
      END
