*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      REAL FUNCTION UNPNEX(LUN)
*
*>> Gets next real number for file LUN, after unpacking it
*>> from differences between two consecutive values
*>> packed into two characters...   and with other refinements.
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   :  02/09/98
*
      IMPLICIT NONE
*
      INTEGER, INTENT(IN) :: LUN
*
      INTEGER NXXX, NZERO, NSAME, NDIFF, NREAD, NUSED
      INTEGER I
      REAL    XXX
      CHARACTER(80) :: CAR80
      CHARACTER(1)  :: CBUF(100)
      CHARACTER(1)  :: STAR, AROB, QUES, EXCL, EQUA, SPAC
      DATA STAR,AROB,QUES,EXCL,EQUA,SPAC/ '*', '@', '?', '!', '=', ' ' /
      DATA CBUF/ 100*' ' /
      DATA NXXX,NZERO,NSAME,NDIFF,NREAD,NUSED/ 6*0 /
      DATA XXX/ 0. /
      INTEGER  NFCHAR
      EXTERNAL NFCHAR
*
*
100   IF(NREAD-NUSED.LT.6) THEN
        DO  I=NUSED+1,NREAD
          CBUF(I-NUSED) = CBUF(I)
        ENDDO
        NREAD = NREAD - NUSED
        NUSED = 0
        READ (LUN,'(A80)') CAR80
        FORALL (I=1:80) CBUF(NREAD+I) = CAR80(I:I)
        NREAD = NREAD + 80
      ENDIF
*
      IF(NZERO.GE.1) THEN
        NZERO = NZERO - 1
      ELSEIF(NSAME.GE.1) THEN
        NSAME = NSAME - 1
        NXXX = NXXX + NDIFF - 2450
        XXX = FLOAT(NXXX) / 5000.
      ELSE
        IF(CBUF(NUSED+1).EQ.STAR) THEN
          NZERO = NFCHAR(2,2,CBUF(NUSED+1)) - 1
          NUSED = NUSED + 2
        ELSEIF(CBUF(NUSED+1).EQ.AROB) THEN
          NZERO = NFCHAR(2,3,CBUF(NUSED+1)) - 1
          NUSED = NUSED + 3
        ELSEIF(CBUF(NUSED+1).EQ.QUES) THEN
          NZERO = NFCHAR(2,4,CBUF(NUSED+1)) - 1
          NUSED = NUSED + 4
        ELSEIF(CBUF(NUSED+1).EQ.EXCL) THEN
          NSAME = NFCHAR(2,2,CBUF(NUSED+1)) - 1
          NDIFF = NFCHAR(3,4,CBUF(NUSED+1))
          NXXX = NXXX + NDIFF - 2450
          XXX = FLOAT(NXXX) / 5000.
          NUSED = NUSED + 4
        ELSEIF(CBUF(NUSED+1).EQ.EQUA) THEN
          NXXX = NFCHAR(2,4,CBUF(NUSED+1)) - 171500
          XXX = FLOAT(NXXX) / 5000.
          NUSED = NUSED + 4
        ELSEIF(CBUF(NUSED+1).EQ.SPAC) THEN
          NUSED = NUSED + 1
          GO TO 100
        ELSE
          NXXX = NXXX + NFCHAR(1,2,CBUF(NUSED+1)) - 2450
          XXX = FLOAT(NXXX) / 5000.
          NUSED = NUSED + 2
        ENDIF
      ENDIF
*
      UNPNEX = XXX
*
      RETURN
      END
