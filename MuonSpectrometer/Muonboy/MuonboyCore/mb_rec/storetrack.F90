!
 SUBROUTINE storeTrack
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_Fit
 USE M_MB_FAX
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER :: L, ISCF, J, I8P, I4P, I4, ISC, I1, I2, It, IC
 INTEGER :: is, icode, ict, irpc, NBCFAX, IPOS, IOLD
 INTEGER :: igeom, jtyp, jff, jzz, job, ip, jsz, jtube, ident, jsl, jspli, jstrip
 REAL(8) :: xyz(3), szflag
 CHARACTER(3) :: ctyp
!
    ITXCAN(1:12,NBCAN) = 0
    NZXCAN(     NBCAN) = 0
    NSXCAN(     NBCAN) = 0
    NSSCAN(     NBCAN) = 0
    NSZCAN(     NBCAN) = 0
    NZOCAN(     NBCAN) = 0
    NHXCAN(     NBCAN) = 0
    NBXCAN(     NBCAN) = 0
    NBCFAX = NBFAX
    IF( NCFAX > 0 ) NBCFAX = NBFAX + 1
    DO L=NBCFAX,1,-1
      DO J=1,2
        DO I8P=1,N8M
          IF( JSFIXE(I8P,L,J) <= -20000000 ) THEN  ! holes for RPC/TGC
            IF( NHXCAN(NBCAN) >= NholMA )   CYCLE
            NHXCAN(NBCAN) = NHXCAN(NBCAN) + 1
            ident = - ISFIXE(I8P,L,J)
            ict    = MOD( ident, 100000 )
            ip     =      ident/100000
            CALL IDECI( JDBTRI(ict), jtyp,jff,jzz,job )
            jsz   = 1
            jsl   = ip
            irpc  = IRPTGC(ict)
            IF( ip > N1BAND(irpc) ) THEN
              jsz = 2
              jsl = ip - N1BAND(irpc)
            ENDIF
            xyz(1) = 10.d0*XSDFIXE(I8P,L,J)
            xyz(2) = 10.d0*YSDFIXE(I8P,L,J)
            xyz(3) = 10.d0*ZSDFIXE(I8P,L,J)
            CALL FGetStationType(jtyp, ctyp)
            IF( ict <= NTRIBA ) THEN
              do jspli=1,4
                CALL FGetRPCindexFromStripPosition(ctyp,jff,jzz,job,jspli,jsl,jsz,xyz,jstrip)
                IF( jstrip /= 0 ) exit
              enddo
              TYHCAN(NHXCAN(NBCAN),NBCAN) = 2
            ELSE
              jspli = 1
              CALL FGetTGCindexFromChannelPos(ctyp,jff,jzz,job,jspli,jsl,jsz,xyz,jstrip)
              TYHCAN(NHXCAN(NBCAN),NBCAN) = 3
            ENDIF
            IF( jstrip == 0 ) THEN
              IF( MBPri >= 1 ) WRITE(*,*) ' Muonboy:storetrack hole typ/jsl/jsz ',ctyp,jsl,jsz,' outside station'
              NHXCAN(NBCAN) = NHXCAN(NBCAN) - 1
              CYCLE
            ENDIF
            CALL Code_Ident_AMDC(jtyp, jff, jzz, job, jsl, jsz, jstrip, ident)
            IHXCAN(NHXCAN(NBCAN),NBCAN) = ident
            ENHCAN(NHXCAN(NBCAN),NBCAN) = VXXCAN(7,NBCAN)*VXXCAN(8,NBCAN)
            XHACAN(NHXCAN(NBCAN),NBCAN) = XSDFIXE(I8P,L,J)
            YHACAN(NHXCAN(NBCAN),NBCAN) = YSDFIXE(I8P,L,J)
            ZHACAN(NHXCAN(NBCAN),NBCAN) = ZSDFIXE(I8P,L,J)
            XHdCAN(NHXCAN(NBCAN),NBCAN) = XSDdFIXE(I8P,L,J)
            YHdCAN(NHXCAN(NBCAN),NBCAN) = YSDdFIXE(I8P,L,J)
            ZHdCAN(NHXCAN(NBCAN),NBCAN) = ZSDdFIXE(I8P,L,J)
          ELSE
            IF( ISFIXE(I8P,L,J) <= 0 )     CYCLE
            IF( NSXCAN(NBCAN) >= NSXMAX )  EXIT
            NSXCAN(NBCAN) = NSXCAN(NBCAN) + 1
            ISXCAN(NSXCAN(NBCAN),NBCAN) = ISFIXE(I8P,L,J)
            XSDCCAN(NSXCAN(NBCAN),NBCAN) = XSDFIXE(I8P,L,J) 
            YSDCCAN(NSXCAN(NBCAN),NBCAN) = YSDFIXE(I8P,L,J) 
            ZSDCCAN(NSXCAN(NBCAN),NBCAN) = ZSDFIXE(I8P,L,J) 
            XSDCdCAN(NSXCAN(NBCAN),NBCAN)= XSDdFIXE(I8P,L,J)
            YSDCdCAN(NSXCAN(NBCAN),NBCAN)= YSDdFIXE(I8P,L,J) 
            ZSDCdCAN(NSXCAN(NBCAN),NBCAN)= ZSDdFIXE(I8P,L,J)
