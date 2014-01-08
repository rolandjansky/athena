!
!> Performs track reconstruction
!> \author M.Virchaux
!
 SUBROUTINE RecoTra
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 USE M_MB_MuGeom
 USE M_MB_FAX
 USE M_MB_PACONS
 USE M_MB_COSURE
 USE M_MB_CODRAT
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: NORDER(NDRAMA)
 INTEGER :: L,ID,ISEARC,ISC0,ISC1,NBEXPE,IDP,NFOUND
 INTEGER :: IDunordered, iok, NBGOOD, currentSearch
 REAL(8) :: CHISEG(NDRAMA)
 INTEGER :: NSEGME
 INTEGER :: NCANGO
 LOGICAL :: isBadTrackSeed
 INTEGER :: NBDROT
!
    CALL RecoTraStart
!
!-------------------------------------------------------------------------------
!-- Loop on all the different track searches on different sets of stations
search: DO 850 ISEARC=1,NSEARC
      currentSearch = ISEARC
      ISENFI = 1
110   CONTINUE
      NFSCAN = 0
!-- Search starts in station ISC0 toward station ISC1
      IF(ISENFI == 1) THEN
        ISC0 = ISCEMI(1,ISEARC)
        ISC1 = ISCEMI(2,ISEARC)
      ELSE
        ISC0 = ISCEMI(2,ISEARC)
        ISC1 = ISCEMI(1,ISEARC)
      ENDIF
      IF(MBPri >= 3) THEN
        PRINT 915,ISEARC,ISENFI,CARISC(ISC0),CARISC(ISC1),IFLECH(ISEARC),CARISC(ISC0),NBDRAT(ISC0),NBDRIT(ISC0)
915     FORMAT(//' =====> Start Search No',I3,'  with ISENFI =',I3,'  (',A3,' -> ',A3,'  Iflech =',I3,')',   &
                /'  Number of track segments found in ',A3,' :  Loose =',I3,'  Strict =',I3)
      ENDIF
!
!-- If no strict track segment outside, start from middle station
      IF( ( ( .NOT.UseAllSeg .AND. NBDRIT(ISC0) <= 0 ) .OR. ( UseAllSeg .AND. NBDRAT(ISC0) <= 0 ) ) .AND. &
            ( ISEARC < NSCSC .OR. NBDRAT(ISC0) <= 0 ) ) THEN
        IF(NBDRAT(ISC1) <= 0) THEN
          CYCLE search
        ELSE
          IF(ISENFI <= 1) THEN
            ISENFI = 2
            GO TO 110
          ELSE
            CYCLE search
          ENDIF
        ENDIF
      ENDIF
!
!-- If no segments in other search stations, start from middle station
      IF( NBDRAT(ISC1) + SUM(NBDRAT(ISCEMI(3:LSEARC(ISEARC),ISEARC))) <= 0 ) THEN
        IF(ISENFI <= 1) THEN
          ISENFI = 2
          GO TO 110
        ELSE
          CYCLE search
        ENDIF
      ENDIF
!
!-- Compute expected number of tracks from station ISC0
      IF( ISEARC < NSCSC ) THEN
        NBEXPE = 0
b130:   DO 130 ID=1,NBDRAT(ISC0)
          IF( IWDRAT(ID ,1,ISC0)*IWDRAT(ID ,2,ISC0) == 0 ) CYCLE
          DO IDP=1,ID-1
            IF( IWDRAT(IDP,1,ISC0)*IWDRAT(IDP,2,ISC0) == 0 )   CYCLE
            IF( ABS(FFDRAT(IDP,  ISC0)-FFDRAT(ID,  ISC0)) < 0.01D0   .AND.   &
                ABS(GGDRAT(IDP,  ISC0)-GGDRAT(ID,  ISC0)) < 0.01D0   .AND.   &
                ABS(ZZDRAT(IDP,1,ISC0)-ZZDRAT(ID,1,ISC0)) < 0.2500D0 .AND.   &
                ABS(ZZDRAT(IDP,2,ISC0)-ZZDRAT(ID,2,ISC0)) < 0.2500D0 .AND.   &
                ABS(TTDRAT(IDP,1,ISC0)-TTDRAT(ID,1,ISC0)) < 0.2500D0 .AND.   &
                ABS(TTDRAT(IDP,2,ISC0)-TTDRAT(ID,2,ISC0)) < 0.2500D0 ) CYCLE b130
          ENDDO
          NBEXPE = NBEXPE + 1
