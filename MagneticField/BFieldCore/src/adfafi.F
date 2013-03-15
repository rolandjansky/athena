*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      SUBROUTINE ADFAFI( NRR,NFF,NZZ, RRPOS,FFPOS,ZZPOS,
     +                   NRMAG,NFMAG,NZMAG, RMAG,FMAG,ZMAG,
     +DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG)
*
*>> Computes arrays of addresses for a fast access
*>> to the magnetic field tables !
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 30/09/97
*
      IMPLICIT NONE
*
*
#include "BFieldCore/cospee.inc"
*
      INTEGER NRR,NFF,NZZ, NRMAG(*),NFMAG,NZMAG
      INTEGER JRPOS(*),JFPOS(*),JZPOS(*), JRMAG(*),JFMAG(*),JZMAG(*)
      REAL    RRPOS(*),FFPOS(*),ZZPOS(*),  RMAG(*), FMAG(*), ZMAG(*)
      REAL    DRPOS, RPOSMI, DFPOS, FPOSMI, DZPOS, ZPOSMI
      REAL    DRMAG, RMAGMI, RMAGMA, DFMAG, FMAGMI, DZMAG, ZMAGMI
*
      INTEGER LF, LZ, NDEB, NFIN, ITL, IT, IARR
      REAL    RMAGDE, RMAGFI, RTEST
      LOGICAL FIRST
      DATA FIRST /.TRUE./
*     JRMAGST(lc,mc,nc) = JRMAG(nc + ncmax*(mc-1) + ncmax*mcmax*(lc-1))
*
*
**      IF( .NOT.FIRST )  RETURN
*
**      FIRST = .FALSE.
*
*>> Compute addresses for fast access to tabulated field in BFELIX
      CALL GEADFI(RRPOS,NRR,NADFEL,  DRPOS,RPOSMI,JRPOS)
      CALL GEADFI(FFPOS,NFF,NADFEL,  DFPOS,FPOSMI,JFPOS)
      CALL GEADFI(ZZPOS,NZZ,NADFEL,  DZPOS,ZPOSMI,JZPOS)
*
*
*>> Compute addresses for fast access to tabulated field in BINMAG
      CALL GEADFI(FMAG,NFMAG,NADMAG,  DFMAG,FMAGMI,JFMAG)
      CALL GEADFI(ZMAG,NZMAG,NADMAG,  DZMAG,ZMAGMI,JZMAG)
      RMAGDE = RMAG(1)
      RMAGFI = RMAG(NRMAG(1))
      DRMAG  = (RMAGFI-RMAGDE) / NADMIG
      RMAGMI = RMAGDE - DRMAG
      RMAGMA = RMAGFI - 0.0100
      DO 210 LF=1,NFMAG
        DO 200 LZ=1,NZMAG
          IF( LF.EQ.1 .AND. LZ.EQ.1 ) THEN
            NDEB = 1
          ELSE
            NDEB = NRMAG(LF-1+(LZ-1)*NFMAG) + 1
          ENDIF
          NFIN    = NRMAG(LF  +(LZ-1)*NFMAG)
          IF( ABS(RMAG(NDEB)-RMAGDE)/RMAGDE .GT. 0.000001 .OR.
     +        ABS(RMAG(NFIN)-RMAGFI)/RMAGFI .GT. 0.000001 ) THEN
            PRINT 7700
7700        FORMAT(//
     +   ' =====> In ADFAFI : array RMAG of common /COBMAG/ is ',
     +   'imcompatible with assumptions  =====>  STOP !!!!!!'//)
            STOP
          ENDIF
          IT = NDEB + 1
          DO 150 ITL=1,NADMIG
            RTEST = RMAGMI + (ITL+1.)*DRMAG
************************************************************************
            IF( RTEST.GT.RMAG(IT) ) THEN
              IF( IT.GE.NFIN ) THEN
******          JRMAG(LF,LZ,ITL) = NFIN
                IARR = ITL+(NADMIG+1)*(LZ-1)+ (NADMIG+1)*MAMAGZ*(LF-1)
                JRMAG(IARR) = NFIN
              ELSE
******          JRMAG(LF,LZ,ITL) = - IT
                IARR = ITL+(NADMIG+1)*(LZ-1)+ (NADMIG+1)*MAMAGZ*(LF-1)
                JRMAG(IARR) = - IT
100             IT = IT + 1
                IF( RTEST.GT.RMAG(IT) .AND. IT.LT.NFIN ) GO TO 100
              ENDIF
            ELSE
******        JRMAG(LF,LZ,ITL) = IT
              IARR = ITL+(NADMIG+1)*(LZ-1)+ (NADMIG+1)*MAMAGZ*(LF-1)
              JRMAG(IARR) = IT
            ENDIF
150       CONTINUE
******    JRMAG(LF,LZ,NADMIG+1) = NFIN
          IARR =NADMIG+1+(NADMIG+1)*(LZ-1)+ (NADMIG+1)*MAMAGZ*(LF-1)
          JRMAG(IARR) = NFIN
************************************************************************
200     CONTINUE
210   CONTINUE
*
*
      RETURN
      END
