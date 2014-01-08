!
!> Final cleaning of candidate muons
!
 SUBROUTINE CLEANCAN(NBMAX)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 USE M_MB_PACONS, ONLY : FLOOK2
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NBMAX
 INTEGER :: Ican, K, ISC, Ihit, It, IC, IML,JML, NStafound, NSt1found, NMLfound, NStamiss, NMLmiss
 INTEGER :: Jcan, Ncomm, Icomm, Is, Icode, ICT, IP, IRPC, ICSCbased
 INTEGER :: NnoCS, KI,KJ
 INTEGER :: Icross(1:2) ! list of stations crossed
 INTEGER :: Ifound(1:2) ! list of stations found
 LOGICAL :: CSCTRK, Reject
 LOGICAL, SAVE :: VERIF = .FALSE.
 INTEGER :: ItI,ItJ, NMLI(NCOCSC),NMLJ(NCOCSC), NMLI2,NMLJ1,NMLJ2, NIJ
 INTEGER :: NstaI,IstaI(9),NmdtI,NcscI,NstaJ,IstaJ(9),NmdtJ,NcscJ, nstadif,istadif
 INTEGER :: NCSCZI, NCSCZJ, NCSCSC
 INTEGER :: nbsta(NBMAX),nb1ml(NBMAX),nbmis(NBMAX)
 REAL(8) :: fac
 INTEGER :: intpos
!
    IF(VERIF) THEN
      VERIF = .FALSE.
      IF(NMATMA > 31)  STOP ' CLEANCAN >> fatal error, NMATMA > 31'
    ENDIF
    nbsta(:) = 0
    nb1ml(:) = 0
    nbmis(:) = 0
!========================================================================================
! kill candidate tracks with no ML with at least 2 hits, or with only 2 stations in which one has only 1 hit
    DO Ican=1,NBCAN
      IF(CHICAN(Ican) > 1000.D0)         CYCLE
      NMLI(:) = 0
      NMLJ(:) = 0
      DO KI=1,NZXCAN(Ican)
        ItI = IZXCAN(KI,Ican)
        IC  = KTUBHI(ItI)/10000
        ISC = (IC+1)/2
        IF(IC <= NCOUCH .OR. RTUBHI(ItI) > -10.D0 ) THEN
          NMLI(IC)  = NMLI(IC)  + 1          ! number of hits by ML
          NMLJ(ISC) = NMLJ(ISC) + 1          ! number of hits by station
        ENDIF
      ENDDO
      NMLI2 = 0
      DO K=1,NCOCSC
        IF( NMLI(K) >= 2 )  NMLI2 = NMLI2 + 1
      ENDDO
      NMLJ1 = 0
      NMLJ2 = 0
      DO K=1,NMACSC
        IF( NMLJ(K) >= 1 )  NMLJ1 = NMLJ1 + 1
        IF( NMLJ(K) >= 2 )  NMLJ2 = NMLJ2 + 1
      ENDDO
      IF( NMLI2 == 0 .OR. ( NMLJ1 <= 2 .AND. NMLJ2 <= 1 ) ) THEN
        CHICAN(Ican) = CHICAN(Ican) + 1000.D0
        IF( MBPri >= 3 ) PRINT 1000,Ican
1000    FORMAT(2X,'Eliminate candidate nb.',I3,' because it has not enough ML with at least 2 digits '   &
                 ,'(not counting CSC S strips)')
        CYCLE
      ENDIF
      IF( NMLJ2 <= 1 .AND. NBXCAN(Ican) >= 2*NZXCAN(Ican) ) THEN
        CHICAN(Ican) = CHICAN(Ican) + 1000.D0
        IF( MBPri >= 3 ) PRINT 1010,Ican
1010    FORMAT(2X,'Eliminate candidate nb.',I3,' because it has only 1 station with at least 2 digits '   &
                 ,'and too much missing tubes')
      ENDIF
      CYCLE 
    ENDDO
