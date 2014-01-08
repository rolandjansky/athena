!
 SUBROUTINE GEVTDUMPDI(LUNIN,IEVtoRead,TOSTORE,IRET,IDEVE,STORE)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_EventInfo
 USE M_MB_InfoSim
 USE M_MB_Persint
 USE M_MB_RTDump
 IMPLICIT NONE
!=================================
!
!>>
!>> Read one event on Dump Dice input file
!>>
!>> Input :
!>>        IEVtoRead   : event number to be read, -1 means next event 
!>>
!>> Output:
!>>        IRET  = 1/0 : new evt is available/no more evt
!>>        IDEVE       : evt id of the evt if available
!>>
!>> NB: IDEVE is also stored in the IEVBOX variable of Module M_MB_EventInfo
!>>
!
#include "AmdcStand/amdcsim_com.inc"
!
      INTEGER, INTENT(IN)    :: LUNIN,TOSTORE,IEVtoRead
      INTEGER, INTENT(OUT)   :: IRET,IDEVE
      INTEGER, INTENT(INOUT) :: STORE
      CHARACTER(256) :: LIGNE
      CHARACTER(6)   :: CHA6
      CHARACTER(3)   :: CHA3,CHA3B
      CHARACTER(80)  :: ATAB(9), NAME
      CHARACTER(1)   :: CPPChar(3)
      INTEGER        :: ITAB(9)
      REAL(8)        :: FTAB(12)
      INTEGER, SAVE :: JTYP=1,JFF=1,JZZ=1
      INTEGER, SAVE :: IEVTCOUNT=0
      INTEGER :: I,IERROR,JJJ,JOB,JSL,JTUBE,IDENT,JSPLI,JSZ,JSTRI,JSZ14
      INTEGER :: ICODE, noamCSC, nhitCSC, IC,INOF,INOZ,ICT,IGEOM
      REAL(8) :: XXX,YYY,ZZZ,THETA, SZflag, Temps
      REAL(8) :: DUMMY1,DUMMY2,DUMMY3,DUMMY4,T0S,DTS
      REAL(8) :: ADCcount,TDCcount,Particle,RSident,Amplitude,Driftime
      REAL(8) :: XYZ(3), XYZE(3), PXYZ(3), ETA, PHI, E, CODE, CHARGE
      INTEGER :: NCELL, IADR(MIDEN), ICHARGE, AthPosOff
      LOGICAL :: MDTremoved
      INTEGER :: JTROU3
      INTEGER, ALLOCATABLE :: IdenRT(:), IordRT(:)
      INTEGER :: NtubRT,ItubRT,IA,IB,IOK
      REAL(8) :: RA(3),RB(3),RC,RD(100)
      INTEGER :: Strip, Status, TimeStatus
      REAL(8) :: Reso
      LOGICAL :: KeepStation
!
      IRET  = 0
      IDEVE = 0
      ICODE = 0
      NtubRT = 0
      ItubRT = 0
      NdigMDT = 0
      nhitCSC = 0
      noamCSC = 0
      RTfromDump = .false.
      SWfromDump = .false.
      MDTremoved = .false.
      CALL Reset_MB_Digis
      CALL Reset_MB_InfoSim
      CALL Reset_MB_Persint
!
!>> Read one event
100   READ(LUNIN,'(A256)',END=910) LIGNE
      CHA3 = LIGNE(1:3)
      SELECT CASE (CHA3)   !---------------------------------------------
        CASE ( ' ==' )
          READ(LIGNE,'(6X,A6)') CHA6
          SELECT CASE (CHA6)
            CASE ( 'RTpara', 'RTPARA' )
              IF( LIGNE(13:14) == ' =' ) THEN
                READ(LIGNE(15:),*) IRTpara
              ELSE
                READ(LIGNE(13:),*) IRTpara
              ENDIF
              WRITE(*,'(" >>>> RT param used ",I4)')  IRTpara
              IF( IRTpara < 0 )  fixedRadius = REAL(-IRTpara,8) / 100.
            CASE ( 'MdtAdc', 'MDTADC' )
              READ(LIGNE(13:),*) MdtAdc
              WRITE(*,'(" >>>> MdtAdc  ",I6)') MdtAdc
          END SELECT
        CASE ( 'RTp', 'RTP' )
          CALL GENUCA(LIGNE,1,1,0,  ATAB,ITAB,FTAB,IERROR)
          IF( IERROR > 0 ) THEN
            WRITE(*,3000) LIGNE
            GO TO 100
          ELSE
            IRTpara = ITAB(1)
            WRITE(*,'(" >>>> RT param used ",I4)')  IRTpara
            IF( IRTpara < 0 )  fixedRadius = REAL(-IRTpara,8) / 100.
          ENDIF
        CASE ( 'NEW' )    !---------- new evt ------------------------------
          nevtlu = nevtlu + 1
          CALL GENUCA(LIGNE,2,5,0,  ATAB,ITAB,FTAB,IERROR)
          IF( IERROR >= 1 ) THEN
            CALL GENUCA(LIGNE,2,4,0,  ATAB,ITAB,FTAB,IERROR)
            IF( IERROR >= 1 ) THEN   ! format = evt run => AMDC indexes assumed
              CALL GENUCA(LIGNE,2,2,0,  ATAB,ITAB,FTAB,IERROR)
              IF( IERROR >= 1 ) THEN ! format = evt
                CALL GENUCA(LIGNE,2,1,0,  ATAB,ITAB,FTAB,IERROR)
                ITAB(2) = 0
              ENDIF
              ITAB(3) = 0
            ENDIF
            ITAB(5) = 0
          ENDIF
          IF( IERROR > 0 ) THEN
            WRITE(*,3000) LIGNE
            IEVTCOUNT = IEVTCOUNT + 1
            ITAB(1)   = IEVTCOUNT
            WRITE(*,3100) IDEVE