!!>> JFL Sat Apr 14 18:38:35 CEST 2007
!!>> TEMPORARY SOLUTION
!           ESNCCAN(NSXCAN(NBCAN),NBCAN) = ESNFIXE(I8P,L,J) 
            ESNCCAN(NSXCAN(NBCAN),NBCAN) = VXXCAN(7,NBCAN)*VXXCAN(8,NBCAN)
            RSDCCAN(NSXCAN(NBCAN),NBCAN) = RSDFIXE(I8P,L,J) 
            IF( CSFIXE(I8P,L,J) < 5.d0 ) THEN
              is = ISFIXE(I8P,L,J)
              icode = KBANHI(is) / 10
              ict   = icode / 100
              ip    = icode - ict*100
              irpc  = IRPTGC(ict)
              IF( ip <= N1BAND(irpc) ) THEN
                NSSCAN(NBCAN) = NSSCAN(NBCAN) + 1
              ELSE
                NSZCAN(NBCAN) = NSZCAN(NBCAN) + 1
              ENDIF
            ENDIF
          ENDIF
        ENDDO
        IF( L > NBFAX ) CYCLE
        DO I4P=1,N4M
          DO I4=1,4
            IF( JZFIXE(I4P,I4,L,J) <= -20000000 ) THEN  ! holes for MDT/CSC
              NBXCAN(NBCAN) = NBXCAN(NBCAN) + 1
              IF( NHXCAN(NBCAN) >= NholMA )   CYCLE
              NHXCAN(NBCAN) = NHXCAN(NBCAN) + 1
              igeom = MOD( IDZFIX(I4P,I4,L,J) , 100000 )
              CALL IDECI( ISTTYPV(igeom), jtyp,jff,jzz,job )
              jsl   = MOD( IDZFIX(I4P,I4,L,J)/100000 , 10 )
              IF( ISCFAX(L) <= NMATTR ) THEN   !  MDT
                jsz   = 1 ! not used for mdt
                jtube = IDZFIX(I4P,I4,L,J)/1000000
                CALL Code_Ident_AMDC(jtyp, jff, jzz, job, jsl, jsz, jtube, ident)
                TYHCAN(NHXCAN(NBCAN),NBCAN) = 1
              ELSE                             ! CSC
                jsz    = IDZFIX(I4P,I4,L,J)/1000000  ! 1  means Z,  2 means S
                szflag = 3.d0 - 2.d0*jsz             ! >0 means Z, <0 means S
                xyz(1) = 10.d0*XDFIXE(I4P,I4,L,J)
                xyz(2) = 10.d0*YDFIXE(I4P,I4,L,J)
                xyz(3) = 10.d0*ZDFIXE(I4P,I4,L,J)
                CALL FGetStationType(jtyp, ctyp)
                CALL FGetCSCindexFromStripPosition(ctyp,jff,jzz,job,jsl,szflag,xyz,jstrip)
                IF( jstrip == 0 ) THEN
                  IF( MBPri >= 1 ) WRITE(*,*) ' Muonboy:storetrack hole typ/jsl/jsz ',ctyp,jsl,jsz,' outside station'
                  NHXCAN(NBCAN) = NHXCAN(NBCAN) - 1
                  CYCLE
                ENDIF
                CALL Code_Ident_AMDC(jtyp, jff, jzz, job, jsl, jsz, jstrip, ident)
                TYHCAN(NHXCAN(NBCAN),NBCAN) = 4
              ENDIF
              IHXCAN(NHXCAN(NBCAN),NBCAN) = ident