!========================================================================================
! kill candidate tracks if all digits not in CSC are common
    DO Ican=1,NBCAN
      IF(CHICAN(Ican) > 1000.D0)         CYCLE
      DO Jcan=1,NBCAN
        IF(Jcan == Ican)                 CYCLE
        IF(CHICAN(Jcan) > 1000.D0)       CYCLE
        Ncomm = 0
        NnoCS = 0
        NMLI(:) = 0
        NMLJ(:) = 0
        DO KI=1,NZXCAN(Ican)
          ItI = IZXCAN(KI,Ican)
          IC  = KTUBHI(ItI)/10000
          NMLI(IC) = NMLI(IC) + 1
          IF(IC <= NCOUCH)  NnoCS = NnoCS + 1
          DO KJ=1,NZXCAN(Jcan)
            ITJ = IZXCAN(KJ,Jcan)
            IC = KTUBHI(ItJ)/10000
            IF(KI == 1)  NMLJ(IC) = NMLJ(IC) + 1
            IF(ItJ /= ItI)        CYCLE
            IF(IC <= NCOUCH)  Ncomm = Ncomm + 1
          ENDDO
        ENDDO
        IF(Ncomm == NnoCS) THEN
!  We keep the best candidate according to the number of ML crossed with more than 2 hits
          NMLI2 = 0
          NMLJ2 = 0
          DO K=1,NCOUCH
            IF(NMLI(K) >= 2)  NMLI2 = NMLI2 + 1
            IF(NMLJ(K) >= 2)  NMLJ2 = NMLJ2 + 1
          ENDDO
          IF(2*NMLI2 <= NMLJ2) THEN
            CHICAN(Ican) = CHICAN(Ican) + 1000.D0
            IF( MBPri >= 3 ) PRINT 1020,Ican,Jcan,NnoCS,NZXCAN(Ican)-NnoCS
1020        FORMAT(2X,'Eliminate candidate nb.',I3,' because it has all non-CSC digits common ',   &
                      'with candidate track ',I3,' (',I2,' non-CSC digits and ',I2,' CSC digits)')
            EXIT
          ENDIF
        ENDIF
      ENDDO
    ENDDO
!========================================================================================
! kill candidate tracks following pattern of crossed and missed stations
    DO Ican=1,NBCAN
      IF(CHICAN(Ican) > 1000.D0)      CYCLE
! first : list of stations crossed = ICROSS
      Icross(:) = 0
      DO K=1,12
        IF(ITXCAN(K,Ican) > 0) THEN
          ISC = MOD(ITXCAN(K,Ican), 100)
          IF(ITXCAN(K,Ican) < 200)                            Icross(1) = IBSET(Icross(1), ISC)
          IF(ITXCAN(K,Ican) < 100 .OR. ITXCAN(K,Ican) > 200)  Icross(2) = IBSET(Icross(2), ISC)
        ENDIF
      ENDDO
! then list of stations found = IFOUND
      CSCTRK = .FALSE.
      Ifound(:) = 0
      Ncomm = 0
b100: DO 100 Ihit=1,NZXCAN(Ican)
        It = IZXCAN(Ihit,Ican)
        IC  = KTUBHI(It)/10000
        ISC = (IC+1)/2
        IF( ISC >  NMATTR ) CSCTRK = .TRUE.
        IF( ISC <= NMATTR .OR. RTUBHI(It) > -10.d0 ) THEN  ! do not count S strips as found
          IF(IC /= 2*ISC) THEN
            Ifound(1) = IBSET(Ifound(1), ISC)
          ELSE
            Ifound(2) = IBSET(Ifound(2), ISC)
          ENDIF
        ENDIF
        DO Jcan=1,NBCAN
          IF(Jcan == Ican)            CYCLE
          IF(CHICAN(Jcan) > 1000.D0)  CYCLE
          DO K=1,NZXCAN(Jcan)
            IF(IZXCAN(K,Jcan)  /= It) CYCLE
            Ncomm = Ncomm + 1
            CYCLE b100
          ENDDO
        ENDDO
100   ENDDO b100
      IF(MBPri >= 3 ) THEN
        PRINT 9000,' cross 1',(btest(Icross(1),K),k=1,NMATMA)
        PRINT 9000,' cross 2',(btest(Icross(2),K),k=1,NMATMA)
        PRINT 9000,' found 1',(btest(Ifound(1),K),k=1,NMATMA)
        PRINT 9000,' found 2',(btest(Ifound(2),K),k=1,NMATMA)
        WRITE(*,*) ' CLEANCAN ',Ican,' : Ntot Ncom ',NZXCAN(Ican),Ncomm,CHICAN(Ican),CXXCAN(2,Ican)
9000    FORMAT(5X,A8,5X,32L2)
      ENDIF