3100        FORMAT(' So, the (new) event number is assumed to be ;',I6)
          ENDIF
          IDEVE     = ITAB(1)
          IEVT      = ITAB(1)
          IRUN      = ITAB(2)
          ICODE     = ITAB(3)
          timestamp = ITAB(5)
          IF( IEVtoRead >= 0 ) THEN
            IF( IDEVE /= IEVtoRead ) THEN  ! not the right event, do not try to read it
              DO
                READ(LUNIN,'(A256)',END=910) LIGNE
                IF( LIGNE(1:7) == 'END EVT' ) EXIT
              ENDDO
              WRITE(*,*) ' Event ',IDEVE,' has been skipped '
              GO TO 100
            ENDIF
          ENDIF
#if defined(ATLAS_PERSINT)
          call downloadcdb
#endif
        CASE ( 'END' )   !--------------------------------------------------
          GO TO 900
        CASE ( 'HIT' )   !---------------------------------------------------
          CALL GENUCA(LIGNE,1,5,0,  ATAB,ITAB,FTAB,IERROR)
          IF( IERROR > 0 ) THEN
            CALL GENUCA(LIGNE,1,4,0,  ATAB,ITAB,FTAB,IERROR) ! old format without clusters
            ITAB(5) = 0
            IF( IERROR > 0 ) THEN
              WRITE(*,3000) LIGNE
              GO TO 100
            ENDIF
          ENDIF
          NdigMDT = ITAB(1)
          NdigRPC = ITAB(2)
          NdigTGC = ITAB(3)
          NdigCSC = ITAB(4)
          NcluCSC = ITAB(5)
          IF( NdigMDT > MaxMdtTubes ) THEN
            WRITE(*,*) ' Event-Run ',IDEVE,IRUN,' has been skipped '   &
                      ,'because it has ',NdigMDT,' tubes hits (max='   &
                      ,MaxMdtTubes,')'
            DO
              READ(LUNIN,'(A256)',END=910) LIGNE
              IF( LIGNE(1:3) == 'END' ) EXIT
            ENDDO
            GO TO 100
          ENDIF
          IF( IRTpara > 0 ) THEN
            ItubRT  = 0
            NRTdump = NdigMDT
            CALL ALLOC_RT
          ENDIF
        CASE ( 'TUB', 'CSC', 'MDT' )  !---- MDT and CSC -----------------------
          SZflag   =    0.d0
          ADCcount =  100.d0
          TDCcount =  100.d0
          Status   =  -99
          IF( ICODE == 0 ) THEN ! Amdc indices
            WRITE(*,*) " Starting from Muonboy-07-77-07, Amdc indices are no more supported in Dumps"
            WRITE(*,*) " ==> Stop "
            STOP
          ELSE ! Athena indices
            IF( CHA3 == 'MDT' ) THEN
              CALL GENUCA(LIGNE,2,5,9,  ATAB,ITAB,FTAB,IERROR)
              IF( IERROR >= 1 ) THEN ! MDT before 07-75-60
                CALL GENUCA(LIGNE,2,5,8,  ATAB,ITAB,FTAB,IERROR)
                FTAB(9) = 1.D0
              ENDIF
              IF( IERROR >= 1 ) THEN ! MDT before 07-72-02
                CALL GENUCA(LIGNE,2,5,7,ATAB,ITAB,FTAB,IERROR)
                Driftime = FTAB(4)
                Particle = FTAB(6)
                RSident  = FTAB(7)
              ELSE
                Driftime = FTAB(4)
                ADCcount = FTAB(5)
                Particle = FTAB(6)
                TDCcount = FTAB(7) 
                RSident  = FTAB(8)
                Status   = NINT( FTAB(9) )
              ENDIF
            ELSE
              CALL GENUCA(LIGNE,2,6,6,  ATAB,ITAB,FTAB,IERROR)
              Amplitude = FTAB(4)
              Particle  = FTAB(5)
              RSident   = FTAB(6)              
            ENDIF
            CHA3B = ATAB(2)(1:3)
            JTYP  = JTROU3(CHA3B,MTYP,CARTYP)
            IF( IERROR >= 1 .OR. JTYP <= 0 ) THEN
              WRITE(*,3000) LIGNE
              GO TO 100
            ENDIF
            DO I=1,3
              CPPChar(I) = CHA3B(I:I)
            ENDDO
            IF( CHA3 == 'MDT' ) THEN
              CALL GetAMDCindFromMDT(CPPChar,ITAB(1),ITAB(2),ITAB(3),ITAB(4),ITAB(5)  &
                                            ,JFF,JZZ,JOB,JSL,JTUBE)
            ELSE
              CALL GetAMDCindFromCSC(CPPChar,ITAB(1),ITAB(2),ITAB(3),ITAB(4),ITAB(5),ITAB(6)  &
                                            ,JFF,JZZ,JOB,JSL,JTUBE,SZflag)
            ENDIF
          ENDIF
