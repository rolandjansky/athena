!
 SUBROUTINE RecoIni
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_Mat_TagInfo
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER :: IC, IMDT, I, L, newRpcTag
 REAL(8) :: AddResTra, AddResSeg
 REAL(8), SAVE :: RESOLD(NMDTMA)
 LOGICAL, SAVE :: FIRST=.true.
!
!>> Check initialisation of field
!
    CALL Initialise_Mubo_Tracking
    CALL Initialise_Mubo_Field(.false.)
!
!>> Alignement error of a multi-layer (e.g. 4 layers of tubes) in a mattress
    AligML(       1:NCOUCH) = AerML
    AligML(NCOUCH+1:NCOCSC) = CerML
!
!>> Alignment precision of a station
    AligST(         1:NCOUBAS2) = AligStBa
    AligST(NCOUBAS2+1:NMATTR  ) = AligStEc
    AligST(  NMATTR+1:NMACSC  ) = AligStCs
!
!>> Intrinsic resolution of a tube (cm)
    RESOTU(1:NMDTMA) = ReTub
!>> Tube efficiency
    EFFITU(1:NMDTMA) = TubEf
!
!>> Strip efficiency
    EFFIST(0:NRPCMA) = StrEf
!
!>> Enlargement of errors due to misalignement
    ResBadCha( :) = ResBadAli
    ResBadCha( 2) = ResBadBIS
    ResBadCha(16) = ResBadBEE
    ResBadCha(19) = ResBadEIL
    ResBadCha(20) = ResBadEIL
    ResBadCha(21) = ResBadEEL   ! EES
    ResBadCha(22) = ResBadEEL   ! EES
    ResBadCha(23) = ResBadEEL
    ResBadCha(24) = ResBadEEL
!
!>> Initialisation of fast RT flag
    LFastRT = DsFastRT > 0.0100D0
!
!>> Initialisation of effective resolution used in the pattern recog.
    AddResTra = 0.D0
    AddResSeg = 0.D0
    DO IC=1,NCOUCH
      AddResTra = AddResTra + AligML(IC)**2
      AddResSeg = AddResSeg + AligML(IC)**2
    ENDDO
    AddResTra = AddResTra / NCOUCH
    AddResSeg = AddResSeg / NCOUCH
    IF( MBPri >= 1 .AND. FIRST ) WRITE(*,'(/" >>> In RecoIni :")')
    IF( FIRST ) RESOLD(:) = RESTUB(:)
    DO IMDT=1,NMDTMA
      IF( RESOLD(IMDT) < 0.0001D0 ) THEN
        RESTUB(IMDT)   = 0.0075D0 + SQRT( RESOTU(IMDT)**2 + AddResTra )
        IF( RESTUB(IMDT) < 0.0150D0 ) RESTUB(IMDT) = 0.0150D0
        RESEGTUB(IMDT) = 0.0040D0 + SQRT( RESOTU(IMDT)**2 + AddResSeg )
        IF( RESEGTUB(IMDT) < 0.0120D0 ) RESEGTUB(IMDT) = 0.0120D0
      ELSE
        RESTUB(IMDT) = MAX( RESOLD(IMDT) , 0.0010D0 )
        RESEGTUB(IMDT) = RESTUB(IMDT)
      ENDIF
      IF( MBPri >= 1 .AND. FIRST ) PRINT 102,IMDT,RESOLD(IMDT),RESTUB(IMDT),RESEGTUB(IMDT)
102   FORMAT(' >>>  RESTUB(',I3,') :',F12.6,'  ----->',F12.6,' and for segments',F12.6)
    ENDDO
!
!>> Check the different cases of RPC digitization
!
    IF( NewTagInfoRpc == 1 ) THEN
      newRpcTag = 0
      DO I=1,NberTagInfo
        IF( TRIM(ChaTagInfoA(I)) == 'RPC_TimeSchema' ) THEN
          newRpcTag = 1
          L = LEN_TRIM(ChaTagInfoB(I))
          IF( ChaTagInfoB(I)(1:8) == 'Datalike' ) THEN
            RpcTimeSchema = 1  ! already corrected for Tof
            SELECT CASE( L )
              CASE( 30 )
                READ(ChaTagInfoB(I)(26:26),'(I1)') RpcShift
              CASE( 31 )
                READ(ChaTagInfoB(I)(26:27),'(I2)') RpcShift
              CASE( 32 )
                READ(ChaTagInfoB(I)(26:28),'(I3)') RpcShift
              CASE DEFAULT
                WRITE(*,*) ' Muonboy/Recoini error : unknown RPC_TimeSchema tag ',TRIM(ChaTagInfoB(I))
            END SELECT
          ELSE IF( ChaTagInfoB(I)(1:6) == 'G4like' ) THEN
            RpcTimeSchema = 2  ! must be corrected for tof
            SELECT CASE( L )
              CASE( 27 )
                READ(ChaTagInfoB(I)(23:23),'(I1)') RpcShift
              CASE( 28 )
                READ(ChaTagInfoB(I)(23:24),'(I2)') RpcShift
              CASE( 29 )
                READ(ChaTagInfoB(I)(23:25),'(I3)') RpcShift
              CASE DEFAULT
                WRITE(*,*) ' Muonboy/Recoini error : unknown RPC_TimeSchema tag ',TRIM(ChaTagInfoB(I))
            END SELECT
          ELSE
            WRITE(*,*) ' Muonboy/Recoini error : unknown RPC_TimeSchema tag ',TRIM(ChaTagInfoB(I))
            RpcTimeSchema = 1
            RpcShift      = 5
          ENDIF
          EXIT
        ENDIF
      ENDDO
