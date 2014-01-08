!
!
!
!
! Author : M.Virchaux & L.Chevalier
!***********************************************************************
 SUBROUTINE RESOMUB(LUNB,LUNC,VECMU,NMSRES, RESOLU,RESELO)
 USE M_MB_General_CONSTANTS
 USE M_MB_PRECIS
 USE M_MB_COMUSC
 USE M_MB_Fit
 USE M_MB_Cholevski
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LUNB, LUNC, NMSRES
 REAL(8), INTENT(IN) :: VECMU(8)
 REAL(8), INTENT(OUT) :: RESOLU, RESELO
 INTEGER, PARAMETER :: NSIM=19
 REAL(8) :: XXX(99), DDD(99), SSS(NSIM,99)
 REAL(8) :: SS(NSIM), SXS(NSIM), SDS(NSIM), SS2(NSIM,NSIM)
 REAL(8) :: AAS(NSIM), BBS(NSIM), DIAG(NSIM)
 REAL(8) :: CC1(NSIM), CC2(NSIM,NSIM), CC1TEM(NSIM), CC2TEM(NSIM,NSIM), AMBDA(NSIM)
 REAL(8) :: PHIMES(2), DDMETE(KMESU)
 REAL(8) :: VIN(8), VOUT(8)
 REAL(8) :: SINTH, EMU, DRPHI, CURVAB,SSBDL,SSBLDL, SINFIM,COSFIM
 REAL(8) :: PAPAS, SU,SX,SD,SXD,SX2,SD2, DELTA,AAA,BBB,CCC, CCR, CHI2
 REAL(8) :: RESOTE, DELOSS, FACT, RCHAMO,ZCHAMO, AZIN, DBDL,DBLDL, R3IN,R3OUT
 REAL(8) :: PAPA0, PPPP, R2OUT, AZOUT, ALPHA
 INTEGER :: I, J, NBIF, IPAS, IME,IME0, L, J1,J2, IFAIL, NMSMAX, NPHIME,IPHIME
!
#if defined(ATLAS_STAND_OBSOLETE)
 REAL(8) :: X0,Y0,Z0, BX0,BY0,BZ0
 INTEGER :: I0, IFLARES
 CHARACTER(4) :: Car4
 LOGICAL, SAVE :: FIRST=.TRUE.
 INTEGER, SAVE :: LUNPRB=0, LUNPRC=0
!
    IF( FIRST ) THEN
      FIRST = .FALSE.
      CALL INCOMU     !>> Initialise basic constants
    ENDIF
    IF(LUNB /= LUNPRB) THEN
!>> Read in mag. field and initialise relevant parameters
      LUNPRB = LUNB
      REWIND LUNB
      CALL RBMAGA(LUNB,Car4)
      FieldVers = Car4
!>> Initialisation of module MB_PRECIS
      STEP(1) = 15.D0
      STEP(2) = 15.D0
      STEP(3) = 15.D0
      STEP(4) = 15.D0
      BBNUL = 0.00005D0
      BBMIN = 0.00200d0
      DBMAX = 0.050D0
      DBMIN = 0.020D0
      PASMIN = 0.20D0
      FACREF = 30.0D0
      FACMIN =  0.20D0
      FACMAX =  2.00D0
!>> Find max.value of |Z| with non-zero mag. field
      X0 =   50.D0
      Y0 =  550.D0
      AZBMIN = 1400.d0
      DO I0=1,100
        Z0 = 1400.D0 + I0*10.D0
        CALL BFIELD(X0,Y0,Z0,BX0,BY0,BZ0)
        IF( SQRT(BX0**2+BY0**2+BZ0**2) < BBMIN ) AZBMIN = Z0 + STEP(4)
        IF( SQRT(BX0**2+BY0**2+BZ0**2) < BBNUL ) EXIT
      ENDDO
      AZBNUL = Z0
      PAMAXB = 300.d0
      WRITE('(/" >>> The magnetic field is considered small   for :  Abs(Z) >",F7.1," cm")') AZBMIN
      WRITE('(" >>> The magnetic field is considered as null for :  Abs(Z) >",F7.1," cm"/)') AZBNUL
    ENDIF
    IF(LUNC /= LUNPRC) THEN
