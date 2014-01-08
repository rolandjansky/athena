!
!> Write out output of pattern contained in Module M_MB_Reco_Tracks
!> \author M.Virchaux
!
 SUBROUTINE WRICAN(LUN,LUNtiming)
 USE M_MB_Control
 USE M_MB_EventInfo
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 USE M_MB_Reco_Segments
 USE M_MB_MuGeom
 USE M_MB_PACONS, ONLY : NSMAX
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LUN, LUNtiming
 CHARACTER(4) :: CAR4A(12),CAR4C(12)
 CHARACTER(3) :: Ctyp
 INTEGER :: ITIISC(12),NORDRE(12)
 INTEGER, SAVE, DIMENSION(5) :: NRESCU = 0
 INTEGER, SAVE, DIMENSION(8) :: NOCAND = 0
 INTEGER, SAVE               :: IEVloc_PRE=0
 INTEGER, SAVE, DIMENSION(NSMAX) :: IJK1=0, IJK2=0 
 INTEGER :: NBGOCA, N, IEVloc, IT, Ident, Iw1,Iw2, NbMl
 INTEGER :: L, JOCAND, JRESCU, Jtyp,Jff,Jzz,Job,Jsl,Jsz,Jtube
 INTEGER :: I, Jstrip, Jspli, Igoca, I12, ITI100, I12red,I12ml
 INTEGER :: ISENFI, ISEARC
 REAL(8) :: Trecons, PTRECONS
 REAL(8) :: STEPSIZE
!
!-- One print out various statistics
    IF(LUN <= 0) THEN
     PRINT 9000,NRESCU(1:5)
     PRINT 9500,NOCAND(1:8)
9000 FORMAT(     '  NUMBER OF EVENTS WITHOUT RESCUE     :',I6  &
                /'                   WITH  1 RESCUE     :',I6  &
                /'                         2 RESCUES    :',I6  &
                /'                         3 RESCUES    :',I6  &
                /'                AT LEAST 4 RESCUES    :',I6)
9500 FORMAT(    /'  NUMBER OF EVENTS WITHOUT CANDIDATE  :',I6  &
                /'                   WITH  1 CANDIDATE  :',I6  &
                /'                         2 CANDIDATES :',I6  &
                /'                         3 CANDIDATES :',I6  &
                /'                         4 CANDIDATES :',I6  &
                /'                         5 CANDIDATES :',I6  &
                /'                         6 CANDIDATES :',I6  &
                /'                AT LEAST 7 CANDIDATES :',I6)
     WRITE(*,*)
     WRITE(*,'(" number of track candidates for search ",I2," >> ",2I6)') (I,IJK1(I),IJK2(I),I=1,NSMAX)
     RETURN
    ENDIF
!
    IEVloc = IEVBOX
    IF(IEVBOX == 0) IEVloc = IpassReco
!
!-- Possibly, one writes the reconstruction times on a special file
    IF(LUNtiming >= 1) THEN
      Trecons = TRECEVT
      IF(Trecons > 9999.999) Trecons = 9999.999
      WRITE(LUNtiming,6600) IEVloc,Trecons
6600  FORMAT(I10,F10.3)
    ENDIF
!
!-- One count the good tracks
    NBGOCA = 0
    DO N=1,NBCAN
      IF(CHICAN(N) < 999.D0) NBGOCA = NBGOCA + 1
    ENDDO
!
!>> One updates various statistics (once by event)
    IF(IEVloc /= IEVloc_PRE) THEN
      IEVloc_PRE = IEVloc
      IF(NBGOCA <= 0) THEN
        NOCAND(1) = NOCAND(1) + 1
        IF(MBPri >= 1) WRITE(*,'(/10X,"**** 0 MUON CANDIDATE !!! ****")')
      ELSE
        JOCAND = MIN( NBGOCA+1 , 8 )
        NOCAND(JOCAND) = NOCAND(JOCAND) + 1
        JRESCU = 1
        DO N=1,NBCAN
          IF( CHICAN(N) < 999.D0 .AND. ISECAN(N) >= 200 )   JRESCU = JRESCU + 1
        ENDDO
        JRESCU = MIN( JRESCU , 5 )
        NRESCU(JRESCU) = NRESCU(JRESCU) + 1
        DO N=1,NBCAN
          IF( CHICAN(N) > 999.d0 ) CYCLE
          ISENFI = ISECAN(N) / 100
          ISEARC = MOD( ISECAN(N), 100)
          IF( ISENFI == 1 ) THEN
            IJK1(ISEARC) = IJK1(ISEARC) + 1
          ELSE
            IJK2(ISEARC) = IJK2(ISEARC) + 1
          ENDIF
        ENDDO
      ENDIF
    ENDIF
