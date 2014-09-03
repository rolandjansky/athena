CDECK  ID>, DECADD.
*CMZ :-        -28/01/92  12.34.44  by  Mike Seymour
*-- Author :    Luca Stanco
C-----------------------------------------------------------------------
      SUBROUTINE DECADD(LOGI)
C-----------------------------------------------------------------------
C     DUMMY SUBROUTINE: DELETE AND SET BDECAY='CLEO'
C     IN MAIN PROGRAM IF YOU USE CLEO DECAY PACKAGE
C-----------------------------------------------------------------------
      LOGICAL LOGI
      WRITE (6,10)
   10 FORMAT(/10X,'DECADD CALLED BUT NOT LINKED')
      STOP
      END