!!>> JFL samedi 11 avril 2009, 16:10:07 (UTC+0200)
!!>> TEMPORARY SOLUTION
              ENHCAN(NHXCAN(NBCAN),NBCAN) = VXXCAN(7,NBCAN)*VXXCAN(8,NBCAN)
              XHACAN(NHXCAN(NBCAN),NBCAN) = XDFIXE(I4P,I4,L,J)
              YHACAN(NHXCAN(NBCAN),NBCAN) = YDFIXE(I4P,I4,L,J)
              ZHACAN(NHXCAN(NBCAN),NBCAN) = ZDFIXE(I4P,I4,L,J)
              XHdCAN(NHXCAN(NBCAN),NBCAN) = XDdFIXE(I4P,I4,L,J)
              YHdCAN(NHXCAN(NBCAN),NBCAN) = YDdFIXE(I4P,I4,L,J)
              ZHdCAN(NHXCAN(NBCAN),NBCAN) = ZDdFIXE(I4P,I4,L,J)
            ELSE
              IF( IZFIXE(I4P,I4,L,J) == 0 ) CYCLE
              IF( NZXCAN(NBCAN) >= NZXMAX ) EXIT
              IF( IZFIXE(I4P,I4,L,J) < 0 ) THEN ! negative for outliers (masked tubes)
                NBXCAN(NBCAN) = NBXCAN(NBCAN) + 1
                IF( NZXCAN(NBCAN) + NZOCAN(NBCAN) >= 100 )                           EXIT
                IF( ISCFAX(L) <= NMATTR .AND. LTUBHI(ABS(IZFIXE(I4P,I4,L,J))) /= 1 ) CYCLE ! to avoid problem with ROT
                NZOCAN(NBCAN) = NZOCAN(NBCAN) + 1
                IPOS = 101 - NZOCAN(NBCAN)
              ELSE
                NZXCAN(NBCAN) = NZXCAN(NBCAN) + 1
                IF( NZXCAN(NBCAN) + NZOCAN(NBCAN) > 100 ) NZOCAN(NBCAN) = 100 - NZXCAN(NBCAN)                
                IPOS = NZXCAN(NBCAN)
              ENDIF
              IZXCAN(IPOS,NBCAN) = ABS(IZFIXE(I4P,I4,L,J))
              RZXCAN(IPOS,NBCAN) = RZFIXE(I4P,I4,L,J)
              RZICAN(IPOS,NBCAN) = RIFIXE(I4P,I4,L,J)
              XDCCAN(IPOS,NBCAN) = XDFIXE(I4P,I4,L,J)
              YDCCAN(IPOS,NBCAN) = YDFIXE(I4P,I4,L,J)
              ZDCCAN(IPOS,NBCAN) = ZDFIXE(I4P,I4,L,J)
              XDCdCAN(IPOS,NBCAN)= XDdFIXE(I4P,I4,L,J)
              YDCdCAN(IPOS,NBCAN)= YDdFIXE(I4P,I4,L,J)
              ZDCdCAN(IPOS,NBCAN)= ZDdFIXE(I4P,I4,L,J)