130     ENDDO b130
        NBEXPE = MAX( NBEXPE , NBDRIT(ISC0) )
      ELSE
        NBEXPE = NBDRAT(ISC0)
      ENDIF
!-- Compute number of good candidate tracks already found in station ISC0
      NFOUND = NCANGO(ISC0)
#if !defined(ATLAS_NOPRINTMU)
      IF(MBPri >= 3) THEN
        PRINT 916,CARISC(ISC0),NBEXPE,NFOUND
916     FORMAT('  Nb. of expected tracks from station ',A3,'     =',I3   &
              /'  Nb. of tracks already found in this station =',I3)
      ENDIF
#endif
!
!-- If expected nb. of tracks = nb. of already found tracks, cancel the search
      IF( NFOUND >= NBEXPE ) CYCLE search
!
!-- Replaces, for MDT station ISC0, loose straight track segments
!-- by strict ones (and store the loose ones in ..DROT)
!-- Except for special searches (>= NSCSC) to recover CSC part not covered by TGC
!-- and for cosmics
!-- warning : GGDRAT not necessary as not used for CSC
      NBDROT = NBDRAT(ISC0)
      IF( .NOT.UseAllSeg .AND. ISC0 <= NMATTR .AND. ISEARC < NSCSC ) NBDRAT(ISC0) = NBDRIT(ISC0)
!
!-- Start real track search from all the straight track segments
!-- found in the station ISC0
!
      IF(MBPri >= 3) PRINT 918,( CARISC(ISCEMI(L,ISEARC)),NBDRAT(ISCEMI(L,ISEARC)), L=1,LSEARC(ISEARC) )
918   FORMAT('  NBDRAT(',A3,') =',I3)
      IF( NBDRAT(ISC1) <= 0 .AND. IFLECH(ISEARC) >=  1 ) GO TO 800
!
!
! add a penalty for segments without strips (usefull when both coexists, i.e. in cosmics or in csc search)
      NSEGME = NBDRAT(ISC0)
      CHISEG(1:NSEGME) = CCDRAT(1:NSEGME,ISC0)
      WHERE( IWDRAT(1:NSEGME,1,ISC0) <= 0 .AND. IWDRAT(1:NSEGME,2,ISC0) <= 0 ) CHISEG(1:NSEGME) = CHISEG(1:NSEGME) + 3.d0
!-- Order the straight track segments in increasing Chi2 values
      CALL CROISS( CHISEG, NSEGME, NORDER )
!
!-- Loop on all the straight track segments found in station ISC0
      DO IDunordered=1,NSEGME
        ID = NORDER(IDunordered)
! Reset the number of m.s. centers and NBFAX and NZSure to 0,
! ItypFit to its original value and tracking limits to their std. values
        CALL Reset_MB_COMUSC
        CALL Reset_MB_FAX
        NZSure  = 0
        IFSure(:) = 999
        ItypFit = ItypFit_STD
        RCHAMI  = RCYLIN(3)
        ZCHAMI  = ZCYLIN(3)
        RCHAMA  = RCYLIN(4)
        ZCHAMA  = ZCYLIN(4)
        IWcross(:,:) = 0
! Check if this segment is OK to seed a track
        IF( isBadTrackSeed(ID,ISC0,currentSearch) ) CYCLE
!
        CALL trackFit(ID,ISC0,ISC1,currentSearch, iok)
        SELECT CASE( iok )
          CASE( 9 ) ; STOP
          CASE( 3 ) ; NBDRAT(ISC0) = NBDROT ; GO TO 900
          CASE( 2 ) ; GO TO 800
          CASE( 1 ) ; CYCLE
          CASE DEFAULT  ! iok=0 for good candidates
        END SELECT
!
! Eliminate possible dupplicate candidate tracks
        CALL CHECAN
!
      ENDDO
!-- Reset the number of m.s. centers and NBFAX and NZSure to 0,
!-- ItypFit to its original value and tracking limits to their std. values
800   CALL Reset_MB_COMUSC
      CALL Reset_MB_FAX
      NZSure  = 0
      IFSure(:) = 999
      ItypFit = ItypFit_STD
      RCHAMI  = RCYLIN(3)
      ZCHAMI  = ZCYLIN(3)
      RCHAMA  = RCYLIN(4)
      ZCHAMA  = ZCYLIN(4)
!
!-- Restores, for station ISC0, loose straight track segments
!-- (which were, up to now, replaced by strict ones)
      NBDRAT(ISC0) = NBDROT