!>> Define chambers and m.s. matter geometry
      LUNPRC = LUNC
      REWIND LUNC
!>> Read Atlas Muon Data Base (without transforming TGC format)
      CALL REAMDC(LUNC,100)
!>> And transfer it to the MUONBOY description of chambers
      CALL AMDCOX
!>> Get MUONBOY description of matter :
!>>             *----------> Dead matter of RPCs (+MDTs ?)
!>>             |*---------> Alignement stuff
!>>             ||*--------> Inner detectors + Solenoid
!>>             |||*-------> Liq. argon calorimeters
!>>             ||||*------> Had. "tile" calorimeters
!>>             |||||*-----> Shieldings
!>>             ||||||*----> Feet
!>>             |||||||*---> End-cap toroids
!>>             ||||||||*--> Barrel toroid
!**** IFLARES = 000001111
      IFLARES = 111111111
      CALL MATIERE(IFLARES, 1)
    ENDIF
#endif
!
!
!>> Get the starting coordinates of the muon,
    VDEB(1:8,1) = VECMU(1:8)
    SINTH = SQRT( VECMU(4)**2 + VECMU(5)**2 )
    EMU   = VECMU(7)
!>> then, find the NBMESU measurement points and
!>> the NBMASC scattering points along the track by calling
!>> the routine TRMUSC (with tracking in one direction only)
!>> after a modification of Common /COFITI/
!>> (internal limits for tracking)
    RCHAMO = RCHAMI
    ZCHAMO = ZCHAMI
    RCHAMI = 0.D0
    ZCHAMI = 0.D0
    NBMASC = 0
    CALL TRMUSC(1)
    RCHAMI = RCHAMO
    ZCHAMI = ZCHAMO
!>> and reorder, discard and/or merge some  and possibly,
!>> reduce the number of these multiple scattering points
    NMSMAX = MIN( 8 , MAX( 1 , NMSRES ) )
    CALL REDUCS(NMSMAX)
!
!
!>> Return here if not enough measurement points or too many m.s. pointS
    RESOLU = 1.D0
    RESELO = 1.D0
    IF( NBMESU < 3 .OR. NBMASC > NSIM ) RETURN
!
!
!>> If MULSCA = 0  ===>  without multiple scattering !
    IF(MULSCA == 0) NBMASC = 0
!
!
!>> Determine the azimuth(s) of the measurement
    DO I=1,NBMESU
      NBIF = 1
      DO J=I+1,NBMESU
        IF(ABS(FFMESU(I)-FFMESU(J)) < 0.001) NBIF = NBIF + 1
        IF(NBIF > NBMESU/2) GO TO 80
      ENDDO
    ENDDO
    I = 1
80  PHIMES(1) = FFMESU(I)
    IF( NBMESU-NBIF >= 3 ) THEN
      NPHIME = 2
      DO I=1,NBMESU
        IF(ABS(FFMESU(I)-PHIMES(1)) > 0.001) GO TO 90
      ENDDO
90    PHIMES(2) = FFMESU(I)
    ELSE
      NPHIME = 1
    ENDIF
!
!
!>> Loop on the azimuth(s) of the measurement
    DO 400 IPHIME=1,NPHIME
!
!>> Compute measurement errors in this azimuthal plane
!----------> @@@@@@@
      DRPHI = 0.8D0
!----------> @@@@@@@
      DO I=1,NBMESU
        DDMETE(I) = SQRT( (COS(FFMESU(I)-PHIMES(IPHIME))*DDMESU(I))**2 + (SIN(FFMESU(I)-PHIMES(IPHIME))*DRPHI)**2 )
      ENDDO
