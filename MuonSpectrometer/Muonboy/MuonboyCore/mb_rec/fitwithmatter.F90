!
!> Final fit taking scattering and Eloss into account
!> \author A.Ouraou
!
 SUBROUTINE FitWithMatter
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_Reco_Tracks
 USE M_MB_COMUSC
 USE M_MB_COSURE
 USE M_MB_Fit
 USE M_MB_FAX
 USE M_MB_Digis
 USE M_MB_CODRAT
 USE M_MB_PACONS
 IMPLICIT NONE
 REAL(8) :: RRFMIN, RRFMAX
 REAL(8) :: S1BACK(KMASC), PARBACK(0:5), err(5+KMASC)
 REAL(8) :: RR1, RDIF, FNEW, PAFIPP
 INTEGER :: L, NN, IA, I4P, I4, ITS, IC, NHITOK
 INTEGER :: ID, ISC, J, N2FIXE, maxcall, nfixbarrel, nfixendcap
 INTEGER :: ISCUSE(NMATMA)
 LOGICAL :: FitOK
 REAL(8), PARAMETER :: errsca=0.10d0
!
    RRFMIN = 999999.d0
    RRFMAX = 0.d0
    DO L=1,NBFIXE
      DO J=1,2
        DO I4P=1,N4M
          DO I4=1,4
            IF( IZFIXE(I4P,I4,L,J) <= 0 ) CYCLE
            RR1 = getRMUSC( XDFIXE(I4P,I4,L,J), YDFIXE(I4P,I4,L,J), ZDFIXE(I4P,I4,L,J)) 
            IF( RR1 < RRFMIN ) RRFMIN = RR1
            IF( RR1 > RRFMAX ) RRFMAX = RR1
          ENDDO
        ENDDO
      ENDDO
    ENDDO
!
    CALL TRAKMS(RRFMIN,RRFMAX) ! Initial value of Mult.Scat. angles
    IF(MBPri >= 3) THEN
      WRITE(*,'(/," Initial values of S1MASC after TRAKMS : ",10F8.4)') S1MASC(1:NBMASC)
      DO L=1,Navant
        IF( ABS(S1MASC(L)) > 0.000000001d0 ) WRITE(*,'(" Probleme TRAKMS pour IMASC=",I2)') L 
      ENDDO
    ENDIF
