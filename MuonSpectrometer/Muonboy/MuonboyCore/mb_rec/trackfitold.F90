!
!> Build and fit the track starting from the segment ID from stations ISC0,ISC1
!> during search currentSearch. It returns iok=0 if everything was ok,
!> iok=1 if the fit failed,
!> iok=2 if too much trials have been done for this search,
!> iok=3 if too much trials have been done for the event,
!> iok=9 is fatal problem.
!
 SUBROUTINE trackFitOld(ID,ISC0,ISC1,currentSearch, iok)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_Fit
 USE M_MB_Reco_Tracks
 USE M_MB_COMUSC
 USE M_MB_COSURE
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ID, ISC0, ISC1, currentSearch
 INTEGER, INTENT(OUT) :: iok
 LOGICAL :: REJECT
 INTEGER :: ISC, ISCF, ISCF0, L, NBCUR, NMSMAX, fitStatus
 INTEGER :: IFLECH_in
 INTEGER :: ISCUSE(NMATMA)
 REAL(8) :: TZ1, TZ2, R2FIN, AZFIN
 INTEGER :: IWDRATfun
 EXTERNAL :: Get_Chi2_LibrFixe_Min
!
    iok = 1
!
    ISCI = ISC0
    CFI  = COS(FFDRAT(ID,ISC0))
    SFI  = SIN(FFDRAT(ID,ISC0))
    CFA  = 1.D0
    SFA  = 0.D0
    Z1   = ZZDRAT(ID,1,ISC0)
    T1   = TTDRAT(ID,1,ISC0)
    S1   = SSDRAT(ID,1,ISC0)
    Z2   = ZZDRAT(ID,2,ISC0)
    T2   = TTDRAT(ID,2,ISC0)
    S2   = SSDRAT(ID,2,ISC0)
    IF( ISC0 <= NCOUBAS2 ) THEN
      TZ1 = T1
      TZ2 = T2
    ELSEIF( ISC0 <= NMATTR ) THEN
      TZ1 = Z1
      TZ2 = Z2
    ELSE
      CFA = COS(GGDRAT(ID,ISC0))
      SFA = SIN(GGDRAT(ID,ISC0))
      TZ1 = CFA * Z1 + SFA * T1
      TZ2 = CFA * Z2 + SFA * T2
    ENDIF
    X1   = CFI * T1 - SFI * S1
    Y1   = SFI * T1 + CFI * S1
    X2   = CFI * T2 - SFI * S2
    Y2   = SFI * T2 + CFI * S2
#if !defined(ATLAS_NOCHECKMU)
    IF( TZ2/TZ1 > 1.D0 ) THEN
      WRITE(*,'(//5X,"TZ2 =",F12.4," > TZ1 =",F12.4," ======> STOP !"/)') TZ2,TZ1
      iok = 9
      RETURN
    ENDIF
#endif
!
! estimate the momentum from the segment position and angle
    CALL getInitialSmu(ISC0, X1,Y1,Z1, X2,Y2,Z2, SMU, fitStatus)
    IF( fitStatus /= 0 ) RETURN
    CALL Reset_MB_Fit
!
!
!-- Initial track search using only 2 stations
    NBFIXE = 1
    ISCFIXE(1) = ISC0
    IDFIXE(1)  = ID
    IF( ISENFI == 1 ) THEN
      IF( NBDRAT(ISC1) <= 0 ) GO TO 500
      NBLIBR = 1
      ISCLIBR(1) = ISC1
    ELSE
      NBLIBR = 0
      DO L=1,LSEARC(currentSearch)
        ISC = ISCEMI(L,currentSearch)
        IF( ISC == ISC0 .OR. NBDRAT(ISC) <= 0 ) CYCLE
        NBLIBR = NBLIBR + 1
        ISCLIBR(NBLIBR) = ISC
      ENDDO
      IF( NBLIBR <= 0 ) RETURN
    ENDIF
    IF(MBPri >= 3) THEN
      WRITE(*,'(2X,"ISCFIXE =",10I4)') ISCFIXE(1:NBFIXE)
      WRITE(*,'(2X,"ISCLIBR =",10I4)') ISCLIBR(1:NBLIBR)
    ENDIF
!
! primary momentum scan and fit (only one segment can be added to the seeding segment)
    IFLECH_in = 0
    if ( currentSearch /= 0 ) IFLECH_in = IFLECH(currentSearch)
    CALL primaryFit(IFLECH_in,fitStatus)
    IF( fitStatus /= 0 ) THEN
      iok = fitStatus
      RETURN
    ENDIF
