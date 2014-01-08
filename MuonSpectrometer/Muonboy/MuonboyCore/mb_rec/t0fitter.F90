!
!> performs the t0 fit by scanning the values, keeping the best value for each station,
!> and then interpolating between the best, the previous and next.
!> \author A.Ouraou
!
 SUBROUTINE t0fitter(nscan)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_Types
 USE M_MB_MuGeom
 USE M_MB_PACONS
 IMPLICIT NONE
!
 INTEGER, INTENT(IN) :: nscan  !< number of step for the t0 scan (from -nscan to nscan)
 INTEGER, ALLOCATABLE :: LTUBHO(:)
 INTEGER, PARAMETER :: nsegmax=20
 LOGICAL          :: ALOCEL,STRICT,SSfromROA,BOGcase
 INTEGER          :: IT, ISC
 TYPE(Segment)    :: segcado(nsegmax)
 TYPE SegmT0   !< simplified segment for T0 fit (to save space)
   INTEGER :: ml, nu, iu(16), iz(2)
   REAL(8) :: chi2, zmoy
 END TYPE
 TYPE(SegmT0) :: segm(nsegmax,-nscan:nscan,8)
 TYPE(SegmT0) :: segmin, segava, segapr
 CHARACTER(len=3) :: StationName
 INTEGER :: nseg(-nscan:nscan,8)
 INTEGER :: IC1,IC2, I, IC, IW, IMDT, IP, ITDEB,ITFIN, IW1, INOF1, INOZ1, IGEOM1
 INTEGER :: IW2, INOF2, INOZ2, IGEOM2, NtubMax, NtubHit, IMDT1, IMDT2
 INTEGER :: NBCADO, IK, J, ICI, IWI, INOFI, INOZI, IGEOMI, IMDTI, ICJ, nn, ifi, itmin, ncom
 INTEGER :: ICC(2),IWW(2), LLI(2), intpos
 INTEGER :: JTYP, JFF, JZZ
 REAL(8) :: T01, T02, T0I
 REAL(8) :: SSI(2), TTI(2)
 REAL(8) :: dT0tmp(NGEOMA)
 REAL(8) :: dt0, chimin, chavan, chapre, dt0add, zbord
 REAL(8), PARAMETER :: zbordcut=20.0d0, dtbord=10000.d0
!
#include "AmdcStand/amdcsim_param.inc"
!
    ALLOCATE( LTUBHO(NTUBHT) )
    LTUBHO(1:NTUBHT) = LTUBHI(1:NTUBHT)
!
!-------------------------------------------------------------------------------
!>> Search of LOOSE segments for T0 fitter in all the MDT stations
!
!>> Set the parameters that define straight segments to loose values
    ALOCEL    = .FALSE.
    STRICT    = .FALSE.
    SSfromROA = .FALSE.
    BOGcase   = .FALSE.
    CHIMA1 = 2.31D0 * SIGMOK**2
    CHIMA2 = 2.00D0 * SIGMOK**2
    CHINEF = 2.00D0 * SIGMOK**2
!
    dT0val(:) =   0.d0
    dT0tmp(:) = 999.d0
    SSI(1:2)  =   0.d0    ! put segments in the middle of station
    LLI(1:2)  =   0
!
    DO ISC=1,NMATTR
      IF( NCHAHI(2*ISC)+NCHAHI(2*ISC-1) <= 0 ) CYCLE
      IC1 = 2*ISC
      IC2 = 2*ISC - 1
      ICC(1) = IC1
      ICC(2) = IC2
!
      nseg(:,:) = 0
      DO it=-nscan,nscan
        dt0 = it * StepT0Fit  ! time in ns
        call ResetSetTShift( dt0 )
!
!>> All DT0s are examined with the same flags
        DO I=1,2
          IC = ICC(I)
          DO IW=1,NCHAHI(IC)
            IMDT = ISTMDTV( IGEOMA(IC,IFCHA(IC,IW),IZCHA(IC,IW)) )
            DO IP=1,NLTUB(IMDT)
              ITDEB = NTUBH0(IC,IW,IP) + 1
              ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
              RTUBHI(ITDEB:ITFIN) = -1.d0
              KTUBHI(ITDEB:ITFIN) =  0
              LTUBHI(ITDEB:ITFIN) = LTUBHO(ITDEB:ITFIN)
            ENDDO
          ENDDO
        ENDDO
