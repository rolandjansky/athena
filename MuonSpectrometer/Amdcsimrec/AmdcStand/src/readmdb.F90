!
!> \author SamuSog
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 SUBROUTINE REAMDC(LUN,IFLAG)
 IMPLICIT REAL(8) (A-H,O-Z)
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
 REAL(8)            :: Pos_Z_Zm_V(0:MTYP_F_Z)
 CHARACTER(1)       :: CAR1,CAR1BE,CarCur, COMA(18)
 CHARACTER(3)       :: CAR3, CTYP(MTYP),CTEC(MTEC),CMAT(20)
 CHARACTER(8)       :: CAR8
 CHARACTER(16)      :: CAR16A,CAR16B
 CHARACTER(78)      :: CAR78
 CHARACTER(89)      :: CAR88
 CHARACTER(89)      :: CAR99
 CHARACTER(128)     :: CAR128
 CHARACTER(512)     :: CAR512
 INTEGER            :: LFI(8),Ichan(20)
 REAL(8)            :: STAEMI(MTEC,MSTA)
 REAL(8)            :: XTEC(MTEC)
 INTEGER            :: NSECTO(MTYP,-MZZ:MZZ)
 INTEGER            :: IGEOSYM(MTYP,MGEO)
 CHARACTER (len=80) :: AAA(20)
 INTEGER            :: III(20)
 REAL(8)            :: RRR(20)
 INTEGER            :: ier
 REAL(8)            :: DeltaZmZp
 REAL(8)            :: ZmZp
!
 DATA NOMA/ 18 /
 DATA COMA/ '*','W','D','P','H','M','E','N','V','A','B','C','R',   &
            'X', 'Q', 'L', 'I' , 'F'/
 DATA NTYP/ 54 /
 DATA CTYP/ 'BIL','BIS','BML','BMS','BOL','BOS','BEE',             &
            'BIR',      'BMF',      'BOF','BOG',                   &
            'BME',                                                 &
            'EIC','EIL',                                           &
                  'EEL','EES',                                     &
            'EMC','EML','EMS',                                     &
            'EOC','EOL','EOS',                                     &
            'T1C','T1L','T1S',                                     &
            'T2C','T2L','T2S',                                     &
            'T3C','T3L','T3S',                                     &
            'CI1','CI2','CI3','CI4',                               &
            'FIL','FIS','FML','FMS','FOL','FOS',                   &
            'T1F','T1E',                                           &
            'T2F','T2E',                                           &
            'T3F','T3E',                                           &
            'T4F','T4E',      'EIS',                               &
            'CSS','CSL',      'BIM','BMG',              6*'XXX' /
 DATA NTEC/ 11 /
 DATA CTEC/ 'MDT','RPC','TGC','CSC','SPA','DED','SUP','CHV',       &
            'CMI','CRO','LB ',                          9*'XXX' /
 DATA XTEC/ .09D0,.11D0,.13D0,.01D0,.01D0,             15*.00D0 /
 DATA NMAT/ 15 /
 DATA CMAT/ 'DES','BAR','ENC','FEE','RAI','SER','SHI','SOL',       &
            'GIR','ECA','TIL','PIX','SCT','TRT','ALI',  5*'XXX' /
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Pos_Z_Zm  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =  &
      Pos_Z_Zm_V(JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
!
!-- Initialise all AMDC commons
      CALL CLDBCO
      DO I=1,MTYP
        CARTYP(I) = CTYP(I)
      ENDDO
      DO I=1,MTEC
        CARTEC(I) = CTEC(I)
      ENDDO
!
!-- and a few constants
      ITOTCU        = 0
!
!
!-- Set switches !
      IRADIA = IFLAG/100
      II     = IFLAG - IRADIA*100
      I3TO4  = II/10
      IPRINT = II - I3TO4*10
!
!
      Ixml   = 0
10    CONTINUE
      IF( Ixml > 0 ) THEN
!-- Store CarAGDD !!! $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        Icomment  =  0
20      READ (LUN,1512,END=90) CAR512
1512    FORMAT(A512)
        IF( CAR512(1:1) == COMA(14) ) THEN
          Ixml = 0
          GO TO 10
        ENDIF
        CarCur    = ' '
        LenCar512 = LEN_TRIM(CAR512)
        DO I=1,LenCar512
          IF( I <= LenCar512-3 ) THEN
            IF( CAR512(I:I+3) == '<!--' ) Icomment = 1
          ENDIF
          IF( Icomment <= 0 .AND. (CAR512(I:I) /= ' ' .OR. CarCur /= ' ') ) THEN
            CarCur  = CAR512(I:I)
            LenAGDD = LenAGDD + 1
            IF( LenAGDD > MaxAGDD ) THEN
              PRINT 7029,LenAGDD-1
7029          FORMAT(/' In REAMDC, Parameter MaxAGDD =',I7,' is too small =====> STOP !'/31X,'********')
              STOP
            ENDIF
            CarAGDD(LenAGDD) = CarCur
          ENDIF
          IF( I >= 3 ) THEN
            IF( CAR512(I-2:I) == '-->' ) Icomment = 0
          ENDIF
        ENDDO
        IF( LenAGDD > 0 .AND. CarAGDD(LenAGDD) /= CHAR(10) ) THEN
          LenAGDD = LenAGDD + 1
          IF( LenAGDD > MaxAGDD ) THEN
            PRINT 7029,LenAGDD-1
            STOP
          ENDIF
          CarAGDD(LenAGDD) = CHAR(10)
        ENDIF
        GO TO 20
!-- $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
      ENDIF
!      READ (LUN,1128,END=90) CAR128
!1128  FORMAT(A128)
      READ (LUN,1128,END=90) CAR512
1128  FORMAT(A512)
!-- Store CarAMDC !!! $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
      IF(     CAR512(1:1) == COMA(7)  ) THEN
        GO TO 90
      ELSEIF( CAR512(1:1) == COMA(14) ) THEN
        Ixml = 1 - Ixml
      ELSEIF( CAR512(1:1) /= '*' ) THEN
        Kcomm = SCAN( CAR512, '!' )
        IF( Kcomm > 1 ) CAR512 = CAR512(1:Kcomm-1)
        Kcomm = SCAN( CAR512, '|' )
        IF( Kcomm > 1 ) CAR512 = CAR512(1:Kcomm-1)
        LenCAR512 = LEN_TRIM(CAR512)
        DO I=1,LenCAR512
          LenAMDC = LenAMDC + 1
          IF( LenAMDC+2 > MaxAMDC ) THEN
            PRINT 7030,MaxAMDC
7030        FORMAT(/' In REAMDC, Parameter MaxAMDC =',I7,' is too small =====> STOP !'/31X,'********')
            STOP
          ENDIF
          CarAMDC(LenAMDC) = CAR512(I:I)
        ENDDO
        LenAMDC = LenAMDC + 1
        CarAMDC(LenAMDC) = CHAR(10)
      ENDIF
      GO TO 10
90    LenAMDC = LenAMDC + 1
      CarAMDC(LenAMDC ) = COMA(7)
!-- $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
!
!
      REWIND(LUN)
!
!
      Ixml   = 0
100   CONTINUE
      IF( Ixml > 0 ) THEN
110      READ (LUN,1000,END=900) CAR1
         IF( CAR1 == COMA(14) ) THEN
            Ixml = 0
            GO TO 100
         ENDIF
         GO TO 110
      ENDIF
      READ (LUN,2128,END=900) CAR1,CAR1BE,CAR128
      CAR78 = CAR128(1:78)
2128  FORMAT(2A1,A128)
1000  FORMAT(A1,1X,A78)
1200  FORMAT(A16)
      IF(CAR1.EQ.COMA(1)) THEN
!--  *  Comment --------------------------------------------------------
!---***-----------------------------------------------------------------
         GO TO 100
      ELSEIF(CAR1.EQ.COMA(2)) THEN
!--  W  Wire (or strip) stacking definition (i.e. internal structure) --
!---***-----------------------------------------------------------------
         IF( CAR78(7:7).EQ.'.' ) THEN
            READ (CAR78,2000) CAR3,JSTA,X0NB,NL1,NL2,PP,RR,EE,PG,EMI
2000        FORMAT(A3,I2,F5.3,2I2,8F8.2)
         ELSE
            READ (CAR78,2100) CAR3,JSTA,     NL1,NL2,PP,RR,EE,PG,EMI
2100        FORMAT(A3,I2,  5X,2I2,8F8.2)
            X0NB = 999.99D0
         ENDIF
         JTEC = JTROU3(CAR3,NTEC,CTEC)
         IF(JTEC.EQ.0) THEN
            PRINT 7001,CAR3
7001        FORMAT(/' In REAMDC, Technology Type : ',A3,' is unknown =====> STOP !'/29X,'*****')
            STOP
         ENDIF
         IF(JSTA.LT.1 .OR. JSTA.GT.MSTA) THEN
            PRINT 7002,JSTA
7002        FORMAT(/' In REAMDC, Stacking type index : ',I3,' is outside range =====> STOP !'/33X,'*****')
            STOP
         ENDIF
         IVALIDSTA (JTEC,JSTA) = 1
         IF( X0NB.GT.10.D0 ) X0NB = XTEC(JTEC)
         STAX0 (JTEC,JSTA) = X0NB
         NLAS  (JTEC,JSTA) = NL1
         NLAZ  (JTEC,JSTA) = NL2
         STAPP (JTEC,JSTA) = PP
         STARR (JTEC,JSTA) = RR
         STAEE (JTEC,JSTA) = EE
         STAPG (JTEC,JSTA) = PG
         IFORMA(JTEC,JSTA) = 1
         if( CTEC(JTEC).eq.'LB ' .and. abs(STAEE(JTEC,JSTA)).lt.0.0001 ) STAEE (JTEC,JSTA)=STARR(JTEC,JSTA)
         IF( CTEC(JTEC).NE.'RPC'.AND.CTEC(JTEC).NE.'TGC' .AND.CTEC(JTEC).NE.'CSC') THEN
!--... General (i.e. mostly MDT's) internal structures ............
            READ (LUN,1000) CAR1
            BACKSPACE(LUN)
            IF( JTROU1(CAR1,NOMA,COMA).EQ.0 ) THEN
               READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=1,8)
               READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL=1,8)
               IF(CTEC(JTEC).EQ.'MDT') STAEMI(JTEC,JSTA) = EMI
            ENDIF
         ELSEIF(CTEC(JTEC).EQ.'RPC') THEN