!== Selection on stations if needed ========================
          IF( .NOT.KeepStation(CHA3B,Jtyp,Jff,Jzz,Job) ) THEN
            MDTremoved = .TRUE.
            GOTO 100
          ENDIF
!===========================================================
          FTAB(1:3) = FTAB(1:3) / 10.D0
          AthPosOff = 0
          IF( MBDum == -2 ) AthPosOff = 1    !>> Do not use the coordinates of the digit
          XXX = FTAB(1)
          YYY = FTAB(2)
          ZZZ = FTAB(3)
          JJJ = NINT(Particle)
          JSZ = 1
          IF( CHA3 == 'CSC' .AND. SZflag < 0.d0 ) JSZ = 2
          CALL Code_Ident_AMDC(JTYP,JFF,JZZ,JOB,JSL,JSZ,JTUBE, Ident)
          CALL GETIND(JTYP,JFF,JZZ,JOB, IC,INOF,INOZ, ICT)
          IF( IC+INOF+INOZ <= 0 ) THEN
            WRITE(*,*) ' Problem GETIND when reading a MDT/CSC hit '
            WRITE(*,*)  LIGNE
            IF( MBPri >= 1 ) THEN
              STOP
            ELSE
              GO TO 100
            ENDIF
          ENDIF
          IGEOM = IGEOMA(IC,INOF,INOZ)
          IF( isOffChav(IGEOM) ) GO TO 210
          IF( CHA3 == 'CSC' ) THEN
            nhitCSC = nhitCSC + 1
            IF( SZflag < 0 .AND. Amplitude > 0.D0 ) noamCSC = noamCSC+1
            IF( Status < 0 ) Status = 0 ! if old dumps, consider all as good CSC clusters
            CALL DIGITU(Ident,JTYP,JFF,JZZ,JOB,JSL,JTUBE,         &
                        XXX,YYY,ZZZ,Amplitude,0.d0,SZflag,JJJ,    &
                        Status,AthPosOff)
          ELSE
            IF( Status < 0 ) THEN
              Status = 1                                                  ! good hit
              IF( ADCcount <= 0.1d0 .AND. TDCcount <= 0.1d0 ) Status = 0  ! masked hit
            ELSE
! transition phase ... check that the old and new status definition agree
              IF( ADCcount <= 0.1d0 .AND. TDCcount <= 0.1d0 ) THEN
                IF( Status /= 0 ) THEN
                  WRITE(*,*) " GEVTDUMPDI : ADC TDC Status ",ADCcount,TDCcount,Status
                ENDIF
              ENDIF
            ENDIF
            IF( TOSTORE == 0 ) THEN
               CALL DIGITU(Ident,JTYP,JFF,JZZ,JOB,JSL,JTUBE,             &
                           XXX,YYY,ZZZ,ADCcount,TDCcount,Driftime,JJJ,   &
                           Status,AthPosOff)
#if !defined(ATLAS_STAND)
            ELSE
               CALL CSSTOREMDT(STORE,Ident,JTYP,JFF,JZZ,JOB,JSL,JTUBE,              &
                               XXX*10.,YYY*10.,ZZZ*10.,ADCcount,TDCcount,Driftime,  &
                               JJJ, Status)
#endif
            ENDIF
          ENDIF
