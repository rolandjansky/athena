!
!> Performs segment reconstruction
!> \author M.Virchaux
!
 SUBROUTINE NewRecoSeg(NROA,TEROA,FIROA,DTEROA,DFIROA)
#if !defined(ATLAS_STAND)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_Types
 USE M_MB_Reco_Segments
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_CODRAT
 USE M_MB_AlowCell
 USE M_MB_SSBAN
 USE M_MB_External_ROA
 IMPLICIT NONE
!
 INTEGER, INTENT(IN) ::   NROA     !< number of ROA in which the reconstruction will be done, 0=everywhere
 REAL(8), INTENT(IN) ::  TEROA(*)  !< theta of the ROAs
 REAL(8), INTENT(IN) ::  FIROA(*)  !< phi   of the ROAs
 REAL(8), INTENT(IN) :: DTEROA(*)  !< theta size of the ROAs
 REAL(8), INTENT(IN) :: DFIROA(*)  !< phi   size of the ROAs
 INTEGER, ALLOCATABLE :: JFLAGI(:),JFLAGO(:)
 LOGICAL       :: ALOCEL,STRICT,SINGML,L_ISC_SEED
 REAL(8)       :: RRR,FFF,THEcur,PHIcur,R3Dcur
 REAL(8)       :: TTT,ZZZ,SSS, SINA,COSA, AZT,AST
 INTEGER       :: L, MACADO, ISC, ID, IDU,IDS, IT, NBSeg, iExtROA
 TYPE(Segment) :: segm(NDRAMA)
 REAL(8)       :: PIMIPI
 INTEGER, PARAMETER :: LTUSHIFT=1000000
 INTEGER I

!!!!    MBPri = 3
!
!-- Reset number of (all kind of) track segments
    CALL Reset_MB_Reco_Segments
    NBDRAT(1:NMACSC) = 0
    NBDRIT(1:NMACSC) = 0
!
    CALL ktime_mubo('s values',1)
!-------------------------------------------------------------------------------
!-- Search of all the "allowed" S values from the strip hits in each
!-- MDT multilayer IC,IW
!--  and   of all the "allowed" (Theta,Phi) cells from the strip hits
!
    CALL Reset_MB_AlowCell
    if (NberExtROA.eq.0) then
        CALL cdoallowcell(NROA,TEROA,FIROA,DTEROA,DFIROA)
!!!        CALL doAllowCell(NROA, TEROA,FIROA,DTEROA,DFIROA)
        IF(MBPri >= 3) THEN
            print 1000, NAlow
1000        FORMAT(' NAlow= ', I3)
            if ( NAlow .gt. 0 ) then
                do i = 1, NAlow
                    print 1001, i, AlowThe(i), AlowPhi(i), AlowR3d(i), AlowWei(i)
1001                FORMAT(' I, The, Phi, R3d, Wei ', I4, 4F12.4)
                enddo
            endif
            CALL PrintAlowCell();
        ENDIF
        CALL cdoRetrieveROA(NROA,TEROA,FIROA,DTEROA,DFIROA)
    else
      do iExtROA=1,NberExtROA
        CALL CreateAlow(ExtROAR3d(iExtROA),&
                        ExtROAThe(iExtROA),&
                        ExtROAPhi(iExtROA))
      enddo
    endif
    IF(MBPri >= 3) THEN
      CALL PrintAlowCell
      WRITE(*,'(/," SSBAN of (IC,IW) :")')
    ENDIF
    CALL Reset_MB_SSBAN
    IF(MBPri >= 3) WRITE(*, '(/" **********About to call cdoclusters()")')
    CALL cdoclusters()
    IF(MBPri >= 3) WRITE(*, '(/" **********Returned from cdoclusters()")')
!!!!    CALL doClusters
    CALL cdoRetrieveClusters()
!-------------------------------------------------------------------------------
    CALL ktime_mubo('s values',2)
!
    ALLOCATE( JFLAGI(NTUBHT) )
    ALLOCATE( JFLAGO(NTUBHT) )
!
    CALL ktime_mubo('csc segm',1)
    IF(MBPri >= 3) WRITE(*, '(/" **********About to call cdocscsegments()")')
    CALL cdocscsegments()
    IF(MBPri >= 3) WRITE(*, '(/" **********Returned from cdocscsegments()")')