!!>> JFL Sat Apr 14 18:38:35 CEST 2007
!!>> TEMPORARY SOLUTION
!             ENCCAN(IPOS,NBCAN) = ENFIXE(I4P,I4,L,J)
              ENCCAN(IPOS,NBCAN) = VXXCAN(7,NBCAN)*VXXCAN(8,NBCAN)
              RDCCAN(IPOS,NBCAN) = RDFIXE(I4P,I4,L,J)
              SDCCAN(IPOS,NBCAN) = SDFIXE(I4P,I4,L,J)
            ENDIF
          ENDDO
        ENDDO
      ENDDO
      IF( L <= MIN(NBFAX,12) ) THEN
        ISCF = ISCFAX(L)
        IF( CARISC(ISCF) /= 'BEE' ) THEN
          IF(IOFFAX(L,1) <= 0) ISCF = ISCF + 100
          IF(IOFFAX(L,2) <= 0) ISCF = ISCF + 200
        ENDIF
        ITXCAN(L,NBCAN) = ISCF
      ENDIF
    ENDDO
    ITFCAN(1:12,NBCAN) = 0
    DO L=1,12
      IF( ITXCAN(L,NBCAN) <= 0 ) EXIT
      ISC = MOD( ITXCAN(L,NBCAN) , 100 )
      I1 = 0
      I2 = 0
      DO J=1,NZXCAN(NBCAN)
        It = IZXCAN(J,NBCAN)
        IC  = KTUBHI(It)/10000
        IF( (IC+1)/2 == ISC ) THEN
          IF( ISC <= NMATTR .OR. RTUBHI(It) > -10.d0 ) THEN  ! do not count S strips as found
            IF(IC /= 2*ISC) THEN
              I1 = 1
            ELSE
              I2 = 1
            ENDIF
          ENDIF
        ENDIF
      ENDDO
      IF( I1 /= 0 .OR. I2 /= 0 ) THEN
        ITFCAN(L,NBCAN) = ISC
        IF( I1 == 0 )  ITFCAN(L,NBCAN) = ISC + 200
        IF( I2 == 0 )  ITFCAN(L,NBCAN) = ISC + 100
      ENDIF
    ENDDO