!--... RPCs internal structures ..................................
            READ (LUN,1200) CAR16A
            READ (LUN,1200) CAR16B
            READ (LUN,1000) CAR1
            DO I=1,3
             BACKSPACE(LUN)
            ENDDO
            IF(   JTROU1(CAR1,NOMA,COMA).NE.0 .AND. CAR16A.EQ.'                '        &
                                              .AND. CAR16B.EQ.'                ' ) THEN
!-- Standard default format !!!!!!!!!!
               IFORMA(JTEC,JSTA) = 0
               READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=1,8)
               READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL=1,8)
            ELSE
!-- Specific RPC format !!!!!!!!!!!!!!
               DO I=1,6
                READ (LUN,1000) CAR1
               ENDDO
               DO I=1,6
                BACKSPACE(LUN)
               ENDDO
               IF( IVERSION_AMDC.LE.3 .AND. JTROU1(CAR1,NOMA,COMA).NE.0 ) THEN
!-- First specific RPC format
                  IFORMA(JTEC,JSTA) = 1
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=1,5)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=6,10)
                  READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL=1,4)
                  READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL=5,6)
                  READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL=7,11)
               ELSEIF( IVERSION_AMDC.LE.3 ) THEN
!-- Second specific RPC format
                  IFORMA(JTEC,JSTA) = 2
                  READ (LUN,2200)  STATT(JTEC,JSTA, 1),STAOO(JTEC,JSTA, 6)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=2,3)
                  READ (LUN,2200)  STATT(JTEC,JSTA, 4)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=5,6)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=7,8)
                  READ (LUN,2200)  STATT(JTEC,JSTA, 9)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=10,12)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=13,14)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=15,16)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=17,18)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=19,20)
                  READ (LUN,2200)  STAOO(JTEC,JSTA, 1),STAOO(JTEC,JSTA, 4),STAOO(JTEC,JSTA, 5)
                  READ (LUN,2204) INBID1,INBID2
                  STAOO(JTEC,JSTA,2) = INBID1*1.
                  STAOO(JTEC,JSTA,3) = INBID2*1.
                  STAEE(JTEC,JSTA) = STATT(JTEC,JSTA,9)
               ELSE
!-- Third specific RPC format
                  IFORMA(JTEC,JSTA) = 4
                  IF( JSTA.EQ.1 ) THEN
                     READ (LUN,2300)  STATT(JTEC,JSTA, 4)
                     READ (LUN,2300) (STATT(JTEC,JSTA,IL),IL=5,6)
                     READ (LUN,2300) (STATT(JTEC,JSTA,IL),IL=7,8)
                     READ (LUN,2300)  STATT(JTEC,JSTA, 9)
                     READ (LUN,2300) (STATT(JTEC,JSTA,IL),IL=10,12)
                     READ (LUN,2300)  STATT(JTEC,JSTA,13),STATT(JTEC,JSTA,14),   &
                                      STAOO(JTEC,JSTA,7), STAOO(JTEC,JSTA,8)
                     READ (LUN,2300)  STATT(JTEC,JSTA,15),STATT(JTEC,JSTA,16),   &
                                      STAOO(JTEC,JSTA,9), STAOO(JTEC,JSTA,10)
                     READ (LUN,2300) (STATT(JTEC,JSTA,IL),IL=17,18)
                     READ (LUN,2300) (STATT(JTEC,JSTA,IL),IL=19,20)
                     READ (LUN,2300)  STAOO(JTEC,JSTA, 1),STAOO(JTEC,JSTA, 4),STAOO(JTEC,JSTA, 5)
                  ELSE
                     DO L=1,20
                      STATT(JTEC,JSTA,L) =  STATT(JTEC,1,L)
                      STAOO(JTEC,JSTA,L) =  STAOO(JTEC,1,L)
                     ENDDO
                     READ (LUN,2300) (STATT(JTEC,JSTA,IL),IL=10,12)
                  ENDIF
                  READ (LUN,2304) INBID1,INBID2
                  INBID3 = 0
                  INBID4 = 0
                  READ (LUN,1000) CAR1
                  BACKSPACE(LUN)
                  IF( JTROU1(CAR1,NOMA,COMA).EQ.0 ) THEN
!-- Third prime sub specific RPC format
                     INBID3 = INBID1
                     INBID4 = INBID2
                     READ (LUN,2300) (STAOO(JTEC,JSTA,IL),IL=11,13)
                     READ (LUN,2304) INBID1,INBID2
                  ENDIF
                  STAOO(JTEC,JSTA, 2) = INBID1*1.
                  STAOO(JTEC,JSTA, 3) = INBID2*1.
                  STAOO(JTEC,JSTA,14) = INBID3*1.
                  STAOO(JTEC,JSTA,15) = INBID4*1.
                  STAEE(JTEC,JSTA) = STATT(JTEC,JSTA,9)
               ENDIF
            ENDIF
         ELSEIF(CTEC(JTEC).EQ.'TGC') THEN
!--... TGCs internal structures ..................................
            DO I=1,3
             READ (LUN,1000) CAR1,CAR78
            ENDDO
            DO I=1,3
             BACKSPACE(LUN)
            ENDDO
            IF(JTROU1(CAR1,NOMA,COMA) /= 0) THEN
               READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=1,8)
               READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL=1,8)
            ELSE
               IFORMA(JTEC,JSTA) = 2
               STAPP(JTEC,JSTA)  = EE
               STAEE(JTEC,JSTA)  = PP
               IF( NL2 > 19 ) PRINT 7024,JSTA
7024           FORMAT(/' In REAMDC, for the TGC Stacking type :',I3/' the number of read lines is limited to 19 !!!')
               DO IL=1,NL2
                IF( IL <= 19 ) THEN
                   READ (LUN,2202) ISTAMA(JTEC,JSTA,IL),STATT(JTEC,JSTA,IL),STAOO(JTEC,JSTA,IL)
                ELSE
                   READ (LUN,2202) Idummy
                ENDIF
               ENDDO
               IF( PG > 0.1D0 .AND. EMI > 0.1D0 ) THEN
                  IFORMA(JTEC,JSTA) = 3
                  STATT(JTEC,JSTA,NL2+1) = PG
                  STAOO(JTEC,JSTA,NL2+1) = EMI
               ENDIF
               IF( IVERSION_AMDC >= 5 ) THEN
                  IFORMA(JTEC,JSTA) = 4
                  READ (LUN,1000) CAR1
                  READ (LUN,2204) (NtgcAdd(JSTA,IL),IL=1,3)
                  READ (LUN,2204) (NtgcAdd(JSTA,IL),IL=4,6)
                  READ (LUN,2204) (NtgcAdd(JSTA,IL),IL=7,9)
                  READ (LUN,2200) (XtgcAdd(JSTA,IL),IL=1,3)
                  READ (LUN,2200) (XtgcAdd(JSTA,IL),IL=4,5)
                  IF( IVERSION_AMDC >= 6 ) THEN
                     IFORMA(JTEC,JSTA) = 5
                     READ (LUN,2303) (XtgcAdd(JSTA,IL),IL=6,7)
                     READ (LUN,2303) (XtgcAdd(JSTA,IL),IL=8,12)
                     READ (LUN,2303) (XtgcAdd(JSTA,IL),IL=13,16)
                  ENDIF
                  DO I=1,3
                   IF( NtgcAdd(JSTA,I) > Mgang ) THEN
                     PRINT 7028,NtgcAdd(JSTA,I)
7028                 FORMAT(/' In REAMDC, Number of TGC wire gangs :',I4,' is outside range =====> STOP !'/38X,'*****')
                     STOP
                   ELSEIF( NtgcAdd(JSTA,I) >= 1 ) THEN
                     READ (LUN,1000) CAR1
                     READ (LUN,2400) (NtgcGang(JSTA,IL,I),IL=1,NtgcAdd(JSTA,I))
                   ENDIF
                  ENDDO
                  READ (LUN,1000) CAR1
                  IF( CAR1 == '-' ) THEN
                     IFORMA(JTEC,JSTA) = 6
                     NBstrp = MAX( NtgcAdd(JSTA,4), NtgcAdd(JSTA,5), NtgcAdd(JSTA,6) )
                     IF( NBstrp+1 > Mstrp ) THEN
                       PRINT 7036,NBstrp
7036                   FORMAT(/' In REAMDC, Number of TGC strip pos. :',I4,' is outside range =====> STOP !'/38X,'*****')
                       STOP
                     ELSE
                       READ (LUN,2500) XtgcStrp(JSTA,1:NBstrp+1,1)
                       READ (LUN,1000) CAR1
                       READ (LUN,2500) XtgcStrp(JSTA,1:NBstrp+1,2)
                     ENDIF
                  ELSE
                     BACKSPACE(LUN)
                  ENDIF
               ENDIF
            ENDIF
         ELSEIF(CTEC(JTEC).EQ.'CSC') THEN
