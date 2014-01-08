!
!> Build and fit the track starting from the segment ID from stations ISC0,ISC1
!> during search currentSearch. It returns iok=0 if everything was ok,
!> iok=1 if the fit failed,
!> iok=2 if too much trials have been done for this search,
!> iok=3 if too much trials have been done for the event,
!> iok=9 is fatal problem.
!
 SUBROUTINE trackFit(ID,ISC0,ISC1,currentSearch, iok)
 USE M_MB_MuGeom
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ID, ISC0, ISC1, currentSearch
 INTEGER, INTENT(OUT) :: iok
 INTEGER :: ISCF, ISCF0, L
 INTEGER :: IWDRATfun
!
    iok = 1
!
    call DoThePrimaryFit(ID,ISC0,ISC1,currentSearch, iok)
    if ( iok > 0 ) return
    
    call DoTheSecondaryFit(currentSearch, iok)
    if ( iok > 0 ) return
    
    call DoTheMainFit(iok)
    if ( iok /= 0 ) return
!
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
!
    call DoTheRefineFit(currentSearch, iok)
    if ( iok /= 0 ) return
!
    iok = 0
!
 END SUBROUTINE trackFit
!
!
!> Do a direction scan (if no field or between EM-EO) before the full fit
!
 SUBROUTINE DirectionScan( Get_Chi2_Min, CCCMIN, ISCMIN )
 USE M_MB_Control
 USE M_MB_Fit
 USE M_MB_PACONS
 IMPLICIT NONE
 EXTERNAL ::  Get_Chi2_Min
 INTEGER, INTENT(OUT)   :: ISCMIN
 REAL(8), INTENT(INOUT) :: CCCMIN
 INTEGER :: NDZT, LDZTMA, IDZTMI, LDZT, IDZT, ISC, IPADZT
 REAL(8) :: DZT10,DZT20, SCADZT, CCC, DZT1MI,DZT2MI, AMPLI
!
    IF( ABS(FixMoCur) < 0.1D0 ) THEN
      RCHAMI = RCYLIN(4) - 1.D0
      ZCHAMI = 1300.D0
    ENDIF
    DZT10  = DZT1
    DZT20  = DZT2
    NDZT   = 2
    SCADZT = 0.0030d0
    LDZTMA = 4
    IDZTMI = 0
    ISCMIN = 0
! Increase the step when tube resolution is increased
    IF( AtCosm /= 0 ) SCADZT = SCADZT * MAX( MIN( ReTub/0.0085d0 , 5.d0 ) , 1.d0 )
    DZT1MI = DZT10
    DZT2MI = DZT20
    IF(MBPri >= 3) WRITE(*,'(2X,"Scan : DZT10,DZT20 =",2F11.4)') DZT10,DZT20
    DO LDZT=0,LDZTMA
      NDZT = NDZT + 1
      IPADZT = 2 * NDZT
      IF( LDZT == 0 ) IPADZT = 1
      DO IDZT=-NDZT,NDZT,IPADZT
        DZT1 = DZT10 + IDZT * SCADZT
        DZT2 = DZT20 - IDZT * SCADZT
        CALL get_Chi2_Min(ISC,CCC)
        IF( CCC < CCCMIN ) THEN
          IDZTMI = IDZT
          DZT1MI = DZT1
          DZT2MI = DZT2
          CCCMIN = CCC
          ISCMIN = ISC
        ENDIF
        IF(MBPri >= 4) WRITE(*,'(2X,"IDZT,DZT1,DZT2,CHI =",I5,2F11.4,F15.3,I5)') IDZT,DZT1,DZT2,CCC,ISC
      ENDDO
      IF( IABS(IDZTMI) <= NDZT-1 .AND. CCCMIN < DCLOOK/3.D0 ) EXIT
      IF( CCCMIN > SCLOOK )                                   EXIT
    ENDDO
    IF( MBPri == 3 ) WRITE(*,'(2X,"NDZT,IDZTMI =",2I5)') NDZT,IDZTMI
    IF( MBPri >= 3 ) WRITE(*,'(2X,"MIN: DZT1,DZT2,CHI =",2F11.4,F15.3)') DZT1MI,DZT2MI,CCCMIN
    AMPLI = 2.D0
    IF( CCCMIN > AMPLI*DCLOOK ) THEN
      DZT1 = DZT10
      DZT2 = DZT20
    ELSE
      DZT1 = DZT1MI
      DZT2 = DZT2MI
    ENDIF
    RCHAMI = RCYLIN(3)
    ZCHAMI = ZCYLIN(3)
!
 END SUBROUTINE DirectionScan
!
!
!> Do a momentum scan before the full fit
!
 SUBROUTINE MomentumScan( Get_Chi2_Min, NPT,NSMU,LSMUMA,LSMUOK,SMU0,SCASMU,D0SMU0, SMUMIN,CCCMIN,ISCMIN )
 USE M_MB_Control
 USE M_MB_Fit
 USE M_MB_PACONS
 IMPLICIT NONE
 EXTERNAL ::  Get_Chi2_Min
 INTEGER, INTENT(INOUT) :: NPT,NSMU
 INTEGER, INTENT(IN)    :: LSMUMA, LSMUOK
 REAL(8), INTENT(IN)    :: SMU0, SCASMU, D0SMU0
 INTEGER, INTENT(INOUT) :: ISCMIN
 REAL(8), INTENT(INOUT) :: SMUMIN,CCCMIN
 INTEGER :: ISMUMI, LSMU, IPASMU, ISMU, I0, IPT, ISCM, IER
 REAL(8) :: XSMUMI, ADXSMU, CCCM, SMUINF, SMUSUP
 INTEGER, PARAMETER :: NPTMAX=1000
 INTEGER :: IORD(NPTMAX),IAMIN(1) 
 REAL(8), SAVE :: CMP(NPTMAX),SMP(NPTMAX)