!
!-- Principal track search using (2) 3 or 4 stations
500 NBLIBR = 0
    ISCUSE(:) = 0
    ISCUSE( ISCFIXE(1:NBFIXE) ) = 1
    DO L=1,LSEARC(currentSearch)
      ISC = ISCEMI(L,currentSearch)
      IF( NBDRAT(ISC) <= 0 .OR. ISCUSE(ISC) /= 0 ) CYCLE
      NBLIBR = NBLIBR + 1
      ISCLIBR(NBLIBR) = ISC
    ENDDO
    IF( NBLIBR <= 0 ) THEN
      IF( NBFIXE <= 1 ) RETURN
      GO TO 600
    ENDIF
    IF(MBPri >= 3) THEN
      WRITE(*,'(2X,"ISCFIXE =",10I4)') ISCFIXE(1:NBFIXE)
      WRITE(*,'(2X,"ISCLIBR =",10I4)') ISCLIBR(1:NBLIBR)
    ENDIF
!
! Momentum scan before fit, and from the best momentum (SMU) found, define in which stations
! one must look during the fit (these are the NBLIBR stations)
!
    CALL secondaryScan(fitStatus)
    IF( fitStatus /= 0 ) THEN
      iok = fitStatus
      RETURN
    ENDIF
!
! Principal Fit : fit the track adding the NBLIBR stations
570 CALL mainFit(fitStatus,.false.)
    IF( fitStatus /= 0 ) THEN
      iok = fitStatus
      RETURN
    ENDIF
!
! Here one candidate track may have been found, from it we perform a
! complementary search using all possible stations
600 CALL searchForCrossedStations
    IF( NBLIBR /= 0 ) GO TO 570
!
! Final pattern recognition fit of type 1 with SIGMAK = 5.
! if not done and at least 3 stations  or  if fit was not of type 1
    IF( ItypFit == 2 ) THEN
! Store the tracks parameters for ItypFit = 2
      IF( AtCosm == 0 ) THEN
        R2FIN = SQRT( VFIN(1)**2 + VFIN(2)**2 )
        AZFIN = ABS( VFIN(3) )
        IF( ABS(R2FIN-RCHAMI) > 0.1D0 .AND. ABS(AZFIN-ZCHAMI) > 0.1D0 ) THEN
          IF(MBPri >= 1) PRINT 950,VFIN(8)*VFIN(7),R2FIN,AZFIN,RCHAMI,ZCHAMI
950       FORMAT(' For a candidate track with Pmu =',F9.2,' GeV,'                      &
                /' One has  R2FIN,AZFIN =',2F12.4,'  while RCHAMI,ZCHAMI =',2F12.4     &
                ,'  =====>  Reject it ! (but for test beam or cosmics)')
          RETURN
        ENDIF
      ENDIF
! Get VTHETAPHI(1 to 5)  from  VFIN(1 to 8)
      CALL TKVFIN2
      NCFIDR = MIN( NCFIDR+1 , NFIDRMA )
      CIFIDR(     NCFIDR) = FMIN
      PAFIDR( 1:5,NCFIDR) = VTHETAPHI(1:5)
      VMFIDR( 1:8,NCFIDR) = VFIN(1:8)
      ITFIDR(1:12,NCFIDR) = 0
      DO L=MIN(12,NBFIXE),1,-1
        ISCF0 = ISCFIXE(L)
        ISCF  = ISCF0
        IF( CARISC(ISCF) /= 'BEE' ) THEN
          IF(IWDRATfun(IDFIXE(L),1,ISCF0) == 0) ISCF = ISCF + 100
          IF(IWDRATfun(IDFIXE(L),2,ISCF0) == 0) ISCF = ISCF + 200
        ENDIF
        ITFIDR( L ,NCFIDR) = ISCF
      ENDDO
      IF( MBSto <= 2 ) THEN
        IF(MBPri >= 1) WRITE(*,'(/," STOP reconstruction after simple fit of straight track segments !")')
        GO TO 700
      ENDIF
      ItypFit = 1
      NBLIBR  = 0
      CALL Set_SIGMAK_CHINA_Z_S_X( 5.D0, 6.D0, 3.D0, 1.5D0 )
      GO TO 570
    ENDIF