!--... CSCs internal structures ..................................
            DO I=1,3
             READ (LUN,1000) CAR1,CAR78
            ENDDO
            DO I=1,3
             BACKSPACE(LUN)
            ENDDO
            IF(JTROU1(CAR1,NOMA,COMA).NE.0) THEN
               READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=1,8)
               READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL=1,8)
            ELSE
               IFORMA(JTEC,JSTA) = 2
               DO I=1,9
                READ (LUN,1000) CAR1,CAR78
               ENDDO
               DO I=1,9
                BACKSPACE(LUN)
               ENDDO
               READ (LUN,2200)  STATT(JTEC,JSTA, 1)
               READ (LUN,2200)  STATT(JTEC,JSTA, 2)
               READ (LUN,2200)  STATT(JTEC,JSTA, 3)
               READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=4,5)
               READ (LUN,2201)  STATT(JTEC,JSTA, 6),CAR8
               IF(CAR8.EQ.'        ') THEN
                  STAOO(JTEC,JSTA,4) = 0.D0
               ELSE
                  READ (CAR8,2300) STAOO(JTEC,JSTA,4)
               ENDIF
               READ (LUN,2200)  STATT(JTEC,JSTA, 7)
               READ (LUN,2200)  STATT(JTEC,JSTA, 8)
               READ (LUN,2200)  STATT(JTEC,JSTA, 9)
               STAEE(JTEC,JSTA) = STATT(JTEC,JSTA,1)
               IF(JTROU1(CAR1,NOMA,COMA).EQ.0) THEN
                  IFORMA(JTEC,JSTA) = 3
                  IF( IVERSION_AMDC.GE.6 ) IFORMA(JTEC,JSTA) = 4
                  READ (LUN,2200)  STATT(JTEC,JSTA,10)
                  READ (LUN,2200)  STATT(JTEC,JSTA,11)
                  READ (LUN,2200)  STATT(JTEC,JSTA,12)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=13,14)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=15,16)
                  READ (LUN,2200) (STATT(JTEC,JSTA,IL),IL=17,18)
                  READ (LUN,2200) (STAOO(JTEC,JSTA,IL),IL= 1, 3)
               ENDIF
            ENDIF
         ENDIF
2200     FORMAT(16X,8F8.2)
2201     FORMAT(16X,F8.2,3A8)
2300     FORMAT(8F8.2)
2303     FORMAT(8F8.3)
2202     FORMAT(4X,I3,2F8.3)
2204     FORMAT(16X,8I8)
2304     FORMAT(8I8)
2400     FORMAT(10I4)
2500     FORMAT(5F9.2)
      ELSEIF(CAR1.EQ.COMA(3)) THEN
!--  D  Definition of stations -----------------------------------------
!---***-----------------------------------------------------------------
         READ (CAR78,3100) CAR3,JGEO,NO
3100     FORMAT(A3,I2,2X,I2)
         JTYP = JTROU3(CAR3,NTYP,CTYP)
         IF(JTYP.LE.0) THEN
            PRINT 7003,CAR3
7003        FORMAT(/' In REAMDC, Station Type : ',A3,' is unknown =====> STOP !'/26X,'*****')
            STOP
         ENDIF
         IF(JGEO.LT.1 .OR. JGEO.GT.MGEO) THEN
            PRINT 7004,JGEO
7004        FORMAT(/' In REAMDC, Station geometry index : ',I3,' is outside range =====> STOP !'/36X,'*****')
            STOP
         ENDIF
         IF(NO.LT.1 .OR. NO.GT.MOBJ) THEN
            PRINT 7005,CAR3,JGEO,NO
7005        FORMAT(/' In REAMDC, Number of objects defining station ',A3,I3,' : ',I3,   &
                    ' is outside range =====> STOP !'/55X,'*****')
            STOP
         ENDIF
         NOBJ(JTYP,JGEO) = NO
         DO 200 JOB=1,NO
         KTYP_G_O = JGET_JTYP_G_O(JTYP,JGEO,JOB)
         IF( IVERSION_AMDC.LE.3 ) THEN
            READ (LUN,3200) Geo_dx_V(KTYP_G_O), Geo_dy_V(KTYP_G_O),   &
                            Geo_dz_V(KTYP_G_O),  IORD,  CAR3,         &
                              ISTA_V(KTYP_G_O), ISPLIX_V(KTYP_G_O),   &
                            ISPLIY_V(KTYP_G_O), ISHAPE_V(KTYP_G_O),   &
                            Geo_Ws_V(KTYP_G_O), Geo_Wl_V(KTYP_G_O),   &
                            Geo_Le_V(KTYP_G_O), Geo_Ex_V(KTYP_G_O),   &
                            Geo_D1_V(KTYP_G_O), Geo_D2_V(KTYP_G_O)
         ELSE
            READ (LUN,3200) Geo_dx_V(KTYP_G_O), Geo_dy_V(KTYP_G_O),   &
                            Geo_dz_V(KTYP_G_O),  IORD,  CAR3,         &
                              ISTA_V(KTYP_G_O), ISPLIX_V(KTYP_G_O),   &
                            ISPLIY_V(KTYP_G_O), ISHAPE_V(KTYP_G_O),   &
                            Geo_Ws_V(KTYP_G_O), Geo_Wl_V(KTYP_G_O),   &
                            Geo_Le_V(KTYP_G_O), Geo_Ex_V(KTYP_G_O),   &
                            Geo_D1_V(KTYP_G_O), Geo_D2_V(KTYP_G_O),   &
                            Geo_D3_V(KTYP_G_O)
         ENDIF
3200     FORMAT(3F7.2,I2,1X,A3,4I2,3F8.2,4F7.2)
         IF(IORD.NE.JOB) THEN
            PRINT 7009,CTYP(JTYP),JGEO,JOB,IORD
7009        FORMAT(/' In REAMDC, During definition of station type : ',A3,I2,' ,'/47X,'*******'   &
                   /' the object read in position',I2,                                            &
                    ' has a different serial number :',I2,' =====> STOP !')
            STOP
         ENDIF
         JTEC = JTROU3(CAR3,NTEC,CTEC)
         IF(JTEC.EQ.0) THEN
            PRINT 7001,CAR3
            STOP
         ENDIF
         ITEC_V(KTYP_G_O) = JTEC
200      CONTINUE
      ELSEIF(CAR1.EQ.COMA(4).or.CAR1.EQ.COMA(15)) THEN
!--  P  Positioning of stations ----------------------------------------
!---***-----------------------------------------------------------------
         if (CAR1.EQ.COMA(4)) then
         CAR88 = CAR128(1:88)
         READ (CAR88,4100) CAR3,JGEO,JCUT,(LFI(L),L=1,8),JZZ,   &
                           F0,ZZ,TT,SS,AA,BB,GG,ZmZp
         endif
         if (CAR1.EQ.COMA(15)) then
         CAR99 = CAR128(1:99)
         READ (CAR99,4102) CAR3,JGEO,JCUT,(LFI(L),L=1,8),JZZ,   &
                           F0,ZZ,TT,SS,AA,BB,GG,ZmZp
         endif
4100     FORMAT(A3,2I2,1X,8I1,I3,3F9.2,4F8.2,F10.2)
4102     FORMAT(A3,2I2,1X,8I1,I3,3F10.2,4F10.2,F10.2)
         JTYP = JTROU3(CAR3,NTYP,CTYP)
         IF(JTYP.LE.0) THEN
            PRINT 7003,CAR3
            STOP
         ENDIF
         IF(JGEO.LT.1 .OR. JGEO.GT.MGEO) THEN
            PRINT 7004,JGEO
            STOP
         ENDIF
         IF(JCUT.LT.0 .OR. JCUT.GT.4) THEN
            PRINT 7010,JCUT
7010        FORMAT(/' In REAMDC, Cut-out index : ',I3,' is outside range =====> STOP !'/27X,'*****')
            STOP
         ENDIF
         IF(JZZ.LT.-MZZ .OR. JZZ.GT.MZZ) THEN
            PRINT 7006,JZZ
7006        FORMAT(/' In REAMDC, Z index : ',I3,' is outside range =====> STOP !'/21X,'*****')
            STOP
         ENDIF
         IF( LFI(1) <= 2 ) THEN
            N02   = 0
            NSECTO(JTYP,JZZ) = 8
         ELSE
            N02   = 2
            NSECTO(JTYP,JZZ) = MIN( 6 , LFI(1) )
         ENDIF
         PI2SN = TWOPI / NSECTO(JTYP,JZZ)
         FF = F0*PIS180 - PI2SN
         DO 300 JFF=1,NSECTO(JTYP,JZZ)
            FF = FF + PI2SN
            IF( LFI(JFF+N02) < 1 ) CYCLE
            KTYP_F_Z = JGET_JTYP_F_Z(JTYP,JFF,JZZ)
            IGEO_V    (KTYP_F_Z) = JGEO
            ICUT_V    (KTYP_F_Z) = JCUT
            Pos_Phi_V (KTYP_F_Z) = FF
            Pos_Z_V   (KTYP_F_Z) = ZZ
            Pos_Z_Zm_V(KTYP_F_Z) = ZmZp
            Pos_R_V   (KTYP_F_Z) = TT
            Pos_S_V   (KTYP_F_Z) = SS
            Pos_Alfa_V(KTYP_F_Z) = AA
            Pos_Beta_V(KTYP_F_Z) = BB
            Pos_Gama_V(KTYP_F_Z) = GG
            IF( CAR1BE == 'b' ) THEN
              IBAREL_V(KTYP_F_Z) = 1
              IF( LFI(JFF+N02) == 2 ) IBAREL_V(KTYP_F_Z) = 2
            ELSEIF( CAR1BE == 'e' ) THEN
              IBAREL_V(KTYP_F_Z) = 0
              IF( LFI(JFF+N02) == 2 ) IBAREL_V(KTYP_F_Z) = -1
            ELSE
              IF( CTYP(JTYP)(1:1) == 'B' ) THEN
                IBAREL_V(KTYP_F_Z) = 1
                ICSCCH_V(KTYP_F_Z) = 0
                IF( LFI(JFF+N02) == 2 ) IBAREL_V(KTYP_F_Z) = 2
              ELSE
                IBAREL_V(KTYP_F_Z) = 0
                IF(CTYP(JTYP)(1:2)=='CS') ICSCCH_V(KTYP_F_Z)=1
                IF( LFI(JFF+N02) == 2 ) IBAREL_V(KTYP_F_Z) = -1
              ENDIF
            ENDIF
300      CONTINUE
      ELSEIF(CAR1.EQ.COMA(5)) THEN
!--  H  Definition of cut-outs (Holes) in stations ---------------------
!---***-----------------------------------------------------------------
         READ (CAR78,3300) CAR3,JGEO,JCUT,JNBCUT