!
!>>--------------------------------------------------------------------------
!>> One starts with straight track segments extending through two
!>> multi-layers
b100:   DO 100 IW1=1,NCHAHI(IC1)
          IWW(1) = IW1
          INOF1  = IFCHA(IC1,IW1)
          INOZ1  = IZCHA(IC1,IW1)
          IGEOM1 = IGEOMA(IC1,INOF1,INOZ1)
          T01    = TCECHAV(IGEOM1)
          IMDT1  = ISTMDTV(IGEOM1)
          NtubMax  = NINT( TubLoo2ML * ZLOCHAV(IGEOM1) * PASTUB_inv(IMDT1) * NLTUB(IMDT1) )
          NtubHit  = SUM( NTUBHI(IC1,IW1,1:NLTUB(IMDT1)) )
          IF( NtubHit > NtubMax ) CYCLE
          DO IW2=1,NCHAHI(IC2)
            IWW(2) = IW2
            INOF2  = IFCHA(IC2,IW2)
            INOZ2  = IZCHA(IC2,IW2)
            IGEOM2 = IGEOMA(IC2,INOF2,INOZ2)
            IMDT2  = ISTMDTV(IGEOM2)
            NtubMax  = NINT( TubLoo2ML * ZLOCHAV(IGEOM2) * PASTUB_inv(IMDT2) * NLTUB(IMDT2) )
            NtubHit  = SUM( NTUBHI(IC2,IW2,1:NLTUB(IMDT2)) )
            IF( NtubHit > NtubMax ) CYCLE
            T02    = TCECHAV(IGEOM2)
! deal with potentially very displaced stations
! do not try segments if multilayers too far
! or if the inner one is farther than the outer one
            IF( ABS(T02-T01) > 150.d0 )         CYCLE
            IF( ABS(T01) < ABS(T02) )           CYCLE
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!>> The two multi-layers must be in the same phi position
            IF( INOF1 /= INOF2 )                CYCLE
!>> If the two multi-layers are in the end caps, they must be in the same one
            IF( T01*T02 < 0.D0 )                CYCLE
!>> Allow for one adjacent chamber only along Z (or T)
            IF( IABS(INOZ1-INOZ2) > 1 )         CYCLE
!
            if( nseg(it,inof1) >= nsegmax ) CYCLE b100
            TTI(1)   = T01
            TTI(2)   = T02
            CALL DROITU(ALOCEL,STRICT,SSfromROA,BOGcase,ICC,IWW,LLI,SSI,TTI,nsegmax,NBCADO, segcado)
!
!>> Stores output of DROITU
            DO IK=1,NBCADO
              if( segcado(ik)%nu <= 2 ) CYCLE
              nn = nseg(it,inof1) + 1
              if( nn > nsegmax )        CYCLE b100
              nseg(it,inof1)          = nn
              segm(nn,it,inof1)%ml    = 2
              segm(nn,it,inof1)%nu    = segcado(ik)%nu
              segm(nn,it,inof1)%iu(:) = segcado(ik)%iu(:)
              segm(nn,it,inof1)%chi2  = segcado(ik)%chi2
              segm(nn,it,inof1)%zmoy  = (segcado(ik)%zpos(1)+segcado(ik)%zpos(2))*0.5d0
              segm(nn,it,inof1)%iz(1) = INOZ1
              segm(nn,it,inof1)%iz(2) = INOZ2
            ENDDO
!
          ENDDO
100     ENDDO b100
! now look for in single ML, if station with only 1 ML
        DO I=1,2
          IWW(:) = 0
          ICI = ICC(I)
b200:     DO 200 IWI=1,NCHAHI(ICI)
            IWW(I) = IWI
            INOFI  = IFCHA(ICI,IWI)
            INOZI  = IZCHA(ICI,IWI)
            IGEOMI = IGEOMA(ICI,INOFI,INOZI)
            ICJ    = ICC(3-I)
            IF( IGEOMA(ICJ,INOFI,INOZI) /= 0 ) CYCLE
            IMDTI  = ISTMDTV(IGEOMI)
            NtubMax  = NINT( TubLoo1ML * ZLOCHAV(IGEOMI) * PASTUB_inv(IMDTI) * NLTUB(IMDTI) )
            NtubHit  = SUM( NTUBHI(ICI,IWI,1:NLTUB(IMDTI)) )
            IF( NtubHit > NtubMax ) CYCLE
            IF( NtubHit <= 1      ) CYCLE
            T0I    = TCECHAV(IGEOMI)