!-------------------------------------------------------------------------------
!-- Search for all straight track segments in CSC stations
!-- and store them in Module  M_MB_Reco_Segments
    IF(MBPri >= 3) PRINT 906
906 FORMAT(/' ------------------------------------------------------------------------------------------------------')
    MACADO = MAX( 10 , 60-40*MBAccRec )
    STRICT = .FALSE.
    SINGML = .TRUE.
    CHIMA1    = 1.91D0 * SIGMOK**2
    CHIMA2    = 1.20D0 * SIGMOK**2
    CHINEF    = 2.00D0 * SIGMOK**2
!
    DO 30 ISC=NMATTR+1,NMACSC
      IF( NCHAHI(2*ISC)+NCHAHI(2*ISC-1) >= 1 ) THEN
        IF(MBPri >= 3) PRINT 908,CARISC(ISC),SIGMOK,CHIMA1,CHIMA2,CHINEF
908     FORMAT(/' Loose straight segment search in INNER station ',A3    &
               ,'  with : SigmOk,ChiMa1,ChiMa2,ChInef =',4F6.2)
        CALL FILICSC(ISC,MACADO,NDRAMA, JFLAGI,JFLAGO,NBSeg, segm )
!-- For CSCs, there is only one type of straight track segments
!-- the "loose" (..DRAT) and "strict" (..DRIT) ones
        NBDRIT(ISC) = NBSeg
        NBDRAT(ISC) = NBSeg
!-- Store straight track segments in Module M_MB_Reco_Segments
        DO IDU=1,NBSeg
          CALL StoreSegment(ISC, segm(IDU), 1)
          CCDRAT(IDU,    ISC) = segm(IDU)%chi2
          FFDRAT(IDU,    ISC) = segm(IDU)%phi0
          GGDRAT(IDU,    ISC) = segm(IDU)%alfa
          EZDRAT(IDU,    ISC) = segm(IDU)%errz
          SSDRAT(IDU,1:2,ISC) = segm(IDU)%spos(1:2)
          ZZDRAT(IDU,1:2,ISC) = segm(IDU)%zpos(1:2)
          TTDRAT(IDU,1:2,ISC) = segm(IDU)%tpos(1:2)
          IWDRAT(IDU,1:2,ISC) = segm(IDU)%iw(1:2)
          JFDRAT(IDU,    ISC) = segm(IDU)%jff
          NUDRAT(IDU,    ISC) = segm(IDU)%nu
          DO L=1,NUDRAT(IDU,ISC)
            IUDRAT(L,IDU,ISC) = segm(IDU)%iu(L)
          ENDDO
        ENDDO
!
      ENDIF
30  ENDDO
!-------------------------------------------------------------------------------
    CALL ktime_mubo('csc segm',2)
!
    DO ISC=NMATTR+1,NMACSC
      DO ID=1,NBDRAT(ISC)
        SINA = SIN( GGDRAT(ID,ISC) )
        COSA = COS( GGDRAT(ID,ISC) )
        AZT  = ( COSA*(TTDRAT(ID,2,ISC)-TTDRAT(ID,1,ISC)) - SINA*(ZZDRAT(ID,2,ISC)-ZZDRAT(ID,1,ISC)) )  &
             / ( SINA*(TTDRAT(ID,2,ISC)-TTDRAT(ID,1,ISC)) + COSA*(ZZDRAT(ID,2,ISC)-ZZDRAT(ID,1,ISC)) ) 
        AST  = ( SSDRAT(ID,2,ISC) - SSDRAT(ID,1,ISC) )                                                  &
             / ( SINA*(TTDRAT(ID,2,ISC)-TTDRAT(ID,1,ISC)) + COSA*(ZZDRAT(ID,2,ISC)-ZZDRAT(ID,1,ISC)) )
        IF( ABS(AZT) > 0.5d0 .OR. ABS(AST) > 0.5d0 ) CYCLE
        TTT = (TTDRAT(ID,1,ISC)+TTDRAT(ID,2,ISC)) / 2.D0
        ZZZ = (ZZDRAT(ID,1,ISC)+ZZDRAT(ID,2,ISC)) / 2.D0
        SSS = (SSDRAT(ID,1,ISC)+SSDRAT(ID,2,ISC)) / 2.D0
        RRR = SQRT( TTT**2 + SSS**2 )
        FFF = FFDRAT(ID,ISC) + ATAN2( SSS , TTT )
        R3Dcur = SQRT( TTT**2 + SSS**2 + ZZZ**2 )
        THEcur = ATAN2( RRR , ZZZ )
        PHIcur = PIMIPI( FFF )
        CALL CreateAlow(R3Dcur,THEcur,PHIcur)
      ENDDO
    ENDDO