3300     FORMAT(A3,3I2)
         JTYP = JTROU3(CAR3,NTYP,CTYP)
         IF(JTYP.LE.0) THEN
            PRINT 7003,CAR3
            STOP
         ENDIF
         IF(JGEO.LT.1 .OR. JGEO.GT.MGEO) THEN
            PRINT 7004,JGEO
            STOP
         ENDIF
         IF(JCUT.LT.0 .OR. JCUT.GT.4) THEN
            PRINT 7010,JCUT
            STOP
         ENDIF
         IF(JNBCUT.LT.1 .OR. JNBCUT.GT.MOCU) THEN
            PRINT 7011,CAR3,JGEO,JCUT,JNBCUT
7011        FORMAT(/' In REAMDC, In definition of cut-out : ',A3,2I3,        &
                    ' ,'/38X,'***********'/' the number of sub-cuts : ',I3,  &
                    ' is outside range =====> STOP !')
            STOP
         ENDIF
         ITOTCU = ITOTCU + 1
         IF(ITOTCU.GT.MCUT) THEN
            PRINT 7012,ITOTCU
7012        FORMAT(/' In REAMDC, Total number of cut-outs > ',I3,' is outside range =====> STOP !'/38X,'*****')
            STOP
         ENDIF
         INOCUT(JTYP,JGEO,JCUT) = ITOTCU
         NOBCUT(ITOTCU) = JNBCUT
         DO 400 JOB=1,JNBCUT
         IF( IVERSION_AMDC.LE.3 ) THEN
            READ (LUN,3400) Cut_dx(ITOTCU,JOB), Cut_dy(ITOTCU,JOB),   &
                            IOBCUT(ITOTCU,JOB),                       &
                            Cut_Ws(ITOTCU,JOB), Cut_Wl(ITOTCU,JOB),   &
                            Cut_Le(ITOTCU,JOB), Cut_Ex(ITOTCU,JOB)
         ELSE
            READ (LUN,3400) Cut_dx(ITOTCU,JOB), Cut_dy(ITOTCU,JOB),   &
                            IOBCUT(ITOTCU,JOB),                       &
                            Cut_Ws(ITOTCU,JOB), Cut_Wl(ITOTCU,JOB),   &
                            Cut_Le(ITOTCU,JOB),                       &
                            Cut_Ex(ITOTCU,JOB), Cut_An(ITOTCU,JOB)
         ENDIF
3400     FORMAT(2F7.2,7X,I2, 12X     ,3F8.2,2F7.2)
400      CONTINUE
      ELSEIF(CAR1.EQ.COMA(6)) THEN
!--  M  Definition of Matter (e.g. of toroids) for m.s. ----------------
!---***-----------------------------------------------------------------
         READ (CAR78,3500) CAR3,JVEMAT
3500     FORMAT(A3,I5)
         JMAT = JTROU3(CAR3,NMAT,CMAT)
         IF(JMAT.EQ.0) THEN
            PRINT 7016,CAR3
7016        FORMAT(/' In REAMDC, Matter Type : ',A3,' is unknown =====> STOP !'/25X,'*****')
            STOP
         ENDIF
         CALL REAMAT(LUN,CMAT(JMAT),JVEMAT)
      ELSEIF(CAR1.EQ.COMA(7)) THEN
!--  E  End of readings ------------------------------------------------
!---***-----------------------------------------------------------------
         GO TO 900
      ELSEIF(CAR1.EQ.COMA(8)) THEN
!--  N  Name of the AMDC data base being read --------------------------
!---***-----------------------------------------------------------------
         C_AMDC_NAME = CAR78(1:16)
      ELSEIF(CAR1.EQ.COMA(9)) THEN
!--  V  Version number of the AMDC data base being read ----------------
!---***-----------------------------------------------------------------
         READ (CAR78,3600) IVERSION_AMDC
3600     FORMAT(I5)
         IF( IVERSION_AMDC /= 0 .AND. (IVERSION_AMDC < 4 .OR. IVERSION_AMDC > 7) ) THEN
            PRINT 7017,IVERSION_AMDC
7017        FORMAT(/' In REAMDC, Version number :',I5,' is unknown =====> STOP !'/28X,'******')
            STOP
         ENDIF
      ELSEIF(CAR1.EQ.COMA(10)) THEN
!--  A  Adjustment of the SZT position of a station (object) -----------
!---***-----------------------------------------------------------------
         CALL GetCharAndNum(CAR128,'A3I6R',JA,JI,JR,AAA,III,RRR,Ierr)
         IF( Ierr > 0 ) THEN
            PRINT 7044,TRIM(CAR128)
7044        FORMAT(/' In REAMDC, line :',/'A ',A,/' is un-understandable =====> STOP !')
            STOP
         ENDIF
         CAR3 = TRIM(AAA(1))
         JFF  = III(1)
         JZZ  = III(2)
         JOB  = III(3)
         JTYP = JTROU3(CAR3,NTYP,CTYP)
         IF(JTYP.LE.0) THEN
            PRINT 7003,CAR3
            STOP
         ENDIF
         IF(JFF.LT.1 .OR. JFF.GT.8) THEN
            PRINT 7018,JFF
7018        FORMAT(/' In REAMDC, Phi index : ',I3,' is outside range =====> STOP !'/23X,'*****')
            STOP
         ENDIF
         IF(JZZ.LT.-MZZ .OR. JZZ.GT.MZZ) THEN
            PRINT 7006,JZZ
            STOP
         ENDIF
         call setalines (JTYP,JFF,JZZ,JOB, RRR, ier)
         if ( ier.ne.0 ) then
            PRINT 7019
7019        FORMAT(/' In REAMDC, Number of SZT position adjustments is outside range =====> STOP !')
            STOP
         endif
      ELSEIF(CAR1.EQ.COMA(17)) THEN
!--  I  Internal alignement of CSC layer -----------
!---***----------------------------------------------
         CALL GetCharAndNum(CAR128,'A4I6R',JA,JI,JR,AAA,III,RRR,Ierr)
         IF( Ierr > 0 ) THEN
            PRINT 7046,TRIM(CAR128)
7046        FORMAT(/' In REAMDC, line :',/'I ',A,/' is un-understandable =====> STOP !')
            STOP
         ENDIF
         CAR3 = TRIM(AAA(1))
         JFF  = III(1)
         JZZ  = III(2)
         JOB  = III(3)
         JLAY = III(4)
         JTYP = JTROU3(CAR3,NTYP,CTYP)
         IF(JTYP.LE.0) THEN
            PRINT 7003,CAR3
            STOP
         ENDIF
         IF(JFF.LT.1 .OR. JFF.GT.8) THEN
            PRINT 7018,JFF
            STOP
         ENDIF
         IF(JZZ.LT.-MZZ .OR. JZZ.GT.MZZ) THEN
            PRINT 7006,JZZ
            STOP
         ENDIF
         call setilines(JTYP,JFF,JZZ,JOB,JLAY, RRR, ier)
         if ( ier.ne.0 ) then
            PRINT 7060
7060        FORMAT(/' In REAMDC, CSC internal alignment problem =====> STOP !')
            STOP
         endif
      ELSEIF(CAR1.EQ.COMA(11)) THEN
!--  B  Deformation in SZT of a station (object) -----------------------
!---***-----------------------------------------------------------------
         CALL GetCharAndNum(CAR128,'A3I11R',JA,JI,JR,AAA,III,RRR,Ierr)
         IF( Ierr > 0 ) THEN
          CALL GetCharAndNum(CAR128,'A3I10R',JA,JI,JR,AAA,III,RRR,Ierr)
          RRR(11) = 0.D0
          IF( Ierr > 0 ) THEN
           CALL GetCharAndNum(CAR128,'A3I9R',JA,JI,JR,AAA,III,RRR,Ierr)
           RRR(10) = 0.D0
          ENDIF
         ENDIF
         IF( Ierr > 0 ) THEN
            PRINT 7045,TRIM(CAR128)
7045        FORMAT(/' In REAMDC, line :',/'B ',A,/' is un-understandable =====> STOP !')
            STOP
         ENDIF
         CAR3 = TRIM(AAA(1))
         JFF  = III(1)
         JZZ  = III(2)
         JOB  = III(3)
         JTYP = JTROU3(CAR3,NTYP,CTYP)
         IF(JTYP.LE.0) THEN
            PRINT 7003,CAR3
            STOP
         ENDIF
         IF(JFF.LT.1 .OR. JFF.GT.8) THEN
            PRINT 7018,JFF
            STOP
         ENDIF
         IF(JZZ.LT.-MZZ .OR. JZZ.GT.MZZ) THEN
            PRINT 7006,JZZ
            STOP
         ENDIF
         IF(JOB.NE.0) THEN
            PRINT 7031
7031        FORMAT(/' In REAMDC, One SZT station',                           &
                    'deformation is given for the object JOB =',I3,/         &
                    ' but only global station deformations are taken into ', &
                    'account (JOB=0) =====> STOP !')
            STOP
         ENDIF
         call setblines (JTYP,JFF,JZZ,JOB, RRR, ier)
         if ( ier.ne.0 ) then
            PRINT 7025
7025        FORMAT(/' In REAMDC, Number of SZT station deformations is outside range =====> STOP !')
            STOP
         ENDIF
      ELSEIF(CAR1.EQ.COMA(12) .OR. CAR1.EQ.COMA(13)) THEN
!--  C  or  R  Dead MDT channels (tubes) or channels Removed from fit --
!---***----***----------------------------------------------------------
         READ (CAR78,4400) CAR3,JFF,JZZ,JOB,JLAY,(Ichan(L),L=1,20)
4400     FORMAT(A3,4I3,3X,20I3)
         JTYP = JTROU3(CAR3,NTYP,CTYP)
         IF(JTYP.LE.0) THEN
            PRINT 7003,CAR3
            STOP
         ENDIF
         IF(JFF.LT.1 .OR. JFF.GT.8) THEN
            PRINT 7018,JFF
            STOP
         ENDIF
         IF(JZZ.LT.-MZZ .OR. JZZ.GT.MZZ) THEN
            PRINT 7006,JZZ
            STOP
         ENDIF
         IF( CAR1.EQ.COMA(12) ) THEN
           CALL setclines (JTYP, JFF, JZZ, JOB, JLAY, Ichan, ier)
         ELSE
           CALL setrlines (JTYP, JFF, JZZ, JOB, JLAY, Ichan, ier)
         ENDIF
      ELSEIF(CAR1.EQ.COMA(14)) THEN