!
            if( nseg(it,inofi) >= nsegmax )    CYCLE
            TTI(1:2) = T0I
            CALL DROITU(ALOCEL,STRICT,SSfromROA,BOGcase,ICC,IWW,LLI,SSI,TTI,nsegmax,NBCADO, segcado)
!
!>> Stores output of DROITU
            DO IK=1,NBCADO
              if( segcado(ik)%nu <= 2 ) CYCLE
              nn = nseg(it,inofi) + 1
              if( nn > nsegmax )        CYCLE b200
              nseg(it,inofi)            = nn
              segm(nn,it,inofi)%ml      = 1
              segm(nn,it,inofi)%nu      = segcado(ik)%nu
              segm(nn,it,inofi)%iu(:)   = segcado(ik)%iu(:)
              segm(nn,it,inofi)%chi2    = segcado(ik)%chi2
              segm(nn,it,inofi)%zmoy    = (segcado(ik)%zpos(1)+segcado(ik)%zpos(2))*0.5d0
              segm(nn,it,inofi)%iz(1:2) = INOZI
            ENDDO
!
200       ENDDO b200
        ENDDO
!>>--------------------------------------------------------------------------
!
      ENDDO
!
!>> Reset the flags of the hits to their nominal values
      DO I=1,2
        IC = ICC(I)
        DO IW=1,NCHAHI(IC)
          IMDT = ISTMDTV( IGEOMA(IC,IFCHA(IC,IW),IZCHA(IC,IW)) )
          DO IP=1,NLTUB(IMDT)
            ITDEB = NTUBH0(IC,IW,IP) + 1
            ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
            RTUBHI(ITDEB:ITFIN) = -1.d0
            KTUBHI(ITDEB:ITFIN) = IC*10000 + IW*10 + IP
            LTUBHI(ITDEB:ITFIN) = LTUBHO(ITDEB:ITFIN)
          ENDDO
        ENDDO
      ENDDO
!
! Now fit of the T0s : one for each values of INOF
!
      IF( DoT0Print ) write(*,*) '=============================================='
      DO ifi=1,8