! Now kill tracks if too many missing stations
      NStafound = 0
      NSt1found = 0
      NMLfound  = 0
      NStamiss  = 0
      NMLmiss   = 0
      NcscI     = 0
      DO K=1,NMATMA
        IML = 0
        IF(BTEST(Ifound(1),K))                                               IML = IML + 1
        IF(BTEST(Ifound(2),K))                                               IML = IML + 1
        IF(IML == 1 .AND. .NOT.(BTEST(Icross(1),K).AND.BTEST(Icross(2),K)))  IML = -1
        JML = 0
        IF(BTEST(Icross(1),K) .AND. .NOT.BTEST(Ifound(1),K))                 JML = JML + 1
        IF(BTEST(Icross(2),K) .AND. .NOT.BTEST(Ifound(2),K))                 JML = JML + 1
        IF(IML == 2) THEN            ! full station found
          NStafound = NStafound + 1
        ELSE IF(IML == 1) THEN       ! only one ML found
          NMLfound = NMLfound + 1
        ELSE IF(IML == -1) THEN
          NSt1found = NSt1found + 1  ! only one ML crossed and found
          IF( K > NMATTR ) NcscI = 1
        ENDIF
        IF(JML == 2) THEN            ! full station missing
          NStamiss = NStamiss + 1
        ELSE IF(JML == 1) THEN       ! only one ML missing
          NMLmiss = NMLmiss + 1
        ENDIF
      ENDDO
      nbsta(Ican) = NStafound + NSt1found
      nb1ml(Ican) = NMLfound
      nbmis(Ican) = NStamiss
      IF(3*NStamiss+NMLmiss >= 3*NStafound+2*NSt1found+NMLfound) THEN
        CHICAN(Ican) = CHICAN(Ican) + 1000.D0
        IF( MBPri >= 3 ) PRINT 1200,Ican,NStamiss,NMLmiss
1200    FORMAT(2X,'Eliminate candidate nb.',I3,' because it misses too many stations (',   &
               I2,' full station and',I2,' single multi-layer)')
        CYCLE
      ELSE IF( 3*NStamiss+NMLmiss >= 3*NStafound+NMLfound .AND. Nst1found == 1 .AND. NcscI == 1 ) THEN
! special case for CSC dubious track : we ask to have at least one well measured point
        DO KI=1,NZXCAN(Ican)
          It  = IZXCAN(KI,Ican)
          IC  = KTUBHI(It) / 10000
          IF( IC > NCOUCH .AND. ( LTUBHI(It) == 0 .OR. LTUBHI(It) == 10 )  ) GO TO 200
        ENDDO
        CHICAN(Ican) = CHICAN(Ican) + 1000.D0
        IF( MBPri >= 3 ) PRINT 1200,Ican,NStamiss,NMLmiss
        CYCLE
200     CONTINUE   
      ENDIF
!----------------------------------------------------------------------------------------
! Then kill CSC based tracks (i.e. special tracks without TGC requirement) when not enough multi-layer
      IF(CSCTRK) THEN
        ICSCbased = 0
        DO Ihit=1,NSXCAN(Ican)
          Is    = ISXCAN(Ihit,Ican)
          Icode = KBANHI(IS) / 10
          ICT   = Icode / 100
          IP    = Icode - ICT*100
          IRPC  = IRPTGC(ICT)
          IF(N2BAND(IRPC) <= 0)   CYCLE
          IF(IP <= N1BAND(IRPC)) THEN  ! S strip hit
            ICSCbased = IBSET(ICSCbased, 0)
          ELSE                         ! Z/T strip hit
            ICSCbased = IBSET(ICSCbased, 1)
          ENDIF
        ENDDO
        NMLfound = 0
        DO K=1,NMATTR
          IF(BTEST(Ifound(1),K))  NMLfound = NMLfound + 1
          IF(BTEST(Ifound(2),K))  NMLfound = NMLfound + 1
        ENDDO
        IF(ICSCbased < 3 .AND. NMLfound < 2) THEN
          CHICAN(Ican) = CHICAN(Ican) + 1000.D0
          IF( MBPri >= 3 ) PRINT 1220,Ican,NMLfound
1220      FORMAT(2X,'Eliminate candidate nb.',I3,' because it has only ',I2,' multi-layer on top of CSC')
          CYCLE
        ENDIF
      ENDIF
    ENDDO
