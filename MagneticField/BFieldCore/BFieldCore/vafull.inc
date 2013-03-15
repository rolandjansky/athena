*
*
*
*
*
* BFieldCore/vafull.inc
*
* Author : Saclay Muon Software Group (SaMuSoG)
#include "BFieldCore/cospee.inc"
* CFELIX
      REAL        SECFEL
      REAL        RRPOS(2),FFPOS(2),ZZPOS(2)
      REAL        TABBR(2),TABBF(2),TABBZ(2)
      INTEGER     NRR, NFF, NZZ
* COBMAG
      REAL        SECMAG
      REAL        RMAG(2) ,FMAG(2) ,ZMAG(2)
      REAL        BRMAG(2),BFMAG(2),BZMAG(2)
      INTEGER     NRMAG(2)
      INTEGER     NFMAG, NZMAG
      INTEGER     NRMAGMAX 
* COBFAST
      REAL    DRPOS, RPOSMI, DFPOS, FPOSMI, DZPOS, ZPOSMI,
     +        DRMAG, RMAGMI, RMAGMA, DFMAG, FMAGMI, DZMAG, ZMAGMI
******INTEGER JRPOS(NADFEL),JFPOS(NADFEL),JZPOS(NADFEL)
******INTEGER JRMAG(MAMAGF,MAMAGZ,NADMIG+1),
******INTEGER JRMAG(MAMAGF*MAMAGZ*(NADMIG+1)),
******+                                    JFMAG(NADMAG),JZMAG(NADMAG)
      INTEGER JRPOS(2),JFPOS(2),JZPOS(2)
      INTEGER JRMAG(2),
     +                                    JFMAG(2),JZMAG(2)
* CFLGEN
*>> NVERSION  field version
*>> BINNER    Inner field value
*>> RMAINN    Inner field radius
*>> ZMAINN    Inner filed half length
*>> RTORI     Toroid inner Radius
*>> ZTORI     Toroid inner Z
*>> RTORO     Toroid outer Radius
*>> ZTORO     Toroid outer Z
*>> EPS0      transition width from B=const to map
*
      REAL             BINNER,RMAINN,ZMAINN,
     +                 RTORI,ZTORI,RTORO,ZTORO,
     +                 EPS0
      INTEGER          NVERSION
* COBFI
      CHARACTER*4      AVER
      INTEGER          NSUBVER
*
*
*