!
    CALL ktime_mubo('stri seg',1)
    IF(MBPri >= 3) THEN
        WRITE(*, '(/" **********About to call cdostrictegments()")')
    ENDIF
    CALL cdostrictsegments()
    IF(MBPri >= 3) THEN
        WRITE(*, '(/" **********Returned from cdostrictsegments()")')
    ENDIF
!-------------------------------------------------------------------------------
!-- Search of STRICT straight track segments extending through two (or one)
!-- MDT multi-layers, among which there is at least one S strip hit
!-- and that can serve as "seed" for one search
!
    IF(MBPri >= 3) PRINT 906
    MACADO = MAX( 6 , 40-30*MBAccRec )
!
!-- Request search to be in a valid Eta_Phi cell (only for the proper Atlas set-up)
    ALOCEL = UseRoA >= 2 .AND. AtCosm == 0
!
    CHIMA1 = 2.01D0 * SIGMOK**2
    CHIMA2 = 1.25D0 * SIGMOK**2
    CHINEF = 1.60D0 * SIGMOK**2
    DO ISC=1,NMATTR
      IF( ( L_ISC_SEED(ISC) .OR. MBSto <= 1 ) .AND. NCHAHI(2*ISC)+NCHAHI(2*ISC-1) >= 1 ) THEN
!-- Set the parameters that define straight segments to strict values
        STRICT = MBSto >= 0
        SINGML = .TRUE.
        IF( CARISC(ISC)(1:2) == 'EO' ) THEN  ! Strict segments in EO stations without TGC
          STRICT = .FALSE.
          SINGML = .FALSE.
        ENDIF
        IF(MBPri >= 3) PRINT 909,CARISC(ISC),SIGMOK,CHIMA1,CHIMA2,CHINEF
909     FORMAT(/' STRICT straight segment search in station ',A3    &
               ,'  with : SigmOk,ChiMa1,ChiMa2,ChInef =',4F6.2)
        CALL FINDRA(ALOCEL,STRICT,SINGML,ISC,MACADO,NDRIMA, JFLAGI,JFLAGO, NBSeg, segm)
        NBDRIT(ISC) = NBSeg
        NBDRAT(ISC) = NBSeg
!--  Store straight track segments in Module M_MB_Reco_Segments
        DO IDU=1,NBSeg
          CALL StoreSegment(ISC, segm(IDU), 1)
          CCDRAT(IDU,    ISC) = segm(IDU)%chi2
          FFDRAT(IDU,    ISC) = segm(IDU)%phi0
          GGDRAT(IDU,    ISC) = segm(IDU)%alfa
          EZDRAT(IDU,    ISC) = segm(IDU)%errz
          SSDRAT(IDU,1:2,ISC) = segm(IDU)%spos(1:2)
          ZZDRAT(IDU,1:2,ISC) = segm(IDU)%zpos(1:2)
          TTDRAT(IDU,1:2,ISC) = segm(IDU)%tpos(1:2)
          IWDRAT(IDU,1:2,ISC) = segm(IDU)%iw(1:2)
          JFDRAT(IDU,    ISC) = segm(IDU)%jff
          NUDRAT(IDU,    ISC) = segm(IDU)%nu
          DO L=1,NUDRAT(IDU,ISC)
            IUDRAT(L,IDU,ISC) = segm(IDU)%iu(L)
          ENDDO
        ENDDO
!-- Second separate loop because T0 fit during storeSegment uses LTUBHI
        DO IDU=1,NBSeg
          DO L=1,NUDRAT(IDU,ISC)
            IT = IUDRAT(L,IDU,ISC)
            IF( LTUBHI(IT) >= LTUSHIFT ) CYCLE
            LTUBHI(IT) = LTUBHI(IT) + LTUSHIFT  ! do not use it for LOOSE segments
          ENDDO
        ENDDO
