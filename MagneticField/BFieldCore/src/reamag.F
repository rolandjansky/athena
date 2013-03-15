*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
************************************************************************
      SUBROUTINE REAMAG(LUNB,I1MAG,I2MAG,
     +SECMAG,NRMAG,NFMAG,NZMAG,RMAG,FMAG,ZMAG,BRMAG,BFMAG,BZMAG)
*
*>> Reads tabulated linear 3D mag. field of toroids from file LUNB
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 14/02/97
*
      IMPLICIT NONE
*
#include "BFieldCore/cobtri.inc"
*
#include "BFieldCore/cospee.inc"
*
      INTEGER LUNB, I1MAG,I2MAG, NRMAG(*),NFMAG,NZMAG, L, LF, LZ, LR
      REAL    SECMAG, FMAG(*),ZMAG(*),RMAG(*),BRMAG(*),BFMAG(*),BZMAG(*)
*
      INTEGER MATABR,MATABF,MATABZ
      CHARACTER*1 ATYPE
      CHARACTER*8 VERSION
      DATA VERSION/'XXXXXXXX'/
      REAL        UNPNEX
      EXTERNAL    UNPNEX
*
      IF( I1MAG.EQ.0 .AND. I2MAG.EQ.0 )  RETURN
*
      MATABR = MAMAGB
      MATABF = MAMAGF
      MATABZ = MAMAGZ
*
*>> Read the first part of Toroid field map
      IF( I1MAG.EQ.1 ) THEN
        READ (LUNB,4000) VERSION
        ATYPE = VERSION(6:6)
        IF(ATYPE.EQ.'A' .OR. ATYPE.EQ.'B' .OR. ATYPE.EQ.'C' .OR.
     +     ATYPE.EQ.'D' .OR. ATYPE.EQ.'S') THEN
          PRINT 1000,ATYPE
1000      FORMAT(/'  Mag. field file of type ',A1,' cannot be read'
     +           ,' by routine REAMAG  ======>  STOP !')
          STOP
        ENDIF
        READ(LUNB,4010) NF,EPPLAK,ACOURA,ACOURE
        READ(LUNB,4020) NX,NY,PACOX,PACOY,RINT,REXT,ZFIN,CURV
        READ(LUNB,4020) NXE,NYE,PACOXE,PACOYE
        READ(LUNB,4030) (NRINTE(L),L=1,NXE)
        READ(LUNB,4040) (RINTE(L),L=1,NXE)
        READ(LUNB,4040) (REXTE(L),L=1,NXE)
        READ(LUNB,4040) (ZDEBE(L),L=1,NXE)
        READ(LUNB,4040) (ZFINE(L),L=1,NXE)
        READ(LUNB,4040) (CURVEI(L),L=1,NXE)
        READ(LUNB,4040) (CURVEE(L),L=1,NXE)
        READ(LUNB,4050) NFMAG,NZMAG,SECMAG
        IF( NFMAG.GT.MATABF .OR. NZMAG.GT.MATABZ ) THEN
          PRINT 7700,MATABF,MATABZ,NFMAG,NZMAG
7700      FORMAT(//
     +      ' =====> In REAMAG, Dimensions of the mag. field arrays (',
     +      I3,',',I3,') are too small !!!!!!'/
     +      ' =====> One needs at least :',I3,',',I3/
     +      ' =====> STOP !!!!!!'//)
          STOP
        ENDIF
        READ (LUNB,4060) (FMAG(LF),LF=1,NFMAG)
        READ (LUNB,4070) (ZMAG(LZ),LZ=1,NZMAG)
        READ (LUNB,4090)((NRMAG(LF+(LZ-1)*NFMAG),LF=1,NFMAG),LZ=1,NZMAG)
        IF( NRMAG(NFMAG*NZMAG).GT.MATABR ) THEN
          PRINT 7710,MATABR,NRMAG(NFMAG*NZMAG)
7710      FORMAT(//
     +      ' =====> In REAMAG, Dimension of the mag. field arrays (',
     +      I8,') is too small !!!!!!'/
     +      ' =====> One needs at least :',I8/' =====> STOP !!!!!!'//)
          STOP
        ENDIF
      ENDIF
4000  FORMAT(A8)
4010  FORMAT(I5,4F10.2)
4020  FORMAT(2I5,7F10.2)
4030  FORMAT(8I5)
4040  FORMAT(8F10.2)
4050  FORMAT(2I6,F15.9)
4060  FORMAT(6F12.9)
4070  FORMAT(6F12.4)
4090  FORMAT(10I8)
*
*>> Read the second part of Toroid field map
      IF( I2MAG.EQ.1 ) THEN
        DO LR=1,NRMAG(NFMAG*NZMAG)
          RMAG(LR)  = UNPNEX(LUNB) * 100.
        ENDDO
        DO LR=1,NRMAG(NFMAG*NZMAG)
          BRMAG(LR) = UNPNEX(LUNB)
        ENDDO
        DO LR=1,NRMAG(NFMAG*NZMAG)
          BFMAG(LR) = UNPNEX(LUNB)
        ENDDO
        DO LR=1,NRMAG(NFMAG*NZMAG)
          BZMAG(LR) = UNPNEX(LUNB)
        ENDDO
*
        PRINT 7000,VERSION,LUNB
7000    FORMAT(/' ===> Tabulated 3D mag. field version ',A8,
     +    ' has been read from file No',I3,' by routine REAMAG',/
     +    ' ===>',32X,'**********',26X,'****',10X,'********'/)
      ENDIF
*
*
      RETURN
      END