!
    IF(MBPri >= 3)   &
      WRITE(*,'(2X,"Scan : SMU0,PMU0,NSMU,SCASMU =",F11.5,F10.2,I5,F8.2)') SMU0,100.D0/ABS(SMU0),NSMU,SCASMU
!
    SMUINF = - 999999.d0
    SMUSUP =   999999.d0
    IF( NPT > 0 ) THEN
      CALL CROISS(SMP,NPT,IORD)
      SMP(1:NPT) = SMP(IORD(1:NPT))
      CMP(1:NPT) = CMP(IORD(1:NPT))
      IF( CMP(1) > SCLOOK ) THEN
        DO IPT=1,NPT-1
          IF( CMP(IPT+1) > SCLOOK ) CYCLE
          SMUINF = SMP(IPT)
          EXIT
        ENDDO
      ENDIF
      IF( CMP(NPT) > SCLOOK ) THEN
        DO IPT=NPT,2,-1
          IF( CMP(IPT-1) > SCLOOK ) CYCLE
          SMUSUP = SMP(IPT)
          EXIT
        ENDDO
      ENDIF
    ENDIF
!
    ISMUMI = 0
    DO LSMU=0,LSMUMA
      NSMU = NSMU + 1
      IPASMU = 2 * NSMU
      IF(LSMU == 0) IPASMU = 1
b100: DO 100 ISMU=-NSMU,NSMU,IPASMU
        SMU = SMU0 + (ISMU+D0SMU0) * SCASMU
        IF( SMU < SMUINF ) CYCLE
        IF( SMU > SMUSUP ) CYCLE
        DO IPT=1,NPT
          IF( ABS(SMU-SMP(IPT)) < 0.000001d0 ) THEN
            IF(MBPri >= 4)   &
              WRITE(*,'(" *ISMU,SMU,PMU,CHI,*** =",I5,F11.5,F10.2,F15.3)') ISMU,SMU,100.D0/ABS(SMU),CMP(IPT)
            CYCLE b100
          ENDIF
        ENDDO
        IF( NPT < NPTMAX ) NPT = NPT + 1
        SMP(NPT) = SMU
        CALL Get_Chi2_Min(ISCM,CMP(NPT))
        IF(MBPri >= 4)   &
          WRITE(*,'("  ISMU,SMU,PMU,CHI,ISC =",I5,F11.5,F10.2,F15.3,I5)') ISMU,SMU,100.D0/ABS(SMU),CMP(NPT),ISCM
        IF( CMP(NPT) < CCCMIN ) THEN
          ISMUMI = ISMU
          SMUMIN = SMU
          CCCMIN = CMP(NPT)
          ISCMIN = ISCM
        ENDIF
100   ENDDO b100
      IF( IABS(ISMUMI) < NSMU .AND. CCCMIN < DCLOOK/3.D0 ) EXIT
      IF( LSMU >= LSMUOK .AND. CCCMIN > 100.D0*DCLOOK )    EXIT
    ENDDO
!
    CALL CROISS(SMP,NPT,IORD)
    SMP(1:NPT) = SMP(IORD(1:NPT))
    CMP(1:NPT) = CMP(IORD(1:NPT))
    IAMIN = MINLOC( CMP(1:NPT) )
    I0 = MIN( NPT-1 , MAX( 2 , IAMIN(1) ) )
    CALL PARABOL(SMP(I0-1:I0+1),CMP(I0-1:I0+1),XSMUMI,IER)
    IF( IER == 0 .AND. ABS(XSMUMI-SMP(I0)) >= 0.000001d0 ) THEN
      ADXSMU = ABS( SMUMIN - XSMUMI )
      IF( ADXSMU < 2.D0*NSMU*SCASMU ) THEN
        SMU = XSMUMI
        CALL Get_Chi2_Min(ISCM,CCCM)
        DO IPT=1,NPT
          IF( ABS(SMU-SMP(IPT)) < 0.000001d0 ) GO TO 120
        ENDDO
        IF( NPT < NPTMAX ) NPT = NPT + 1
        SMP(NPT) = SMU
        CMP(NPT) = CCCM
120     CONTINUE
        IF( CCCM-CCCMIN < -ADXSMU ) THEN
          SMUMIN = SMU
          CCCMIN = CCCM
          ISCMIN = ISCM
          IF( ABS(SMUMIN-SMU0) > NSMU*SCASMU ) THEN ! if minimum outside initial range, try to improve it    
            CALL CROISS(SMP,NPT,IORD)
            SMP(1:NPT) = SMP(IORD(1:NPT))
            CMP(1:NPT) = CMP(IORD(1:NPT))
            IAMIN = MINLOC( CMP(1:NPT) )
            I0 = MIN( NPT-1 , MAX( 2 , IAMIN(1) ) )
            CALL PARABOL(SMP(I0-1:I0+1),CMP(I0-1:I0+1),XSMUMI,IER)
            IF( IER == 0 .AND. ABS(XSMUMI-SMP(I0)) >= 0.000001d0 ) THEN
              ADXSMU = ABS( SMUMIN - XSMUMI )
              IF( ADXSMU < NSMU*SCASMU ) THEN
                SMU = XSMUMI
                CALL Get_Chi2_Min(ISCM,CCCM)
                DO IPT=1,NPT
                  IF( ABS(SMU-SMP(IPT)) < 0.000001d0 ) GO TO 140
                ENDDO
                IF( NPT < NPTMAX ) NPT = NPT + 1
                SMP(NPT) = SMU
                CMP(NPT) = CCCM
140             CONTINUE
                IF( CCCM-CCCMIN < -ADXSMU ) THEN
                  SMUMIN = SMU
                  CCCMIN = CCCM
                  ISCMIN = ISCM
                ENDIF
              ENDIF
            ENDIF
          ENDIF
        ENDIF
      ENDIF
    ENDIF
