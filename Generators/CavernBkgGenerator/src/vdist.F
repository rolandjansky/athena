      FUNCTION VDIST (X,Y,N)
C
C CERN PROGLIB# F121    VDIST           .VERSION KERNFOR  4.06  830526
C ORIG. 26/09/76
C
      DIMENSION X(*), Y(*)
C
      XX = 0.
      IF (N.LE.0)            GO TO 24
C
      DO 9 J=1,N
        XX = XX + (X(J)-Y(J)) **2
 9    end do
      XX = SQRT (XX)
   24 VDIST = XX
      RETURN
      END