!========================================================================================
!
! Then, look at tracks with all hits but those from only one station, shared with a better track
!
b300: DO 300 Ican=1,NBCAN
        IF( CHICAN(Ican) > 1000.d0 )   CYCLE
        NstaI = 0
        NmdtI = 0
        NcscI = 0
        DO KI=1,NZXCAN(Ican)
          It  = IZXCAN(KI,Ican)
          IC  = KTUBHI(It) / 10000
          ISC = (IC+1) / 2
          IF( ISC > NMATTR .AND. RTUBHI(It) < -10.d0 ) CYCLE  ! do not use CSC S strips
          IF( ISC > NMATTR ) THEN
            NcscI = NcscI + 1
          ELSE
            NmdtI = NmdtI + 1
          ENDIF
          IF( intpos( ISC, IstaI, NstaI) == 0 ) THEN
            IF( NstaI < 9 ) THEN
              NstaI = NstaI + 1
              IstaI(NstaI) = ISC
            ENDIF
          ENDIF
        ENDDO
        IF( NstaI < 2 ) THEN  ! should never happen, except (?) CSC without any Z strips
          CHICAN(Ican) = CHICAN(Ican) + 1000.d0
          CYCLE b300
        ENDIF
b250:   DO 250 Jcan=1,NBCAN
          IF( Jcan == Ican )           CYCLE
          IF( CHICAN(Jcan) > 1000.d0 ) CYCLE
          NstaJ = 0
          NmdtJ = 0
          NcscJ = 0
          DO KI=1,NZXCAN(Jcan)
            It  = IZXCAN(KI,Jcan)
            IC  = KTUBHI(It) / 10000
            ISC = (IC+1) / 2
            IF( ISC > NMATTR .AND. RTUBHI(It) < -10.d0 ) CYCLE
            IF( ISC > NMATTR ) THEN
              NcscJ = NcscJ + 1
            ELSE
              NmdtJ = NmdtJ + 1
            ENDIF
            IF( intpos( ISC, IstaJ, NstaJ) == 0 ) THEN
              IF( NstaJ < 9 ) THEN
                NstaJ = NstaJ + 1
                IstaJ(NstaJ) = ISC
              ENDIF
            ENDIF
          ENDDO
          IF( NstaJ < 2 ) THEN
            CHICAN(Jcan) = CHICAN(Jcan) + 1000.d0
            CYCLE
          ENDIF
          IF( NstaJ < NstaI ) CYCLE
          nstadif = 0   ! now look for # of stations with at least one hit not in common
          DO IS=1,NstaI
            DO KI=1,NZXCAN(Ican)
              It  = IZXCAN(KI,Ican)
              IC  = KTUBHI(It) / 10000
              ISC = (IC+1) / 2
              IF( ISC /= IstaI(IS) )                       CYCLE
              IF( ISC > NMATTR .AND. RTUBHI(It) < -10.d0 ) CYCLE
              IF( intpos( It, IZXCAN(:,Jcan), NZXCAN(Jcan) ) == 0 ) THEN
                istadif = isc
                nstadif = nstadif + 1
                EXIT
              ENDIF
            ENDDO
            IF( nstadif >= 2 )  CYCLE b250
          ENDDO
          Reject = ( nstadif == 0 )
          IF( .NOT.Reject ) THEN 
            IF( istadif > NMATTR ) THEN
              IF( NcscI <= MIN( 0, NcscJ-2) )                Reject = .true.
            ELSE IF( CHICAN(Jcan) < CHICAN(Ican) ) THEN
              IF( NmdtI+NcscI <  NmdtJ+NcscJ ) THEN
                                                             Reject = .true.
              ELSE
                NIJ = 1 + NmdtI + NcscI - NmdtJ - NcscJ
                IF( NSSCAN(Ican) < NSSCAN(JCAN)-NIJ ) THEN
                  IF( CHICAN(Jcan) < CHICAN(Ican)-NIJ*9.d0 ) Reject = .true.
                ENDIF
              ENDIF
            ENDIF
          ENDIF
          IF( Reject ) THEN
            CHICAN(Ican) = CHICAN(Ican) + 1000.d0
            IF( MBPri >= 3 ) PRINT 1300,Ican,Jcan