!
    IF(MBPri >= 3) THEN
      WRITE(*,'("  NSMU,ISMUMI,XSMUMI =",2I5,F8.2)')                     NSMU,ISMUMI,XSMUMI
      WRITE(*,'("  MIN:   SMU, PMU,CHI,ISC =",5X,F11.5,F10.2,F15.3,I5)') SMUMIN,100.D0/ABS(SMUMIN),CCCMIN,ISCMIN
      IF( NPT == NPTMAX ) WRITE(*,'(" >>> Warning NPT=NPTMAX <<<")') 
    ENDIF
!
 END SUBROUTINE MomentumScan
!
!
!> Perform primary scan and fit starting from the seeding segment (only one segment can be added here)
!> return 0 if everything ok
!
 SUBROUTINE primaryFit(IFLECH_in,fitStatus)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_Reco_Tracks
 USE M_MB_Fit
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IFLECH_in
 INTEGER, INTENT(OUT) :: fitStatus
 INTEGER :: ISCMIN, NSMU, LSMUMA, LSMUOK, NPT, iscadd, maxcall
 REAL(8) :: CCCMIN, SMUMIN, SMU0, Cnsmu, Ansmu, SCASMU, D0SMU0
 REAL(8) :: err(5), dzt1bak, dzt2bak
 LOGICAL :: doMomentumScan
 LOGICAL :: doPhiRecovery
 EXTERNAL :: Get_Chi2_Libr_Min
!
    fitStatus = 0
!
!-- If extrapolation is to "EO" stations or if momentum is fixed, perform a direction scan
    CALL ktime_mubo('scan    ',1)
    doMomentumScan = ( ABS(FixMoCur) < 0.1D0 )
    CCCMIN = 99999999.D0
    CALL GEOKLI
    IF( NBOKLI >= 1 ) THEN
      IF( (NBLIBR == 1 .AND. CARISC(ISCLIBR(1))(1:2) == 'EO') .OR. .NOT.doMomentumScan ) THEN
        doMomentumScan = .false. ! do not loose time as there is no field between EM-EO 
        CALL DirectionScan( Get_Chi2_Libr_Min, CCCMIN, ISCMIN )
      ENDIF
!
!-- Momentum scan before fit (if momentum not fixed) !
      SMU0   = SMU
      SMUMIN = SMU0
      IF( doMomentumScan ) THEN
        Cnsmu  = MAX(  0.5D0 ,  1.0D0- 0.3D0*MBAccRec )
        Ansmu  = MAX( 10.1D0 , 50.1D0-25.0D0*MBAccRec )
        NSMU   = MIN( Cnsmu*ABS(SMU0) , Ansmu )
        SCASMU = MIN(  1.0D0 ,  0.5D0+ 0.2D0*MBAccRec )
        LSMUMA = 5
        LSMUOK = 5
        NPT    = 0
        D0SMU0 = 0.d0
        ISCMIN = 0
        IF( AtCosm /= 0 ) THEN
          NSMU   = 0
          SCASMU = 5.d0
          LSMUMA = 15
          LSMUOK = 12
        ELSE IF( CARISC(ISCFIXE(1))(2:2) == 'I' .OR. CARISC(ISCFIXE(1))(1:2) == 'CS' ) THEN
          LSMUMA = 15
          LSMUOK = 12
        ENDIF
!
100     CALL MomentumScan( Get_Chi2_Libr_Min, NPT,NSMU,LSMUMA,LSMUOK,SMU0,SCASMU,D0SMU0, SMUMIN,CCCMIN,ISCMIN )
        IF( AtCosm == 0 ) THEN
          IF( CCCMIN < SCLOOK .AND. CCCMIN > DCLOOK .AND. SCASMU > 0.3D0 ) THEN
            Cnsmu  = MAX(  1.5D0 ,   5.0D0-  2.5D0*MBAccRec )
            Ansmu  = MAX( 60.1D0 , 250.1D0-125.0D0*MBAccRec )
            NSMU   = MIN( Cnsmu*ABS(SMU0) , Ansmu )
            SCASMU = MIN(  0.3D0 ,   0.1D0+  0.1D0*MBAccRec )
            GO TO 100
          ENDIF
        ENDIF
      ENDIF
    ELSE
      SMU0   = SMU
      SMUMIN = SMU0
    ENDIF
    CALL ktime_mubo('scan    ',2)
!
!
    IF( CCCMIN <= 4.d0*DCLOOK ) THEN
      NBLIBR = 1
      ISCLIBR(1) = ISCMIN
      SMU = SMUMIN
    ELSE IF( CCCMIN <= 30.d0*DCLOOK .AND. NBLIBR == 1 .AND. NBOKLI >= 1 ) THEN
      SMU = SMUMIN
    ELSE
      IF( NBLIBR == 1 .AND. CCCMIN <= 200.d0*DCLOOK ) THEN
        IF( doPhiRecovery(ISCLIBR(1)) ) THEN
          IF( MBPri >= 3 ) WRITE(*,*) ' >> try to recover problem due to bad phi prediction : CCCMIN ',CCCMIN
          call ktime_mubo('recovphi',1)
          SMU  = SMUMIN
          CALL recoverPhi( DCLOOK, iscadd)
          call ktime_mubo('recovphi',2)
          IF( iscadd > 0 ) THEN
            NBLIBR = 1
            ISCLIBR(1) = iscadd 
            GO TO 110
          ENDIF
        ENDIF
      ENDIF
      IF( ISENFI == 1 .AND. IFLECH_in < 1 ) THEN
        SMU = SMU0
        RETURN
      ELSE
        fitStatus = 1
        RETURN
      ENDIF
    ENDIF
110 IF(MBPri >= 3) THEN
      CALL GEOKLI
      CALL TRAKMU(0)
      CALL PRFILI(1)
    ENDIF
!
!-- Initial fit !
    IF( NFICAN >= NFIMAX ) THEN
      fitStatus = 3
      RETURN
    ELSE IF( NFSCAN >= NFSMAX(ISENFI) ) THEN
      fitStatus = 2
      RETURN
    ENDIF
    dzt1bak = DZT1
    dzt2bak = DZT2
    CALL GEOKLI
    maxcall = 300
    err     = (/ PAFITZ, PAFITS, PAFITZ, PAFITS, PAFITP /)
    IF(MBPri >= 3) THEN
      PRINT 944,ItypFit,NBFIXE,NBLIBR,maxcall