210       CONTINUE
          IF( CHA3 == 'TUB' .OR. CHA3 == 'MDT' ) THEN
            IF( IRTpara > 0 ) THEN
              WRITE(*,*) ' obsolete option >>> STOP '
              STOP
            ELSE IF( IRTpara == 0 ) THEN
              IF(NtubRT == 0) THEN
                IF(NdigMDT == 0) THEN
                  PRINT '(" == GEVTDUMPDI : number of digits missing")'
                  NdigMDT = 3000
                ENDIF
                IF(ALLOCATED(IdenRT)) THEN
                  DEALLOCATE(IdenRT)
                  DEALLOCATE(IordRT)
                ENDIF
                ALLOCATE(IdenRT(NdigMDT))
                ALLOCATE(IordRT(NdigMDT))
              ENDIF
              NtubRT = NtubRT + 1
              IF(NtubRT > NdigMDT) THEN
                STOP  ' == GEVTDUMPDI : NdigMDT too small '
              ELSE
                IordRT(NtubRT) = NINT(RSident)
                IdenRT(NtubRT) = Ident
              ENDIF
            ENDIF
          ENDIF
        CASE ( 'RS2', 'RS3' )   !----------- RT relations ---------------------
          IF( IRTpara == 0 ) THEN   ! dont use RS lines if parametrization already asked for
            IF(NtubRT > 0) THEN
              IF(ItubRT == 0) THEN
                RTfromDump = .TRUE.
                NRTdump = NtubRT
                CALL ALLOC_RT
              ENDIF
              BACKSPACE(LUNIN)
              IF(CHA3 == 'RS2') THEN
                READ(LUNIN,*) CHA3,IA,RD(1)
                IB = 0
                RA = 0.D0
                RB = 0.D0
                RC = 0.D0
              ELSE
                READ(LUNIN,*) CHA3,IA,RA,RB,IB,RC,RD(1:IB+1)
              ENDIF
              IOK = 0
              DO I=1,NtubRT
                IF(IordRT(I) == IA) THEN
                  IOK = IdenRT(I)
                  EXIT
                ENDIF
              ENDDO
              IF( IOK == 0 .AND. .NOT.MDTremoved ) THEN
                PRINT '(" == GEVTDUMPDI : tube not found ",I4)',IA 
              ELSE IF( IOK /= 0 ) THEN
                ItubRT = ItubRT + 1
                IdeRTdump(     ItubRT) = IOK
                PosRTdump( 1:3,ItubRT) = RA / 10.D0
                DirRTdump( 1:3,ItubRT) = RB
                NpaRTdump(     ItubRT) = IB
                PasRTdump(     ItubRT) = RC / 10.D0
                RayRTdump(0:IB,ItubRT) = RD(1:IB+1) / 10.D0
              ENDIF
            ENDIF
          ENDIF
        CASE ( 'SXP', 'SYP', 'SZP', 'SXD', 'SYD', 'SZD' )   !----------- Wire Sagging stuff ---------------------
          IF( IRTpara == 0 ) THEN   ! dont use RS lines if parametrization already asked for
            IF(NtubRT > 0) THEN
              SWfromDump = .TRUE.
              BACKSPACE(LUNIN)
              READ(LUNIN,*) CHA3,IA,RA,RB,IB,RC,RD(1:IB+1)
              IOK = 0
              DO I=1,NtubRT
                IF(IordRT(I) == IA) THEN
                  IOK = IdenRT(I)
                  EXIT
                ENDIF
              ENDDO
              IF( IOK == 0 .AND. .NOT.MDTremoved ) THEN
                PRINT '(" == GEVTDUMPDI : tube not found ",I4)',IA 
              ELSE IF( IOK /= 0 ) THEN
                SELECT CASE ( CHA3 ) 
                  CASE( 'SXP' ); SXPdump(0:IB,ItubRT) = RD(1:IB+1)/10.D0
                  CASE( 'SYP' ); SYPdump(0:IB,ItubRT) = RD(1:IB+1)/10.D0
                  CASE( 'SZP' ); SZPdump(0:IB,ItubRT) = RD(1:IB+1)/10.D0
                  CASE( 'SXD' ); SXDdump(0:IB,ItubRT) = RD(1:IB+1)
                  CASE( 'SYD' ); SYDdump(0:IB,ItubRT) = RD(1:IB+1)
                  CASE( 'SZD' ); SZDdump(0:IB,ItubRT) = RD(1:IB+1)
                END SELECT
              ENDIF
            ENDIF
          ENDIF
        CASE ( 'TRI', 'RPC', 'TGC' )  !------- RPC and TGC -------------------
          TEMPS = -999.d0
          IF( ICODE == 0 ) THEN
            CALL GENUCA(LIGNE,2,7,6,  ATAB,ITAB,FTAB,IERROR)
            IF( IERROR >= 1) THEN
              CALL GENUCA(LIGNE,2,7,5,  ATAB,ITAB,FTAB,IERROR)
            ELSE
              TEMPS = FTAB(6)
            ENDIF
            CHA3B = ATAB(2)
            JTYP  = JTROU3(CHA3B,MTYP,CARTYP)
            IF( IERROR >= 1 .OR. JTYP <= 0 ) THEN
              PRINT 3000,LIGNE
              GO TO 100
            ENDIF
            JFF   = ITAB(1)
            JZZ   = ITAB(2)
            JOB   = ITAB(3)
            JSPLI = ITAB(4)
            JSL   = ITAB(5)
            JSZ   = ITAB(6)
            JSTRI = ITAB(7)
          ELSE
            IF(CHA3 == 'RPC' ) THEN
              CALL GENUCA(LIGNE,2,8,6,  ATAB,ITAB,FTAB,IERROR)
              TEMPS = FTAB(6)
            ELSE
              CALL GENUCA(LIGNE,2,5,5,  ATAB,ITAB,FTAB,IERROR)
            ENDIF
            CHA3B = ATAB(2)
            JTYP  = JTROU3(CHA3B,MTYP,CARTYP)
            IF( IERROR >= 1 .OR. JTYP <= 0 ) THEN
              PRINT 3000,LIGNE
              GO TO 100
            ENDIF
            DO I=1,3
              CPPChar(I) = CHA3B(I:I)
            ENDDO
            IF(CHA3 == 'RPC' ) THEN
              CALL GetAMDCindFromRPC(CPPChar,ITAB(1),ITAB(2),ITAB(3),ITAB(4),ITAB(5),ITAB(6),ITAB(7),ITAB(8)   &
                                            ,JFF,JZZ,JOB,JSPLI,JSL,JSZ,JSTRI)
            ELSE
              CALL GetAMDCindFromTGC(CPPChar,ITAB(1),ITAB(2),ITAB(3),ITAB(4),ITAB(5)   &
                                            ,JFF,JZZ,JOB,JSPLI,JSL,JSZ,JSTRI)
            ENDIF
          ENDIF