!--  X  AGDD/xml matter description section ----------------------------
!---***-----------------------------------------------------------------
         Ixml = 1 - Ixml
      ELSEIF(CAR1.EQ.COMA(16)) THEN
!--  L  Correspondance between Hard and software nbers -----------------
!---***-----------------------------------------------------------------
        HARDENTRIES = HARDENTRIES + 1
        READ (CAR78,8200)  HARDNAME   (HARDENTRIES) ,    &
                           SOFTNAME   (HARDENTRIES) ,    &
                           SOFTOCTANT (HARDENTRIES) ,    &
                           SOFTIZ     (HARDENTRIES) 
8200    FORMAT(A7,14X,A3,2I20)
      ELSEIF(CAR1.EQ.COMA(18)) THEN
!--  F  Tomo stuff from Florian Bauer ----------------------------------
!---***-----------------------------------------------------------------
1300    continue
        READ (LUN,1512,END=90) CAR512
        IF( CAR512(1:1) == COMA(18) ) GO TO 100
        XTOMOENTRIES = XTOMOENTRIES + 1
        READ (CAR512,*)    XtomoChberName (XTOMOENTRIES) ,    &
                           XtomoSite      (XTOMOENTRIES) ,    &
                           XtomoSiteId    (XTOMOENTRIES) ,    &
                           XtomoTime      (XTOMOENTRIES) ,    &
                           XtomoPassed    (XTOMOENTRIES) ,    &
                           XtomoSide      (XTOMOENTRIES) ,    &
                           XtomoNberTube1 (XTOMOENTRIES) ,    &
                           XtomoNberTube2 (XTOMOENTRIES) ,    &
                           XtomoNberML    (XTOMOENTRIES) ,    &
                           XtomoNberLayer (XTOMOENTRIES) ,    &                           
                           XtomoML1NYtub (XTOMOENTRIES) ,    &
                           XtomoML1NZtub (XTOMOENTRIES) ,    &
                           XtomoML1NDely (XTOMOENTRIES) ,    &
                           XtomoML1NDelz (XTOMOENTRIES) ,    & 
                           XtomoML1NDela (XTOMOENTRIES) ,    & 
                           XtomoML1NYpit (XTOMOENTRIES) ,    & 
                           XtomoML1NZpit (XTOMOENTRIES) ,    &                            
                           XtomoML1PYtub (XTOMOENTRIES) ,    &
                           XtomoML1PZtub (XTOMOENTRIES) ,    &
                           XtomoML1PDely (XTOMOENTRIES) ,    &
                           XtomoML1PDelz (XTOMOENTRIES) ,    & 
                           XtomoML1PDela (XTOMOENTRIES) ,    & 
                           XtomoML1PYpit (XTOMOENTRIES) ,    & 
                           XtomoML1PZpit (XTOMOENTRIES) ,    &                            
                           XtomoML2NYtub (XTOMOENTRIES) ,    &
                           XtomoML2NZtub (XTOMOENTRIES) ,    &
                           XtomoML2NDely (XTOMOENTRIES) ,    &
                           XtomoML2NDelz (XTOMOENTRIES) ,    & 
                           XtomoML2NDela (XTOMOENTRIES) ,    & 
                           XtomoML2NYpit (XTOMOENTRIES) ,    & 
                           XtomoML2NZpit (XTOMOENTRIES) ,    &                            
                           XtomoML2PYtub (XTOMOENTRIES) ,    &
                           XtomoML2PZtub (XTOMOENTRIES) ,    &
                           XtomoML2PDely (XTOMOENTRIES) ,    &
                           XtomoML2PDelz (XTOMOENTRIES) ,    & 
                           XtomoML2PDela (XTOMOENTRIES) ,    & 
                           XtomoML2PYpit (XTOMOENTRIES) ,    & 
                           XtomoML2PZpit (XTOMOENTRIES) 
       goto 1300
      ELSE
!-- Unknown command ===> STOP ! ----------------------------------------
!-----------------------------------------------------------------------
         PRINT 7015,CAR1
7015     FORMAT(/' In REAMDC, Command ',A1,' is unknown =====> STOP !'/19X,'***')
         STOP
      ENDIF
      GO TO 100
900   CONTINUE
!
!--Connect AMDCHARDSOFT
      do ihardsoft=1,HARDENTRIES
       JTYP = JTROU3(SOFTNAME   (ihardsoft),NTYP,CTYP)
       JFF  =        SOFTOCTANT (ihardsoft)
       JZZ  =        SOFTIZ     (ihardsoft)
       KTYP_F_Z = JGET_JTYP_F_Z(JTYP,JFF,JZZ)
       IentryHardSoft_V(KTYP_F_Z) = ihardsoft
      enddo
!
!--Connect XTOMO
      do ixtomo=1,XTOMOENTRIES
        idone = 0 
        do ihardsoft=1,HARDENTRIES
          if ( idone == 0 ) then
            if ( XtomoChberName (ixtomo) == HARDNAME (ihardsoft) ) then
              idone = 1 
              jtyp =  JTROU3(SOFTNAME   (ihardsoft),NTYP,CTYP)
              JFF  =        SOFTOCTANT (ihardsoft)
              JZZ  =        SOFTIZ     (ihardsoft)
              KTYP_F_Z = JGET_JTYP_F_Z(JTYP,JFF,JZZ)
              IentryXtomo_V(KTYP_F_Z) = ixtomo
           endif
         endif
        enddo
        if ( idone == 0 ) then
          print *,'In REAMDC XTOMO No way to connect*',XtomoChberName (ixtomo),'*'
        endif
      enddo
!
!
!-- Perform  Z+ / Z-  symetry ------------------------------------------
      IGEOSYM(MTYP,MGEO) = 777777
      CALL SETNIL( IGEOSYM(1,1), MTYP*MGEO, 777777 )
      DO 931 JTYP=1,MTYP
      DO 930 JFF=1,8
!
      DO 920 JZZ=1,MZZ
        IF(IGEO(JTYP,JFF, JZZ) == 0) CYCLE
        IF(IGEO(JTYP,JFF,-JZZ) /= 0) CYCLE
        IF( IBAREL(JTYP,JFF,JZZ) == 1 ) THEN
!>> Barrel !!!
           JGEO = IGEO(JTYP,JFF,JZZ)
           JCUT = ICUT(JTYP,JFF,JZZ)
!>> Warning !!!  The dimensions of the Station given by the routine
!>> Dim_Station are in  cm  !!!  (and not mmm)  and one needs these
!>> dimensions without cuts !!!
           ICUT_V( JTYP_F_Z(JTYP,JFF,JZZ) ) = 0
           CALL Dim_Station(JTYP,JFF,JZZ, SC_Station,ZC_Station,TC_Station, DS_Station,DZ_Station,DT_Station)
!
           DeltaZmZp = 20.D0*ZC_Station
           if (abs(Pos_Z_Zm(JTYP,JFF,JZZ)).ge.1.) then
             DeltaZmZp =-Pos_Z(JTYP,JFF,JZZ)-Pos_Z_Zm(JTYP,JFF,JZZ)
           else
             if (iprint.eq.1) then
             print *,' readmdb ====================='
             print *,' readmdb This station is symetrised '
             print *,' readmdb using default prescription: check'
             print *,' readmdb   CTYP(JTYP) ',CTYP(JTYP)
             print *,' readmdb   JFF        ',JFF
             print *,' readmdb   JZZ        ',JZZ
             endif
           endif
!
           ICUT_V( JTYP_F_Z(JTYP,JFF,JZZ) ) = JCUT
           IF( IGEOSYM(JTYP,JGEO) > 0 ) THEN
              JGEONEW = IGEOSYM(JTYP,JGEO)
           ELSE
              DO J=1,MGEO
               IF( NOBJ(JTYP,J) < 1 ) GO TO 910
              ENDDO
                 PRINT 7004,J+100
                 STOP