!
!-- One print out Modules  M_MB_Reco_Segments  and  M_MB_Reco_Tracks  for the H8 or X5 beam tests
    IF( MBSto <= -2 ) THEN
      WRITE (LUN,5000) IEVloc,NTUBHT
5000  FORMAT('--------------------------------'/'Evt.Digi.   ',2I10)
      DO IT=1,NTUBHT
        Ident = ITUBHI(IT)
        IF( Ident > 10000000 ) THEN
          CALL Deco_Ident_AMDC(Ident,Jtyp,Jff,Jzz,Job,Jsl,Jsz,Jtube)
          CALL FGetStationType(Jtyp,Ctyp)
          WRITE(LUN,5300) IT,Ctyp,Jff,Jzz,Job,Jsl,Jtube,DTUBHI(IT),RTUBHI(IT)
5300      FORMAT(I5,2X,A3,5I5,F12.2,F12.4)
        ELSE
          WRITE(LUN,5310) IT,Ident,DTUBHI(IT),RTUBHI(IT)
5310      FORMAT(I5,15X,I15,F12.2,F12.4)
        ENDIF
      ENDDO
      RETURN
    ELSE IF( MBSto <= 1 ) THEN
      WRITE(LUN,7000) IEVloc,NBINTR,NBGOCA
7000  FORMAT('--------------------------------'/'Evt.Seg.Tra.',I10,2I5)
      DO I=1,NBINTR
        Iw1 = ABS( IKINTR(I) ) / 1000
        Iw2 = ABS( IKINTR(I) ) - 1000*Iw1
        NbMl = 2
        IF(Iw1*Iw2 == 0 .OR. MBSto < 0) NbMl = 1
        IF(IKINTR(I) < 0)               NbMl = - NbMl
        WRITE (LUN,7100) I,NbMl,NUINTR(I),NAINTR(I),CHINTR(I),CARISC(ISCINTR(I))
7100    FORMAT(' Seg.------>',4I5,F10.4,2X,A3)
        WRITE(LUN,7200) PTINTR(1:3,I),VEINTR(1:3,I)
7200    FORMAT(3F12.4,3F12.6)
        DO L=1,NUINTR(I)
          IT = IUINTR(L,I)
          Ident = ITUBHI(IT)
          IF(Ident > 10000000) THEN
            CALL Deco_Ident_AMDC(Ident,Jtyp,Jff,Jzz,Job,Jsl,Jsz,Jtube)
            CALL FGetStationType(Jtyp,Ctyp)
            WRITE(LUN,7300) L,Ctyp,Jff,Jzz,Job,Jsl,Jtube,    &
              RdigINTR(L,I),RtraINTR(L,I),DTUBHI(IT),         &
              XDINTR(L,I),YDINTR(L,I),ZDINTR(L,I),RDINTR(L,I)
            STEPSIZE = sqrt( XDdINTR(L,I)**2   &
                            +YDdINTR(L,I)**2   &
                            +ZDdINTR(L,I)**2 )
            WRITE(LUN,7302) XDINTR(L,I)+XDdINTR(L,I),&
                            YDINTR(L,I)+YDdINTR(L,I),&
                            ZDINTR(L,I)+ZDdINTR(L,I),&
                            STEPSIZE
7300        FORMAT(I5,2X,A3,5I5,10X,2F12.4,F12.2,4F12.4)
7302        FORMAT(5X,2X,3X,25X,10X,   24X,  12X,4F12.4)
          ELSE
            WRITE(LUN,7310) L,Ident,                          &
              RdigINTR(L,I),RtraINTR(L,I),DTUBHI(IT),         &
              XDINTR(L,I),YDINTR(L,I),ZDINTR(L,I),RDINTR(L,I)
            STEPSIZE = sqrt( XDdINTR(L,I)**2   &
                            +YDdINTR(L,I)**2   &
                            +ZDdINTR(L,I)**2 )
            WRITE(LUN,7312) XDINTR(L,I)+XDdINTR(L,I),&
                            YDINTR(L,I)+YDdINTR(L,I),&
                            ZDINTR(L,I)+ZDdINTR(L,I),&
                            STEPSIZE