!== Selection on stations if needed ========================
          IF( .NOT.KeepStation(CHA3B,Jtyp,Jff,Jzz,Job) ) GOTO 100
!===========================================================
          FTAB(1:3) = FTAB(1:3) / 10.D0
          AthPosOff = 0
          IF( MBDum == -2 ) AthPosOff = 1    !>> Do not use the coordinates of the digit 
          XXX = FTAB(1)
          YYY = FTAB(2)
          ZZZ = FTAB(3)
          JJJ   = NINT(FTAB(4))
          JSZ14 = JSZ
          IF( JSPLI > 1 ) JSZ14 = JSZ14 + 2
          CALL Code_Ident_AMDC(JTYP,JFF,JZZ,JOB,JSL,JSZ14,JSTRI, Ident)
          CALL GETIND(JTYP,JFF,JZZ,JOB, IC,INOF,INOZ, ICT)
          IF( ICT <= 0 ) THEN
            WRITE(*,*) ' Problem GETIND when reading a RPC/TGC strip '
            WRITE(*,*)  LIGNE
            IF( MBPri >= 1 ) THEN
              STOP
            ELSE
              GO TO 100
            ENDIF
          ENDIF
          IF( isOffTrig(ICT) ) GO TO 220
          IF( TOSTORE == 0 ) THEN
             CALL DIGIBA(Ident,JTYP,JFF,JZZ,JOB,JSPLI, JSL,JSZ, JSTRI,   &
                         TEMPS, XXX,YYY,ZZZ,JJJ, AthPosOff)
#if !defined(ATLAS_STAND)
          ELSE
            IF(CHA3 == 'RPC' ) THEN
               CALL CSSTORERPC(STORE,Ident,JTYP,JFF,JZZ,JOB,JSPLI,JSL,JSZ,JSTRI,   &
                               TEMPS, XXX*10.,YYY*10.,ZZZ*10., JJJ)
            ELSE
               CALL CSSTORETGC(STORE,Ident,JTYP,JFF,JZZ,JOB,JSPLI,JSL,JSZ,JSTRI,   &
                               TEMPS, XXX*10.,YYY*10.,ZZZ*10., JJJ)
            ENDIF
#endif
          ENDIF
220       CONTINUE
        CASE ( 'CLU' )  !------- CSC clusters -------------------
          CALL GENUCA(LIGNE,2,6,10, ATAB,ITAB,FTAB,IERROR)
          IF( IERROR >= 1 ) THEN
            CALL GENUCA(LIGNE,2,6,9, ATAB,ITAB,FTAB,IERROR)
            FTAB(10) = 0.d0
            IF( IERROR >= 1 ) THEN
              CALL GENUCA(LIGNE,2,6,6, ATAB,ITAB,FTAB,IERROR)
              FTAB(7:10) = 0.D0
            ENDIF
          ENDIF
          CHA3B = ATAB(2)(1:3)
          JTYP  = JTROU3(CHA3B,MTYP,CARTYP)
          IF( IERROR >= 1 .OR. JTYP <= 0 ) THEN
            PRINT 3000,LIGNE
            GO TO 100
          ENDIF
          DO I=1,3
            CPPChar(I) = CHA3B(I:I)
          ENDDO
          IF( ICODE == 0 ) THEN
            JFF    = ITAB(1)
            JZZ    = ITAB(2)
            JOB    = ITAB(3)
            JSL    = ITAB(4)
            SZflag = REAL(ITAB(5),8)
          ELSE
            Strip = 1
            CALL GetAMDCindFromCSC(CPPChar,ITAB(1),ITAB(2),ITAB(3),ITAB(4),ITAB(5),Strip   & 
                                          ,JFF,JZZ,JOB,JSL,JTUBE,SZflag)
           
          ENDIF
!== Selection on stations if needed ========================
          IF( .NOT.KeepStation(CHA3B,Jtyp,Jff,Jzz,Job) ) GOTO 100
