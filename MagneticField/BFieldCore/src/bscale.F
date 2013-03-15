*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      SUBROUTINE BSCALE(SCALEB, NRR  ,NFF  ,NZZ  , TABBR,TABBF,TABBZ,
     +                          NRMAG,NFMAG,NZMAG, BRMAG,BFMAG,BZMAG)
*
*>> Scale Field arrays
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 3/09/98
*
      IMPLICIT NONE
*
*
      INTEGER NRR,NFF,NZZ, NRMAG(*),NFMAG,NZMAG
      REAL    SCALEB, TABBR(*),TABBF(*),TABBZ(*)
      REAL    BRMAG(*),BFMAG(*),BZMAG(*)
*
      INTEGER I, LZ,LF,LR
      LOGICAL FIRST 
      DATA FIRST /.TRUE./
*
      IF( .NOT.FIRST )  RETURN
*
      FIRST = .FALSE.
*
      DO I=1,NRMAG(NFMAG*NZMAG)
        BRMAG(I) = BRMAG(I) * SCALEB
        BFMAG(I) = BFMAG(I) * SCALEB
        BZMAG(I) = BZMAG(I) * SCALEB
      ENDDO
*
      I = 0
      DO LZ=1,NZZ
        DO LF=1,NFF
          DO LR=1,NRR
            I = I + 1
            TABBR(I) = TABBR(I) * SCALEB
            TABBF(I) = TABBF(I) * SCALEB
            TABBZ(I) = TABBZ(I) * SCALEB
          ENDDO
        ENDDO
      ENDDO
*
*
      RETURN
      END