! rearrange arrays of outliers
! and initialisations needed to avoid fpe when pushArray
    IF( NZXCAN(NBCAN) + NZOCAN(NBCAN) < NZXMAX ) THEN
      DO L=1,NZOCAN(NBCAN)
        IOLD = NZXMAX - NZOCAN(NBCAN) + L
        IPOS = NZXCAN(NBCAN) + L
        IZXCAN( IPOS,NBCAN) = IZXCAN( IOLD,NBCAN)
        RZXCAN( IPOS,NBCAN) = RZXCAN( IOLD,NBCAN)
        RZICAN( IPOS,NBCAN) = RZICAN( IOLD,NBCAN)
        XDCCAN( IPOS,NBCAN) = XDCCAN( IOLD,NBCAN)
        YDCCAN( IPOS,NBCAN) = YDCCAN( IOLD,NBCAN)
        ZDCCAN( IPOS,NBCAN) = ZDCCAN( IOLD,NBCAN)
        XDCdCAN(IPOS,NBCAN) = XDCdCAN(IOLD,NBCAN)
        YDCdCAN(IPOS,NBCAN) = YDCdCAN(IOLD,NBCAN)
        ZDCdCAN(IPOS,NBCAN) = ZDCdCAN(IOLD,NBCAN)
        ENCCAN( IPOS,NBCAN) = ENCCAN( IOLD,NBCAN)
        RDCCAN( IPOS,NBCAN) = RDCCAN( IOLD,NBCAN)
        SDCCAN( IPOS,NBCAN) = SDCCAN( IOLD,NBCAN)
      ENDDO
      IPOS = NZXCAN(NBCAN) + NZOCAN(NBCAN) + 1
      IZXCAN( IPOS:NZXMAX,NBCAN) = 0
      RZXCAN( IPOS:NZXMAX,NBCAN) = 0.d0
      RZICAN( IPOS:NZXMAX,NBCAN) = 0.d0
      XDCCAN( IPOS:NZXMAX,NBCAN) = 0.d0
      YDCCAN( IPOS:NZXMAX,NBCAN) = 0.d0
      ZDCCAN( IPOS:NZXMAX,NBCAN) = 0.d0
      XDCdCAN(IPOS:NZXMAX,NBCAN)= 0.d0
      YDCdCAN(IPOS:NZXMAX,NBCAN)= 0.d0
      ZDCdCAN(IPOS:NZXMAX,NBCAN)= 0.d0
      ENCCAN( IPOS:NZXMAX,NBCAN) = 0.d0
      RDCCAN( IPOS:NZXMAX,NBCAN) = 0.d0
      SDCCAN( IPOS:NZXMAX,NBCAN) = 0.d0
    ENDIF
    IF( NSXCAN(NBCAN) < NSXMAX ) THEN
      IPOS = NSXCAN(NBCAN) + 1
      ISXCAN(  IPOS:NSXMAX,NBCAN)  = 0
      XSDCCAN( IPOS:NSXMAX,NBCAN) = 0.d0
      YSDCCAN( IPOS:NSXMAX,NBCAN) = 0.d0
      ZSDCCAN( IPOS:NSXMAX,NBCAN) = 0.d0
      XSDCdCAN(IPOS:NSXMAX,NBCAN)= 0.d0
      YSDCdCAN(IPOS:NSXMAX,NBCAN)= 0.d0
      ZSDCdCAN(IPOS:NSXMAX,NBCAN)= 0.d0
      ESNCCAN( IPOS:NSXMAX,NBCAN) = 0.d0
      RSDCCAN( IPOS:NSXMAX,NBCAN) = 0.d0
    ENDIF
    IF( NHXCAN(NBCAN) < NholMA ) THEN
      IPOS = NHXCAN(NBCAN) + 1
      IHXCAN(IPOS:NholMA,NBCAN) = 0
      ENHCAN(IPOS:NholMA,NBCAN) = 0.d0
      XHACAN(IPOS:NholMA,NBCAN) = 0.d0
      YHACAN(IPOS:NholMA,NBCAN) = 0.d0
      ZHACAN(IPOS:NholMA,NBCAN) = 0.d0
      XHdCAN(IPOS:NholMA,NBCAN) = 0.d0
      YHdCAN(IPOS:NholMA,NBCAN) = 0.d0
      ZHdCAN(IPOS:NholMA,NBCAN) = 0.d0
    ENDIF
! and initialize some arrays
    RZISIM(1:NZXMAX,NBCAN) =   0.d0
    EZIFIT(1:NZXMAX,NBCAN) = 999.d0
    RSISIM(1:NSXMAX,NBCAN) =   0.d0
    ESIFIT(1:NSXMAX,NBCAN) = 999.d0