!===========================================================
          FTAB(1:4) = FTAB(1:4) / 10.D0
          XXX = FTAB(1)
          YYY = FTAB(2)
          ZZZ = FTAB(3)
          RESO = FTAB(4)
          JJJ  = NINT(FTAB(5))
          JTUBE = 1
          Temps      = FTAB(7)
          Amplitude  = FTAB(8)
          Status     = NINT(FTAB(9))
          TimeStatus = NINT(FTAB(10))
          JSZ = 1
          IF( SZflag < 0.d0 ) JSZ = 2
          CALL Code_Ident_AMDC(JTYP,JFF,JZZ,JOB,JSL,JSZ,JTUBE, Ident)
          CALL GETIND(JTYP,JFF,JZZ,JOB, IC,INOF,INOZ, ICT)
          IF( IC+INOF+INOZ <= 0 ) THEN
            WRITE(*,*) ' Problem GETIND when reading a CSC cluster '
            WRITE(*,*)  LIGNE
            IF( MBPri >= 1 ) THEN
              STOP
            ELSE
              GO TO 100
            ENDIF
          ENDIF
          IGEOM = IGEOMA(IC,INOF,INOZ)
          IF( isOffChav(IGEOM) ) GO TO 230
          IF( TOSTORE == 0 ) THEN
            CALL DigiClu(Ident,JTYP,JFF,JZZ,JOB,JSL,SZflag,            &
                         XXX,YYY,ZZZ,Reso,JJJ,Amplitude,Temps,Status,TimeStatus)
#if !defined(ATLAS_STAND)
          ELSE
            CALL CSSTORECSC(STORE,Ident,JTYP,JFF,JZZ,JOB,JSL,SZflag,         &
                            XXX*10.,YYY*10.,ZZZ*10.,Reso*10.,JJJ,Amplitude,  &
                            Temps, Status)
#endif
          ENDIF
230       CONTINUE
        CASE ( 'FIT' )  !---------------------------------------------------
          CALL GENUCA(LIGNE,1,0,4,  ATAB,ITAB,FTAB,IERROR)
          IF( IERROR >= 1 ) THEN
            PRINT 3000,LIGNE
            GO TO 100
          ENDIF
          NB2NDLEV = MIN( NB2NDLEV+1, M2NDLEV )
          AA2NDLEV(NB2NDLEV) = FTAB(1)
          BB2NDLEV(NB2NDLEV) = FTAB(2)
          Z02NDLEV(NB2NDLEV) = FTAB(3)
          T02NDLEV(NB2NDLEV) = FTAB(4)
          CALL Dim_Station(JTYP,JFF,JZZ, DUMMY1,DUMMY2,T0S,DUMMY3,DUMMY4,DTS )
          F02NDLEV(NB2NDLEV) = Pos_Phi_V(JTYP_F_Z(JTYP,JFF,JZZ))
          TC2NDLEV(NB2NDLEV) = Pos_R_V(JTYP_F_Z(JTYP,JFF,JZZ))/10.D0 + T0S
          DT2NDLEV(NB2NDLEV) = DTS
        CASE ( 'CAL' )   !------------ Calo cells ------------------------
          CALL GENUCA(LIGNE,2,0,12,ATAB,ITAB,FTAB,IERROR)
! for backward compatibility
          IF( IERROR >= 1 ) THEN     
             CALL GENUCA(LIGNE,2,0,11,ATAB,ITAB,FTAB,IERROR)
             IF( IERROR >= 1 ) THEN
                CALL GENUCA(LIGNE,2,0,5,ATAB,ITAB,FTAB,IERROR)
                IF( IERROR >= 1 ) THEN
                   PRINT 3000,LIGNE
                   GO TO 100
                ELSE
                   FTAB(6:12) = 0.D0
                ENDIF
             ELSE
                FTAB(12) = 0.D0
             ENDIF
          ENDIF
          CALL FillCell( ATAB(2), NINT(FTAB(4)), FTAB(1)/10.D0,          &
                         FTAB(5)/1000.D0, FTAB(2), FTAB(3),              &
                         FTAB(6), FTAB(7), FTAB(8)/10.D0,                &
                         FTAB(9)/10.D0, FTAB(10)/10.D0, FTAB(11)/10.D0,  &
                         NINT(FTAB(12)))
        CASE ( 'EGA' )  !-------------- Elec-gamma objects ---------------
! EGA CollectionName X Y Z E Eta Phi Charge 
          CALL GENUCA(LIGNE,2,0,10,ATAB,ITAB,FTAB,IERROR)
          IF( IERROR >= 1 ) THEN
            PRINT 3000,LIGNE
            GO TO 100
          ENDIF
          NAME = ATAB(2)
          XYZ(1) = FTAB(1)/10.D0
          XYZ(2) = FTAB(2)/10.D0
          XYZ(3) = FTAB(3)/10.D0
          PXYZ(1) = FTAB(4)/1000.D0
          PXYZ(2) = FTAB(5)/1000.D0
          PXYZ(3) = FTAB(6)/1000.D0
          E = FTAB(7)/1000.D0
          ETA = FTAB(8)
          PHI = FTAB(9)
          CHARGE  = FTAB(10)
          ICHARGE = NINT(CHARGE)
