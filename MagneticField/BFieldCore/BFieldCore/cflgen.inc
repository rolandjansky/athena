*
*
*
*
*
* BFieldCore/cflgen.inc
*
* Author : Saclay Muon Software Group (SaMuSoG)
*
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
      COMMON/CFLGEN/   BINNER,RMAINN,ZMAINN,
     +                 RTORI,ZTORI,RTORO,ZTORO,
     +                 EPS0,NVERSION
*