!
!
700 CONTINUE
!
! The candidate track is kept only if it hits at least 2 stations
    IF( NBFIXE <= 1 )                   RETURN
! and  have an acceptable Chi2
    IF( FMIN > FLOOK1 )                 RETURN
! when currentSearch > NSCSC (i.e. recovery search for CSC not covered by TGC)
! we require the presence of CSS or CSL
    IF( currentSearch >= NSCSC ) THEN
      REJECT = .TRUE.
      DO L=1,NBFIXE
        IF( CARISC(ISCFIXE(L))(1:2) == 'CS' )   REJECT = .FALSE.
      ENDDO
      IF(REJECT)                         RETURN
    ENDIF
! furthermore, when a sagitta measurement is required, a candidate
! that hits less than 3 stations is kept only when its Chi2 is low
    IF( NBFIXE <= 2 .AND. IFLECH(currentSearch) >= 1 ) THEN
      IF( FMIN > FLOOK2 )                RETURN
! and is not considered as "good"
      FMIN = FMIN + FLOOK2
    ENDIF
! and finally, candidate must enter the calo volume (but for beam or cosmic tests)
    IF( AtCosm == 0 ) THEN
      R2FIN = SQRT( VFIN(1)**2 + VFIN(2)**2 )
      AZFIN = ABS( VFIN(3) )
      IF( ABS(R2FIN-RCHAMI) > 0.1D0 .AND. ABS(AZFIN-ZCHAMI) > 0.1D0 ) THEN
        IF(MBPri >= 1) PRINT 950,VFIN(8)*VFIN(7),R2FIN,AZFIN,RCHAMI,ZCHAMI
        RETURN
      ENDIF
! add also cut on pseudo impact parameter inside inner volume
      IF(     REXTRA(1)  > RCYLIN(2) )   RETURN
      IF( ABS(ZEXTRA(1)) > ZCYLIN(2) )   RETURN
    ENDIF
!
!
! Here, we begin to store the final candidate track !!!
    IF( SIZE(NDFCAN) < NBCAN + 1 ) CALL Push_MB_Reco_Tracks_NCANMA
    NBCUR = NBCAN + 1
    CHICAN(    NBCUR) = FMIN
    VMUCAN(1:8,NBCUR) = VFIN(1:8)
    ISECAN(    NBCUR) = ISENFI*100 + currentSearch
    ITICAN(1:12,NBCUR) = 0
    DO L=MIN(12,NBFIXE),1,-1
      ISCF0 = ISCFIXE(L)
      ISCF  = ISCF0
      IF( CARISC(ISCF) /= 'BEE' ) THEN
        IF(IWDRATfun(IDFIXE(L),1,ISCF0) == 0) ISCF = ISCF + 100
        IF(IWDRATfun(IDFIXE(L),2,ISCF0) == 0) ISCF = ISCF + 200
      ENDIF
      ITICAN(L,NBCUR) = ISCF
    ENDDO
!
!
!XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
! Grand final fit with matter for m.s. with ItypFit = 3 (purely statistical)
! Before all, one sets the parameters and "fixes" the "sure" hits
    NBLIBR = 0
    NZSure = 0
    IFSure(:) = 999
!......................................
!
    CALL Set_SIGMAK_CHINA_Z_S_X( 5.D0, 3.D0, 1.5D0, 1.D0 )
!
!
! Then, first, find-out all the radii of m.s. and e.loss along the track
! in the muon spectro. region
    NBMASC = 0
    CALL GIVDEB1(1)
    CALL listOfFixedStations ! Compute the list of fixed stations to be sure they will not be rejected by trmusc
    CALL TRMUSC(112)
    IF(MBPri >= 3) THEN
      WRITE(*,'(/" After TRMUSC : NBmesu,NBmasc =",2I6)') NBMESU,NBMASC
      CALL PRIMASC
    ENDIF
!
!
! Second, reduce the number of these radii to NMSMAX
    IF( MaxNberMS < 0 ) THEN
      IF( VFIN(7) <= 7.0D0 ) THEN
        NMSMAX = 5
      ELSEIF( VFIN(7) <= 9.D0 ) THEN
        NMSMAX = 4
      ELSE
        NMSMAX = 3
      ENDIF
!**** X0MASCtot = 0.D0
!**** DO L=1,NBMASC
!****  X0MASCtot = X0MASCtot + X0MASC(L)
!**** ENDDO
!**** NMSMAX = 35.D0 * SQRT(X0MASCtot) / VFIN(7)
!**** NMSMAX = MIN( 5 , MAX( 3 , NMSMAX ) )
    ELSE
      NMSMAX = MaxNberMS
    ENDIF