910           JGEONEW = J
              IGEOSYM(JTYP,JGEO) = JGEONEW
              NOBJ(JTYP,JGEONEW)  =   NOBJ  (JTYP,JGEO)
              DO JOB=1,NOBJ(JTYP,JGEO)
               KTYP_G_O = JGET_JTYP_G_O(JTYP,JGEONEW,JOB)
               Geo_dx_V(KTYP_G_O) =   Geo_dx(JTYP,JGEO,JOB)
               Geo_dy_V(KTYP_G_O) =   DeltaZmZp - Geo_dy(JTYP,JGEO,JOB) - Geo_Le(JTYP,JGEO,JOB)
               Geo_dz_V(KTYP_G_O) =   Geo_dz(JTYP,JGEO,JOB)
               Geo_Ws_V(KTYP_G_O) =   Geo_Ws(JTYP,JGEO,JOB)
               Geo_Wl_V(KTYP_G_O) =   Geo_Wl(JTYP,JGEO,JOB)
               Geo_Le_V(KTYP_G_O) =   Geo_Le(JTYP,JGEO,JOB)
               Geo_Ex_V(KTYP_G_O) = - Geo_Ex(JTYP,JGEO,JOB)
               Geo_D1_V(KTYP_G_O) =   Geo_D1(JTYP,JGEO,JOB)
               Geo_D2_V(KTYP_G_O) =   Geo_D2(JTYP,JGEO,JOB)
               Geo_D3_V(KTYP_G_O) =   Geo_D3(JTYP,JGEO,JOB)
               ITEC_V  (KTYP_G_O) =   ITEC  (JTYP,JGEO,JOB)
               ISTA_V  (KTYP_G_O) =   ISTA  (JTYP,JGEO,JOB)
               ISPLIX_V(KTYP_G_O) =   ISPLIX(JTYP,JGEO,JOB)
               ISPLIY_V(KTYP_G_O) =   ISPLIY(JTYP,JGEO,JOB)
               ISHAPE_V(KTYP_G_O) =   ISHAPE(JTYP,JGEO,JOB)
              ENDDO
           ENDIF
           IF( JCUT > 0 ) THEN
             IF( INOCUT(JTYP,JGEONEW,JCUT) < 1 ) THEN
               ITOTCUOLD = INOCUT(JTYP,JGEO,JCUT)
               ITOTCU = ITOTCU + 1
               IF( ITOTCU > MCUT ) THEN
                 PRINT 7012,ITOTCU
                 STOP
               ENDIF
               INOCUT(JTYP,JGEONEW,JCUT) = ITOTCU
               NOBCUT (ITOTCU)     =   NOBCUT(ITOTCUOLD)
               DO JOB=1,NOBCUT(ITOTCUOLD)
                 IOBCUT(ITOTCU,JOB) =   IOBCUT(ITOTCUOLD,JOB)
                 Cut_dx(ITOTCU,JOB) =   Cut_dx(ITOTCUOLD,JOB)
                 Cut_dy(ITOTCU,JOB) =   DeltaZmZp - Cut_dy(ITOTCUOLD,JOB) - Cut_Le(ITOTCUOLD,JOB)
                 Cut_Ws(ITOTCU,JOB) =   Cut_Wl(ITOTCUOLD,JOB)
                 Cut_Wl(ITOTCU,JOB) =   Cut_Ws(ITOTCUOLD,JOB)
                 Cut_Le(ITOTCU,JOB) =   Cut_Le(ITOTCUOLD,JOB)
                 Cut_Ex(ITOTCU,JOB) = - Cut_Ex(ITOTCUOLD,JOB)
                 Cut_An(ITOTCU,JOB) = - Cut_An(ITOTCUOLD,JOB)
               ENDDO
             ENDIF
           ENDIF
           KTYP_F_Z = JGET_JTYP_F_Z(JTYP,JFF,-JZZ)
           IGEO_V    (KTYP_F_Z) =   JGEONEW
           ICUT_V    (KTYP_F_Z) =   JCUT
           IBAREL_V  (KTYP_F_Z) =   IBAREL  (JTYP,JFF,JZZ)
           ICSCCH_V  (KTYP_F_Z) =   ICSCCH  (JTYP,JFF,JZZ)
           Pos_Phi_V (KTYP_F_Z) =   Pos_Phi (JTYP,JFF,JZZ)
           Pos_Z_V   (KTYP_F_Z) = - Pos_Z   (JTYP,JFF,JZZ) - DeltaZmZp
           Pos_R_V   (KTYP_F_Z) =   Pos_R   (JTYP,JFF,JZZ)
           Pos_S_V   (KTYP_F_Z) =   Pos_S   (JTYP,JFF,JZZ)
           Pos_Alfa_V(KTYP_F_Z) =   Pos_Alfa(JTYP,JFF,JZZ)
           Pos_Beta_V(KTYP_F_Z) =   Pos_Beta(JTYP,JFF,JZZ)
           Pos_Gama_V(KTYP_F_Z) =   Pos_Gama(JTYP,JFF,JZZ)
           NSECTO   (JTYP,-JZZ) =   NSECTO  (JTYP,JZZ)
        ELSEIF( IBAREL(JTYP,JFF,JZZ) == 0 ) THEN
!-- End-Caps !!!
           IF( IVERSION_AMDC < 7 ) THEN
             KTYP_F_Z = JGET_JTYP_F_Z(JTYP,JFF,-JZZ)
             IGEO_V    (KTYP_F_Z) =   IGEO    (JTYP,JFF,JZZ)
             ICUT_V    (KTYP_F_Z) =   ICUT    (JTYP,JFF,JZZ)
             IBAREL_V  (KTYP_F_Z) =   IBAREL  (JTYP,JFF,JZZ)
             ICSCCH_V  (KTYP_F_Z) =   ICSCCH  (JTYP,JFF,JZZ)
             Pos_Phi_V (KTYP_F_Z) =   Pos_Phi (JTYP,JFF,JZZ)
             Pos_Z_V   (KTYP_F_Z) = - Pos_Z   (JTYP,JFF,JZZ)
             Pos_R_V   (KTYP_F_Z) =   Pos_R   (JTYP,JFF,JZZ)
             Pos_S_V   (KTYP_F_Z) =   Pos_S   (JTYP,JFF,JZZ)
             Pos_Alfa_V(KTYP_F_Z) =   Pos_Alfa(JTYP,JFF,JZZ)
             Pos_Beta_V(KTYP_F_Z) =   Pos_Beta(JTYP,JFF,JZZ)
             Pos_Gama_V(KTYP_F_Z) =   Pos_Gama(JTYP,JFF,JZZ)
             NSECTO   (JTYP,-JZZ) =   NSECTO  (JTYP,JZZ)
           ELSE
             JGEO = IGEO(JTYP,JFF,JZZ)
             JCUT = ICUT(JTYP,JFF,JZZ)
             IF( IGEOSYM(JTYP,JGEO) > 0 ) THEN 
                JGEONEW = IGEOSYM(JTYP,JGEO)
             ELSE
                DO J=1,MGEO
                 IF( NOBJ(JTYP,J) < 1 ) GO TO 911
                ENDDO
                   PRINT 7004,J+100
                   STOP
911             JGEONEW = J
                IGEOSYM(JTYP,JGEO) = JGEONEW
                NOBJ(JTYP,JGEONEW)  =   NOBJ  (JTYP,JGEO)
                DO JOB=1,NOBJ(JTYP,JGEO)
                 KTYP_G_O = JGET_JTYP_G_O(JTYP,JGEONEW,JOB)
                 Geo_dx_V(KTYP_G_O) = - Geo_dx(JTYP,JGEO,JOB)
                 Geo_dy_V(KTYP_G_O) =   Geo_dy(JTYP,JGEO,JOB)
                 Geo_dz_V(KTYP_G_O) =   Geo_dz(JTYP,JGEO,JOB)
                 Geo_Ws_V(KTYP_G_O) =   Geo_Ws(JTYP,JGEO,JOB)
                 Geo_Wl_V(KTYP_G_O) =   Geo_Wl(JTYP,JGEO,JOB)
                 Geo_Le_V(KTYP_G_O) =   Geo_Le(JTYP,JGEO,JOB)
                 IF( CARTEC(ITEC(JTYP,JGEO,JOB)) == 'CSC' ) THEN
                    Geo_Ex_V(KTYP_G_O) =   Geo_Ex(JTYP,JGEO,JOB)
                 ELSE
                    Geo_Ex_V(KTYP_G_O) = - Geo_Ex(JTYP,JGEO,JOB)
! JFL: Mon Feb 26 22:11:36 CET 2007
                    if( cartyp(jtyp) == 'CSS' .or. cartyp(jtyp) == 'CSL')  Geo_Ex_V(KTYP_G_O) =   Geo_Ex(JTYP,JGEO,JOB)
! JFL: Mon Feb 26 22:11:36 CET 2007
                 ENDIF
                 Geo_D1_V(KTYP_G_O) =   Geo_D1(JTYP,JGEO,JOB)
                 Geo_D2_V(KTYP_G_O) =   Geo_D2(JTYP,JGEO,JOB)
                 Geo_D3_V(KTYP_G_O) =   Geo_D3(JTYP,JGEO,JOB)
                 ITEC_V  (KTYP_G_O) =   ITEC  (JTYP,JGEO,JOB)
                 ISTA_V  (KTYP_G_O) =   ISTA  (JTYP,JGEO,JOB)
                 ISPLIX_V(KTYP_G_O) =   ISPLIX(JTYP,JGEO,JOB)
                 ISPLIY_V(KTYP_G_O) =   ISPLIY(JTYP,JGEO,JOB)
                 ISHAPE_V(KTYP_G_O) =   ISHAPE(JTYP,JGEO,JOB)
                ENDDO
             ENDIF
             IF( JCUT > 0 ) THEN
               IF( INOCUT(JTYP,JGEONEW,JCUT) < 1 ) THEN
                 ITOTCUOLD = INOCUT(JTYP,JGEO,JCUT)
                 ITOTCU = ITOTCU + 1
                 IF( ITOTCU > MCUT ) THEN
                   PRINT 7012,ITOTCU
                   STOP
                 ENDIF
                 INOCUT(JTYP,JGEONEW,JCUT) = ITOTCU
                 NOBCUT (ITOTCU)     =   NOBCUT(ITOTCUOLD)
                 DO JOB=1,NOBCUT(ITOTCUOLD)
                   IOBCUT(ITOTCU,JOB) =   IOBCUT(ITOTCUOLD,JOB)
                   Cut_dx(ITOTCU,JOB) = - Cut_dx(ITOTCUOLD,JOB)
                   Cut_dy(ITOTCU,JOB) =   Cut_dy(ITOTCUOLD,JOB)
                   Cut_Ws(ITOTCU,JOB) =   Cut_Wl(ITOTCUOLD,JOB)
                   Cut_Wl(ITOTCU,JOB) =   Cut_Ws(ITOTCUOLD,JOB)
                   Cut_Le(ITOTCU,JOB) =   Cut_Le(ITOTCUOLD,JOB)
                   Cut_Ex(ITOTCU,JOB) = - Cut_Ex(ITOTCUOLD,JOB)
                   Cut_An(ITOTCU,JOB) =   Cut_An(ITOTCUOLD,JOB)
                 ENDDO
               ENDIF
             ENDIF
             KTYP_F_Z = JGET_JTYP_F_Z(JTYP,JFF,-JZZ)
             IGEO_V    (KTYP_F_Z) =   JGEONEW
             ICUT_V    (KTYP_F_Z) =   JCUT
             IBAREL_V  (KTYP_F_Z) =   IBAREL  (JTYP,JFF,JZZ)
             ICSCCH_V  (KTYP_F_Z) =   ICSCCH  (JTYP,JFF,JZZ)
             Pos_Phi_V (KTYP_F_Z) =   Pos_Phi (JTYP,JFF,JZZ)
             Pos_Z_V   (KTYP_F_Z) = - Pos_Z   (JTYP,JFF,JZZ)
             Pos_R_V   (KTYP_F_Z) =   Pos_R   (JTYP,JFF,JZZ)
             Pos_S_V   (KTYP_F_Z) =   Pos_S   (JTYP,JFF,JZZ)
             Pos_Alfa_V(KTYP_F_Z) = - Pos_Alfa(JTYP,JFF,JZZ)
             Pos_Beta_V(KTYP_F_Z) = - Pos_Beta(JTYP,JFF,JZZ)
             Pos_Gama_V(KTYP_F_Z) =   Pos_Gama(JTYP,JFF,JZZ)
             NSECTO   (JTYP,-JZZ) =   NSECTO  (JTYP,JZZ)
           ENDIF
        ENDIF