1300        FORMAT(2X,'Eliminate candidate nb.',I3,' because it shares too many digits with candidate',I3)
            CYCLE b300          
          ENDIF
250     ENDDO b250
300   ENDDO b300
!
!========================================================================================
!
! Then, if inside CSC, with common CSC S strips, kill the one without CSC-Z strips 
b400: DO 400 Ican=1,NBCAN
        IF( CHICAN(Ican) > 1000.d0 )    CYCLE
b350:   DO 350 Jcan=Ican+1,NBCAN
          IF( CHICAN(Jcan) >= 1000.d0 ) CYCLE
          NCSCZI = 0
          NCSCZJ = 0
          NCSCSC = 0   ! number of common CSC-S strips
          DO KI=1,NZXCAN(Ican)
            It  = IZXCAN(KI,Ican)
            IC  = KTUBHI(It) / 10000
            ISC = (IC+1) / 2
            IF( ISC > NMATTR ) THEN
              IF( RTUBHI(It) < -10.d0 ) THEN
                IF( intpos( It, IZXCAN(1:NZXCAN(Jcan),Jcan), NZXCAN(Jcan) ) > 0 ) NCSCSC = NCSCSC + 1
              ELSE
                NCSCZI = NCSCZI + 1
              ENDIF
            ENDIF
          ENDDO
          DO KJ=1,NZXCAN(Jcan)
            It  = IZXCAN(KJ,Jcan)
            IC  = KTUBHI(It) / 10000
            ISC = (IC+1) / 2
            IF( ISC > NMATTR ) THEN
              IF( RTUBHI(It) > -10.d0 ) NCSCZJ = NCSCZJ + 1
            ENDIF
          ENDDO
          IF( NCSCSC >= 2 ) THEN
            IF( NCSCZI - NCSCZJ >= 2 .AND. NCSCZJ <= 2 .AND. CXXCAN(2,Jcan)-CXXCAN(2,Ican) > FLOOK2/15.d0 ) THEN
              CHICAN(Jcan) = CHICAN(Jcan) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1400,Jcan,Ican,NCSCZI,NCSCZJ,NCSCSC
              CYCLE b350
            ELSE IF( NCSCZJ - NCSCZI >= 2 .AND. NCSCZI <= 2 .AND. CXXCAN(2,Ican)-CXXCAN(2,Jcan) > FLOOK2/15.d0 ) THEN
              CHICAN(Ican) = CHICAN(Ican) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1400,Ican,Jcan,NCSCZJ,NCSCZI,NCSCSC
              CYCLE b400
            ENDIF
          ENDIF
1400      FORMAT(2X,'Eliminate candidate nb.',I3,' because of CSC common with track',I3,' (NCSCZI-NCSCZJ-NCSCSC',3I2,')')
350     ENDDO b350
400   ENDDO b400
!
!========================================================================================
!
! Finally, remove tracks sharing (precision) hits  
!
b500: DO 500 Ican=1,NBCAN-1
        IF( CHICAN(Ican) > 1000.d0 )   CYCLE
b450:   DO 450 Jcan=Ican+1,NBCAN
          IF( CHICAN(Jcan) > 1000.d0 ) CYCLE
          Reject = .false.
          DO KI=1,NZXCAN(Ican)
            It  = IZXCAN(KI,Ican)
            IF( intpos( It, IZXCAN(1:NZXCAN(Jcan),Jcan), NZXCAN(Jcan) ) > 0 ) THEN ! shared hit
              IC  = KTUBHI(It) / 10000
              ISC = (IC+1) / 2
              IF( ISC > NMATTR ) THEN   ! do not use S ou badly measured Z CSC
                IF( RTUBHI(It) < -10.d0 .OR. ( LTUBHI(It) /= 0 .AND. LTUBHI(It) /= 10 ) ) CYCLE
              ENDIF
              Reject = .true.
              EXIT
            ENDIF
          ENDDO
          IF( .NOT.Reject )            CYCLE