!
    IF( NMSMAX > 0 ) THEN
      CALL REDUCSA( NMSMAX, 3)
    ELSE
      NBMASC = 0
    ENDIF
    S1MASC(1:NBMASC) = 0.d0
    LLMASC(1:NBMASC) = 5
! no Eloss when momentum fixed
    IF( ABS(FixMoCur) > 0.1d0 ) LLMASC(1:NBMASC) = 4
    IF(MBPri >= 3) THEN
      WRITE(*,'(/" After REDUCSA : NBmesu,NBmasc,Navant =",3I6)') NBMESU,NBMASC,Navant
      CALL PRIMASC
    ENDIF
!
    IF( MBSto >= 3 ) THEN
      CALL FitWithMatter
    ELSE
      CALL TRAKMU(0)
      IF(MBPri >= 3) CALL PRFILI(3)
    ENDIF
!
    IF( FMIN > FLOOK3 ) THEN
      IF(MBPri >= 1) WRITE(*,'(" Candidate kill because bad final fit : chi2 = ",F8.2)') FMIN 
      RETURN
    ENDIF 
!
    R2FIN = SQRT( VFIN(1)**2 + VFIN(2)**2 )
    AZFIN = ABS( VFIN(3) )
    IF( ( ABS(R2FIN-RCHAMI) > 0.1D0 .AND. ABS(AZFIN-ZCHAMI) > 0.1D0 ) .OR.   &
        DOT_PRODUCT(VFIN(1:2),VFIN(4:5)) < 0.d0 ) THEN
      IF( AtCosm == 0 ) THEN
        IF( MBPri >= 1 ) THEN
          IF( ABS(R2FIN-RCHAMI) > 0.1D0 .AND. ABS(AZFIN-ZCHAMI) > 0.1D0 ) THEN
            PRINT 950,VFIN(8)*VFIN(7),R2FIN,AZFIN,RCHAMI,ZCHAMI
          ELSE
            WRITE(*,'(" For a candidate track with Pmu =",F9.2," GeV, ",       &
              &       "The perigee is inside spectrometer =====>  Reject it ! ")') VFIN(8)*VFIN(7)
          ENDIF
        ENDIF
        RETURN
      ENDIF
      iok   = 0
      NBCAN = NBCUR
      FLGCAN(NBCAN) = 1
      CALL GIVDEB1(1)
! Perigee parameters for tracks which do not leave the spectro
      CALL TOPERI
      CALL TKVFIN3
      VXXCAN(1:8,NBCAN) = VFIN(1:8)
      PAXCAN(1:5,NBCAN) = VPERIGEE(1:5)
    ELSE
      iok   = 0
      NBCAN = NBCUR
      FLGCAN(NBCAN) = 3
      CALL TKVFIN2
      CALL GIVDEB2(1)
      VXXCAN(1:8,NBCAN) = VDEB(1:8,1)
      PAXCAN(1:5,NBCAN) = VTHETAPHI(1:5)
    ENDIF
! initialise dummy values (necessary for FLGCAN=1 or 2 and to avoid fpe in pushArrays)
    COXCAN(1:5,1:5,NBCAN ) = 0.d0
    DO L=1,5
      COXCAN(L,L,NBCAN) = 1.d0
    ENDDO
    VYYCAN(1:8,NBCAN) = VXXCAN(1:8,NBCAN)
    PAYCAN(1:5,NBCAN) = PAXCAN(1:5,NBCAN)
    COYCAN(1:5,1:5,NBCAN ) = COXCAN(1:5,1:5,NBCAN)
    VZZCAN(1:8,NBCAN) = VYYCAN(1:8,NBCAN)
    PAZCAN(1:5,NBCAN) = PAYCAN(1:5,NBCAN)
    COZCAN(1:5,1:5,NBCAN) = COYCAN(1:5,1:5,NBCAN)
!
! Sixth, continue to store the final candidate track !!!
! beware for flgcan=1, do not keep the scattering point before the first measurement
    CXXCAN( 2 ,NBCAN) = FMIN
    CALL storeTrack
!
    IF(MBPri >= 3) WRITE(*,'(2X,"*************** CANDIDATE",I3," ***************"/)') NBCAN
!
 END SUBROUTINE trackFitOld
!