920   CONTINUE
!
      IF( IVERSION_AMDC > 6 ) THEN
         DO JZZ=-MZZ,MZZ
          IF(IGEO(JTYP,JFF, JZZ) == 0) CYCLE
             KTYP_F_Z = JGET_JTYP_F_Z(JTYP,JFF,JZZ)
             IF( IBAREL(JTYP,JFF,JZZ) < 1 ) THEN
                IBAREL_V(KTYP_F_Z) = 0
             ELSE
                IBAREL_V(KTYP_F_Z) = 1
             ENDIF
             IF( ICSCCH(JTYP,JFF,JZZ) < 1 ) THEN
                ICSCCH_V(KTYP_F_Z) = 0
             ELSE
                ICSCCH_V(KTYP_F_Z) = 1
             ENDIF
         ENDDO
      ENDIF
!
930   CONTINUE
931   CONTINUE
!-----------------------------------------------------------------------
!
!
!
!-- Check of the SZT station (object) position adjustments -------------
      DO J1=1,NBadjust
       Icode = JTYPFZOLadjust(J1)
       CALL DECO_JTYP_F_Z_O_L(Icode, JTYP,JFF,JZZ,JOB,JLAY)
       DO J2=J1+1,NBadjust
        Icode = JTYPFZOLadjust(J2)
        CALL DECO_JTYP_F_Z_O_L(Icode, JTYP2,JFF2,JZZ2,JOB2,JLAY2)
        IF(  JTYP.EQ.JTYP2 .AND. JFF.EQ.JFF2 .AND. JZZ.EQ.JZZ2  .AND. JOB.EQ.JOB2 ) THEN
           PRINT 7022,CARTYP(JTYP),JFF,JZZ,JOB
7022       FORMAT(/' In REAMDC, SZT position adjustment',                     &
                   ' for the station (object) : Type,JFF,JZZ,JOB = ',A3,3I3,  &
                   ' is defined several times =====> STOP !')
           STOP
        ENDIF
       ENDDO
       JGEO = IGEO(JTYP,JFF,JZZ)
       IF( JGEO.LE.0 ) THEN
          PRINT 7023,CARTYP(JTYP),JFF,JZZ,JOB
7023      FORMAT(/' In REAMDC, SZT position adjustment is defined',          &
                  ' for the station (object) : Type,JFF,JZZ,JOB = ',A3,3I3,  &
                  ' which does not exist =====> STOP !')
          STOP
       ENDIF
       IF( JOB.LT.0 .OR. JOB.GT.NOBJ(JTYP,JGEO) ) THEN
          PRINT 7023,CARTYP(JTYP),JFF,JZZ,JOB
          STOP
       ENDIF
      ENDDO
!-- Check of the SZT station (object) deformations ---------------------
      DO J1=1,NBdeform
       Icode = JTYPFZOLdeform(J1)
       CALL DECO_JTYP_F_Z_O_L(Icode, JTYP,JFF,JZZ,JOB,JLAY)
       DO J2=J1+1,NBdeform
        Icode = JTYPFZOLdeform(J2)
        CALL DECO_JTYP_F_Z_O_L(Icode, JTYP2,JFF2,JZZ2,JOB2,JLAY2)
        IF(  JTYP.EQ.JTYP2 .AND. JFF.EQ.JFF2 .AND. JZZ.EQ.JZZ2  .AND. JOB.EQ.JOB2 ) THEN
           PRINT 7026,CARTYP(JTYP),JFF,JZZ,JOB
7026       FORMAT(/' In REAMDC, SZT deformation',                             &
                   ' for the station (object) : Type,JFF,JZZ,JOB = ',A3,3I3,  &
                   ' is defined several times =====> STOP !')
           STOP
        ENDIF
       ENDDO
       JGEO = IGEO(JTYP,JFF,JZZ)
       IF( JGEO.LE.0 ) THEN
          PRINT 7027,CARTYP(JTYP),JFF,JZZ,JOB
7027      FORMAT(/' In REAMDC, SZT deformation is defined',                  &
                  ' for the station (object) : Type,JFF,JZZ,JOB = ',A3,3I3,  &
                  ' which does not exist =====> STOP !')
          STOP
       ENDIF
       IF( JOB.LT.0 .OR. JOB.GT.NOBJ(JTYP,JGEO) ) THEN
          PRINT 7027,CARTYP(JTYP),JFF,JZZ,JOB
          STOP
       ENDIF
      ENDDO
!-- Check of the SZT station (object) CSC internalaligment -------------
      DO J1=1,NBdwnlay
       Icode = JTYPFZOLdwnlay(J1)
       CALL DECO_JTYP_F_Z_O_L(Icode, JTYP,JFF,JZZ,JOB,JLAY)
       DO J2=J1+1,NBdwnlay
        Icode = JTYPFZOLdwnlay(J2)
        CALL DECO_JTYP_F_Z_O_L(Icode, JTYP2,JFF2,JZZ2,JOB2,JLAY2)
        IF(  JTYP.EQ.JTYP2 .AND. JFF.EQ.JFF2 .AND. JZZ.EQ.JZZ2  .AND. JOB.EQ.JOB2 &
        .AND. JLAY.EQ.JLAY2 ) THEN
           PRINT 7038,CARTYP(JTYP),JFF,JZZ,JOB,JLAY
7038       FORMAT(/' In REAMDC, SZT position CSC internal adjustment',                     &
                   ' for the station (object) : Type,JFF,JZZ,JOB,JLAY = ',A3,4I3,  &
                   ' is defined several times =====> STOP !')
           STOP
        ENDIF
       ENDDO
       JGEO = IGEO(JTYP,JFF,JZZ)
       IF( JGEO.LE.0 ) THEN
          PRINT 7040,CARTYP(JTYP),JFF,JZZ,JOB,JLAY
7040      FORMAT(/' In REAMDC, SZT position CSC internal adjustment is defined',          &
                  ' for the station (object) : Type,JFF,JZZ,JOB = ',A3,4I3,  &
                  ' which does not exist =====> STOP !')
          STOP
       ENDIF
       IF( JOB.LT.0 .OR. JOB.GT.NOBJ(JTYP,JGEO) ) THEN
          PRINT 7040,CARTYP(JTYP),JFF,JZZ,JOB
          STOP
       ENDIF
      ENDDO
      CALL SETICSC
!-- Check of the DEAD channels -----------------------------------------
      DO J=1,NBdeadChan
       Icode = JTYPFZOLdeadChan(J)
       CALL DECO_JTYP_F_Z_O_L(Icode, JTYP,JFF,JZZ,JOB,JLAY)
       JGEO = IGEO(JTYP,JFF,JZZ)
       IF( JGEO.LE.0 ) THEN
          PRINT 7033,CARTYP(JTYP),JFF,JZZ,JOB
7033      FORMAT(/' In REAMDC, dead (or removed) channel numbers are ',         &
                  'given for the station object : Type,JFF,JZZ,JOB = ',A3,3I3,  &
                  ' which does not exist =====> STOP !')
          STOP
       ENDIF
       JTEC = ITEC(JTYP,JGEO,JOB)
       IF(   CARTEC(JTEC).NE.'MDT' .AND. CARTEC(JTEC).NE.'RPC' .AND. CARTEC(JTEC).NE.'TGC' ) THEN
          PRINT 7034,CARTYP(JTYP),JFF,JZZ,JOB
7034      FORMAT(/' In REAMDC, dead (or removed) channel numbers are ',          &
                  'given for the station object : Type,JFF,JZZ,JOB = ',A3,3I3,   &
                  ' which is not of type MDT or RPC or TGC =====> STOP !')
          STOP
       ENDIF
       JSTA = ISTA(JTYP,JGEO,JOB)
       NLAY = MAX( NLAS(JTEC,JSTA), NLAZ(JTEC,JSTA) )
       IF( NLAY.GT.6 ) NLAY = (NLAY-3) / 4
       IF( JLAY.LT.0 .OR. JLAY.GT.NLAY ) THEN
          PRINT 7035,CARTYP(JTYP),JFF,JZZ,JOB,CARTEC(JTEC),JLAY
7035      FORMAT(/' In REAMDC, dead (or removed) channel numbers are ',         &
                  'given for the station object : Type,JFF,JZZ,JOB = ',A3,3I3,  &
                  ' of technology ',A3,/' but for layer : JLAY =',I3,           &
                  ' which does not exist =====> STOP !')
          STOP
       ENDIF
      ENDDO
!-----------------------------------------------------------------------
!
!
!
!-- Convert TGC radial coordinates to cartesian coordinates ------------
      IF(IRADIA.EQ.0) CALL TGCCV
!-----------------------------------------------------------------------
!
!
!
!-- Go to 4 layers of tubes instead of 3 in each m.l. ? ----------------
      IF(I3TO4.EQ.1) THEN
         DO JTEC=1,MTEC
           IF(CARTEC(JTEC).NE.'MDT') CYCLE
           IF( NLAZ(JTEC,1).NE.3 .OR. NLAZ(JTEC,2).NE.3 .OR.   &
               NLAZ(JTEC,3).NE.3 .OR. NLAZ(JTEC,5).NE.4 .OR.   &
               NLAZ(JTEC,6).NE.4 .OR. NLAZ(JTEC,7).NE.4 ) THEN
              PRINT 7020