!
      ENDIF
    ENDDO
!-------------------------------------------------------------------------------
    CALL ktime_mubo('stri seg',2)
!
    IF( MBSto <= 0 ) THEN
      IF(MBPri >= 1) WRITE(*,'(/" STOP reconstruction after STRICT straight track segment search !")')
      RETURN
    ENDIF
!
!
    CALL ktime_mubo('Rec. T,P',1)
!-- Recompute the "allowed" (Theta,Phi) cells from the strict segments
    CALL Reset_MB_AlowCell
    DO ISC=1,NMATTR
      DO ID=1,NBDRIT(ISC)
        TTT = (TTDRAT(ID,1,ISC)+TTDRAT(ID,2,ISC)) / 2.D0
        ZZZ = (ZZDRAT(ID,1,ISC)+ZZDRAT(ID,2,ISC)) / 2.D0
        SSS = (SSDRAT(ID,1,ISC)+SSDRAT(ID,2,ISC)) / 2.D0
        RRR = SQRT( TTT**2 + SSS**2 )
        FFF = FFDRAT(ID,ISC) + ATAN2( SSS , TTT )
        R3Dcur = SQRT( TTT**2 + SSS**2 + ZZZ**2 )
        THEcur = ATAN2( RRR , ZZZ )
        PHIcur = PIMIPI( FFF )
        CALL CreateAlow(R3Dcur,THEcur,PHIcur)
      ENDDO
    ENDDO
    DO ISC=NMATTR+1,NMACSC
      DO ID=1,NBDRAT(ISC)
        SINA = SIN( GGDRAT(ID,ISC) )
        COSA = COS( GGDRAT(ID,ISC) )
        AZT  = ( COSA*(TTDRAT(ID,2,ISC)-TTDRAT(ID,1,ISC)) - SINA*(ZZDRAT(ID,2,ISC)-ZZDRAT(ID,1,ISC)) )  &
             / ( SINA*(TTDRAT(ID,2,ISC)-TTDRAT(ID,1,ISC)) + COSA*(ZZDRAT(ID,2,ISC)-ZZDRAT(ID,1,ISC)) ) 
        AST  = ( SSDRAT(ID,2,ISC) - SSDRAT(ID,1,ISC) )                                                  &
             / ( SINA*(TTDRAT(ID,2,ISC)-TTDRAT(ID,1,ISC)) + COSA*(ZZDRAT(ID,2,ISC)-ZZDRAT(ID,1,ISC)) )
        IF( ABS(AZT) > 0.5d0 .OR. ABS(AST) > 0.5d0 ) CYCLE
        TTT = (TTDRAT(ID,1,ISC)+TTDRAT(ID,2,ISC)) / 2.D0
        ZZZ = (ZZDRAT(ID,1,ISC)+ZZDRAT(ID,2,ISC)) / 2.D0
        SSS = (SSDRAT(ID,1,ISC)+SSDRAT(ID,2,ISC)) / 2.D0
        RRR = SQRT( TTT**2 + SSS**2 )
        FFF = FFDRAT(ID,ISC) + ATAN2( SSS , TTT )
        R3Dcur = SQRT( TTT**2 + SSS**2 + ZZZ**2 )
        THEcur = ATAN2( RRR , ZZZ )
        PHIcur = PIMIPI( FFF )
        CALL CreateAlow(R3Dcur,THEcur,PHIcur)
      ENDDO
    ENDDO    
    IF(MBPri >= 3) CALL PrintAlowCell
    CALL ktime_mubo('Rec. T,P',2)
!
    CALL ktime_mubo('loos seg',1)
!-------------------------------------------------------------------------------
!-- Search of LOOSE straight track segments in all the MDT stations
!
    IF(MBPri >= 3) WRITE(*, '(/" **********About to call cdomdtroa()")')
    CALL cdomdtroa()
    IF(MBPri >= 3) WRITE(*, '(/" **********Returned from cdomdtroa()")')
    CALL cdoRetrieveROA(NROA,TEROA,FIROA,DTEROA,DFIROA)
    IF(MBPri >= 3) CALL PrintAlowCell
    IF(MBPri >= 3) PRINT 906