7310        FORMAT(I5,15X,I15,10X,2F12.4,F12.2,4F12.4)
7312        FORMAT(5X,15X,15X,10X,   24X,  12X,4F12.4)
          ENDIF
        ENDDO
        DO L=NUINTR(I)+1,NUINTR(I)+NAINTR(I)
          IT = IUINTR(L,I)
          Ident = IBANHI(IT)
          IF(Ident > 10000000) THEN
            CALL Deco_Ident_AMDC(Ident,Jtyp,Jff,Jzz,Job,Jsl,Jsz,Jstrip)
            Jspli = 1
            IF(Jsz > 2) THEN
              Jspli = 2
              Jsz = Jsz - 2
            ENDIF
            CALL FGetStationType(Jtyp,Ctyp)
            WRITE(LUN,7320) L,Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstrip,  &
              XDINTR(L,I),YDINTR(L,I),ZDINTR(L,I),RDINTR(L,I)
            STEPSIZE = sqrt( XDdINTR(L,I)**2   &
                            +YDdINTR(L,I)**2   &
                            +ZDdINTR(L,I)**2 )
            WRITE(LUN,7322) XDINTR(L,I)+XDdINTR(L,I),&
                            YDINTR(L,I)+YDdINTR(L,I),&
                            ZDINTR(L,I)+ZDdINTR(L,I),&
                            STEPSIZE
7320        FORMAT(I5,2X,A3,7I5,   24X,  12X,4F12.4)
7322        FORMAT(5X,   5X,35X,   24X,  12X,4F12.4)
          ELSE
            WRITE(LUN,7330) L,Ident,XDINTR(L,I),YDINTR(L,I),ZDINTR(L,I),RDINTR(L,I)
            STEPSIZE = sqrt( XDdINTR(L,I)**2   &
                            +YDdINTR(L,I)**2   &
                            +ZDdINTR(L,I)**2 )
            WRITE(LUN,7332) XDINTR(L,I)+XDdINTR(L,I),&
                            YDINTR(L,I)+YDdINTR(L,I),&
                            ZDINTR(L,I)+ZDdINTR(L,I),&
                            STEPSIZE
7330        FORMAT(I5,15X,I15,10X,   24X,  12X,4F12.4)
7332        FORMAT(5X,15X,15X,10X,   24X,  12X,4F12.4)
          ENDIF
        ENDDO
      ENDDO
      Igoca = 0
      DO I=1,NBCAN
        IF(CHICAN(I) >= 999.D0) CYCLE
        Igoca = Igoca + 1
        DO I12=1,12
          ITI100 = ITXCAN(I12,I)/100
          ITIISC(I12) = ITXCAN(I12,I) - ITI100*100
        ENDDO
        CALL KROISS(ITIISC,12,NORDRE)
        I12red = 0
        I12ml  = 0
        DO I12=1,12
          IF(ITXCAN(NORDRE(I12),I) > 0) THEN
            I12red = I12red + 1
            IF(ITXCAN(NORDRE(I12),I) <= 99) THEN
              CAR4C(I12red) = ' '//CARISC(ITIISC(NORDRE(I12)))
              I12ml  = I12ml  + 2
            ELSEIF(ITXCAN(NORDRE(I12),I) <= 199) THEN
              CAR4C(I12red) = 'i'//CARISC(ITIISC(NORDRE(I12)))
              I12ml  = I12ml  + 1
            ELSEIF(ITXCAN(NORDRE(I12),I) <= 299) THEN
              CAR4C(I12red) = 'o'//CARISC(ITIISC(NORDRE(I12)))
              I12ml  = I12ml  + 1
            ENDIF
          ENDIF
        ENDDO
        WRITE (LUN,7400)Igoca,I12ml,NZXCAN(I),CXXCAN(2,I),CAR4C(1:I12red)
7400    FORMAT(' Tra.------>',3I5,F10.4,12(1X,A4))
        WRITE(LUN,7500) VXXCAN(1:8,I)
7500    FORMAT(4X,3F12.4,3F12.6,F12.3,F12.1)
        DO L=1,NBMSXX(I)
           WRITE(LUN,7700) L,VinVouXX(1:8,L,I),RRMSXX(L,I),X0MSXX(L,I),LLMSXX(L,I),  &
             s1msxx(L,I)*thmsxx(L,I)*1000.d0