!
!
!>> Compute magnetic deflections in the plane of measurement
      CURVAB = 0.D0
      SSBDL  = 0.D0
      SSBLDL = 0.D0
      SINFIM = SIN(PHIMES(IPHIME))
      COSFIM = COS(PHIMES(IPHIME))
      VIN(1:8) = VDEB(1:8,1)
      DO 300 IPAS=1,2000
        AZIN = ABS( VIN(3) )
        IF(AZIN < AZBMIN) THEN
          PAPAS = STEP(3)
        ELSE
          PAPAS = STEP(4)
        ENDIF
        CALL ELEMHB(VIN,PAPAS,VOUT,CURVAB,SINFIM,COSFIM, DBDL,DBLDL)
        R3IN  = getRMUSC(VIN)
        R3OUT = getRMUSC(VOUT)
        IME0 = 0
        PAPA0 = 9999999.D0
        DO IME=1,NBMESU
          IF((RRMESU(IME)-R3IN)*(RRMESU(IME)-R3OUT) > 0.D0)  CYCLE
          PPPP = PAPAS * (RRMESU(IME)-R3IN) / (R3OUT-R3IN)
          IF( PPPP > PAPA0 )                                 CYCLE
          IME0 = IME
          PAPA0 = PPPP
        ENDDO
        IF( IME0 >= 1 ) THEN
          PAPAS = PAPA0 + 0.1D0
          CALL ELEMHB(VIN,PAPAS,VOUT,CURVAB,SINFIM,COSFIM, DBDL,DBLDL)
          IF( IME0 == 1 ) THEN
            CURVAB = 0.D0
            SSBDL  = 0.D0
            SSBLDL = 0.D0
          ELSE
            CURVAB = CURVAB + PAPAS
            SSBDL  = SSBDL  + DBDL
            SSBLDL = SSBLDL + DBLDL
          ENDIF
          DDD(IME0) = ( CURVAB*SSBDL - SSBLDL ) * 0.003D0 / EMU
        ELSE
          CURVAB = CURVAB + PAPAS
          SSBDL  = SSBDL  + DBDL
          SSBLDL = SSBLDL + DBLDL
        ENDIF
        R2OUT = SQRT( VOUT(1)**2 + VOUT(2)**2 )
        AZOUT = ABS( VOUT(3) )
        IF( R2OUT > RCHAMA .OR. AZOUT > ZCHAMA ) EXIT
        VIN(1:8) = VOUT(1:8)
300   ENDDO
!
!
!>> Compute resolution !!!
!
      DO 100 L=1,NBMESU
        XXX(L)   = RRMESU(L) - RRMESU(1)
        DO J=1,NBMASC
          SSS(J,L) = 0.D0
          IF( RRMESU(L) <= RRMASC(J) )   CYCLE
          ALPHA = 0.0136D0*SQRT(X0MASC(J)) * (1.D0+0.038D0*LOG(X0MASC(J))) / EMU
          SSS(J,L) = ALPHA * ( RRMESU(L) - RRMASC(J) )
        ENDDO
100   ENDDO
!
      SU  = 0.D0
      SX  = 0.D0
      SD  = 0.D0
      SXD = 0.D0
      SX2 = 0.D0
      SD2 = 0.D0
      DO L=1,NBMESU
        SU  = SU   + 1.D0          / DDMETE(L)**2
        SX  = SX   + XXX(L)        / DDMETE(L)**2
        SD  = SD   + DDD(L)        / DDMETE(L)**2
        SXD = SXD  + XXX(L)*DDD(L) / DDMETE(L)**2
        SX2 = SX2  + XXX(L)**2     / DDMETE(L)**2
        SD2 = SD2  + DDD(L)**2     / DDMETE(L)**2
      ENDDO
      DO J=1,NBMASC
        SS(J)  = 0.D0
        SXS(J) = 0.D0
        SDS(J) = 0.D0
        DO L=1,NBMESU
          SS(J)  = SS(J)  + SSS(J,L)        / DDMETE(L)**2
          SXS(J) = SXS(J) + XXX(L)*SSS(J,L) / DDMETE(L)**2
          SDS(J) = SDS(J) + DDD(L)*SSS(J,L) / DDMETE(L)**2
        ENDDO
      ENDDO
      DO J1=1,NBMASC
        DO J2=1,NBMASC
          SS2(J1,J2)  = 0.D0
          DO L=1,NBMESU
            SS2(J1,J2) = SS2(J1,J2) + SSS(J1,L)*SSS(J2,L) / DDMETE(L)**2
          ENDDO
        ENDDO
      ENDDO