!
!
!-- Possible rescue !
      IF( ISENFI <= 1 ) THEN
        NBGOOD = NCANGO(ISC0)
        IF( NBGOOD < NBEXPE .AND. IFLECH(ISEARC) >= 0 ) THEN
          ISENFI = 2
          IF(MBPri >= 3) PRINT 968,NBGOOD,NBEXPE,ISENFI
968       FORMAT(/'  **** Nb. of good candidates found (',I2,') < Expected (',I2,') **'     &
                 ,'** Try Rescue with : ISENFI =',I2,' ****')
          GO TO 110
        ENDIF
      ENDIF
!
!
850 ENDDO search
!-------------------------------------------------------------------------------
!
900 CONTINUE
!
  CALL RecoTraEnd  
!
 END SUBROUTINE RecoTra
!
!--------------------------------------------------------------------------------
!> Performs track reconstruction Start
!> \author M.Virchaux
!
 SUBROUTINE RecoTraStart
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 USE M_MB_General_CONSTANTS
 USE M_MB_CODRAT
 USE M_MB_Fit
 IMPLICIT NONE
!
    CALL Reset_MB_Reco_Tracks
!
    PAFITP = 0.02d0
    IF( ABS(FixMoCur) > 0.1D0 ) PAFITP = 0.00d0
!
    NCFIDR  = 0
    NFICAN  = 0
!
!-- set parameters used by GEOKLI
    IF( AtCosm == 0 ) THEN
      AFFMAXbarrel = 0.99D0*PiSect
      AEEMAXbarrel = Pi/12.D0
      AFFMAXendcap = 0.99D0*PiSect
      AEEMAXendcap = Pi/12.D0
    ELSE
      AFFMAXbarrel = Pi/2.d0
      AEEMAXbarrel = Pi/2.d0
      AFFMAXendcap = Pi/2.d0
      AEEMAXendcap = Pi/2.d0
    ENDIF
!
 END SUBROUTINE RecoTraStart
!
!--------------------------------------------------------------------------------
!> Performs track reconstruction: End
!> \author M.Virchaux
!
 SUBROUTINE RecoTraEnd
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
!
    CALL CLEANCAN(NBCAN)
!
!-------------------------------------------------------------------------------
!-- If in real experiment i.e. not in beam test :
!-- Back-track the muon candidates to the entrance of the calos and to perigee
    CALL ktime_mubo('baktrak ',1)
    CALL BAKTRAK
    CALL ktime_mubo('baktrak ',2)
!-- And compute final covariance matrice
    CALL ktime_mubo('matcov  ',1)
    CALL MATCOV
    CALL AddMsIduncertainty
    IF( AtCosm == 0 )  CALL IPcheck  ! loose track rejection based on IP compatibility
    CALL ktime_mubo('matcov  ',2)
!-------------------------------------------------------------------------------
!
!-- If needed, change the FLGCAN to take into account :
!      track reversal FLGCAN -> FLGCAN + 10
!      straightLine   FLGCAN -> FLGCAN + 100
    IF( AtCosm /= 0 ) CALL ReverseTracks
    IF( NoField )     FLGCAN(1:NBCAN) = FLGCAN(1:NBCAN) + 100
!
 END SUBROUTINE RecoTraEnd
!
!--------------------------------------------------------------------------------
!> Check if the segment ID from station ISC0 can seed a track from search currentSearch.
!> Return = true if ok, false otherwise
!
 LOGICAL FUNCTION isBadTrackSeed(ID, ISC0, currentSearch)
 USE M_MB_Control
 USE M_MB_CODRAT
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ID, ISC0, currentSearch
 INTEGER :: L, ic, iw, inof, inoz, IGEOM, Ncom, NGoodCom
 INTEGER :: NTrCommonHits, NGoodTrCommonHits
 REAL(8) :: TTT, ZZZ, SSS, CAA, SAA, deltaS, Smini, Smaxi, CHIKCA, CHIKSA
!
    isBadTrackSeed = .true.
!
! if special search with CSC, ask to be in the CSC region
    IF( currentSearch >= NSCSC ) THEN
      TTT = (TTDRAT(ID,1,ISC0)+TTDRAT(ID,2,ISC0)) * 0.5d0
      ZZZ = (ZZDRAT(ID,1,ISC0)+ZZDRAT(ID,2,ISC0)) * 0.5d0
      SSS = (SSDRAT(ID,1,ISC0)+SSDRAT(ID,2,ISC0)) * 0.5d0
      IF( (TTT**2+SSS**2)/ZZZ**2 > 0.12d0 ) RETURN  ! approximately eta=1.8
    ENDIF