7700       FORMAT(I2,2X,3F12.4,3F12.6,F12.3,F12.1,'|',2F12.4,I4,F12.8)
        ENDDO
        DO L=1,NZXCAN(I)
          IT = IZXCAN(L,I)
          Ident = ITUBHI(IT)
          IF( Ident > 10000000 ) THEN
            CALL Deco_Ident_AMDC(Ident,Jtyp,Jff,Jzz,Job,Jsl,Jsz,Jtube)
            CALL FGetStationType(Jtyp,Ctyp)
            WRITE(LUN,7340) L,Ctyp,Jff,Jzz,Job,Jsl,Jtube,RZXCAN(L,I),RZICAN(L,I),DTUBHI(IT),  &
              XDCCAN(L,I),YDCCAN(L,I),ZDCCAN(L,I),RDCCAN(L,I)
            STEPSIZE = sqrt( XDCdCAN(L,I)**2   &
                            +YDCdCAN(L,I)**2   &
                            +ZDCdCAN(L,I)**2 )
            WRITE(LUN,7342) XDCCAN(L,I)+XDCdCAN(L,I),&
                            YDCCAN(L,I)+YDCdCAN(L,I),&
                            ZDCCAN(L,I)+ZDCdCAN(L,I),&
                            STEPSIZE
            WRITE(LUN,7344) ENCCAN(L,I)
7340        FORMAT(I5,2X,A3,5I5,10X,2F12.4,F12.2,4F12.4)
7342        FORMAT(5X,   5X,25X,10X,   24X,  12X,4F12.4)
7344        FORMAT(5X,   5X,25X,10X,   24X,  12X, F12.4)
          ELSE
            WRITE (LUN,7350) L,Ident,RZXCAN(L,I),RZICAN(L,I),DTUBHI(IT),  &
              XDCCAN(L,I),YDCCAN(L,I),ZDCCAN(L,I),RDCCAN(L,I)
            STEPSIZE = sqrt( XDCdCAN(L,I)**2   &
                            +YDCdCAN(L,I)**2   &
                            +ZDCdCAN(L,I)**2 )
            WRITE(LUN,7352) XDCCAN(L,I)+XDCdCAN(L,I),&
                            YDCCAN(L,I)+YDCdCAN(L,I),&
                            ZDCCAN(L,I)+ZDCdCAN(L,I),&
                            STEPSIZE
            WRITE(LUN,7354) ENCCAN(L,I)
7350        FORMAT(I5,15X,I15,10X,2F12.4,F12.2,4F12.4)
7352        FORMAT(5X,15X,15X,10X,   24X,  12X,4F12.4)
7354        FORMAT(5X,15X,15X,10X,   24X,  12X, F12.4)
          ENDIF
        ENDDO
        DO L=1,NSXCAN(I)
          IT = ISXCAN(L,I)
          Ident = IBANHI(IT)
          IF(Ident > 10000000) THEN
            CALL Deco_Ident_AMDC(Ident,Jtyp,Jff,Jzz,Job,Jsl,Jsz,Jstrip)
            Jspli = 1
            IF(Jsz > 2) THEN
              Jspli = 2
              Jsz = Jsz - 2
            ENDIF
            CALL FGetStationType(Jtyp,Ctyp)
            WRITE(LUN,7600) L,Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstrip,  &
              XSDCCAN(L,I),YSDCCAN(L,I),ZSDCCAN(L,I),RSDCCAN(L,I)
            STEPSIZE = sqrt( XSDCdCAN(L,I)**2   &
                            +YSDCdCAN(L,I)**2   &
                            +ZSDCdCAN(L,I)**2 )
            WRITE(LUN,7602) XSDCCAN(L,I)+XSDCdCAN(L,I),&
                            YSDCCAN(L,I)+YSDCdCAN(L,I),&
                            ZSDCCAN(L,I)+ZSDCdCAN(L,I),&
                            STEPSIZE
            WRITE(LUN,7604) ESNCCAN(L,I)
7600        FORMAT(I5,2X,A3,7I5,   24X,  12X,4F12.4)
7602        FORMAT(5X,   5X,35X,   24X,  12X,4F12.4)
7604        FORMAT(5X,   5X,35X,   24X,  12X, F12.4)
         ELSE
            WRITE(LUN,7650) L,Ident,XSDCCAN(L,I),YSDCCAN(L,I),ZSDCCAN(L,I),RSDCCAN(L,I)
            STEPSIZE = sqrt( XSDCdCAN(L,I)**2   &
                            +YSDCdCAN(L,I)**2   &
                            +ZSDCdCAN(L,I)**2 )
            WRITE(LUN,7652) XSDCCAN(L,I)+XSDCdCAN(L,I),&
                            YSDCCAN(L,I)+YSDCdCAN(L,I),&
                            ZSDCCAN(L,I)+ZSDCdCAN(L,I),&
                            STEPSIZE
            WRITE (LUN,7654) ESNCCAN(L,I)