!
!-- Do a first fit with ItypFit = 4
    ItypFit = 4
    CALL GEOKLI
    maxcall = 400
    IF(MBPri >= 3) THEN
      WRITE(*,'(" Before fit 4 with matter : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
      WRITE(*,'("                                      S1MASC =",8F12.7)') S1MASC(1:NBMASC)
      CALL TRAKMU(0)
      CALL PRFILI(3)
      WRITE(*,944) ItypFit,NBFIXE,NBLIBR,SIGMAK,maxcall
    ENDIF
944 FORMAT('>>>>  Perform FIT with  ItypFit=',I2,',  Nb-Fix-Libr=',2I2,',  SigmAK=',F5.1,'  and  MaxCall=',I6,' !!!!')
    err(1:4) = 0.d0
    err( 5 ) = PAFITP
    DO L=1,NBMASC
      IF( L > Navant .AND. RRMASC(L) > RRFMIN .AND. RRMASC(L) < RRFMAX ) THEN
        err(5+L) = errsca
      ELSE
        err(5+L) = 0.d0
      ENDIF
    ENDDO
    CALL ktime_mubo('fit m 4 ',1)
    CALL minimizeFcn(5+NBMASC,maxcall,err)
    CALL ktime_mubo('fit m 4 ',2)
    IF( FMIN > DCLOOK ) RETURN
    CALL TRAKMU(0)
    IF(MBPri >= 3) THEN
      WRITE(*,'("  After fit 4 with matter : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
      WRITE(*,'("                                      S1MASC =",8F12.7)') S1MASC(1:NBMASC)
      CALL PRFILI(3)
    ENDIF
! ask at least one acceptable hit
    NHITOK = 0
    DO L=1,NBFIXE
      DO J=1,2
        DO I4P=1,N4M
          DO I4=1,4
            IF( IZFIXE(I4P,I4,L,J) <= 0 ) CYCLE
            NHITOK = NHITOK + 1
          ENDDO
        ENDDO
      ENDDO
    ENDDO    
    IF( NHITOK <= 0 ) RETURN
!
!-- Perform fit with ItypFit = 5 if necessary
    NN = 0
    DO L=Navant+1,NBMASC
      IF( RRMASC(L) <= RRFMIN .OR. RRMASC(L) >= RRFMAX ) NN = NN + 1
    ENDDO
    IF( NN > 0 ) THEN
      S1BACK(1:NBMASC) = S1MASC(1:NBMASC)
      PARBACK(0:5) = (/ FMIN, DZT1, DS1, DZT2, DS2, SMU /)
      ItypFit = 5
      CALL GIVDEB1(1)
      CALL DEFFAX( -0.1D0 , -0.1D0 )
      NBLIBR = 0
! recompute NBFIXE to have only fixed stations from ItypFit=2 (because IDFIXE is needed)
      N2FIXE = NBFIXE
      NBFIXE = 0
      DO L=1,N2FIXE
        IF( IWcross(ISCFIXE(L),1)+IWcross(ISCFIXE(L),2) >= 1 ) EXIT
        NBFIXE = L
      ENDDO
      IF( NBFIXE <= 0 )  GO TO 200 ! should never happen
      ISCUSE(:) = 0
      ISCUSE( ISCFIXE(1:NBFIXE) ) = 1
!-- Determine all the multilayers to take into account in the fit of type 5
      IF(MBPri >= 3) WRITE(*,'("  Crossed M.L. (Fax) :",I3,2X,A3,I6,I3,I6,I3)')   &
          (ISCFOX(L),CARISC(ISCFAX(L)),IOFFAX(L,1),IOZFAX(L,1),IOFFAX(L,2),IOZFAX(L,2),L=1,NBFAX)
      DO IA=1,NBFAX
        ISC = ISCFAX(IA)
        IF( ISCUSE(ISC) > 0 ) CYCLE
        DO ID=1,NBDRAT(ISC)
          RR1 = getRMUSC( 0.5d0*(TTDRAT(ID,1,ISC)+TTDRAT(ID,2,ISC)) ,   &
                          0.5d0*(SSDRAT(ID,1,ISC)+SSDRAT(ID,2,ISC)) ,   &
                          0.5d0*(ZZDRAT(ID,1,ISC)+ZZDRAT(ID,2,ISC)) )
          DO L=1,NBMASC
            IF( RR1 < RRMASC(L) .AND. RRMASC(L) < RRFMIN ) GO TO 100
            IF( RR1 > RRMASC(L) .AND. RRMASC(L) > RRFMAX ) GO TO 100
          ENDDO
        ENDDO
        CYCLE
100     NBLIBR = NBLIBR + 1
        ISCLIBR(NBLIBR) = ISC
        IWcross(ISC,1:2) = 0
      ENDDO
      IF( NBLIBR <= 0 )  GO TO 200
      CALL Reset_MB_FAX
      CALL GEOKLI
      IF( NBOKLI <= 0 )  GO TO 200
      maxcall  = 300
      err(1:5) = 0.d0
      DO L=1,NBMASC
        IF( L <= Navant .OR. ( RRMASC(L) > RRFMIN .AND. RRMASC(L) < RRFMAX ) ) THEN
          err(5+L) = 0.d0
        ELSE
          err(5+L) = errsca
        ENDIF
      ENDDO
      IF(MBPri >= 3) THEN
        WRITE(*,'(" Before fit 5 with matter : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
        WRITE(*,'("                                      S1MASC =",8F12.7)') S1MASC(1:NBMASC)
        CALL TRAKMU(0)
        CALL PRFILI(3)
      ENDIF
      CALL ktime_mubo('fit m 5 ',1)
      CALL minimizeFcn(5+NBMASC,maxcall,err)
      CALL ktime_mubo('fit m 5 ',2)
!-- And check that all new scattering angles are reasonnable
      FitOK = .true.
      DO L=1,NBMASC
        IF( RRMASC(L) <= RRFMIN .OR. RRMASC(L) >= RRFMAX ) THEN
          IF( ABS(S1MASC(L)) > SigMaxFit5 ) THEN
            FitOK = .false.
            EXIT
          ENDIF
        ENDIF
      ENDDO
      IF(MBPri >= 3) THEN
        WRITE(*,'("  After fit 5 with matter : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
        WRITE(*,'("                                      S1MASC =",8F12.7)') S1MASC(1:NBMASC)
        CALL TRAKMU(0)
        CALL PRFILI(3)
      ENDIF
      IF( FMIN >= FLOOK2 .OR. .NOT.FitOK ) THEN
!-- Go back to old parameters
        FMIN = PARBACK(0)
        DZT1 = PARBACK(1)
        DS1  = PARBACK(2)
        DZT2 = PARBACK(3)
        DS2  = PARBACK(4)
        SMU  = PARBACK(5)
        S1MASC(1:NBMASC) = S1BACK(1:NBMASC)
      ENDIF
200   NBFIXE = N2FIXE
    ENDIF
!
! On recalcule la trajectoire pour trouver les SURE hits
    ItypFit = 4
    CALL TRAKMU(0)
!XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
!-- Grand final fit with matter for m.s. with ItypFit = 6 (purely statistical)
!-- Before all, one sets the parameters and "fixes" the "sure" hits
!-- and decide if we should increase the endcap or barrel hits if in overlap
    NBLIBR = 0
    NZSure = 0
    IFSure(:) = 999
    nfixbarrel = 0
    nfixendcap = 0
    increaseErrorBarrel = .false.
    increaseErrorEndcap = .false.
!......................................
    DO L=NBFIXE,1,-1
      DO J=1,2
        DO I4P=1,N4M
          DO I4=1,4
            IF( IZFIXE(I4P,I4,L,J) <= 0 )       CYCLE
            IF( ISCFIXE(L) <= NCOUBAS2 ) THEN
              nfixbarrel = nfixbarrel + 1
            ELSE
              nfixendcap = nfixendcap + 1
            ENDIF
            IF( NZSure >= NZSureMax )           CYCLE
            IF( JZFIXE(I4P,I4,L,J) > 10000000 ) CYCLE
            RDIF = ABS( RZFIXE(I4P,I4,L,J)-RIFIXE(I4P,I4,L,J) )
            IF( ABS(RDIF) > 0.0500D0 )    CYCLE
            NZSure = NZSure + 1
            ITS = IZFIXE(I4P,I4,L,J)
            IC  = KTUBHI(ITS)/10000
            IZSure(NZSure) =  ITS
            ICSure(NZSure) =  IC
            IWSure(NZSure) = (KTUBHI(ITS)-IC*10000)/10
            IPSure(NZSure) =  KTUBHI(ITS)-IC*10000-IWSure(NZSure)*10
            IF( NZSure < IFSure(IC) ) IFSure(IC) = NZSure
          ENDDO
        ENDDO
      ENDDO
    ENDDO
    IF( ResBadAli > 0.0001d0 ) THEN
      IF( nfixbarrel > 0 .and. nfixendcap > 0 ) then
        IF( nfixendcap > nfixbarrel ) then
           increaseErrorBarrel = .true.
        ELSE
           increaseErrorEndcap = .true.
        ENDIF
      ENDIF
    ENDIF
    IF(MBPri >= 3) THEN
      WRITE(*,'(/," At beginning of the g.f.f.m. : NZSure =",I6,/,(" IZSure =",20I6))') NZSure,IZSure(1:NZSure)
      IF( increaseErrorBarrel ) WRITE(*,'(" >>> we will increase errors on barrel tubes ")')
      IF( increaseErrorEndcap ) WRITE(*,'(" >>> we will increase errors on endcap tubes ")')
    ENDIF
!
!-- Fourth, determine all the multilayers to take into account in the
!-- statistical fit (NBFAX, ...)
    CALL GIVDEB1(1)
    CALL DEFFAX( -0.1D0 , -0.1D0 )
    ItypFit = 6
!
    IF(MBPri >= 3) THEN
      WRITE(*,'(" Before fit 6 with matter : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
      WRITE(*,'("                                      S1MASC =",8F12.7)') S1MASC(1:NBMASC)
      CALL TRAKMU(0)
      CALL PRFILI(3)
    ENDIF
!
!
! Try first to improve phi coordinate
    IF( AtCosm == 0 .AND. REXTRA(1)/DREXTR(1) > 3.d0 ) THEN
      CALL ktime_mubo('fit phi6',1)
      CALL refinePhi
      CALL ktime_mubo('fit phi6',2)
    ENDIF
!
    PAFIPP = PAFITP
300 CONTINUE
    maxcall = 800
    err(1) = PAFITZ
    err(2) = PAFITS
    err(3) = PAFITZ
    err(4) = PAFITS
    err(5) = PAFIPP
    err(6:5+Navant) = 0.d0
    err(5+Navant+1:5+NBMASC) = errsca
    IF(MBPri >= 3) PRINT 944,ItypFit,NBFIXE,NBLIBR,SIGMAK,maxcall
    CALL ktime_mubo('fit m 6 ',1)
    CALL minimizeFcn(5+NBMASC,maxcall,err)
    CALL ktime_mubo('fit m 6 ',2)
    NFICAN = NFICAN + 1
    NFSCAN = NFSCAN + 1
!-- if momentum > 10 TeV, fix it to 10 TeV and refit
    IF( ABS(SMU) < 0.01d0 ) THEN
      IF( PAFIPP > 0.0001d0 ) THEN
        IF( MBPri >= 1 ) WRITE(*,*) ' warning track with SMU= ',SMU
        PAFIPP = 0.d0
        SMU    = SIGN( 0.01d0 , SMU )
        GO TO 300
      ENDIF
    ENDIF
!-- now recompute chi2 in case there was a change in list of crossed stations
    CALL GIVDEB1(1)
    CALL DEFFAX(-0.1d0,-0.1d0)
    CALL TRAFAX(1, 0)
    CALL getChi2(FNEW)
    IF(MBPri >= 3) WRITE(*,'(" Final chi2 before/after latest deffax ",2F9.3)') FMIN,FNEW
    FMIN = FNEW
!
    IF( MBPri >= 3 ) THEN
      WRITE(*,'(" After fit 6 with matter : DZT1-2,DS1-2,SMU =",5F12.7)') DZT1,DZT2,DS1,DS2,SMU
      WRITE(*,'("                                     S1MASC =",8F12.7)') S1MASC(1:NBMASC)
      CALL PRFILI(3)
    ENDIF
!
 END SUBROUTINE FitWithMatter
!
!
!> Get the chi2 of the fit (valid only for ItypFit=6)
!
 SUBROUTINE getChi2(C)
 USE M_MB_Control
 USE M_MB_FAX
 USE M_MB_PACONS
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
 REAL(8), INTENT(OUT) :: C
 INTEGER :: IMS, NN
 REAL(8) :: X
!
   C =  0.D0
   X = -5.D0   ! 5 parameters to be fitted
   DO IMS=1,NBMASC
     C = C + S1MASC(IMS)**2
   ENDDO
   DO NN=1,NBFAX
     C = C + DZFIXE(NN,1,1) + DZFIXE(NN,2,1) + DSFIXE(NN,1,1)       + DSFIXE(NN,2,1)
     X = X + NZFIXE(NN,1,1) + NZFIXE(NN,2,1) + NSFIXE(NN,1,1)*FacSZ + NSFIXE(NN,2,1)*FacSZ
   ENDDO
   IF( NCFAX > 0 ) THEN
     NN = NBFAX + 1
     C = C + DSFIXE(NN,1,1)       + DSFIXE(NN,2,1)
     X = X + NSFIXE(NN,1,1)*FacSZ + NSFIXE(NN,2,1)*FacSZ
   ENDIF
   IF( AtCosm == 0 ) THEN
     X = X + 2.d0
     C = C + (REXTRA(1)/DREXTR(1))**2 + (ZEXTRA(1)/DZEXTR(1))**2
   ENDIF
   X = MAX( X , 1.D0 )
   C = C / X
!
 END SUBROUTINE getChi2
!
!
!> Scan values of phi to try to improve it before the last final fit
!> \author A.Ouraou
!
 SUBROUTINE refinePhi
 USE M_MB_Control
 USE M_MB_Fit
 IMPLICIT NONE
 REAL(8) :: ds1ref, ds2ref, fnew, ds1min, ds2min, shift, direc, xyz1, xyz2, dist
 REAL(8) :: DsFastRTref
 REAL(8), PARAMETER :: bigDs=99999999.d0
 INTEGER :: i, j
!
   IF( LFastRT ) THEN   ! keep RT fixed during fit to improve convergence
     DsFastRTref = DsFastRT
     DsFastRT    = bigDs
   ENDIF
   saveDcaTrk = .false.
!
   IF( MBPri >= 3 ) THEN
     CALL TRAKMU(0)
     CALL getChi2(fmin)
     WRITE(*,*) ' refinePhi with old ds1/ds2/fmin',ds1,ds2,fmin
     CALL PRFILI(3)
   ENDIF
   ds1min = ds1
   ds2min = ds2
   fmin   = 999999.d0
   ds1ref = ds1
   ds2ref = ds2
   xyz1    = sqrt( x1**2 + y1**2 + z1**2 )
   xyz2    = sqrt( x2**2 + y2**2 + z2**2 )
   dist    = 0.5d0*abs(xyz2-xyz1)
   direc   = rextra(1) * dist / xyz1 / 4.d0
   DO j=-2,2
     shift = j*0.5d0
     DO i=-5,5
       ds1 = ds1ref + shift + i*direc
       ds2 = ds2ref + shift - i*direc
       CALL TRAKMU(0)
       CALL getChi2(fnew)
       IF( fnew < fmin ) THEN
         ds1min = ds1
         ds2min = ds2
         fmin   = fnew
       ENDIF
     ENDDO
   ENDDO
!
   ds1  = ds1min
   ds2  = ds2min
   IF( MBPri >= 3 ) THEN
     WRITE(*,*) ' refinephi with new ds1/ds2/fmin',ds1,ds2,fmin
     CALL TRAKMU(0)
     CALL PRFILI(3)
   ENDIF
!
   IF( LFastRT ) DsFastRT = DsFastRTref
   saveDcaTrk = .true.
!
 END SUBROUTINE refinePhi
!
