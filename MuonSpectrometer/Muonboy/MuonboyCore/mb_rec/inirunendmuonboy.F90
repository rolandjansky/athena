!
!> \author M.Virchaux
!
!--------------------------------------------------------------------
!> This routine initializes Muonboy
!--------------------------------------------------------------------
 SUBROUTINE InitialiseMuonboy
!
 USE M_MB_Control
#if defined(ATLAS_STAND)
 USE M_MB_StandAlone
#endif
 IMPLICIT NONE
#if defined(ATLAS_STAND)
 CHARACTER(len=4) :: Car4='xxxx'
 INTEGER          :: ierr
 LOGICAL          :: FullMapFile=.false.
#endif
!
!-- Initialise constants and geometry parameters of the MUONBOY package
!
 INTEGER, SAVE :: IPASS = 0
!
!-- Initialisation is made only once
   IF(IPASS >= 1) RETURN
   IPASS = IPASS + 1
   CALL ktime_mubo('initiali', 1)
!-- Basic initialisation of MUONBOY Modules
   CALL INCOMU
!
#if defined(ATLAS_STAND)
!-- In stand-alone mode : Read MUONBOY control data cards on input (LUN=29)
!--                       --> Fill Module  M_MB_Control
   OPEN( unit=29, file='default_data_cards', status='old')
   CALL ReadMBcontrol(29)
   CLOSE(29)
!
!-- In stand-alone mode : Read input 3D field B file on LUNBFIELD   and ...
   INQUIRE( file='bmagatlas_09_fullAsym20400.data', exist=FullMapFile)
   IF( FullMapFile ) THEN
     Call READ_MAG_FIELD('bmagatlas_09_fullAsym20400.data','FBYTE',ierr)
     IF( ierr /= 0 ) THEN
       WRITE(*,*) ' Error from READ_MAG_FIELD ',ierr
       STOP
     ENDIF
     Call REBUILD_LOOKUP(ierr)
     IF( ierr /= 0 ) THEN
       WRITE(*,*) ' Error from REBUILD_LOOKUP ',ierr
       STOP
     ENDIF
     FieldVers = 'FULL'
   ELSE
     CALL RBMAGA(LUNBFIELD,Car4)
     FieldVers = Car4
   ENDIF
!-- Read chamber layout and geometry on amdc_simrec data base
!-- on LUNAMDC <--+    +----> Convert TGC radial coordinates to cartesian coordinates
!--               |    |+---> Obsolete (was used to transform MDT multilayers from 3 to 4)
!--               |    ||+--> Do NOT print out AMDC
!--               |    |||
   CALL REAMDC(LUNAMDC,100)
!
!-- First read global tags (GEOmetry, TAGinfo and CondDataBase)
   CALL readTags(LUNHIT)
!
#endif
!
!-- Fill Modules  M_MB_MuGeom
!-- Transfer chamber layout and geometry from amdc_simrec data base
!-- to MUONBOY geometry
   CALL AMDCOX
!-- Initialisation of pattern recognition proper !
   CALL PATINI
!-- Initialise also some reconstruction parameters
!-- (this routine is called again before each event)
   CALL RecoIni
!
!-- Open file of rec. events when in Athena
   Lun_RecInfo = 0
   Lun_Timing  = 0
   IF( MBPri >= 0 ) THEN
     Lun_RecInfo = 22
     Lun_Timing  = 23
     Open(unit=Lun_RecInfo, file='Out.MuonboyRecInfo', status='unknown')
     Open(unit=Lun_Timing,  file='Out.MuonboyTiming',  status='unknown')
   ENDIF
!
   CALL ktime_mubo('initiali', 2)
!
 END SUBROUTINE InitialiseMuonboy
!
!--------------------------------------------------------------------
!> This routine runs Muonboy. It contains the basic steering.
!--------------------------------------------------------------------
 SUBROUTINE RunMuonboy
 USE M_MB_Control
 USE M_MB_EventInfo
 IMPLICIT NONE
 INTEGER, SAVE :: NROA = 0
 INTEGER :: nscan
 REAL    :: TMUON_IN, TMUON_OUT
 REAL(8) :: TEROA(20), FIROA(20), DTEROA(20), DFIROA(20)
 LOGICAL :: LFastRTref
 REAL(8) :: DsFastRTref
 REAL(8), PARAMETER :: bigDs=99999999.d0