944   FORMAT('>>>>  Perform FIT with  ItypFit=',I2,                  &
             ',  Nb-Fix-Libr=',2I2,'  and  MaxCall=',I6,' !!!!')
      WRITE(*,'(" Before fit : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
    ENDIF
    call ktime_mubo('fit i 2 ',1)
    CALL minimizeFcn(5,maxcall,err)
    call ktime_mubo('fit i 2 ',2)
    NFICAN = NFICAN + 1
    NFSCAN = NFSCAN + 1
    CALL TRAKMU(0)
    IF(MBPri >= 3) THEN
      WRITE(*,'("  After fit : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
      CALL PRFILI(2)
    ENDIF
!-- Do not deal with too low Pt
    IF( VerySmallPt(LoosePtCut) ) THEN
      fitStatus = 1
      RETURN
    ENDIF  
    IF( FMIN > FLOOK1 ) THEN
      IF(IFLECH_in >= 1) THEN
        fitStatus = 1
        RETURN
      ENDIF
      DZT1 = dzt1bak
      DS1  = 0.D0
      DZT2 = dzt2bak
      DS2  = 0.D0
      SMU  = SMU0
      RETURN
    ENDIF
    ISCFIXE(2:NBLIBR+1) = ISCLIBR(1:NBLIBR)
     IDFIXE(2:NBLIBR+1) =  IDLIBR(1:NBLIBR)
    NBFIXE = NBLIBR + 1
!
 END SUBROUTINE primaryFit
!
!
!> Perform secondary scan before fit, and from the best momentum (SMU) found,
!> define in which stations one must look during the fit
!> return 0 if everything ok
!
 SUBROUTINE secondaryScan(fitStatus)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_Fit
 USE M_MB_PACONS
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: fitStatus
 INTEGER :: NSMU, NPT, ISCMIN, N0SMU0, LSMUMA, LSMUOK
 INTEGER :: IA, L, LLL, ISCLLL(NMATMA), iscadd
 REAL(8) :: CCCMIN, SMU0, Cnsmu, Ansmu, SCASMU, SMUMIN, D0SMU0
 REAL(8) :: AMPLI, CCC
 REAL(8) :: Chi2_Libr
 LOGICAL :: doPhiRecovery
 EXTERNAL :: Get_Chi2_LibrFixe_Min
!
    fitStatus = 1
!
! Momentum scan before fit !
! Estimated momentum is limited to 1 TeV
    CALL GEOKLI
    IF( NBFIXE + NBOKLI < 2 ) RETURN
    CCCMIN = 99999999.D0
    CALL ktime_mubo('scanB   ',1)
    IF( ABS(FixMoCur) < 0.1D0 ) THEN
      SMU0   = SIGN( MAX(CP1000,ABS(SMU)) , SMU )
      IF( NBFIXE <= 1 ) THEN
        Cnsmu  = MAX(  0.5D0 ,  1.0D0- 0.3D0*MBAccRec )
        Ansmu  = MAX( 10.1D0 , 50.1D0-25.0D0*MBAccRec )
        NSMU   = MIN( Cnsmu*ABS(SMU0) , Ansmu )
        SCASMU = MIN(  1.0D0 ,  0.5D0+ 0.2D0*MBAccRec )
      ELSE
        Cnsmu  = MAX(  0.3D0 ,  1.0D0- 0.5D0*MBAccRec )
        Ansmu  = MAX(  2.1D0 ,  5.1D0- 0.0D0*MBAccRec )
        NSMU   = MIN( Cnsmu*ABS(SMU0) , Ansmu )
        SCASMU = MIN(  0.3D0 ,  0.1D0+ 0.1D0*MBAccRec )
      ENDIF
      NPT    = 0
      SMUMIN = SMU0
      ISCMIN = 0
      D0SMU0 = 0.D0
      N0SMU0 = NSMU
      LSMUMA = 4
      LSMUOK = 999
      IF( NBFIXE <= 1 ) THEN
        IF( AtCosm /= 0 ) THEN
          NSMU   = 0
          SCASMU = 5.d0
          LSMUMA = 15
          LSMUOK = 12
        ELSE IF( CARISC(ISCFIXE(1))(2:2) == 'I' .OR. CARISC(ISCFIXE(1))(1:2) == 'CS' ) THEN
          LSMUMA = 15
        ENDIF
      ENDIF
!
520   CALL MomentumScan( Get_Chi2_LibrFixe_Min, NPT,NSMU,LSMUMA,LSMUOK,SMU0,SCASMU,D0SMU0, SMUMIN,CCCMIN,ISCMIN )
      IF( AtCosm == 0 .OR. NBFIXE > 1 ) THEN
        IF( CCCMIN < SCLOOK .AND. CCCMIN > DCLOOK/2.D0 .AND. SCASMU > 0.2D0 ) THEN
          Cnsmu  = MAX(  1.5D0 ,   5.0D0-  2.5D0*MBAccRec )
          Ansmu  = MAX( 60.1D0 , 250.1D0-125.0D0*MBAccRec )
          NSMU   = MIN( Cnsmu*ABS(SMU0) , Ansmu )
          SCASMU = MIN(  0.3D0 ,   0.1D0+  0.1D0*MBAccRec )
          GO TO 520
        ENDIF
        IF( NBOKLI > 1 ) THEN   ! if only one segment, no need to refine the scan
          IF( CCCMIN < SCLOOK .AND. CCCMIN > DCLOOK/3.D0 .AND. D0SMU0 < 0.1D0 ) THEN
            NSMU = N0SMU0
            D0SMU0 = 0.5D0
            GO TO 520
          ENDIF
        ENDIF
      ENDIF
      SMU = SMUMIN
    ELSE IF( NBFIXE == 1 ) THEN
      CALL DirectionScan( Get_Chi2_LibrFixe_Min, CCCMIN, ISCMIN )
    ENDIF
    CALL ktime_mubo('scanB   ',2)
!
    CALL TRAKMU(0)
    IF(MBPri >= 3) CALL PRFILI(1)
!
! From the best momentum (SMU) found, define in which stations
! one must look during the fit
!
    DO IA=1,3
      IF( IA == 1 )  AMPLI = 0.5D0
      IF( IA == 2 )  AMPLI = 2.0D0
      IF( IA == 3 )  AMPLI = 8.0D0
      IF( IA == 3 .AND. NBFIXE >= 2 ) AMPLI = 4.0D0
      LLL = 0
      DO L=1,NBLIBR
        CCC = Chi2_Libr(L)
        IF( CCC > AMPLI*DCLOOK )  CYCLE
        LLL = LLL + 1
        ISCLLL(LLL) = ISCLIBR(L)
      ENDDO
      IF( LLL >= 1 )              EXIT
      IF( IA >= 2 ) THEN
! do third case only if only one fixed station or two fixed stations without S measurement
        IF( NBFIXE >  2 )                                    EXIT
        IF( NBFIXE == 2 ) THEN
          IF( ISCFIXE(1) > NMATTR .OR. ISCFIXE(2) > NMATTR ) EXIT
          IF( IWDRAT(IDFIXE(1),1,ISCFIXE(1)) > 0 .OR.  &
              IWDRAT(IDFIXE(1),2,ISCFIXE(1)) > 0 .OR.  &
              IWDRAT(IDFIXE(2),1,ISCFIXE(2)) > 0 .OR.  &
              IWDRAT(IDFIXE(2),2,ISCFIXE(2)) > 0 )           EXIT 
        ENDIF
      ENDIF
    ENDDO
    IF( LLL <= 0 ) THEN
      IF( NBFIXE <= 1 ) THEN
        IF( ItypFit == 2 ) THEN  ! try to rescue cases when the bad chi2 is due to a bad phi prediction
          CCCMIN = 99999999.d0
          ISCMIN = 0
          DO L=1,NBLIBR
            CCC = Chi2_Libr(L)
            IF( CCC > 200.d0*DCLOOK )                        CYCLE
            IF( doPhiRecovery(ISCLIBR(L)) ) THEN
              IF( CCC < CCCMIN ) THEN
                CCCMIN = CCC
                ISCMIN = ISCLIBR(L)
              ENDIF
            ENDIF
          ENDDO
          IF( ISCMIN > 0 ) THEN
            NBLIBR = 1
            ISCLIBR(1) = ISCMIN
            IF( MBPri >= 3 ) WRITE(*,*) ' >> try to recover problem due to bad phi prediction : CCCMIN ',CCCMIN
            call ktime_mubo('recovphi',1)
            CALL recoverPhi( DCLOOK, iscadd)
            call ktime_mubo('recovphi',2)
            IF( iscadd > 0 ) THEN
              NBLIBR = 1
              ISCLIBR(1) = iscadd
              fitStatus = 0
              RETURN
            ENDIF
          ENDIF
        ENDIF
        RETURN
      ENDIF
    ENDIF
!
    NBLIBR = LLL
    ISCLIBR(1:NBLIBR) = ISCLLL(1:NBLIBR)
    fitStatus = 0
!
 END SUBROUTINE secondaryScan
!
!
!> Main fit, adding the NBLIBR stations to the track
!> rescue is used to try to rescue the first fit after the switch from ItypFit=2 to ItypFit=1
!
 SUBROUTINE mainFit(fitStatus,rescue)
 USE M_MB_Control
 USE M_MB_Fit
 USE M_MB_Reco_Tracks
 USE M_MB_PACONS
 IMPLICIT NONE
 LOGICAL, INTENT(IN)  :: rescue
 INTEGER, INTENT(OUT) :: fitStatus
 INTEGER :: maxcall, L, I1, I2, ntry
 REAL(8) :: err(5)
 REAL(8) :: dzt1bak, ds1bak, dzt2bak, ds2bak, smubak
!
    fitStatus = 0
    IF( NFICAN >= NFIMAX ) THEN
      fitStatus = 3
      RETURN
    ELSE IF( NFSCAN >= NFSMAX(ISENFI) ) THEN
      fitStatus = 2
      RETURN
    ENDIF
    NFICAN = NFICAN + 1
    NFSCAN = NFSCAN + 1
    IF( rescue ) THEN
      ntry    = 1
      dzt1bak = DZT1
      ds1bak  = DS1
      dzt2bak = DZT2
      ds2bak  = DS2
      smubak  = SMU
    ENDIF
    CALL GEOKLI
    IF( ItypFit == 1 ) call ktime_mubo('fit p 1 ',1)
    IF( ItypFit == 2 ) call ktime_mubo('fit p 2 ',1)
    maxcall = 300
    err = (/ PAFITZ, PAFITS, PAFITZ, PAFITS, PAFITP /)
100 CONTINUE
    IF(MBPri >= 3) THEN
      IF( rescue ) THEN
        PRINT 200,ItypFit,ntry,NBFIXE,NBLIBR,SIGMAK,maxcall
200     FORMAT('>>>>  Perform FIT with  ItypFit=',I2,',  Ntry=',I2,                  &
               ',  Nb-Fix-Libr=',2I2,',  SigmAK=',F5.1,'  and  MaxCall=',I6,' !!!!')
      ELSE IF( ItypFit /= 2 ) THEN 
        PRINT 201,ItypFit,NBFIXE,NBLIBR,SIGMAK,maxcall
201     FORMAT('>>>>  Perform FIT with  ItypFit=',I2,                                &
               ',  Nb-Fix-Libr=',2I2,',  SigmAK=',F5.1,'  and  MaxCall=',I6,' !!!!')
      ELSE
        PRINT 202,ItypFit,NBFIXE,NBLIBR,maxcall
202     FORMAT('>>>>  Perform FIT with  ItypFit=',I2,',  Nb-Fix-Libr=',2I2,'  and  MaxCall=',I6,' !!!!')
      ENDIF
      WRITE(*,'(" Before fit : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
    ENDIF
    CALL minimizeFcn(5,maxcall,err)
    IF(MBPri >= 3) THEN
      WRITE(*,'("  After fit : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
    ENDIF
    IF( rescue ) THEN
      IF( ntry == 1 ) THEN
        IF( FMIN > FLOOK1 ) THEN
          CALL Set_SIGMAK_CHINA_Z_S_X( 10.D0, 4.D0, 2.D0, 1.D0 )
          DZT1 = dzt1bak
          DS1  = ds1bak
          DZT2 = dzt2bak
          DS2  = ds2bak
          SMU  = smubak
          ntry = 2
          GO TO 100
        ENDIF
      ELSE IF( ntry == 2 ) THEN
        CALL Set_SIGMAK_CHINA_Z_S_X( 5.D0, 6.D0, 3.D0, 1.5D0 )
        ntry = 3
        GO TO 100
      ENDIF
    ENDIF
    IF( ItypFit == 1 ) call ktime_mubo('fit p 1 ',2)
    IF( ItypFit == 2 ) call ktime_mubo('fit p 2 ',2)
! If potential candidate is too bad, reject it already now
    IF( FMIN > DCLOOK*3.D0 ) THEN
      IF(MBPri >= 3) WRITE(*,'(">>>>  Fmin =",F15.4,"  =====>   Give up !!!")') FMIN
      fitStatus = 1
      RETURN
    ENDIF
    CALL TRAKMU(0)
!-- Do not deal with too low Pt
    IF( VerySmallPt(LoosePtCut) ) Then
      fitStatus = 1
      RETURN
    ENDIF
    DO L=1,NBLIBR
      IF( ItypFit == 1 ) THEN ! ask for at least one good digit
        DO I1=1,N4M
          DO I2=1,4
            IF( JZLIBR(I1,I2,L,1) > 0 .OR. JZLIBR(I1,I2,L,2) > 0 ) GO TO 300
          ENDDO
        ENDDO
        CYCLE
300     CONTINUE
      ENDIF
      NBFIXE = NBFIXE + 1
      ISCFIXE(NBFIXE) = ISCLIBR(L)
       IDFIXE(NBFIXE) =  IDLIBR(L)
    ENDDO
!
 END SUBROUTINE mainFit
!
!
!> Here one candidate track may have been found, from it we perform a
!> complementary search using all possible stations
!
 SUBROUTINE searchForCrossedStations
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_Digis
 USE M_MB_Fit
 USE M_MB_FAX
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER :: ISC, L, J, IC, INOF, INOZ, IA, LLL, I1, I2, ISCMIN, iscadd
 INTEGER :: ISCUSE(NMATMA), ISCLLL(NMATMA)
 REAL(8) :: AMPLI, AMPLA, CCC, DDD, CCCMIN
 REAL(8) :: Chi2_Libr, CCDRATfun
 LOGICAL :: doPhiRecovery
!
    NBLIBR = 0
    ISCUSE(:) = 0
    ISCUSE( ISCFIXE(1:NBFIXE) ) = 1
    IF( ItypFit == 2 ) THEN
!-- Determine all the (straight) segments to take into account in the fit of type 2
      DO ISC=1,NMACSC
        IF( NBDRAT(ISC) <= 0 .OR. ISCUSE(ISC) /= 0 ) CYCLE
        NBLIBR = NBLIBR + 1
        ISCLIBR(NBLIBR) = ISC
      ENDDO
    ELSE
! Determine all the multilayers to take into account in the fit of type 1
      CALL GIVDEB1(1)
      CALL DEFFAX( 1.D0, 1.D0 )
      IF(MBPri >= 3) PRINT 948,(ISCFOX(L),CARISC(ISCFAX(L)),IOFFAX(L,1),IOZFAX(L,1)    &
                              ,IOFFAX(L,2),IOZFAX(L,2),L=1,NBFAX)
948   FORMAT('  Crossed M.L. (Fax) :',I3,2X,A3,I6,I3,I6,I3)
      DO L=1,NBFAX
        ISC = ISCFAX(L)
        IF( ISCUSE(ISC) > 0 ) CYCLE
        DO J=1,2
          IC   = 2*ISC + 1 - J
          INOF = IOFFAX(L,J)
          INOZ = IOZFAX(L,J)
          IF( INOF > 0 .AND. INOZ > 0 ) IWcross(ISC,J) = IWCHA(IC,INOF,INOZ)
        ENDDO
        NBLIBR = NBLIBR + 1
        ISCLIBR(NBLIBR) = ISC
      ENDDO
      CALL Reset_MB_FAX
    ENDIF
    CALL GEOKLI
    CALL TRAKMU(9)
    IF(MBPri >= 3) CALL PRFILI(2)
!
    DO IA=1,3
      IF( IA == 1 ) THEN
        AMPLI = 1.0D0
        AMPLA = 999.D0
      ELSE IF( IA == 2 ) THEN
        AMPLI = 4.0D0
        AMPLA = 999.D0
      ELSE
        AMPLI = 12.0D0
        AMPLA = 0.1D0
      ENDIF
      LLL = 0
      DO L=1,NBLIBR
        CCC = Chi2_Libr(L)
        IF( CCC > AMPLI*DCLOOK )    CYCLE
        DDD = CCDRATfun( IDLIBR(L) , ISCLIBR(L) )
        IF( DDD > AMPLA*SIGMOK**2 ) CYCLE
        IF( ItypFit == 1 ) THEN ! ask for at least one good digit
          DO I1=1,N4M
            DO I2=1,4
              IF( JZLIBR(I1,I2,L,1) > 0 .OR. JZLIBR(I1,I2,L,2) > 0 ) GO TO 949
            ENDDO
          ENDDO
          CYCLE
949       CONTINUE
        ENDIF
        LLL = LLL + 1
        ISCLLL(LLL) = ISCLIBR(L)
      ENDDO
      IF( LLL >= 1 .OR. ItypFit /= 2 ) EXIT
    ENDDO
    IF( LLL >= 1 ) THEN
      NBLIBR = LLL
      ISCLIBR(1:NBLIBR) = ISCLLL(1:NBLIBR)
      RETURN
    ENDIF
    IF( ItypFit == 2 .AND. NBLIBR >= 1 ) THEN   ! try to rescue cases when the bad chi2 is due to a bad phi prediction
      CCCMIN = 99999999.d0
      ISCMIN = 0
      DO L=1,NBLIBR
        CCC = Chi2_Libr(L)
        IF( CCC > 200.d0*DCLOOK ) CYCLE
        IF( doPhiRecovery(ISCLIBR(L)) ) THEN
          IF( CCC < CCCMIN ) THEN
            CCCMIN = CCC
            ISCMIN = ISCLIBR(L)
          ENDIF
        ENDIF
      ENDDO
      IF( ISCMIN > 0 ) THEN
        NBLIBR = 1
        ISCLIBR(1) = ISCMIN
        IF( MBPri >= 3 ) WRITE(*,*) ' >> try to recover problem due to bad phi prediction : CCCMIN ',CCCMIN
        call ktime_mubo('recovphi',1)
        CALL recoverPhi( DCLOOK, iscadd)
        call ktime_mubo('recovphi',2)
        IF( iscadd > 0 ) THEN
          NBLIBR = 1
          ISCLIBR(1) = iscadd 
          RETURN
        ENDIF
      ENDIF
    ENDIF
    NBLIBR = 0   
!
 END SUBROUTINE searchForCrossedStations
!
!
!> Try to recover segment rejected due to bad phi prediction
!
 SUBROUTINE recoverPhi(chi2cut,iscadd)
 USE M_MB_Control
 USE M_MB_Fit
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: chi2cut
 INTEGER, INTENT(OUT) :: iscadd
 INTEGER :: maxcall, NN
 REAL(8) :: err(5)
 REAL(8) :: FMINbak, DZT1bak, DS1bak, DZT2bak, DS2bak, SMUbak, Cbak, C
 REAL(8) :: Chi2_Libr
!
    iscadd = 0
    IF( ItypFit /= 2 ) RETURN
    IF( NBLIBR  /= 1 ) RETURN
    FMINbak = FMIN
    DZT1bak = DZT1
    DS1bak  = DS1
    DZT2bak = DZT2
    DS2bak  = DS2
    SMUbak  = SMU
    CALL TRAKMU(0)
! Contribution Cbak from fixed stations with previous parameters
    Cbak =  0.D0
    DO NN=1,NBFIXE
      Cbak = Cbak + DZFIXE(NN,1,1) + DZFIXE(NN,2,1) + DSFIXE(NN,1,1) + DSFIXE(NN,2,1)
    ENDDO
    IF( AtCosm == 0 )  Cbak = Cbak + (REXTRA(1)/DREXTR(1))**2 + (ZEXTRA(1)/DZEXTR(1))**2
    maxcall = 200
    err = (/ 0.d0, 0.5d0*PAFITS, 0.d0, 0.5d0*PAFITS, 0.d0 /)
    CALL minimizeFcn(5,maxcall,err)
    CALL TRAKMU(0)
! Contribution from fixed stations ... to be compared to Cbak
    C =  0.D0
    DO NN=1,NBFIXE
      C = C + DZFIXE(NN,1,1) + DZFIXE(NN,2,1) + DSFIXE(NN,1,1) + DSFIXE(NN,2,1)
    ENDDO
    IF( AtCosm == 0 )  C = C + (REXTRA(1)/DREXTR(1))**2 + (ZEXTRA(1)/DZEXTR(1))**2
! end of contribution from fixed stations
    IF( MBPri >= 3 ) WRITE(*,*) ' segment tested : Cfix_old Cfix chi2_libr ',Cbak,C,Chi2_Libr(1)
    IF( C < Cbak + 0.5d0*chi2cut .AND. Chi2_Libr(1) < chi2cut ) THEN
      IF( MBPri >= 3 ) THEN
        WRITE(*,'(" Before fit : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1bak,DZT2bak,DS1bak,DS2bak,SMUbak
        WRITE(*,'("  After fit : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
        CALL PRFILI(2)
      ENDIF
      iscadd = ISCLIBR(1)
    ELSE
      DZT1 = DZT1bak
      DS1  = DS1bak
      DZT2 = DZT2bak
      DS2  = DS2bak
      SMU  = SMUbak
      CALL TRAKMU(0)
    ENDIF
    FMIN = FMINbak
!
 END SUBROUTINE recoverPhi
!
!
!> Check if we should try a phi recovery
!> because of uncertainty in phi prediction when in overlap region
!
 LOGICAL FUNCTION doPhiRecovery(iscl)
 USE M_MB_MuGeom
 USE M_MB_Fit
 USE M_MB_CODRAT
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: iscl  !< ISC of the station to be tested
 INTEGER :: i, nphi
!
    doPhiRecovery = .false.
    nphi = 0
    DO i=1,NBFIXE
! we must not have already found a segment in this sector
      IF( isLarge(iscl) .EQV. isLarge(ISCFIXE(i)) ) RETURN
      IF( IWDRAT(IDFIXE(i),1,ISCFIXE(i)) > 0 .OR. IWDRAT(IDFIXE(i),2,ISCFIXE(i)) > 0 ) nphi = nphi + 1
    ENDDO
    IF( nphi >= 2 ) RETURN   ! if more than two stations with strips, we suppose that phi is ok
    doPhiRecovery = .true.
!
 END FUNCTION doPhiRecovery
!
 SUBROUTINE PARABOL(X,Y,XMIN,IER)
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: X(3), Y(3)
 REAL(8), INTENT(OUT) :: XMIN
 INTEGER, INTENT(OUT) :: IER
 REAL(8) :: DENOM
!
   IF( ABS(Y(2)-Y(1)) < 0.00001d0 .AND. ABS(Y(2)-Y(3)) < 0.00001d0 ) THEN
     IER = 1
     XMIN = X(2)
   ELSE
     DENOM = (X(1)-X(2))*(Y(3)-Y(2)) - (X(3)-X(2))*(Y(1)-Y(2))
     IF( ABS(DENOM) < 1.d-18 ) THEN
       IER  = 2
       XMIN = X(2)
     ELSE
       IER = 0
       XMIN = ( (X(1)+X(2))*(X(1)-X(2))*(Y(3)-Y(2)) - (X(3)+X(2))*(X(3)-X(2))*(Y(1)-Y(2)) ) / DENOM / 2.d0
     ENDIF
   ENDIF
!
 END SUBROUTINE PARABOL
!
 SUBROUTINE listOfFixedStations
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: l, j, i4p, i4, it, icode, ic, iw, inof, inoz, igeom
 INTEGER :: intpos
!
   ngeofix = 0
   DO l=1,nbfixe
     DO j=1,2
       DO i4p=1,n4m
         DO i4=1,4
           it = izfixe(i4p,i4,l,j)
           IF( it <= 0 )                             CYCLE
           icode = ktubhi(it)
           ic    =  icode / 10000
           iw    = (icode - ic*10000) / 10
           inof  = ifcha(ic,iw)
           inoz  = izcha(ic,iw)
           igeom = igeoma(ic,inof,inoz)
           IF( intpos(igeom, lgeofix, ngeofix) > 0 ) CYCLE
           IF( ngeofix >= 20 )                       EXIT
           ngeofix = ngeofix + 1
           lgeofix(ngeofix) = igeom
         ENDDO
       ENDDO
     ENDDO
   ENDDO
!
 END SUBROUTINE listOfFixedStations
!
!
!> Get the initial approximation of the momentum (smu = 100/P)
!> from the segment position and angle
!
 SUBROUTINE getInitialSmu(ISC, X1,Y1,Z1, X2,Y2,Z2, Smu, iok)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_PRECIS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ISC
 REAL(8), INTENT(IN)  :: X1,Y1,Z1, X2,Y2,Z2
 INTEGER, INTENT(OUT) :: iok
 REAL(8), INTENT(OUT) :: Smu
 REAL(8) :: X21, Y21, Z21, X2PY2, A, XM, YM, ZM, RM
 REAL(8) :: PROSCA, SINTFA, PESTIM, CESTIM, SESTIM
 REAL(8) :: FACPHI
 REAL(8), PARAMETER :: RLOOK0 = 500.d0
 REAL(8), PARAMETER :: ZLOOK0 = 999999.d0
 REAL(8), PARAMETER :: CP9999 = 0.9999999d0
!
    iok = 1
    IF( AtCosm == 0 ) THEN
      X21 = X2 - X1
      Y21 = Y2 - Y1
      Z21 = Z2 - Z1
      X2PY2 = X21**2 + Y21**2
      IF( ABS(X2PY2) > 0.000001D0 ) THEN
        A = - ( X1*X21 + Y1*Y21 ) / X2PY2
        XM = X1 + A*X21
        YM = Y1 + A*Y21
        ZM = Z1 + A*Z21
        RM = SQRT( XM**2 + YM**2 )
      ELSE
        ZM = 9999999.0D0
        RM = 9999999.0D0
      ENDIF
      IF(MBPri >= 3) WRITE(*,'(2X,"RM =",F12.4/2X,"ZM =",F12.4)') RM,ZM
      IF( RM > RLOOK0 )      RETURN
      IF( ABS(ZM) > ZLOOK0 ) RETURN
      IF( ABS(FixMoCur) < 0.1D0 ) THEN
        PROSCA = (   A  * X2PY2 - Z1*Z21 ) / SQRT( ( A**2 * X2PY2 + Z1**2 ) * ( X2PY2 + Z21**2 ) )
        PROSCA = MIN( PROSCA , CP9999 )
        IF( ISC <= NCOUBAS2 ) THEN
          SINTFA = SQRT( (X1**2+Y1**2) / (X1**2+Y1**2+Z1**2) )
          SINTFA = SINTFA * FACPHI(Y1,X1)
          IF( CARISC(ISC)(2:2) == 'O' ) THEN
            CESTIM = 0.55D0
          ELSE
            CESTIM = 0.60D0
          ENDIF
        ELSE
          SINTFA = SQRT(     Z1**2     / (X1**2+Y1**2+Z1**2) )
          CESTIM = 0.70d0
        ENDIF
        PESTIM = CESTIM / SINTFA / ACOS(PROSCA)
        PESTIM = MAX( 2.5d0, MIN( 1000.d0, PESTIM ) )
        IF(ISC > NCOUBAS2 .AND. PESTIM < 20.D0) PESTIM = PESTIM + 5.D0
        IF(MBPri >= 3) WRITE(*,'(2X,"PESTIM =",F10.2)') PESTIM
        IF( RFIELD < 0.95d0 .OR. RFIELD > 1.05d0 ) PESTIM = RFIELD * PESTIM  ! scale estimate with field scale
        SESTIM = SFIELD * SIGN( 100.D0/PESTIM , ZM )
        CALL IMPRO_ESTIM(PESTIM,SESTIM)
        Smu = SESTIM
      ELSE
        Smu = - 100.D0/FixMoCur  ! 28-nov-2006  Warning : the sign of charge is inversed in GIVDEB1
      ENDIF
    ELSE
      IF( ABS(FixMoCur) < 0.1D0 ) THEN
        PESTIM        = 1000.d0
        Smu = - 100.d0/PESTIM
      ELSE
        PESTIM = ABS(FixMoCur)
        Smu = - 100.D0/FixMoCur
      ENDIF
    ENDIF
    iok = 0   
!
 END SUBROUTINE getInitialSmu
!
