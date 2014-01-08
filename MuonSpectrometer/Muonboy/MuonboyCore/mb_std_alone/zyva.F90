!
 PROGRAM ZYVA
 USE M_MB_Control
 USE M_MB_StandAlone
 IMPLICIT NONE
 CHARACTER(80) :: IDFIELD, FOUTPUT,FRECONS,FTIMING
 CHARACTER(6)  :: CAR6
 INTEGER :: Iversion, IRET, IDEVE, Idum
!
    CALL GET_ENVIRONMENT_VARIABLE('IDFIELD',IDFIELD)
    CALL GET_MUONBOY_VERSION( Iversion )
    WRITE (CAR6,'(I6)') Iversion
    CAR6 = ADJUSTL( CAR6 )
    FOUTPUT = 'muonboy'//TRIM(CAR6)//'.output.'//IDFIELD
    FRECONS = 'muonboy'//TRIM(CAR6)//'.recons.'//IDFIELD
    FTIMING = 'muonboy'//TRIM(CAR6)//'.timing.'//IDFIELD
!
!>> The 3d field map file
    OPEN( unit=LUNBFIELD, file='default_bmagatlas', status='old')
!>> The chamber layout and geometry data base
    OPEN( unit=LUNAMDC,   file='default_amdb_simrec', status='old')
!>> Input Evts file
    OPEN( unit=LUNHIT,    file='default_muhits', status='old')
!>> General output
    OPEN( unit=6, file=FOUTPUT, status='unknown')
!>> Output file of track candidates
    IF( LUNOUT /= 6 ) OPEN( unit=LUNOUT, file=FRECONS, status='unknown')
!>> Output file of recons. timing
    OPEN( unit=LUNtiming, file=FTIMING, status='unknown')
!
!>> Initialise muon chambers geometry (and various parameters)
!>> for muon pattern recognition + reconstruction
!>>       ********************
    CALL InitialiseMuonboy
!>>       ********************
!
!>> Check tracking resolution
    IF( CheckTracking ) THEN
      SimBug = 8       ! remove scattering and eloss
      write(42,*) '============================================'
      write(42,*) ' test tracking '
      CALL ktime_mubo('test tra', 1)
      CALL checkTRABAZ
      CALL ktime_mubo('test tra', 2)
      write(42,*) '============================================'
      CALL ktime_mubo('fin     ', 3)
      STOP 
    ENDIF
!
    IF( checkBderiv ) THEN
      call testChamp
      STOP
    ENDIF
!
    Idum = 0
!
!>> Event Loop -->-->-->-->-->-->-->-->-->-->-->-->-->
    DO
!
     call ktime_mubo('getevt  ',1)
!>> Get one event from the "hits or digis" file
     CALL GETEVT(LUNHIT, LUNPILEUP, -1, 0, IRET, IDEVE, Idum)
     call ktime_mubo('getevt  ',2)
!
     IF(IRET.EQ.-1) CYCLE
     IF(IRET.EQ. 0) EXIT
!
!>> Reconstruct the event
        call ktime_mubo('Muonboy ',1)
!>>         *************
        CALL RunMuonboy
!>>         *************
        call ktime_mubo('Muonboy ',2)
!
!>> Save the candidate tracks
        CALL WRICAN(LUNOUT,LUNtiming)
!
    ENDDO
!>> <--<--<--<--<--<--<--<--<--<--<--<--<--<--<--<--<-
!
!
!>> End of job !!!
!
    CALL GETEVT(0, 0, -1, 0, IRET, IDEVE, Idum)
    CALL WRICAN(0,0)
    CALL Finish_MB_Digis(0)
    call ktime_mubo('the end ',3)
!
    STOP
!
 END PROGRAM ZYVA
!