!
!-- Steering routine for the  MUONBOY pattern recognition and reconstruction
!-- of tracks in the muon toroids (Barrel and End caps)
!-- INPUT  : MDT Hits in module M_MB_Digis
!-- OUTPUT : Track candidates in Module  M_MB_Reco_Tracks
!
    IpassReco = IpassReco + 1
!
    IF( MBSto <= -2 ) RETURN
!
!-- Possible refit of T0s
    doRTmiddle = .false.        ! T0 fit already done at middle of station

    IF( DoT0fit ) THEN
      CALL ktime_mubo('t0fitter',1)
      LFastRTref  = LFastRT
      DsFastRTref = DsFastRT
      LFastRT     = .true.
      DsFastRT    = bigDs       ! T0 fit is done with radius constant along wire
      nscan = NSteT0Fit
      CALL setenabletomap       ! enable the t0 map
      CALL t0fitter(nscan)
!     call dumpt0shift
!     call setverboset0shift
      IF( .NOT.UseT0fit ) THEN
        CALL setdisabletomap ! disable the t0 map if not needed by reco
        LFastRT  = LFastRTref
        DsFastRT = DsFastRTref
      ELSE
        doRTmiddle = .true.
      ENDIF
      CALL ktime_mubo('t0fitter',2)
    ELSE
      CALL setdisabletomap
    ENDIF
!
    IF( MBPri >= 3 ) WRITE(*,100) ' Start'
!
!-- Actual pat. recog. + reconstruction ---> Fill Module  M_MB_Reco_Tracks
    CALL CPU_TIME( TMUON_IN )
    CALL ktime_mubo('recoseg ',1)
#if !defined(ATLAS_STAND)
    CALL getnewsegfindflag
    IF ( NewSegFind .EQ. 0 ) THEN
        IF(MBPri >= 3) THEN
            print *,'********************CALLING RecoSeg'
        ENDIF
#endif
        CALL RecoSeg(NROA,TEROA,FIROA,DTEROA,DFIROA)
#if !defined(ATLAS_STAND)
    ELSE
        IF(MBPri >= 3) THEN
            print *,'********************CALLING NewRecoSeg'
        ENDIF
        CALL NewRecoSeg(NROA,TEROA,FIROA,DTEROA,DFIROA)
    ENDIF
#endif
    CALL ktime_mubo('recoseg ',2)
    IF( MBSto <= 1 ) THEN
      IF(MBPri >= 1) WRITE(*,'(/" STOP reconstruction after STRICT+LOOSE straight track segment search !")')
      CALL CPU_TIME( TMUON_OUT )
      TRECEVT = TMUON_OUT - TMUON_IN
      GO TO 900
    ENDIF
    CALL ktime_mubo('recotra ',1)
    CALL RecoTra
    CALL ktime_mubo('recotra ',2)
    CALL CPU_TIME( TMUON_OUT )
    TRECEVT = TMUON_OUT - TMUON_IN
!
!-- When in Athena, save info on rec. event
    IF(Lun_RecInfo > 0) CALL WRICAN(Lun_RecInfo,Lun_Timing)
!
!-- Print Module  M_MB_Reco_Tracks
    IF(MBPri >= 1) CALL PRCOCA
!
900 CONTINUE
!
! restore input values if modified (only necessary if UseT0fit is true)
    IF( DoT0fit ) THEN
      LFastRT  = LFastRTref
      DsFastRT = DsFastRTref
    ENDIF
!
!-- When asked, one does some printouts
    IF( MBPri >= 3 ) THEN
      WRITE(*,100) '  End '
      CALL MUDEMO01
      CALL MUDEMO03
      CALL MUDEMO04
      IF( MBPri >= 4 ) CALL MUDEMO05
    ENDIF
!
100 FORMAT(/'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'   &
           /'XXXXXXX>',A6,' Muon Reconstruction <XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'   &
           /'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'/)
!
 END SUBROUTINE RunMuonboy
!
!--------------------------------------------------------------------
!> This routine ends Muonboy
!--------------------------------------------------------------------
 SUBROUTINE EndMuonboy
!
 USE M_MB_Control
 IMPLICIT NONE
!
   IF( Lun_RecInfo > 0 ) Close( unit=Lun_RecInfo )
   IF( Lun_Timing  > 0 ) Close( unit=Lun_Timing  )
   call ktime_mubo('the end ',3)
   
   print *,'CALLING EndRunMuonboy'
   call EndRunMuonboy
!
 END SUBROUTINE EndMuonboy
!
