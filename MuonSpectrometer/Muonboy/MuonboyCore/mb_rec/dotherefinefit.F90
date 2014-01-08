!
!> Build and fit the track starting from the segment ID from stations ISC0,ISC1
!> during search currentSearch. It returns iok=0 if everything was ok,
!> iok=1 if the fit failed,
!> iok=2 if too much trials have been done for this search,
!> iok=3 if too much trials have been done for the event,
!> iok=9 is fatal problem.
!
 SUBROUTINE DoTheRefineFit(currentSearch, iok)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_Fit
 USE M_MB_Reco_Tracks
 USE M_MB_COMUSC
 USE M_MB_COSURE
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: currentSearch
 INTEGER, INTENT(OUT) :: iok
 LOGICAL :: REJECT
 INTEGER :: ISCF, ISCF0, L, NBCUR, NMSMAX
 REAL(8) :: R2FIN, AZFIN, SUMX0P
 INTEGER :: IWDRATfun
!
    IF( MBSto <= 2 ) THEN
      IF(MBPri >= 1) WRITE(*,'(/," STOP reconstruction after simple fit of straight track segments !")')
      GO TO 100
    ENDIF

b1: DO NBCUR=1,NBCAN
      IF( NBFXXX(NBCUR) /= NBFIXE )          CYCLE b1
      DO L=1,NBFIXE
        IF( ISCFXXX(L,NBCUR) /= ISCFIXE(L) ) CYCLE b1
        IF( IDFXXX(L,NBCUR)  /= IDFIXE(L)  ) CYCLE b1
      ENDDO
      IF( MBPri >= 1 ) WRITE(*,'(" Candidate track already found by another search --> give up")')
      iok = 1
      RETURN
    ENDDO b1
    IF( SIZE(NDFCAN) < NBCAN + 1 ) CALL Push_MB_Reco_Tracks_NCANMA
    NBCUR = NBCAN + 1
    NBFXXX(NBCUR) = NBFIXE
    ISCFXXX(1:NBFIXE,NBCUR) = ISCFIXE(1:NBFIXE)
    IDFXXX( 1:NBFIXE,NBCUR) = IDFIXE( 1:NBFIXE)
!
    call DoTheFitWithTubes(iok)
    if( iok /= 0 ) return
!
100 CONTINUE
    iok = 1
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
    IF ( currentSearch /= 0 ) THEN
      IF( NBFIXE <= 2 .AND. IFLECH(currentSearch) >= 1 ) THEN
        IF( FMIN > FLOOK2 )                RETURN
! and is not considered as "good"
        FMIN = FMIN + FLOOK2
      ENDIF
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
! do not deal with too low Pt
      IF( VerySmallPt(FinalPtCut) )      RETURN 
    ENDIF
!
!
! Here, we begin to store the final candidate track !!!
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
      SUMX0P = SQRT(SUM(X0MASC(1:NBMASC))) / VFIN(7)
      IF( SUMX0P > 0.40d0 ) THEN
        NMSMAX = 5
      ELSE IF( SUMX0P > 0.10d0 ) THEN
        NMSMAX = 4
      ELSE
        NMSMAX = 3
      ENDIF
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
950         FORMAT(' For a candidate track with Pmu =',F9.2,' GeV,'                      &
                  /' One has  R2FIN,AZFIN =',2F12.4,'  while RCHAMI,ZCHAMI =',2F12.4     &
                  ,'  =====>  Reject it ! (but for test beam or cosmics)')
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
    iok = 0
!
 END SUBROUTINE DoTheRefineFit
!