! read-in the cell identifiers
! Stupid! formatted read!
          READ(LUNIN,*) NCELL
          IF( NCELL > MIDEN ) THEN
            WRITE(*,*) ' GEVTDUMPDI : Increase MIDEN to ',NCELL
            NCELL = MIDEN
          ENDIF
          READ(LUNIN,'(I10,9I11)') IADR(1:NCELL)
          CALL FillElga( NAME, ICHARGE, XYZ(1), XYZ(2), XYZ(3),   &
                         PXYZ(1), PXYZ(2), PXYZ(3),               &
                         E, ETA, PHI, NCELL, IADR)
! If Charged object fill in track particle
          IF ( ICHARGE /= 0 ) THEN
             CODE = -11.d0
             IF ( ICHARGE > 0 ) CODE = 11.d0
             XYZE(1:3) = 0.D0
             CALL FillTRK( 'Egamma', NAME, XYZ(1:3), PXYZ(1:3), CHARGE, CODE, XYZE(1:3))
          ENDIF
        CASE ( 'TRA' )  !------------------------------------------------
!==  Read :  Icolor   a0   z0   cotg(the)   phi   1/pt
          CALL GENUCA(LIGNE,1,1,5,  ATAB,ITAB,FTAB,IERROR)
          IF( IERROR >= 1 ) THEN
            PRINT 3000,LIGNE
3000        FORMAT(/' In routine GEVTDUMPDI, input line : ',A130,   /'  is un-understandable !!!!!!')
            GO TO 100
          ENDIF
          IF( ABS(1.D0/FTAB(5)) >= PTMIN_TRAID ) THEN
            NBTRAID = MIN( NBTRAID+1 , MTRAID )
            THETA = ATAN2( 1.D0 , FTAB(3) )
            JCTRAID(NBTRAID) =   ITAB(1)
            XXTRAID(NBTRAID) =   FTAB(1) * SIN(FTAB(4))
            YYTRAID(NBTRAID) = - FTAB(1) * COS(FTAB(4))
            ZZTRAID(NBTRAID) =   FTAB(2)
            EETRAID(NBTRAID) = - LOG( TAN( THETA/2.D0 ) )
            FFTRAID(NBTRAID) =   FTAB(4)
            PTTRAID(NBTRAID) = 1.D0 / FTAB(5)
            IReprTRAID(NBTRAID) = 1
            ISignTRAID(NBTRAID) = 0
          ENDIF
        CASE ( 'TRK' )  !-------------------------------------------------
!== Read TRK name container origin X0 Y0 Z0 PX PY PZ charge PDGcode
          CALL GENUCA(LIGNE,4,0,8, ATAB,ITAB,FTAB,IERROR)
!== end vertex added for MC Truth
!== Read TRK name container origin X0 Y0 Z0 XE YE ZE PX PY PZ charge PDGcode
          IF ( ATAB(3) == 'TruthEvent' ) THEN
             CALL GENUCA(LIGNE,4,0,11, ATAB,ITAB,FTAB,IERROR)
             IF( IERROR >= 1 ) THEN ! old format, kept for backward compatibility
                CALL GENUCA(LIGNE,4,0,8, ATAB,ITAB,FTAB,IERROR)
                XYZE(1:3) = 0.D0
             ELSE
                XYZE(1:3) = FTAB(4:6)/10.D0
                FTAB(4:8) = FTAB(7:11)
             ENDIF
          ELSE
             XYZE(1:3) = 0.D0
          ENDIF
          IF( IERROR >= 1 ) THEN  ! old format, kept for backward compatibility
!== Read TRK name container X0 Y0 Z0 PX PY PZ charge/PDGcode
             CALL GENUCA(LIGNE,3,0,7, ATAB,ITAB,FTAB,IERROR)
             IF( IERROR >= 1 ) THEN ! old format, kept for backward compatibility