!
      IF( newRpcTag == 0 ) THEN ! check problematic new productions without the RPC digitization in taginfo
        DO I=1,NberTagInfo
          IF( TRIM(ChaTagInfoA(I)) == 'AtlasRelease' ) THEN
            IF( ChaTagInfoB(I)(1:19) == 'AtlasOffline-15.3.1' ) THEN
              RpcTimeSchema = 1
              RpcShift      = 0
            ENDIF
          ENDIF                        
        ENDDO
      ENDIF
!
      NewTagInfoRpc = 0
    ENDIF
!
    IF( TStripMin > TStripMax ) THEN
      IF( AtCosm /= 0 ) THEN  ! no cut for cosmics
        RpcTimeSchema = 1
        StripTimeMin  = -900.d0
        StripTimeMax  =  900.d0
      ELSE IF( IsSimu == 0 ) THEN  ! temporary, loose cut for data
        RpcTimeSchema = 1
        StripTimeMin  =  50.d0
        StripTimeMax  = 160.d0
      ELSE
        StripTimeMin  = -10.d0 + RpcShift
        StripTimeMax  =  20.d0 + RpcShift
      ENDIF
    ELSE
      IF( AtCosm /= 0 .OR. IsSimu == 0 ) RpcTimeSchema = 1
      StripTimeMin = TStripMin
      StripTimeMax = TStripMax
    ENDIF
    StripTimeMoy = 0.5d0 * ( StripTimeMin + StripTimeMax ) 
!
!>> "Severity" of the reconstruction
    SEVERI = XSeve
!>> Maximum value of Chi2 before fitting
    DCLOOK =  600.D0 * SEVERI
!>> Maximum value of Chi2 to accept a track
    FLOOK1 =  300.D0 * SEVERI
!>> Maximum value of Chi2 to define a track as "good"
    FLOOK2 =  150.D0 * SEVERI
!>>  Maximum value of Chi2 for final selection
    IF( FinalCut > 0.d0 ) THEN
      FLOOK3 = FinalCut * SEVERI
    ELSE
      IF( UseAllSeg ) THEN
        FLOOK3 = 90.d0 * SEVERI  ! looser cut as RPC/TGC supposed to be bad
      ELSE
        FLOOK3 = 60.d0 * SEVERI
      ENDIF
    ENDIF
    IF( FLOOK3 > FLOOK2 ) FLOOK3 = FLOOK2
!>> Maximum value during momentum scan
    SCLOOK = 1000.d0 * DCLOOK
!>> Define a few constants for pattern recog.
!!!    SIGMOK = 3.0D0
!!!    SIGMOK = MAX( SIGMOK , SIGMOK*SEVERI )
    SIGMOK = 3.0D0 * SQRT(XSeve)
    CHI1ML = SIGMOK**2
!
    IF( FIRST ) THEN
      WRITE(*,*) '----------------------------------------------------------------------------------'
      WRITE(*,'(" Muonboy time windows for RPC strips ",F6.1," : ",F6.1)') StripTimeMin,StripTimeMax
      IF( RpcTimeSchema == 2 ) WRITE(*,'("   after applying TOF correction")')
      WRITE(*,*) '----------------------------------------------------------------------------------' 
!
      IF( NoField ) THEN
        WRITE(*,*) '----------------------------------------------------------------------------------'
        WRITE(*,*) '|                 The magnetic field will be put to 0                            |'
        WRITE(*,*) '----------------------------------------------------------------------------------'
      ENDIF
!
      IF( UseAllSeg .AND. AtCosm == 0 ) THEN
        WRITE(*,*) '--------------------------------------------------------------------------------------------'
        WRITE(*,*) '| warning the standard way of running Muonboy on collisions is with UseAllSegments = false |'
        WRITE(*,*) '--------------------------------------------------------------------------------------------'
      ENDIF
!
      IF( UseRoaMdt .AND. AtCosm /= 0 ) THEN
        WRITE(*,*) '----------------------------------------------------------------------------------------'
        WRITE(*,*) '| warning the standard way of running Muonboy on cosmics is with UseRoaFromMdt = false |'
        WRITE(*,*) '----------------------------------------------------------------------------------------'
      ENDIF
!
      IF( UseT0Fit .AND. .NOT.DoT0Fit ) THEN
        WRITE(*,*) '---------------------------------------------------------------------------------------------'
        WRITE(*,*) '| warning inconsistency in Muonboy Running : t0 are required to be used, but are not fitted |'
        WRITE(*,*) '---------------------------------------------------------------------------------------------'
      ENDIF
!
      WRITE(*,*) '-----------------------------------------------------------------------------------------------'
      WRITE(*,*) '| Error enlargement due to misalignement                                                      |'
      WRITE(*,'("|",3X,"BIS",F8.4,3X,"BEE",F8.4,3X,"EIL",F8.4,3X,"EEL",F8.4,3X,"others",F8.4,3X,"bad tomo",F8.4)')  &
                ResBadBIS,ResBadBEE,ResBadEIL,ResBadEEL,ResBadAli,ResBadTomo
      WRITE(*,*) '-----------------------------------------------------------------------------------------------'
!
      IF( DoAligFit > 0 ) THEN
        WRITE(*,*) '----------------------------------------------------------------------------------------'
        WRITE(*,*) '| Track fit will be done taking angular resolution of stations                         |'
        WRITE(*,*) '| Values used for barrel and endcap (in mrad) ',ResAngBa*1000.d0,ResAngEc*1000.d0
        WRITE(*,*) '----------------------------------------------------------------------------------------'
      ENDIF
!
      FIRST = .false.
    ENDIF
!
 END SUBROUTINE RecoIni