! if special search for tracks going from L to S sector, cut on S
    IF( IBORDU(currentSearch) == 1 ) THEN  ! special search for tracks going from L to S sector
      L = IUDRAT(1,ID,ISC0)
      ic  = KTUBHI(L)/10000
      IF( isc0 /= (1+ic)/2 ) then
        IF( MBPri >= 1 ) THEN
          WRITE(*,'(/" Problem in search L<->S ",I3," ISC0 IC ",2I3," => Stop !")') currentSearch, ISC0, IC
          STOP
        ENDIF
        RETURN
      ENDIF
      iw  = (KTUBHI(L)-ic*10000)/10
      inof = ifcha(ic,iw)
      inoz = izcha(ic,iw)
      IGEOM = igeoma(ic,inof,inoz)
      IF( ISC0 <= NCOUBAS2 ) THEN
        ZZZ = (ZZDRAT(ID,1,ISC0)+ZZDRAT(ID,2,ISC0)) * 0.5d0
      ELSE IF( ISC0 <= NMATTR ) THEN
        ZZZ = (TTDRAT(ID,1,ISC0)+TTDRAT(ID,2,ISC0)) * 0.5d0
      ELSE
        CAA = COS(GGDRAT(ID,ISC0))
        SAA = SIN(GGDRAT(ID,ISC0))
        ZZZ = ( -SAA*(ZZDRAT(ID,1,ISC0)+ZZDRAT(ID,2,ISC0)) + CAA*(TTDRAT(ID,1,ISC0)+TTDRAT(ID,2,ISC0)) ) * 0.5d0
      ENDIF
      SSS = (SSDRAT(ID,1,ISC0)+SSDRAT(ID,2,ISC0)) * 0.5d0
      call Get_Tube_Lateral_Dim(IGEOM,ZZZ, Smini,Smaxi)
      deltaS = MIN( abs(SSS-Smini), abs(SSS-Smaxi) )
      IF( deltaS > 150.d0 ) RETURN
    ENDIF
! When using segment without strips, require two multi-layers 
    IF( IWDRAT(ID,1,ISC0)*IWDRAT(ID,2,ISC0) == 0 ) THEN                ! single layer
      IF( IWDRAT(ID,1,ISC0) <= 0 .AND. IWDRAT(ID,2,ISC0) <= 0 ) RETURN ! no strips
    ENDIF
!
! If more than 2 found "good" candidate share some hits from this segment, skip the search !
! (for cases when a lot of identical segments with different S positions)
    NCom = NTrCommonHits(NUDRAT(ID,ISC0), IUDRAT(1:NUDRAT(ID,ISC0),ID,ISC0))
! Or if a very good candidate already sharing one of the hits from this segment, skip the search !
    NGoodCom = NGoodTrCommonHits(NUDRAT(ID,ISC0), IUDRAT(1:NUDRAT(ID,ISC0),ID,ISC0))
    IF( MBPri >= 3 ) WRITE(*,'(/2X,"ISC0,ID =",2I3,12X,"NCom =",I3,I5)') ISC0,ID,NCom,NGoodCom
    IF( NCom >= 3     ) RETURN
    IF( NGoodCom >= 1 ) RETURN
! If an already found "good" candidate track passes very close to the
! straight track segment, skip the search !
    CALL TRAKCA(ID,ISC0, CHIKCA,CHIKSA)
    IF( MBPri >= 3 ) WRITE(*,'(2X,"ISC0,ID =",2I3,12X,"CHIKCA =",2F15.2)') ISC0,ID,CHIKCA,CHIKSA
    IF( CHIKCA < DCLOOK/6.D0 .AND. CHIKSA < DCLOOK/30.D0 ) RETURN
!
! Finally, check if compatible with at least one other segment
    NBFIXE = 1
    ISCFIXE(1) = ISC0
    IDFIXE(1)  = ID
    NBLIBR = 0
    DO L=1,LSEARC(currentSearch)
      IF( ISCEMI(L,currentSearch) == ISC0 ) CYCLE
      NBLIBR = NBLIBR + 1
      ISCLIBR(NBLIBR) = ISCEMI(L,currentSearch)      
    ENDDO
    CALL GEOKLI
    IF( MBPri >= 3 ) WRITE(*,'(2X,"     NBOKLI =",I4)') NBOKLI
    IF( NBOKLI <= 0 ) RETURN
!
    isBadTrackSeed = .false.
!
 END FUNCTION isBadTrackSeed
!