! here we have a well measured shared hit : try to kill one track
          Icomm = 0
          Ncomm = 0  ! will count the number of stations with shared hits
          NmdtI = 0
          DO KI=1,NZXCAN(Ican)
            It  = IZXCAN(KI,Ican)
            IC  = KTUBHI(It) / 10000
            ISC = (IC+1) / 2
            IF( ISC > NMATTR ) THEN   ! do not use S ou badly measured Z CSC
              IF( RTUBHI(It) < -10.d0 .OR. ( LTUBHI(It) /= 0 .AND. LTUBHI(It) /= 10 ) ) CYCLE
            ENDIF
            NmdtI = NmdtI + 1
            IF( .NOT.btest(Icomm,ISC) ) THEN
              IF( intpos( It, IZXCAN(1:NZXCAN(Jcan),Jcan), NZXCAN(Jcan) ) > 0 ) THEN
                Ncomm = Ncomm + 1
                Icomm = IBSET( Icomm, ISC)
              ENDIF
            ENDIF
          ENDDO
          NmdtJ = 0
          DO KJ=1,NZXCAN(Jcan)
            It  = IZXCAN(KJ,Jcan)
            IC  = KTUBHI(It) / 10000
            ISC = (IC+1) / 2
            IF( ISC > NMATTR ) THEN   ! do not use S ou badly measured Z CSC
              IF( RTUBHI(It) < -10.d0 .OR. ( LTUBHI(It) /= 0 .AND. LTUBHI(It) /= 10 ) ) CYCLE
            ENDIF
            NmdtJ = NmdtJ + 1
          ENDDO
          if( mbpri >= 3 ) then
            write(*,*) ' track 1 ',Ican,nmdti,CXXCAN(2,Ican)
            write(*,*) '         ',nbsta(Ican),nb1ml(Ican),nbmis(Ican)
            write(*,*) ' track 2 ',Jcan,nmdtj,CXXCAN(2,Jcan)
            write(*,*) '         ',nbsta(Jcan),nb1ml(Jcan),nbmis(Jcan)
            write(*,*) ' stations shared ',Ncomm
          endif
          fac = 15.d0 + 2.d0*abs(NmdtI-NmdtJ)
          IF( NmdtI >= NmdtJ .AND. CXXCAN(2,Jcan)-CXXCAN(2,Ican) > FLOOK2/fac ) THEN
            CHICAN(Jcan) = CHICAN(Jcan) + 1000.d0
            IF( MBPri >= 3 ) PRINT 1500,Jcan,Ican
            CYCLE b450
          ELSE IF( NmdtJ >= NmdtI .AND. CXXCAN(2,Ican)-CXXCAN(2,Jcan) > FLOOK2/fac ) THEN
            CHICAN(Ican) = CHICAN(Ican) + 1000.d0
            IF( MBPri >= 3 ) PRINT 1500,Ican,Jcan
            CYCLE b500
          ENDIF
! no clear choice, try nonetheless to remove one if tracks with only two stations or shared hits in mote than 2 stations
          IF( nbsta(Ican)+nb1ml(Ican) <= 2 .OR. nbsta(Jcan)+nb1ml(Jcan) <= 2 .OR. Ncomm >= 2 ) THEN
            IF( nbsta(Ican)+nb1ml(Ican) > nbsta(Jcan)+nb1ml(Jcan) ) THEN
              CHICAN(Jcan) = CHICAN(Jcan) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1500,Jcan,Ican
              CYCLE b450
            ELSE IF( nbsta(Jcan)+nb1ml(Jcan) > nbsta(Ican)+nb1ml(Ican) ) THEN
              CHICAN(Ican) = CHICAN(Ican) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1500,Ican,Jcan
              CYCLE b450
            ELSE IF( nbmis(Ican) < nbmis(Jcan) ) THEN
              CHICAN(Jcan) = CHICAN(Jcan) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1500,Jcan,Ican
              CYCLE b450
            ELSE IF( nbmis(Jcan) < nbmis(Ican) ) THEN
              CHICAN(Ican) = CHICAN(Ican) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1500,Ican,Jcan
              CYCLE b450
            ELSE IF( CXXCAN(2,Ican) < CXXCAN(2,Jcan) ) THEN
              CHICAN(Jcan) = CHICAN(Jcan) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1500,Jcan,Ican
              CYCLE b450
            ELSE
              CHICAN(Ican) = CHICAN(Ican) + 1000.d0
              IF( MBPri >= 3 ) PRINT 1500,Ican,Jcan
              CYCLE b450
            ENDIF
          ENDIF
1500      FORMAT(2X,'Eliminate candidate nb.',I3,' because of shared (precise) hit with track',I3)
450     ENDDO b450
500   ENDDO b500
!
 END SUBROUTINE CLEANCAN
!