!
      DELTA = SX2*SU - SX**2
      AAA = ( SXD*SU - SD*SX )  / DELTA
      BBB = ( SX2*SD - SXD*SX ) / DELTA
      CCC = SD2 - AAA*SXD - BBB*SD
!
      DO J=1,NBMASC
        AAS(J) = ( SXS(J)*SU - SS(J)*SX )  / DELTA
        BBS(J) = ( SX2*SS(J) - SXS(J)*SX ) / DELTA
        CC1(J) = 2.D0*SDS(J) - AAA*SXS(J) - AAS(J)*SXD - BBB*SS(J)  - BBS(J)*SD
        CC1TEM(J) = - CC1(J)
      ENDDO
      DO J1=1,NBMASC
        DO J2=1,NBMASC
          CC2(J1,J2)= 2.D0*SS2(J1,J2) - AAS(J1)*SXS(J2) - AAS(J2)*SXS(J1) - BBS(J1)*SS(J2)  - BBS(J2)*SS(J1)
          IF( J2 == J1 ) CC2(J1,J2) = CC2(J1,J2) + 2.D0
          CC2TEM(J1,J2) = CC2(J1,J2)
        ENDDO
      ENDDO
!>> Resolution du systeme (CC2TEM * AMBDA = CC1TEM) par decomposition de cholevski
      IF(NBMASC /= 0) THEN
        CALL CHOLDC(CC2TEM,NBMASC,NSIM,DIAG,IFAIL)
        IF( IFAIL == 0 ) THEN
          CALL CHOLSL(CC2TEM,NBMASC,NSIM,DIAG,CC1TEM(1:NBMASC),AMBDA(1:NBMASC))
        ELSE
          WRITE(6,*)" Probleme d'inversion = ", IFAIL
          AMBDA(1:NBMASC) = 0.d0
        ENDIF         
      ENDIF
!
      CCR = 0.D0
      DO J1=1,NBMASC
        CCR  = CCR + AMBDA(J1)          *CC1(J1)
        CCR  = CCR + AMBDA(J1)**2       *CC2(J1,J1)/2.D0
        DO J2=1,J1-1
          CCR = CCR + AMBDA(J1)*AMBDA(J2)*CC2(J1,J2)
        ENDDO
      ENDDO
!
      CHI2 = CCC + CCR
      RESOTE = 1.D0
      IF( CHI2 > 0.01D0 ) RESOTE = 1.D0 / SQRT(CHI2)
!>> Take as resolution the best one among the (1 or) 2 planes of measurement
      RESOLU = MIN( RESOLU , RESOTE )
!******************************************************************
!      PRINT 5551,
!     +(L,RRMESU(L),FFMESU(L),PHIMES(IPHIME),DDMETE(L),L=1,NBMESU)
!5551  FORMAT(' i,Rmesu,Fmesu,Fref,Dmesu =',I5,F8.2,2F8.5,F8.4)
!      PRINT 5552,RESOTE,RESOLU
!5552  FORMAT(' ResoTe,Resolu =',2F10.4)
!******************************************************************
400 ENDDO
!
!
!
!>> Contibution from energy losses fluctuations in the calos
!>> IF NRJLOS = 0  ===>  without energy loss !
    IF(NRJLOS == 0) THEN
      DELOSS = 0.D0
    ELSE
      FACT = SQRT(14.D0/11.D0)
!------------ |Eta| < 1.1 --------------------------------
      IF( SINTH > 0.599334D0 ) FACT = 1.D0/SQRT(SINTH)
      DELOSS = (0.480D0 + 0.105D0*EMU/100.D0)/EMU * FACT
    ENDIF
    RESELO = SQRT( RESOLU**2 + DELOSS**2 )
!
 END