300     chimin = 999.d0
        DO it=-nscan,nscan
          DO i=1,nseg(it,ifi)
            IF( segm(i,it,ifi)%chi2 < chimin ) THEN
              segmin = segm(i,it,ifi)
              chimin = segmin%chi2
              itmin  = it
            ENDIF
          ENDDO
        ENDDO
        IF( chimin < 900.d0 ) THEN
          nn = segmin%nu
          chavan = 999.d0
          chapre = 999.d0
          IF( DoT0Print ) THEN
            write(*,*) ' segment trouvé pour isc ifi it ',isc,ifi,itmin
            write(*,*) ' nbre tubes ',nn
            write(*,*) ' chi2       ',chimin
          ENDIF
          IF( itmin > -nscan ) THEN
            DO i=1,nseg(itmin-1,ifi)
              segava = segm(i,itmin-1,ifi)
              IF( segava%nu < nn - 1 ) CYCLE
              ncom = 0
              DO j=1,segava%nu
                IF( intpos( segava%iu(j), segmin%iu(1:nn), nn) > 0 ) ncom = ncom + 1
              ENDDO
              IF( ncom < nn - 1 )  CYCLE
              if( segava%chi2 < chavan ) chavan = segava%chi2
            ENDDO
          ENDIF
          IF( itmin < nscan ) THEN
            DO i=1,nseg(itmin+1,ifi)
              segapr = segm(i,itmin+1,ifi)
              IF( segapr%nu < nn - 1 ) CYCLE
              ncom = 0
              DO j=1,segapr%nu
                IF( intpos( segapr%iu(j), segmin%iu(1:nn), nn) > 0 ) ncom = ncom + 1
              ENDDO
              IF( ncom < nn - 1 )  CYCLE
              IF( segapr%chi2 < chapre ) chapre = segapr%chi2
            ENDDO
          ENDIF
          IF( DoT0Print ) write(*,*) ' chavan chimin chapre ',chavan,chimin,chapre
          if( chavan < 900.d0 .and. chapre < 900.d0 ) then
            if( chavan > chimin .or. chapre > chimin ) then
              dt0 = ( itmin - (chapre-chavan)/(chapre+chavan-2.d0*chimin)/2.d0 ) * StepT0Fit
              StationName = CARISC(isc)
              do i=1,4
                dt0add = 0.d0
                if( i <= 2 ) then
                  ic = 2*isc - 1
                else
                  ic = 2*isc
                endif
                if( i == 1 .or. i == 3 ) then
                  jzz = segmin%iz(1)
                else
                  jzz = segmin%iz(2)
                  if( jzz == segmin%iz(1) ) then  ! segment in one station, check if on edge
                    if( jzz > 1 ) then
                      igeomi = igeoma(ic,ifi,jzz-1)
                      if( igeomi > 0 ) then
                        zbord = ZCECHAV(igeomi) + ZLOCHAV(igeomi)/2.d0
                        if( abs(segmin%zmoy-zbord) < zbordcut ) jzz = jzz - 1
                      endif
                    endif
                    if( jzz < NZM ) then
                      igeomi = igeoma(ic,ifi,jzz+1)
                      if( igeomi > 0 ) then
                        zbord = ZCECHAV(igeomi) - ZLOCHAV(igeomi)/2.d0
                        if( abs(segmin%zmoy-zbord) < zbordcut ) jzz = jzz + 1
                      endif
                    endif
                    if( jzz == segmin%iz(1) ) CYCLE
                    dt0add = dtbord
                  endif
                endif
                igeomi = igeoma(ic,ifi,jzz)
                IF( igeomi <= 0 ) CYCLE
                IF( dT0tmp(igeomi) > 900.d0 )  dT0tmp( igeomi ) = dt0 + dt0add
              enddo
              DO it=-nscan,nscan
                DO i=1,nseg(it,ifi)
                  IF( segm(i,it,ifi)%iz(1) == segmin%iz(1) .or. segm(i,it,ifi)%iz(1) == segmin%iz(2) .or. &
                      segm(i,it,ifi)%iz(2) == segmin%iz(1) .or. segm(i,it,ifi)%iz(2) == segmin%iz(2) ) THEN
                    segm(i,it,ifi)%chi2 = 1000.d0
                  ENDIF
                ENDDO
              ENDDO
              GO TO 300              
            endif
          endif
        ENDIF
      ENDDO
!
    ENDDO
!
    DEALLOCATE( LTUBHO )
!
    dt0 = 0.d0
    call ResetSetTShift( dt0 )
    dT0val(:) = dT0tmp(:)
!
    DO JTYP = 1,MTYP
      ISC = ISCISC(JTYP)
      IF( ISC < 1      ) CYCLE
      IF( ISC > NMATTR ) CYCLE
      CALL FGetStationType( JTYP, StationName )
      DO JFF=1,8
        DO JZZ=-MZZ,MZZ
          dt0 = 999.d0
          DO IC=2*ISC-1,2*ISC
            DO INOZI=1,NZM
              IGEOMI = IGEOMA(IC,JFF,INOZI)
              IF( IGEOMI <= 0 )              CYCLE
              IF( dT0val(IGEOMI) > 9000.d0 ) dT0val(IGEOMI) = dT0val(IGEOMI) - dtbord
              IF( dT0val(IGEOMI) > 900.d0  ) CYCLE
              IF( IBITS(ISTTYPV(IGEOMI),24,8) /= JTYP    ) CYCLE
              IF( IBITS(ISTTYPV(IGEOMI), 8,8) /= JZZ+MZZ ) CYCLE
              IF( dt0 > 900.d0 ) THEN
                dt0 = dT0val(IGEOMI)
                CALL InfSetStatTShift( StationName, JZZ, JFF, dt0 )
                IF( DoT0Print ) write(*,*) ' T0 set for ',StationName,JZZ,JFF,' to ',dt0
              ENDIF
            ENDDO
          ENDDO
        ENDDO
      ENDDO
    ENDDO
!
 END SUBROUTINE t0fitter
!
