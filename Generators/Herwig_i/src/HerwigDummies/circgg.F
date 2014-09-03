CDECK  ID>, CIRCGG.
*CMZ :-        -03/07/01  17.07.47  by  Bryan Webber
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      FUNCTION CIRCGG (X1, X2)
C-----------------------------------------------------------------------
C     DUMMY FUNCTION: DELETE AND SET CIRCOP NON-ZERO
C     IN MAIN PROGRAM IF YOU USE CIRCE BEAM SPECTRUM PACKAGE
C-----------------------------------------------------------------------
      DOUBLE PRECISION CIRCGG, X1, X2
      WRITE (6,10)
   10 FORMAT(/10X,'CIRCGG CALLED BUT NOT LINKED')
      CIRCGG = 0.0D0
      STOP
      END
