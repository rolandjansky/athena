*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      SUBROUTINE RFELIX(LUNB,I1FEL,I2FEL,
     +NVERSION,BINNER,RMAINN,ZMAINN,RTORI,ZTORI,RTORO,ZTORO,EPS0,
     +SECFEL,NRR,NFF,NZZ,RRPOS,FFPOS,ZZPOS,TABBR,TABBF,TABBZ)
*
*>> Reads tabulated nonlinear 3D mag. field computed by F.Bergsma
*>> from file LUNB
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 21/11/95
*>> Date   : 13/05/04 read H8 and new dtb LC
*
      IMPLICIT NONE
*
#include "BFieldCore/cospee.inc"
*
      INTEGER LUNB, I1FEL,I2FEL, NVERSION, NRR,NFF,NZZ
      REAL    BINNER,RMAINN,ZMAINN,RTORI,ZTORI,RTORO,ZTORO,EPS0, SECFEL
      REAL    RRPOS(*),FFPOS(*),ZZPOS(*), TABBR(*),TABBF(*),TABBZ(*)
*
      INTEGER LR,LF,LZ, I13, LRFZ
      REAL    MATABR,MATABF,MATABZ,MATABB
      CHARACTER*80 CAR80
      CHARACTER*2  A2
      REAL     UNPNEX
      EXTERNAL UNPNEX
*
      IF( I1FEL.EQ.0 .AND. I2FEL.EQ.0 )  RETURN
*
      MATABR = MAFELR
      MATABF = MAFELF
      MATABZ = MAFELZ
      MATABB = MAFELB
*
*>> Read the first part of Solenoid field map
      IF( I1FEL.EQ.1 ) THEN
        READ (LUNB,4040) CAR80
        IF( CAR80(1:2).NE.'IC'.AND. CAR80(1:2).NE.'  ') THEN
          write(6,*)'===> Data File unknown !!!!!   ',CAR80(1:2)
          STOP
        ENDIF
        IF( CAR80(1:2).EQ.'IC') THEN
          READ(CAR80,4051)A2,NRR,NFF,NZZ,SECFEL,RTORI,ZTORI,RTORO,ZTORO
          write(6,*)'===> Iron conribution(IC) : ',A2
        ELSEIF( CAR80(41:41).EQ.'.' ) THEN
          READ (CAR80,4050) NRR,NFF,NZZ,SECFEL,RTORI,ZTORI,RTORO,ZTORO
        ELSE
          READ (CAR80,4050) NRR,NFF,NZZ,SECFEL
        ENDIF
*
        IF( NRR.GT.MATABR .OR. NFF.GT.MATABF .OR. NZZ.GT.MATABZ .OR.
     +      NRR*NFF*NZZ.GT.MATABB ) THEN
          PRINT 7700,MATABR,MATABF,MATABZ,MATABB,NRR,NFF,NZZ,NRR*NFF*NZZ
7700      FORMAT(//
     +      ' =====> In RFELIX, Dimensions of the mag. field arrays (',
     +      I3,',',I3,',',I3,',',I7,') are too small !!!!!!'/
     +      ' =====> One needs at least :',I4,',',I3,',',I3,',',I7/
     +      ' =====> STOP !!!!!!'//)
          STOP
        ENDIF
        READ(LUNB,4070) (RRPOS(LR),LR=1,NRR)
        READ(LUNB,4060) (FFPOS(LF),LF=1,NFF)
        READ(LUNB,4070) (ZZPOS(LZ),LZ=1,NZZ)
      ENDIF
4040  FORMAT(A80)
4050  FORMAT(3I6,F15.9,4F11.3)
4051  FORMAT(A2,I4,2I6,F15.9,4F11.3)
4060  FORMAT(6F12.9)
4070  FORMAT(6F12.4)
*
*
*>> Read the second part of Solenoid field map
      IF( I2FEL.EQ.1 ) THEN
        DO I13=1,3
          LRFZ = 0
          DO LZ=1,NZZ
            DO LF=1,NFF
              DO LR=1,NRR
                LRFZ = LRFZ + 1
                IF(I13.EQ.1) THEN
                  TABBR(LRFZ) = UNPNEX(LUNB)
                ELSEIF(I13.EQ.2) THEN
                  TABBF(LRFZ) = UNPNEX(LUNB)
                ELSEIF(I13.EQ.3) THEN
                  TABBZ(LRFZ) = UNPNEX(LUNB)
                ENDIF
              ENDDO
            ENDDO
          ENDDO
        ENDDO
*
        PRINT 7000,LUNB
7000    FORMAT(/' ===> Tabulated nonlinear 3D mag. field',
     +          ' has been read from file No',I3,' by routine RFELIX',
     +         /' ===>',61X,'****',10X,'********'/)
      ENDIF
*
*
      RETURN
      END