!
!!!!    CALL doMakeROAFromMDT()
!!!!    IF( UseRoaMdt ) THEN
!!!!      DO ISC=1,NMATTR
!!!!        IF( NCHAHI(2*ISC)+NCHAHI(2*ISC-1) < 1 ) CYCLE
!!!!        CALL makeRoaFromMdt(ISC)
!!!!      ENDDO
!!!!    ENDIF
!
    IF(MBPri >= 3) WRITE(*, '(/" **********About to call cdoloosesegments()")')
    CALL cdoloosesegments()
    IF(MBPri >= 3) WRITE(*, '(/" **********Returned from cdoloosesegments()")')
    IF(MBPri >= 3 .AND. NAlow /= NAlowGood ) THEN
      WRITE(*,'(5X,"New allowed cells added using MDT")')
      CALL PrintAlowCell
    ENDIF
!
!-- Set the parameters that define straight segments to loose values
    MACADO = MAX( 6 , 60-40*MBAccRec )
    STRICT = .FALSE.
    SINGML = .TRUE.
    CHIMA1 = 2.31D0 * SIGMOK**2
    CHIMA2 = 2.00D0 * SIGMOK**2
    CHINEF = 2.00D0 * SIGMOK**2
!
!-- Request search to be in a valid Eta_Phi cell (only for the proper Atlas set-up)
    ALOCEL = UseRoA >= 1 .AND. AtCosm == 0
!
    DO ISC=1,NMATTR
      IF( NCHAHI(2*ISC)+NCHAHI(2*ISC-1) >= 1 ) THEN
        IF(MBPri >= 3) PRINT 912,CARISC(ISC),SIGMOK,CHIMA1,CHIMA2,CHINEF
912     FORMAT(/' LOOSE straight segment search in station ',A3   &
               ,'  with : SigmOk,ChiMa1,ChiMa2,ChInef =',4F6.2)
        CALL FINDRA(ALOCEL,STRICT,SINGML,ISC,MACADO,NDRAMA-NBDRIT(ISC), JFLAGI,JFLAGO, NBSeg, segm)
        NBDRAT(ISC) = NBDRIT(ISC) + NBSeg
!--  Store straight track segments in Module M_MB_Reco_Segments
        DO IDS=1,NBSeg
          CALL StoreSegment(ISC, segm(IDS), 0)
          IDU = NBDRIT(ISC) + IDS
          CCDRAT(IDU,    ISC) = segm(IDS)%chi2
          FFDRAT(IDU,    ISC) = segm(IDS)%phi0
          GGDRAT(IDU,    ISC) = segm(IDS)%alfa
          EZDRAT(IDU,    ISC) = segm(IDS)%errz
          SSDRAT(IDU,1:2,ISC) = segm(IDS)%spos(1:2)
          ZZDRAT(IDU,1:2,ISC) = segm(IDS)%zpos(1:2)
          TTDRAT(IDU,1:2,ISC) = segm(IDS)%tpos(1:2)
          IWDRAT(IDU,1:2,ISC) = segm(IDS)%iw(1:2)
          JFDRAT(IDU,    ISC) = segm(IDS)%jff
          NUDRAT(IDU,    ISC) = segm(IDS)%nu
          DO L=1,NUDRAT(IDU,ISC)
            IUDRAT(L,IDU,ISC) = segm(IDS)%iu(L)
          ENDDO
        ENDDO
!
      ENDIF
    ENDDO
    WHERE( LTUBHI(1:NTUBHM) >= LTUSHIFT )  LTUBHI(1:NTUBHM) = LTUBHI(1:NTUBHM) - LTUSHIFT
    IF(MBPri >= 3) PRINT 906
!-------------------------------------------------------------------------------
    CALL ktime_mubo('loos seg',2)
!
    DEALLOCATE( JFLAGI )
    DEALLOCATE( JFLAGO )
!
!   Compute errors of segments
    IF(MBPri >= 3) WRITE(*, '(/" **********About to call cdoerrorsegment()")')
    CALL cdoerrorsegment()
    IF(MBPri >= 3) WRITE(*, '(/" **********Returned from cdoerrorsegment()")')
    CALL ErrorSegment
!
#endif
 END SUBROUTINE NewRecoSeg
!