!== Read  Sim   PDGcode  x0   y0   z0   Px   Py   Pz
                CALL GENUCA(LIGNE,2,1,6,  ATAB,ITAB,FTAB,IERROR)
                IF( IERROR >= 1 .OR. ATAB(2) /= 'Sim' ) THEN
                   PRINT 3000,LIGNE
                   GO TO 100
                ENDIF
                ATAB(2) = 'TrackRecord'
                ATAB(3) = 'MuonEntryLayerFilter'
                FTAB(7) = ITAB(1)
             ENDIF
             IF( ATAB(2) == 'TrackRecord' ) THEN
                FTAB(8) = FTAB(7)
                FTAB(7) = 1.D0
                IF( NINT(FTAB(8)) == 13 )  FTAB(7) = -1.D0
             ELSE
                FTAB(8) = 0.D0
             ENDIF
             ATAB(4) = 'Dummy'
          ENDIF
          IF( ATAB(3) == 'MuonEntryRecordFilter' ) ATAB(3) = 'MuonEntryLayerFilter'  ! old name changedfor backward compatibility
          FTAB(1:3) = FTAB(1:3)/10.D0
          FTAB(4:6) = FTAB(4:6)/1000.D0
          IF( ATAB(2) == 'TrackRecord' .AND. ATAB(3) == 'MuonEntryLayerFilter' ) THEN
            CALL AddTrSim(NINT(FTAB(8)), FTAB(1:3), FTAB(4:6))
          ENDIF
          CALL FillTRK( ATAB(2), ATAB(3), FTAB(1:3), FTAB(4:6), FTAB(7), FTAB(8), XYZE(1:3) )
        CASE ( 'SEG' )  !------------------------------------------------
          CALL GENUCA(LIGNE,3,0,9, ATAB,ITAB,FTAB,IERROR)
          IF( IERROR >= 1 ) THEN
            CALL GENUCA(LIGNE,3,0,7, ATAB,ITAB,FTAB,IERROR)
            IF( IERROR >= 1 ) THEN
              CALL GENUCA(LIGNE,3,0,6, ATAB,ITAB,FTAB,IERROR)
              FTAB(7) = -1.d0
            ENDIF
            FTAB(8) = 9999.d0
            FTAB(9) = 9999.d0
          ENDIF
          IF( ATAB(3) == 'ConvertedMBoySegments' .AND. NSegAth < MaxSegAth ) THEN
            NSegAth = NSegAth + 1
            PtSegAth(1:3,NSegAth) = FTAB(1:3)*0.1d0
            VeSegAth(1:3,NSegAth) = FTAB(4:6)
            ChSegAth(    NSegAth) = FTAB(7)
            T0SegAth(    NSegAth) = FTAB(8)
            ContSegAth(NSegAth) = ATAB(3)
          ELSE IF( ATAB(3) == 'MooreSegments' .AND. NSegMoo < MaxSegMoo ) THEN
            NSegMoo = NSegMoo + 1
            PtSegMoo(1:3,NSegMoo) = FTAB(1:3)*0.1d0
            VeSegMoo(1:3,NSegMoo) = FTAB(4:6)
            ChSegMoo(    NSegMoo) = FTAB(7)
            T0SegMoo(    NSegMoo) = FTAB(8)
            ContSegMoo(NSegMoo) = ATAB(3)
          ENDIF
        CASE ( 'TRT' ) !--------- TRT hits -----------------------------
          CALL GENUCA(LIGNE,2,5,8, ATAB,ITAB,FTAB,IERROR)
          FTAB(2:8) = FTAB(2:8)/10.d0
          CALL FillTRT(ATAB(2),ITAB(1:5),FTAB(1),FTAB(2),FTAB(3:5),FTAB(6:8))
        CASE ( 'SCT' ) !--------- SCT hits -----------------------------
          ! to be done
        CASE ( 'PIX' ) !--------- Pixel hits ---------------------------
          ! to be done
        CASE ( 'VTX' ) !------------------------------------------------
          CALL GENUCA(LIGNE,2,0,3, ATAB,ITAB,FTAB,IERROR)
          IF( IERROR == 0 ) THEN
            FTAB(1:3) = FTAB(1:3) * 0.1d0
            CALL FillVert(ATAB(2),FTAB(1:3))
          ENDIF
        CASE ( 'JET' ) !------------ Jet -------------------------------
          CALL GENUCA(LIGNE,2,0,4, ATAB,ITAB,FTAB,IERROR)
          IF( IERROR == 0 ) CALL FillJet(ATAB(2),FTAB(1:4))
        CASE ( 'MET' ) !------------- MET ------------------------------
          CALL GENUCA(LIGNE,2,0,2, ATAB,ITAB,FTAB,IERROR)
          IF( IERROR == 0 ) CALL FillMet(ATAB(2),FTAB(1:2))
        CASE ( 'ROI' ) !------------- Road Of Interest ------------------------------
          CALL GENUCA(LIGNE,2,0,3, ATAB,ITAB,FTAB,IERROR)
          IF( IERROR == 0 ) CALL FillRoi(ATAB(2),FTAB(1:3))
        CASE DEFAULT  !----------------------------------------------
          IF( CHA3 /= '   ' .AND. CHA3 /= 'H8A' .AND. CHA3 /= 'GEO'   &
                            .AND. CHA3 /= 'TAG' .AND. CHA3 /= 'CDB'   &
                            .AND. CHA3 /= 'A2G' .AND. CHA3 /= 'COM'   &
                            .AND. CHA3 /= 'TSO' .AND. CHA3 /= 'SIM'   &
                            .AND. CHA3(1:2) /= 'C ' ) THEN
            WRITE(*,'(/" In routine GEVTDUMPDI : Keyword : ",A3," is unknown (?)  =====>  STOP !")') CHA3
            WRITE(*,*) LIGNE
            STOP
          ENDIF
      END SELECT
      GO TO 100
!
!
900   CONTINUE
      IF(ALLOCATED(IdenRT)) THEN
        DEALLOCATE(IdenRT)
        DEALLOCATE(IordRT)
      ENDIF
!>> End of event, store evt id
      IEVBOX    = IDEVE
      IEVTCOUNT = IDEVE
!== and keep the event
      IRET      = 1
      RETURN
!
910   CONTINUE
      IF(ALLOCATED(IdenRT)) THEN
        DEALLOCATE(IdenRT)
        DEALLOCATE(IordRT)
      ENDIF
!== EoF encountered !!!
      IEVTCOUNT = 0
      RETURN
!
 END SUBROUTINE GEVTDUMPDI
!