7020          FORMAT(/' ===> In AMDC_simrec, the MDT stacking types ',         &
                      'do not allow to switch from 3 to 4 layers of tubes '    &
                     /' ===> in each multilayer as required  ===>  STOP !!!')
              STOP
           ENDIF
         ENDDO
         DO 951 JTYP=1,MTYP
           DO 950 JGEO=1,MGEO
             IF(NOBJ(JTYP,JGEO).LE.0) CYCLE
             DO JOB=1,NOBJ(JTYP,JGEO)
               JTEC = ITEC(JTYP,JGEO,JOB)
               IF(CARTEC(JTEC).NE.'MDT') CYCLE
               KTYP_G_O = JGET_JTYP_G_O(JTYP,JGEO,JOB)
               IF( ISTA(JTYP,JGEO,JOB).EQ.1 ) ISTA_V(KTYP_G_O) = 5
               IF( ISTA(JTYP,JGEO,JOB).EQ.2 ) THEN
                 IF( NLAZ(JTEC,8).EQ.4 ) THEN
                                              ISTA_V(KTYP_G_O) = 8
                 ELSE
                                              ISTA_V(KTYP_G_O) = 6
                 ENDIF
               ENDIF
               IF( ISTA(JTYP,JGEO,JOB).EQ.3 ) ISTA_V(KTYP_G_O) = 7
             ENDDO
950        ENDDO
951      ENDDO
         PRINT 7021
7021     FORMAT(/' ===> During reading of AMDC_simrec, the MDT ',              &
                 'multilayers with 3 layers of tubes have been '               &
                /' ===> transformed to multilayers with 4 layers of tubes !')
      ENDIF
!-----------------------------------------------------------------------
!
!
!
!-- Final checks, re-shuffling and (possibly) prints -------------------
      DO 962 JTYP=1,MTYP
      IF(IPRINT.EQ.1) PRINT 8001
8001  FORMAT('*')
      DO 961 JGEO=1,MGEO
      IF(NOBJ(JTYP,JGEO).GE.1) THEN
         IF(IPRINT.EQ.1) PRINT 6100,CTYP(JTYP),JGEO,NOBJ(JTYP,JGEO)
6100     FORMAT('D ',A3,I2,2X,I2)
         DO 955 JOB=1,NOBJ(JTYP,JGEO)
         JTEC = ITEC(JTYP,JGEO,JOB)
         JSTA = ISTA(JTYP,JGEO,JOB)
         NLLL = NLAZ(JTEC,JSTA)+NLAS(JTEC,JSTA)
         IF(NLLL.LE.0) THEN
            PRINT 7008,CTEC(JTEC),JSTA
7008        FORMAT(/' In REAMDC, Stacking type : ',A3,I3,                       &
                    ' is used but is not defined =====> STOP !'/27X,'********')
            STOP
         ENDIF
         KTYP_G_O = JGET_JTYP_G_O(JTYP,JGEO,JOB)
         IF(CTEC(JTEC).EQ.'MDT') Geo_dz_V(KTYP_G_O) = Geo_dz(JTYP,JGEO,JOB) + STAEMI(JTEC,JSTA)
         IF(IPRINT.EQ.1) PRINT 3200,Geo_dx(JTYP,JGEO,JOB),  Geo_dy(JTYP,JGEO,JOB),  &
                                    Geo_dz(JTYP,JGEO,JOB),  JOB, CTEC(JTEC), JSTA,  &
                                    ISPLIX(JTYP,JGEO,JOB),  ISPLIY(JTYP,JGEO,JOB),  &
                                    ISHAPE(JTYP,JGEO,JOB),                          &
                                    Geo_Ws(JTYP,JGEO,JOB),  Geo_Wl(JTYP,JGEO,JOB),  &
                                    Geo_Le(JTYP,JGEO,JOB),  Geo_Ex(JTYP,JGEO,JOB),  &
                                    Geo_D1(JTYP,JGEO,JOB),  Geo_D2(JTYP,JGEO,JOB),  &
                                    Geo_D3(JTYP,JGEO,JOB)
955      CONTINUE
      ENDIF
      DO 960 JCUT=1,4
      IF(INOCUT(JTYP,JGEO,JCUT).GE.1) THEN
         ITOTCU = INOCUT(JTYP,JGEO,JCUT)
         IF(IPRINT.EQ.1) PRINT 6200,CTYP(JTYP),JGEO,JCUT,NOBCUT(ITOTCU)
6200     FORMAT('H ',A3,3I2)
         DO 957 JOB=1,NOBCUT(ITOTCU)
         IF( IOBCUT(ITOTCU,JOB).LE.0 .OR. IOBCUT(ITOTCU,JOB).GT.NOBJ(JTYP,JGEO) ) THEN
            PRINT 7014,CTYP(JTYP),JGEO,JCUT,IOBCUT(ITOTCU,JOB)
7014        FORMAT(/' In REAMDC, In cut-out type : ',A3,2I3,            &
                    ' object index : ',I3,                              &
                    ' is not defined =====> STOP !'/29X,'***********')
            STOP
         ENDIF
         IF(IPRINT.EQ.1) PRINT 3400,Cut_dx(ITOTCU,JOB), Cut_dy(ITOTCU,JOB),   &
                                    IOBCUT(ITOTCU,JOB),                       &
                                    Cut_Ws(ITOTCU,JOB), Cut_Wl(ITOTCU,JOB),   &
                                    Cut_Le(ITOTCU,JOB),                       &
                                    Cut_Ex(ITOTCU,JOB), Cut_An(ITOTCU,JOB)
957      CONTINUE
      ENDIF
960   CONTINUE
961   CONTINUE
962   CONTINUE
!
      DO 982 JTYP=1,MTYP
      IF(IPRINT.EQ.1) PRINT 8001
      DO 981 JFF=1,8
      DO 980 JZZ=-MZZ,MZZ
      JGEO = IGEO(JTYP,JFF,JZZ)
      IF(JGEO.LE.0) CYCLE
         JCUT = ICUT(JTYP,JFF,JZZ)
         IF(NOBJ(JTYP,JGEO).LE.0) THEN
            PRINT 7007,CTYP(JTYP),JGEO
7007        FORMAT(/' In REAMDC, Station type : ',A3,I3,                         &
                    ' is used but is not defined =====> STOP !'/26X,'********')
            STOP
         ENDIF
         IF(JCUT.GE.1) THEN
          IF(INOCUT(JTYP,JGEO,JCUT).LE.0) THEN
            PRINT 7013,CTYP(JTYP),JGEO,JCUT
7013        FORMAT(/' In REAMDC, Cut-out type : ',A3,2I3,                          &
                    ' is used but is not defined =====> STOP !'/26X,'***********')
            STOP
          ENDIF
         ENDIF
         LFI(1:8) = 0
         LFI(JFF) = 1
         IF( IVERSION_AMDC > 6 ) LFI(JFF) = 2
         PI2SN = TWOPI / NSECTO(JTYP,JZZ)
         IF(IPRINT.EQ.1) THEN
           PHI0 = ( Pos_Phi(JTYP,JFF,JZZ) - (JFF-1.)*PI2SN ) / PIS180
           IF( NSECTO(JTYP,JZZ).EQ.8 ) THEN
             PRINT 5000, CTYP(JTYP), JGEO, JCUT, (LFI(L),L=1,8), JZZ, PHI0,   &
                         Pos_Z(   JTYP,JFF,JZZ), Pos_R   (JTYP,JFF,JZZ),      &
                         Pos_S(   JTYP,JFF,JZZ), Pos_Alfa(JTYP,JFF,JZZ),      &
                         Pos_Beta(JTYP,JFF,JZZ), Pos_Gama(JTYP,JFF,JZZ)
           ELSE
             PRINT 5100, CTYP(JTYP), JGEO, JCUT, NSECTO(JTYP,JZZ), (LFI(L),L=1,6), JZZ, PHI0,   &
                         Pos_Z   (JTYP,JFF,JZZ), Pos_R   (JTYP,JFF,JZZ),                        &
                         Pos_S   (JTYP,JFF,JZZ), Pos_Alfa(JTYP,JFF,JZZ),                        &
                         Pos_Beta(JTYP,JFF,JZZ), Pos_Gama(JTYP,JFF,JZZ)
           ENDIF
5000     FORMAT('P ',A3,2I2,1X,   8I1,   I3,3F9.2,4F8.2)
5100     FORMAT('P ',A3,2I2,1X,I1,1X,6I1,I3,3F9.2,4F8.2)
         ENDIF
980   CONTINUE
981   CONTINUE
982   CONTINUE
!
      DO 991 JTEC=1,MTEC
      DO 990 JSTA=1,MSTA
      NLL = NLAZ(JTEC,JSTA) + NLAS(JTEC,JSTA)
      IF(NLL.LE.0) CYCLE
        IF(IPRINT.EQ.1) THEN
         PRINT 9100,CTEC(JTEC), JSTA, NLAS(JTEC,JSTA),NLAZ(JTEC,JSTA),   &
                    STAPP(JTEC,JSTA),STARR(JTEC,JSTA),STAEE(JTEC,JSTA)
9100     FORMAT(2X,A3,I2,5X,2I2,8F8.2)
         PRINT 9200,STATT(JTEC,JSTA,1:8)
         PRINT 9200,STAOO(JTEC,JSTA,1:8)
9200     FORMAT(16X,8F8.2)
        ENDIF
990   CONTINUE
991   CONTINUE
!-----------------------------------------------------------------------
!
!
!
      PRINT 7000,C_AMDC_NAME,IVERSION_AMDC,I_AMDC_VERSION
7000  FORMAT(/' ===> AMDB_SIMREC data base named : ',A16,' version :',I3,  &
              ' has been read by routine REAMDC version :',I3              &
             /' ===>',30X,'******************',9X,'****',41X,'****'/)
!
!
!-- Compute indexes for specific technologies (MDT, RPC, TGC, ...) -----
!-- following the "official" ATLAS muon indexation ---------------------
      CALL ComputeIndexTEC
!-----------------------------------------------------------------------
!
 END SUBROUTINE REAMDC
!