7650        FORMAT(I5,15X,I15,10X,   24X,  12X,4F12.4)
7652        FORMAT(5X,15X,15X,10X,   24X,  12X,4F12.4)
7654        FORMAT(5X,15X,15X,10X,   24X,  12X, F12.4)
          ENDIF
        ENDDO
      ENDDO
      RETURN
    ENDIF
!
!-- One print out Module  M_MB_Reco_Tracks
    WRITE (LUN,8000) IEVloc,NBGOCA,NFICAN
    DO N=1,NBCAN
      IF(CHICAN(N) > 999.D0)  CYCLE
!-----------------------------------------------------------------
      DO I=1,12
        ITI100 = ITICAN(I,N)/100
        ITIISC(I) = ITICAN(I,N) - ITI100*100
      ENDDO
      CALL KROISS(ITIISC,12,NORDRE)
      DO I=1,12
        IF(ITICAN(NORDRE(I),N).LE.0) THEN
          CAR4A(I) = '    '
        ELSEIF(ITICAN(NORDRE(I),N).LE.99) THEN
          CAR4A(I) = ' '//CARISC(ITIISC(NORDRE(I)))
        ELSEIF(ITICAN(NORDRE(I),N).LE.199) THEN
          CAR4A(I) = 'i'//CARISC(ITIISC(NORDRE(I)))
        ELSEIF(ITICAN(NORDRE(I),N).LE.299) THEN
          CAR4A(I) = 'o'//CARISC(ITIISC(NORDRE(I)))
        ENDIF
      ENDDO
!-----------------------------------------------------------------
      DO I=1,12
        ITI100 = ITXCAN(I,N)/100
        ITIISC(I) = ITXCAN(I,N) - ITI100*100
      ENDDO
      CALL KROISS(ITIISC,12,NORDRE)
      DO I=1,12
        IF(ITXCAN(NORDRE(I),N).LE.0) THEN
          CAR4C(I) = '    '
        ELSEIF(ITXCAN(NORDRE(I),N).LE.99) THEN
          CAR4C(I) = ' '//CARISC(ITIISC(NORDRE(I)))
        ELSEIF(ITXCAN(NORDRE(I),N).LE.199) THEN
          CAR4C(I) = 'i'//CARISC(ITIISC(NORDRE(I)))
        ELSEIF(ITXCAN(NORDRE(I),N).LE.299) THEN
          CAR4C(I) = 'o'//CARISC(ITIISC(NORDRE(I)))
        ENDIF
      ENDDO
!-----------------------------------------------------------------
      PTRECONS = VZZCAN(7,N)*SQRT(VZZCAN(4,N)**2+VZZCAN(5,N)**2)
      WRITE(LUN,8100) CHICAN(N),VMUCAN(1:8,N),CAR4A(1:12)
      WRITE(LUN,8200) CXXCAN(2,N),CXXCAN(3,N),NZXCAN(N),NBXCAN(N),       &
        VXXCAN(1:8,N),SQRT(COXCAN(5,5,N))*VXXCAN(7,N),CAR4C(1:12),       &
        VYYCAN(1:8,N),SQRT(COYCAN(5,5,N))*VYYCAN(7,N),                   &
        VZZCAN(1:8,N),SQRT(COZCAN(5,5,N))*PTRECONS,  PTRECONS
!-----------------------------------------------------------------
    ENDDO
!
8000  FORMAT('*>',I10,2I4)
8100  FORMAT('<>',F8.3/                       &
             '<>',3F10.4,3F10.6,F10.2,F8.1/   &
             '<>',12A4,2X,7A4)
8200  FORMAT('<>',2F8.3,20X,2I3/                       &
             '<>',3F10.4,3F10.6,F10.2,F8.1,F8.4/       &
             '<>',12A4/                                &
             '<>',3F10.4,3F10.6,F10.2,F8.1,F8.4/       &
             '<>',3F10.4,3F10.6,F10.2,F8.1,F8.4,F10.2)
!
  END SUBROUTINE WRICAN
!