!
    IF( FLGCAN(NBCAN) /= 1 ) Navant = 0
    NBMSXX(    NBCAN) = MIN( NBMASC-Navant , NXXMAX )
    ENMSXX(    NBCAN) = ENMASC
    IPOS = 0
    DO L=1,NBMSXX(NBCAN)
      J = L + Navant
      IF( IDMASC(J) == 0 ) CYCLE
      IPOS = IPOS + 1
      LLMSXX(IPOS, NBCAN)   = LLMASC(J)
      RRMSXX(IPOS, NBCAN)   = RRMASC(J)
      X0MSXX(IPOS, NBCAN)   = X0MASC(J)
      S1MSXX(IPOS, NBCAN)   = S1MASC(J)
      S2MSXX(IPOS, NBCAN)   = S2MASC(J)
      THMSXX(IPOS, NBCAN)   = THMASC(J)
      IF( MOD(LLMASC(J),2) == 1 ) THEN
        EAMSXX(IPOS, NBCAN) = ALMASC(J)
        EBMSXX(IPOS, NBCAN) = BLMASC(J)
      ELSE
        EAMSXX(IPOS, NBCAN) = 1.d0
        EBMSXX(IPOS, NBCAN) = 0.d0
      ENDIF
      VinVouXX( 1:8,IPOS, NBCAN) = VinVouMASC(1:8,J)         
      VinVouXX(  4 ,IPOS, NBCAN) = VinVouXX(4,IPOS,NBCAN)*VinVouXX(8,IPOS,NBCAN)*VXXCAN(8,NBCAN)
      VinVouXX(  5 ,IPOS, NBCAN) = VinVouXX(5,IPOS,NBCAN)*VinVouXX(8,IPOS,NBCAN)*VXXCAN(8,NBCAN)
      VinVouXX(  6 ,IPOS, NBCAN) = VinVouXX(6,IPOS,NBCAN)*VinVouXX(8,IPOS,NBCAN)*VXXCAN(8,NBCAN)
      VinVouXX(  8 ,IPOS, NBCAN) = VXXCAN(8,NBCAN)
      
      VinVouXX2(1:8,IPOS, NBCAN) = VinVou2MASC(1:8,J)         
      VinVouXX2( 4 ,IPOS, NBCAN) = VinVouXX2(4,IPOS,NBCAN)*VinVouXX2(8,IPOS,NBCAN)*VXXCAN(8,NBCAN)
      VinVouXX2( 5 ,IPOS, NBCAN) = VinVouXX2(5,IPOS,NBCAN)*VinVouXX2(8,IPOS,NBCAN)*VXXCAN(8,NBCAN)
      VinVouXX2( 6 ,IPOS, NBCAN) = VinVouXX2(6,IPOS,NBCAN)*VinVouXX2(8,IPOS,NBCAN)*VXXCAN(8,NBCAN)
      VinVouXX2( 8 ,IPOS, NBCAN) = VXXCAN(8,NBCAN)
    ENDDO
    IF( IPOS /= NBMSXX(NBCAN) ) THEN
      IF( MBPri >= 1 ) WRITE(*,*) ' Muonboy:storetrack scattering total and effectif ',NBMSXX(NBCAN),IPOS
      NBMSXX(NBCAN) = IPOS
    ENDIF

    IF( NBMSXX(NBCAN) < NXXMAX ) THEN  ! initialisations needed to avoid fpe when pushArray
      IPOS = NBMSXX(NBCAN) + 1
      RRMSXX(       IPOS:NXXMAX,NBCAN) = 0.d0
      LLMSXX(       IPOS:NXXMAX,NBCAN) = 0
      X0MSXX(       IPOS:NXXMAX,NBCAN) = 0.d0
      S1MSXX(       IPOS:NXXMAX,NBCAN) = 0.d0
      S2MSXX(       IPOS:NXXMAX,NBCAN) = 0.d0
      THMSXX(       IPOS:NXXMAX,NBCAN) = 0.d0
      EAMSXX(       IPOS:NXXMAX,NBCAN) = 0.d0
      EBMSXX(       IPOS:NXXMAX,NBCAN) = 0.d0
      VinVouXX( 1:8,IPOS:NXXMAX,NBCAN) = 0.d0
      VinVouXX2(1:8,IPOS:NXXMAX,NBCAN) = 0.d0
    ENDIF
    IF( NBFXXX(NBCAN) < NMATMA ) THEN  ! initialisations needed to avoid fpe when pushArray
      IPOS = NBFXXX(NBCAN) + 1
      ISCFXXX(IPOS:NMATMA,NBCAN) = 0
       IDFXXX(IPOS:NMATMA,NBCAN) = 0
    ENDIF
!
 END SUBROUTINE storeTrack
!
