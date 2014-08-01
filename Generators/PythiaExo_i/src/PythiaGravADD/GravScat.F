C...PYKCUT
C...Dummy routine, which the user can replace in order to make cuts on
C...the kinematics on the parton level before the matrix elements are
C...evaluated and the event is generated. The cross-section estimates
C...will automatically take these cuts into account, so the given
C...values are for the allowed phase space region only. MCUT=0 means
C...that the event has passed the cuts, MCUT=1 that it has failed.

      SUBROUTINE PYKCUT(MCUT)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
     
C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      SAVE /PYDAT1/,/PYINT1/,/PYINT2/

C--common block for the probabilities of SM particles
      DOUBLE PRECISION PQUARK,PLEPT,PNEUT,PGLUON,PGAMMA,PWBOSN,
     & PZBOSN,PHIGGS,PFERM(3),PBOSON(5)
      COMMON /BHPROB/PQUARK,PLEPT,PNEUT,PGLUON,PGAMMA,PWBOSN,
     & PZBOSN,PHIGGS,PFERM,PBOSON
C--common block for the main parameters
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT
C--common block for decay of the black hole
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM
      SAVE /BLACKH/,/BHPARM/,/BHPROB/
C      write(*,*) 'in subroutine pycut, MPLNCK: ',MPLNCK
C...Set default value (accepting event) for MCUT.
      MCUT=0

C...Read out subprocess number.
      ISUB=MINT(1)
      ISTSB=ISET(ISUB)

C...Read out tau, y*, cos(theta), tau' (where defined, else =0).
      TAU=VINT(21)
      YST=VINT(22)
      CTH=0D0
      IF(ISTSB.EQ.2.OR.ISTSB.EQ.4) CTH=VINT(23)
      TAUP=0D0
      IF(ISTSB.GE.3.AND.ISTSB.LE.5) TAUP=VINT(26)

C...Calculate x_1, x_2, x_F.
      IF(ISTSB.LE.2.OR.ISTSB.GE.5) THEN
        X1=SQRT(TAU)*EXP(YST)
        X2=SQRT(TAU)*EXP(-YST)
      ELSE
        X1=SQRT(TAUP)*EXP(YST)
        X2=SQRT(TAUP)*EXP(-YST)
      ENDIF
      XF=X1-X2

C...Calculate shat, that, uhat, p_T^2.
      SHAT=TAU*VINT(2)
      SQM3=VINT(63)
      SQM4=VINT(64)
      RM3=SQM3/SHAT
      RM4=SQM4/SHAT
      BE34=SQRT(MAX(0D0,(1D0-RM3-RM4)**2-4D0*RM3*RM4))
      RPTS=4D0*VINT(71)**2/SHAT
      BE34L=SQRT(MAX(0D0,(1D0-RM3-RM4)**2-4D0*RM3*RM4-RPTS))
      RM34=2D0*RM3*RM4
      RSQM=1D0+RM34
      RTHM=(4D0*RM3*RM4+RPTS)/(1D0-RM3-RM4+BE34L)
      THAT=-(0.5D0*SHAT*MAX(RTHM,1D0-RM3-RM4-BE34*CTH))
      UHAT=-(0.5D0*SHAT*MAX(RTHM,1D0-RM3-RM4+BE34*CTH))
      PT2=MAX(VINT(71)**2,0.25D0*SHAT*BE34**2*(1D0-CTH**2))

C...Decisions by user to be put here.

      IF ( TOTDIM.LE.3 ) RETURN

      IF ( SQRT(SHAT).LT.MINMSS ) RETURN

      PLPOW=2.0D0/(TOTDIM-3.0D0)

      RHORSQ=(RHFACT*SQRT(SHAT))**PLPOW
C...  If pT for the hard scattering (scale used in parton densiy) is
C...  high enough use CHARYBDIS, VINT(52)=Q^2 of the hard subprocess
      IF ( VINT(52)*RHORSQ*FACTOR.LE.1.0D0 ) RETURN

      MCUT=1

      RETURN
      END


C*********************************************************************

C...PYEVWT
C...Dummy routine, which the user can replace in order to multiply the
C...standard PYTHIA differential cross-section by a process- and
C...kinematics-dependent factor WTXS. For MSTP(142)=1 this corresponds
C...to generation of weighted events, with weight 1/WTXS, while for
C...MSTP(142)=2 it corresponds to a modification of the underlying
C...physics.

      SUBROUTINE PYEVWT(WTXS)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      SAVE /PYDAT1/,/PYINT1/,/PYINT2/

C...Parameters used during the run, to be printed in the data files, therefore needed by WRIPAR
      DOUBLE PRECISION UBMUL,WTPA
    
C...  For chosing clustering algorithm
C      character CLALG*6
      COMMON/RUNPAR/UBMUL,WTPA
      SAVE /RUNPAR/ 
      WTPA=1.0D0
      

C...Set default weight for WTXS.
      WTXS=1D0

C...Read out subprocess number.
      ISUB=MINT(1)
      ISTSB=ISET(ISUB)

C...Read out tau, y*, cos(theta), tau' (where defined, else =0).
      TAU=VINT(21)
      YST=VINT(22)
      CTH=0D0
      IF(ISTSB.EQ.2.OR.ISTSB.EQ.4) CTH=VINT(23)
      TAUP=0D0
      IF(ISTSB.GE.3.AND.ISTSB.LE.5) TAUP=VINT(26)

C...Read out x_1, x_2, x_F, shat, that, uhat, p_T^2.
      X1=VINT(41)
      X2=VINT(42)
      XF=X1-X2
      SHAT=VINT(44)
      THAT=VINT(45)
      UHAT=VINT(46)
      PT2=VINT(48)
C...If Charybdis is not used the events (if(ISUB.ne.4)) should be
C...weighted, else not
c      IF( ISUB.NE.4 ) WTXS=(PT2**3)/(70.0D0*70.0D0)**3
c      IF( ISUB.NE.4 ) WTXS=(PT2**3)/(SHAT/800.0D0)**3
C      IF( ISUB.NE.4 ) WTXS=((PT2)**5)/((WTPA*SHAT/(X1*X2))/800.0D0)**5
      IF( ISUB.NE.4 ) WTXS=((PT2)**3)/((WTPA*SHAT/(X1*X2))/800.0D0)**3
C      write(*,*) 'in pyevwt: ',ISUB, WTXS, WTPA
      RETURN
      END


C*********************************************************************
C...WTGRAV
C...03/09/09 changed by boelaert in order to return GRAVEV flag
C...Routine which returnes the weight due to gravity 
      SUBROUTINE WTGRAV(ISUB, WT, GRAVE)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
C...Some parameters from CHARYBDIS commom block are needed
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM
C...  Gravity parameters
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
C...Parameters used during the run, to be printed in the data files, therefore needed by WRIPAR
C      DOUBLE PRECISION PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA
C      INTEGER NEVE,NOINT,NCUT,TUNEA,NOQCD
C...  For chosing clustering algorithm
C      character CLALG*6
C      COMMON/RUNPAR/PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA,
C     $     NEVE,NCUT,TUNEA,NOINT,NOQCD,CLALG

      INTEGER GRAVE
      DOUBLE PRECISION WT
C      SAVE /RUNPAR/,/MASSES/,/BLACKH/,/BHPARM/
      SAVE /MASSES/,/BLACKH/,/BHPARM/
C...Inline function definitions
      include 'inlines.f'
C      write(*,*) 'in subroutine pycut, wtgrav: ',MPLNCK
C...Get shat from Pythia
      SHAT=PARI(14)
C      SHAT=VINT(44)
CC...Get z=cos(theta) from Pythia
      Z=PARI(41)
C      Z=VINT(23)
C...Get alpha_s
      AS=PYALPS(PARI(22))
C      AS=PYALPS(VINT(52))
C...Get incoming flavours, negative for anti
      IFI1=MSTI(15)
C      IFI1=MINT(15)
      IFI2=MSTI(16)
C      IFI2=MINT(16)
C...Get outgoing flavours
      IFO1=MSTI(21)
C      IFO1=MINT(21)
      IFO2=MSTI(22)
C      IFO2=MINT(22)

C...WT is set to sigma_{grav+qcd}/sigma_qcd=(sigma_{grav+qcd}+sigma_{qcd})/sigma_{qcd} 
C...for the different
C...sub-processes
      WT=1.0D0

C...If we have 'effective' gravity or totgrav and SHAT.lt.MS**2
      if(SHAT.lt.MS**2) then
         IF ( ISUB.EQ.12 ) THEN
C...  q + qbar -> q' + qbar'
            WT=WTRAT(fqapb(z),gqapb(z),hqapb(z),1/36.0D0)
         ELSEIF ( ISUB.EQ.11 ) THEN
            IF((IFI1.EQ.IFI2).AND.(IFO1.EQ.IFO2).AND.(IFO1.EQ.IFI1))THEN
C...  If q + q -> q + q (all are equal)
               WT=WTRAT(fqqqq(z),gqqqq(z),hqqqq(z),1/72.0D0) 
            ELSEIF((IFI1.EQ.-IFI2).AND.(IFO1.EQ.-IFO2).AND.
     &              (ABS(IFI1).EQ.ABS(IFO1))) THEN
C...  If q + qbar -> q + qbar
               WT=WTRAT(fqaqa(z),gqaqa(z),hqaqa(z),1/36.0D0)
            ELSE
C...  (q + q' -> q + q') = (q + qbar' -> q + qbar')
               WT=WTRAT(fqpqp(z),gqpqp(z),hqpqp(z),1/36.0D0)
            ENDIF
         ELSEIF ( ISUB.EQ.28 ) THEN
C...  q + g -> q + g
C...  qbar + g -> qbar + g 
            WT=WTRAT(fqgqg(z),gqgqg(z),hqgqg(z),1/96.0D0)
         ELSEIF ( (ISUB.EQ.53).OR.(ISUB.EQ.13) ) THEN
C...  g + g -> q + qbar
C...  or q + qbar -> g + g
            WT=WTRAT(fggqa(z),gggqa(z),hggqa(z),1/256.0D0)
         ELSEIF ( ISUB.EQ.68 ) THEN
C...  g + g -> g + g
            WT=WTRAT(fgggg(z),ggggg(z),hgggg(z),1/512.0D0)
         ENDIF
      endif

C... If 'eikonal' gravitational scattering or totgrav at high energies
      if(SHAT.ge.MS**2) then
         IF ( ISUB.EQ.12 ) THEN
C...  q + qbar -> q' + qbar'
            WT=1.0D0
            if ( noqcd.eq.1 ) WT=0.0D0
         ELSEIF ( ISUB.EQ.11 ) THEN
            IF((IFI1.EQ.IFI2).AND.(IFO1.EQ.IFO2).AND.(IFO1.EQ.IFI1))THEN
C...  If q + q -> q + q (all are equal)
               WT=WTEIK(fqqqq(z),hqqqq(z),1.0D0/72.0D0,0) 
            ELSEIF((IFI1.EQ.-IFI2).AND.(IFO1.EQ.-IFO2).AND.
     &              (ABS(IFI1).EQ.ABS(IFO1))) THEN
C...  If q + qbar -> q + qbar
               WT=WTEIK(fqaqa(z),hqaqa(z),1.0D0/36.0D0,0)
            ELSE
C...  (q + q' -> q + q') = (q + qbar' -> q + qbar')
               WT=WTEIK(fqpqp(z),hqpqp(z),1.0D0/36.0D0,0)
            ENDIF
         ELSEIF ( ISUB.EQ.28 ) THEN
C...  q + g -> q + g
C...  qbar + g -> qbar + g 
            WT=WTEIK(fqgqg(z),hqgqg(z),1.0D0/96.0D0,0)
         ELSEIF ( (ISUB.EQ.53).OR.(ISUB.EQ.13) ) THEN
C...  g + g -> q + qbar
C...  or q + qbar -> g + g
            WT=1.0D0
            if ( noqcd.eq.1 ) WT=0.0D0
         ELSEIF ( ISUB.EQ.68 ) THEN
C...  g + g -> g + g
c gluons need same color
            WT=WTEIK(fgggg(z),hgggg(z),1.0D0/512.0D0,0)
         ENDIF
      endif

      GRAVE = GRAVEV
C      write (*,*) 'in wtgrav, GRAVEV: ', GRAVEV
      RETURN
      END

C*********************************************************************
C...WTRAT
C...Routine which calculates the weight due to effective gravity 

      DOUBLE PRECISION FUNCTION WTRAT(f,g,h,ks)
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
C...Some parameters from CHARYBDIS commom block are needed
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM
C...KKMASS to be used for KK-mass maximum integration, and MSAT as Atwoods Ms
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
C...Parameters used during the run, to be printed in the data files, therefore needed by WRIPAR
C      DOUBLE PRECISION ETCOND,UBMUL,WTPA
C     INTEGER NEVE,NOINT,NOQCD,NCUT,TUNEA
C.. Complex parameters needed to take eikonalization into account
C      DOUBLE COMPLEX X, xgeof
      DOUBLE COMPLEX  xgeof
      DOUBLE PRECISION ks,k
      SAVE /MASSES/,/BLACKH/, /BHPARM/

      PI=3.141593D0
      n=TOTDIM-4
C...Get shat from Pythia
      SHAT=PARI(14)
     
C...Get alpha_s
      AS=PYALPS(PARI(22))
C      write(*,*) 'in function WTRAT, alpha_s: ', AS
C...Get z=cos(theta) from Pythia
      z=PARI(41)
      t=-(shat*(1.0d0-z)/2.0d0)
      k=sqrt(-t)
      WTRAT=1.0D0

      IF ((TOTDIM-4).LT.2) THEN
         RETURN
      ELSEIF ((TOTDIM-4).EQ.2) THEN
C...  I now divide Atwoods F (FF) by 2 to make Atwoods results better Agree with Giudice
C...  Starting from Hans (hep-ph/9911350) article I get an expression for D(s) 
C...  (equation 4 in hep-ph/9911231) wich is only half as big as that given by Atwood. 
C...  This factor 2 I now absorb into F 
         FF=LOG(shat/MSAT**2)
      ELSEIF ((TOTDIM-4).GT.2) THEN
         FF=2.0D0/(TOTDIM-4-2)
      ENDIF

C...  Interference term 
      QCDGI=-(4.0D0*g*AS*FF*shat**2/
     $     (MSAT**(TOTDIM-4+2)*KKMASS**(2-(TOTDIM-4))))

C      IF(NOINT.eq.1.or.NOQCD.eq.1) QCDGI=0.0D0

C...Error      cutfac=Gammafn((2.0D0 + n)/2.0D0)
      cutfac=Gammafn(dble(n)/2.0D0)


C...  no unitarization
C...  Default cutoff KKMASS is now MP

C...  Now with 'geometric' amplitude GG&MS 5.4
C...  Gravity is universally attractive, and for small arguments,
C.... there is no difference in sign between the channels,
C...  GG and MS has calculated the Born amplitude to be negative
C...  (eq 4.3), this is also the right sign to get an attractive
C...  potential (eq 4.5)

C...Complex return values seem difficult so we call a subroutine instead.
        call geof(h,ks,0,xgeof)

C...  Interference term, only real part interferes 
        QCDGI=-(DBLE(xgeof)*tfac(Ms,t,n)*cutfac*4.0D0*g*AS*FF*
     $       shat**2/
     $       (MSAT**(TOTDIM-4+2)*MS**(2-(TOTDIM-4))))

C        IF(NOINT.eq.1.or.NOQCD.eq.1) QCDGI=0.0D0
C...  Make Aborn depend on s,t,u to ensure continuous transition
C...  with A(t,Ms)/A(Ms) in tfac from AtwoodMpCutRescale.nb
C...  Weight=(QCD+Interference +gravity)/QCD
        QCD = f*AS**2
        gravity = tfac(Ms,t,n)**2*cutfac**2*Abs(xgeof)**2*16*h*FF**2
     $       *shat**4/
     $       (MSAT**(TOTDIM-4+2)*MS**(2-(TOTDIM-4)))**2

        WTRAT=(QCD+QCDGI+gravity)/qcd
        if ( NOQCD.eq.1 ) WTRAT=gravity/qcd

     

      RETURN
      END
C*********************************************************************
C...WTEIK
C...Routine which calculates the weight due to 'eikonal' gravity 

      DOUBLE PRECISION FUNCTION WTEIK(f,h,ks,ip)
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)

C...Some parameters from CHARYBDIS commom block are needed
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
C      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL,Gammafn
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM
C...GRAVEV is a flag to see if it was a gravitationally mediated event
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
C...The factor ks from Atwood et al hep-ph/99112
      DOUBLE PRECISION ks
      DOUBLE PRECISION MD
C      DOUBLE PRECISION PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA
C      INTEGER NEVE,NOINT,NCUT,TUNEA,NOQCD
C...  For chosing clustering algorithm
C      character CLALG*6
C      COMMON/RUNPAR/PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA,
C     $     NEVE,NCUT,TUNEA,NOINT,NOQCD,CLALG
C...  Parameters usd if eikgrav or effgrav
      DOUBLE PRECISION PLPOW, RHORSQ
C      SAVE /RUNPAR/,/MASSES/,/BLACKH/, /BHPARM/
      SAVE /MASSES/,/BLACKH/, /BHPARM/
 

      if(ip.eq.0) then
C...  Get shat from Pythia
         SHAT=PARI(14)
C...  Get z=cos(theta) from Pythia
         Z=PARI(41)
C...  Get alpha_s
         AS=PYALPS(PARI(22))
      else
        if ( NOQCD.eq.1 ) then
          wteik = 1.0D99
          return
        endif
C...  Get shat from Pythia
         SHAT=VINT(44)
C...  Get z=cos(theta) from Pythia
         Z=VINT(23)
C...  Get alpha_s
         AS=PYALPS(VINT(52))
      endif

      t=-(shat*(1.0d0-z)/2.0d0)
      q=sqrt(-t)

      PI=3.141593D0
      n=TOTDIM-4
      SQCD=f*AS**2*PI*ks/2.0D0/SHAT

      WTEIK=1.0D0

      IF ((TOTDIM-4).LT.2.OR.(TOTDIM-4).GT.7) THEN
        RETURN
      ENDIF

C...  Giudices Planck scale in terms of our
      MD=2.0D0**((-3.0D0+n)/(n+2.0D0))*PI**((-1.0D0+n)/(n+2.0D0))*MPLNCK


C...  Calculate cross section using the amplitude 2.5 in MSGG with
C...  \chi from eq 4.7 
C...  cf. 060714:1

        PLPOW=2.0D0/(TOTDIM-3.0D0)
        RHORSQ=(RHFACT*SQRT(shat))**PLPOW

        SEIK=1.0D0/(16.0D0*Pi*shat**2)
     $       *Aeik(shat,q,Ms,MD,n,sqrt(rhorsq))**2*shat/2.0D0

      WTEIK=(SQCD+SEIK)/SQCD
      if ( NOQCD.eq.1 ) WTEIK=SEIK/SQCD

      RETURN
      END
C*********************************************************************
C...  GCOLFX Routine to determine if a scattering is graviton-mediated
C...  which fixes the corresponding colour flow it that is the case.
      SUBROUTINE GCOLFX(ISUB,IPU1,IPU2)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
C...Some parameters from CHARYBDIS commom block are needed
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM

C...GRAVEV should be set to 0 for QCD-events
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
      SAVE /MASSES/,/BLACKH/,/BHPARM/
C...Inline function definitions
      include 'inlines.f'
         
C...Set flag for gravityevent to 0 (non-gravity)
      GRAVEV=0
      PARP(199)=0.0d0
C...Exit immediately if this is not a 2->2 scattering.
      IF ( N-IPU1.NE.3 ) RETURN

C...Get shat from Pythia
      SHAT=VINT(44)
CC...Get z=cos(theta) from Pythia
      Z=VINT(23)
C...Get alpha_s
      AS=PYALPS(VINT(52))
C...Get incoming flavours, negative for anti
      IFI1=MINT(15)
      IFI2=MINT(16)
C...Get outgoing flavours
      IFO1=MINT(21)
      IFO2=MINT(22)

C...Scaled mandelstam variables.      
      tz=-((1.0d0-z)/2.0d0)
      uz=-((1.0d0+z)/2.0d0)
      sz=1.0d0

C...Color flow for 'efective gravity' and totgrav SHAT.lt.MS
      if(SHAT.lt.MS**2) then
      IF ( ISUB.EQ.12 ) THEN
C...  q + qbar -> q' + qbar'
        if ( igprob(fqapb(z),gqapb(z),hqapb(z),1/36.0D0).ne.0 )
     $       call gcolfl(-1,ipu1,ipu2)
      ELSEIF ( ISUB.EQ.11 ) THEN
        IF((IFI1.EQ.IFI2).AND.(IFO1.EQ.IFO2).AND.(IFO1.EQ.IFI1)) THEN
C...  If q + q -> q + q (all are equal)
          if ( igprob(fqqqq(z),gqqqq(z),hqqqq(z),1/72.0D0).ne.0 ) then          
C     Fast med geometriserade amplituder här **** HÄR ***
              if ( pyr(0)*(Gfn(tz,uz)+Gfn(uz,tz)).lt.Gfn(tz,uz) ) then
C...  t-channel
                call gcolfl(1,ipu1,ipu2)
              else
C...  u-channel
                call gcolfl(2,ipu1,ipu2)
             
            endif
          endif
        ELSEIF((IFI1.EQ.-IFI2).AND.(IFO1.EQ.-IFO2).AND.
     &         (ABS(IFI1).EQ.ABS(IFO1))) THEN
C...  If q + qbar -> q + qbar
          if ( igprob(fqaqa(z),gqaqa(z),hqaqa(z),1/36.0D0).ne.0 ) then
 
C     Fast med geometriserade amplituder här **** HÄR ***
              if ( PYR(0)*(Gfn(sz,tz)+Gfn(tz,sz)).lt.Gfn(sz,tz) ) then
C...  s-channel
                 call gcolfl(-1,ipu1,ipu2)
              else
C...  t-channel
                 call gcolfl(1,ipu1,ipu2)
              endif
           
        endif
      ELSE
C...  (q + q' -> q + q') = (q + qbar' -> q + qbar')
          if ( igprob(fqpqp(z),gqpqp(z),hqpqp(z),1/36.0D0).ne.0 )
     $         call gcolfl(1,ipu1,ipu2)
        ENDIF
      ELSEIF ( ISUB.EQ.28 ) THEN
C...  q + g -> q + g
C...  qbar + g -> qbar + g 
        if ( igprob(fqgqg(z),gqgqg(z),hqgqg(z),1/96.0D0).ne.0 )
     $       call gcolfl(1,ipu1,ipu2)
      ELSEIF ( (ISUB.EQ.53).OR.(ISUB.EQ.13) ) THEN
C...  g + g -> q + qbar
C...  or q + qbar -> g + g
        if ( igprob(fggqa(z),gggqa(z),hggqa(z),1/256.0D0).ne.0 )
     $       call gcolfl(-1,ipu1,ipu2)
      ELSEIF ( ISUB.EQ.68 ) THEN
C...  g + g -> g + g
        if ( igprob(fgggg(z),ggggg(z),hgggg(z),1/512.0D0).ne.0 ) then
   
C     Fast med geometriserade amplituder här **** HÄR ***
            xs=16.0d0*(1.0d0+6*z**2+z**4)
            xt=8.0d0*(17.0d0+4.0d0*z+6.0d0*z**2+4.0d0*z**3+z**4)
            xu=8.0d0*(17.0d0-4.0d0*z+6.0d0*z**2-4.0d0*z**3+z**4)
 
          xr=pyr(0)*(xs+xt+xu)
          if ( xr.lt.xs ) then
C...  s-channel
            call gcolfl(-1,ipu1,ipu2)
          elseif ( xr.lt.xs+xt ) then
C...  t-channel
            call gcolfl(1,ipu1,ipu2)
          else
C...  u-channel
            call gcolfl(2,ipu1,ipu2)
          endif
        endif
      ENDIF
      endif

C...Color flow for eikonal grativty, only 't-channel'
      if(SHAT.ge.MS**2) then
         IF ( ISUB.EQ.11 ) THEN
            IF((IFI1.EQ.IFI2).AND.(IFO1.EQ.IFO2).AND.(IFO1.EQ.IFI1))THEN
C...  If q + q -> q + q (all are equal)
               if (pyr(0)*wteik(fqqqq(z),hqqqq(z),1.0D0/72.0D0,1).gt.1 )
     $             call gcolfl(1,ipu1,ipu2)
            ELSEIF((IFI1.EQ.-IFI2).AND.(IFO1.EQ.-IFO2).AND.
     &              (ABS(IFI1).EQ.ABS(IFO1))) THEN
C...  If q + qbar -> q + qbar
               if (pyr(0)*wteik(fqaqa(z),hqaqa(z),1.0D0/36.0D0,1).gt.1)
     $             call gcolfl(1,ipu1,ipu2)
            ELSE
C...  (q + q' -> q + q') = (q + qbar' -> q + qbar')
              if (pyr(0)*wteik(fqpqp(z),hqpqp(z),1.0D0/36.0D0,1).gt.1)
     $              call gcolfl(1,ipu1,ipu2)
            ENDIF
         ELSEIF ( ISUB.EQ.28 ) THEN
C...  q + g -> q + g
C...  qbar + g -> qbar + g 
            if (pyr(0)*wteik(fqgqg(z),hqgqg(z),1.0D0/96.0D0,1).gt.1)
     $           call gcolfl(1,ipu1,ipu2)
         ELSEIF ( ISUB.EQ.68 ) THEN
C...  g + g -> g + g
C...  t-channel
           if ( pyr(0)*wteik(fgggg(z),hgggg(z),1.0D0/512.0D0,1).gt.1 )
     $          call gcolfl(1,ipu1,ipu2)
         ENDIF
      endif
C      write(*,*) 'in GCOLFX: ',PARP(199)
      RETURN
      END


C*********************************************************************
C...IGPROB
C...Used for totgrav or effgrav
C...Routine which calculates the probability that we had
C...a gravity mediated scattering

      INTEGER FUNCTION IGPROB(f,g,h,ks)
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)

C...Some parameters from CHARYBDIS commom block are needed
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM

C...KKMASS to be used for KK-mass maximum integration, and MSAT as Atwoods Ms
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
      DOUBLE PRECISION ks
      DOUBLE COMPLEX xgeof
C...Parameters used during the run, to be printed in the data files, therefore needed by WRIPAR
C      DOUBLE PRECISION PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA
C      INTEGER NEVE,NOINT,NCUT,TUNEA,NOQCD
C...  For chosing clustering algorithm
C      character CLALG*6
C      COMMON/RUNPAR/PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA,
C     $     NEVE,NCUT,TUNEA,NOINT,NOQCD,CLALG
C      SAVE /RUNPAR/,/MASSES/,/BLACKH/,/BHPARM/
       SAVE /MASSES/,/BLACKH/,/BHPARM/
      if ( NOQCD.eq.1 ) then
        IGPROB=1
        return
      endif

      PI=3.141593D0
      n=TOTDIM-4
C...Get shat from Pythia
C      SHAT=PARI(14)
      SHAT=VINT(44)
C...Get alpha_s
C      AS=PYALPS(PARI(22))
      AS=PYALPS(vint(52))
C...Get z
      Z=VINT(23)
      t=-(shat*(1.0d0-z)/2.0d0)


      IGPROB=0

      IF ((TOTDIM-4).LT.2) THEN
        RETURN
      ELSEIF ((TOTDIM-4).EQ.2) THEN
C... I now divide Atwoods F (FF) by 2 to make Atwoods results better Agree with Giudice
C... Starting from Hans (hep-ph/9911350) article I get an expression for D(s) 
C... (equation 4 in hep-ph/9911231) wich is only half as big as that given by Atwood. 
C... This factor 2 I now absorb into F 
        FF=LOG(shat/MSAT**2)
      ELSEIF ((TOTDIM-4).GT.2) THEN
        FF=2.0D0/(TOTDIM-4-2)
      ENDIF

C...  Factor from gaussian cut of in MS&GG
C...ERROR      cutfac=Gammafn((2.0D0 + n)/2.0D0)
      cutfac=Gammafn(dble(n)/2.0D0)
 
C       return här om detta var en vanlig QCD i region 3,4. *** HÄR ***

         QCD=f*AS**2
         call geof(h,ks,1,xgeof)
         gravity=tfac(Ms,t,n)**2*cutfac**2*Abs(xgeof)**2*
     $        16*h*FF**2*shat**4/
     $     (MSAT**(TOTDIM-4+2)*MS**(2-(TOTDIM-4)))**2
         IF ( gravity.LT.(QCD + gravity)*PYR(0) ) RETURN
   

      IGPROB=1

  
      RETURN
      END

C...GCOLFL
C...Rearranges the colour flow of a standard QCD event as if it was
C...mediated by a graviton. IF ITYP=0, there is no reconnection, if
C...ITYP=1(2) we assume t(u)-channel graviton else an s-channel graviton.

      SUBROUTINE GCOLFL(ITYP,IPU1,IPU2)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK
C...Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
C...GRAVEV should be set to 1 to mark the event as gravitational
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)

      IF ( ITYP.EQ.0.OR.N-IPU1.NE.3 ) RETURN

C...GRAVEV is set to 1 to mark the event as gravitational
      GRAVEV=1
      PARP(199)=1.0d0
C      PARP(199)=DBLE(GRAVEV)
C      write (*,*) 'in GCOLFL, GRAVEV: ',GRAVEV,' PARP(199): ',PARP(199)
      
      IF ( ITYP.EQ.1 ) THEN
C...  Same in and out.
         IF ( PYK(IPU1,12).EQ.1.OR.PYK(IPU1,12).EQ.2 ) THEN
            K(IPU1,4)=IPU1+2
            K(IPU1+2,4)=IPU1*10000
         ENDIF
         IF ( PYK(IPU1,12).EQ.-1.OR.PYK(IPU1,12).EQ.2 ) THEN
            K(IPU1,5)=IPU1+2
            K(IPU1+2,5)=IPU1*10000
         ENDIF
        IF ( PYK(IPU2,12).EQ.1.OR.PYK(IPU2,12).EQ.2 ) THEN
          K(IPU2,4)=IPU2+2
          K(IPU2+2,4)=IPU2*10000
        ENDIF
        IF ( PYK(IPU2,12).EQ.-1.OR.PYK(IPU2,12).EQ.2 ) THEN
          K(IPU2,5)=IPU2+2
          K(IPU2+2,5)=IPU2*10000
        ENDIF
      ELSEIF ( ITYP.EQ.2 ) THEN
C...Same in and out.
        IF ( PYK(IPU1,12).EQ.1.OR.PYK(IPU1,12).EQ.2 ) THEN
          K(IPU1,4)=IPU2+2
          K(IPU2+2,4)=IPU1*10000
        ENDIF
        IF ( PYK(IPU1,12).EQ.-1.OR.PYK(IPU1,12).EQ.2 ) THEN
          K(IPU1,5)=IPU2+2
          K(IPU2+2,5)=IPU1*10000
        ENDIF
        IF ( PYK(IPU2,12).EQ.1.OR.PYK(IPU2,12).EQ.2 ) THEN
          K(IPU2,4)=IPU1+2
          K(IPU1+2,4)=IPU2*10000
        ENDIF
        IF ( PYK(IPU2,12).EQ.-1.OR.PYK(IPU2,12).EQ.2 ) THEN
          K(IPU2,5)=IPU1+2
          K(IPU1+2,5)=IPU2*10000
        ENDIF
      ELSE
C...Annihilation.
        IF ( PYK(IPU1,12).EQ.1.OR.PYK(IPU1,12).EQ.2 ) THEN
          K(IPU1,4)=IPU2
          K(IPU2,5)=IPU1
        ENDIF
        IF ( PYK(IPU1,12).EQ.-1.OR.PYK(IPU1,12).EQ.2 ) THEN
          K(IPU1,5)=IPU2
          K(IPU2,4)=IPU1
        ENDIF
        IF ( PYK(IPU1+2,12).EQ.1.OR.PYK(IPU1+2,12).EQ.2 ) THEN
          K(IPU1+2,4)=(IPU2+2)*10000
          K(IPU2+2,5)=(IPU1+2)*10000
        ENDIF
        IF ( PYK(IPU1+2,12).EQ.-1.OR.PYK(IPU1+2,12).EQ.2 ) THEN
          K(IPU1+2,5)=(IPU2+2)*10000
          K(IPU2+2,4)=(IPU1+2)*10000
        ENDIF
          
      ENDIF

      RETURN
      END
C*********************************************************************
C ...init_datapath
C        SUBROUTINE init_datapath(a,s)
C        IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C        IMPLICIT INTEGER(I-N)
C
C      PARAMETER (NMAX=1000)
C      character a*256,path*256 
C      integer s, size 
C        common /eikfnblk/ x(NMAX,6),f(NMAX,6),size,path
C        save eikfnblk
C       size = s
C       path = a
       
C       RETURN
C       END
  
C*********************************************************************
C...  Geometric Amplitude GG&MS eq 5.4
      subroutine geof(h,ks,ip,xgeof)


C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)

C...Some parameters from CHARYBDIS commom block are needed
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM

C...KKMASS to be used for KK-mass maximum integration, and MSAT as Atwoods Ms
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
C...Parameters used during the run, to be printed in the data files, therefore needed by WRIPAR
C      DOUBLE PRECISION PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA
C      INTEGER NEVE,NOINT,NOQCD,NCUT,TUNEA
C...  For chosing clustering algorithm
C      character CLALG*6
C      COMMON/RUNPAR/PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA,
C     $     NEVE,NCUT,TUNEA,NOINT,NOQCD,CLALG
C.. Complex parameters needed to take eikonalization into account
      DOUBLE COMPLEX  X,xgeof
      DOUBLE PRECISION ks,k
C      SAVE /RUNPAR/,/MASSES/,/BLACKH/,/BHPARM/
      SAVE /MASSES/,/BLACKH/,/BHPARM/

      xgeof = (0.0d0,0.0d0)
      PI=3.141593D0
      n=TOTDIM-4
      if(ip.eq.0) then
C...  Get shat from Pythia
         SHAT=PARI(14)
C...  Get z=cos(theta) from Pythia
         Z=PARI(41)
C...  Get alpha_s
         AS=PYALPS(PARI(22))
      else
C...  Get shat from Pythia
         SHAT=VINT(44)
C...  Get z=cos(theta) from Pythia
         Z=VINT(23)
C...  Get alpha_s
         AS=PYALPS(VINT(52))
      endif
      t=-(shat*(1.0d0-z)/2.0d0)
      k=sqrt(-t)

C...  Factor X, cf eq 5.5 in MS GG
C...  t and u-channels are the same, as we effectively have a 
C...  \phi^4 -interaction. We treat s-channel similarly to assure
C...  unitarization
      X=1.0D0/(32.0D0*PI**2)*CMPLX(LOG(4.0d0*Ms**2/shat), PI)
C...  Aborn is the result from Atwood multiplied with a factor from the 
C...  fact that we are using a Gaussian cutoff 
C...  The Gaussian cutoff gives this much more in the Amplitude:
C...ERROR      cutfac=Gammafn((2.0D0 + n)/2.0D0)
      cutfac=Gammafn(dble(n)/2.0D0)

C...  Gravity is universally attractive, and for small arguments,
C.... there is no difference in sign between the channels,
C...  GG and MS has calculated the Born amplitude to be negative
C...  (eq 4.3) this is also the right sign to get an attractive
C...  potential (eq 4.5)

C...  Factor F in Atwoods cross section
      IF ((TOTDIM-4).LT.2) THEN
         RETURN
      ELSEIF ((TOTDIM-4).EQ.2) THEN
C...  I now divide Atwoods F (FF) by 2 to make Atwoods results better Agree with Giudice
C...  Starting from Hans (hep-ph/9911350) article I get an expression for D(s) 
C...  (equation 4 in hep-ph/9911231) wich is only half as big as that given by Atwood. 
C...  This factor 2 I now absorb into F 
         FF=LOG(shat/MSAT**2)
      ELSEIF ((TOTDIM-4).GT.2) THEN
         FF=2.0D0/(TOTDIM-4-2)
      ENDIF

C...  Make Aborn depend on s,t,u to ensure continuous transition
C...  From 060727:2, the (spin-dependent) Amplitude from Atwood using
C...  AtwoodMpCutRescale.nb for conversion factors
      AAtw=-((2**(5.0D0 - n)*FF*Sqrt(h)*Sqrt(ks)*Ms**(-2 + n)*
     $     MPLNCK**(-2.0D0-n)*Pi**(1.0D0 - n/2.0D0)*shat**2)/
     $     Gammafn(n/2.0D0))
C...  With t-dependent, gaussian cutoff this gives  
      Aborn=tfac(Ms,t,n)*cutfac*AAtw
C...  Unitarization of cross section, cf eq 5.5 in MS GG
      xgeof=1/(1-Aborn*X)

      return
      end
C*********************************************************************
C...  Multiplicative factor when t-dependence is taken into account in
C...  Born amplitude Aborn(Ms,t)/Aborn(Ms)
      DOUBLE PRECISION  function tfac(Ms,t,n)


C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      DOUBLE PRECISION k, Ms , Igamma
C...  with A(t,Ms)/A(Ms) in AtwoodMpCutRescale.nb

      k=sqrt(-t)
      tfac=(exp(k**2/Ms**2)*k**(-2 + n)*Ms**(2 - n)*
     $     Gammafn(n/2.0D0)*Igamma(1.0D0 - n/2.0D0,k**2/Ms**2))/
     $     Gammafn(-1.0D0 + n/2.0D0)

      return
      end
C*********************************************************************
C...  Function to tell which region 1,2,3,4 5  cf. GG&MS a 
C...  gravitational scattering was in
      INTEGER function IREG()

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DOUBLE COMPLEX  X,abx

C...  Need MPLNCK
C...  To be used with CHARYBDIS
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT 
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)

C...KKMASS to be used for KK-mass maximum integration, and MSAT as Atwoods Ms
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM
      SAVE /MASSES/,/BLACKH/,/BHPARM/

      PI=3.141593D0
      S=PARI(14)
      n=TOTDIM-4

      IREG=0

C...  Region 1 or 2 
      if(s.gt.Ms**2) then
C...  scd from GS3.nb
         scd=(MPLNCK**(2.0D0 + n)*Pi**(n/2.0D0))/
     $        (Ms**n*Gammafn(dble(n)/2.0D0))
         if(s.gt.scd) then 
            IREG=1 
         else
           IREG=2
         endif
C...  Region 3 or 4 or 5
      else
         X=1.0D0/(32.0D0*PI**2)*CMPLX(LOG(4.0d0*Ms**2/s), PI)
         call getabx(abx)
C...  Region 3
         if (Abs(abx).le.1) then
            IREG=3
C...  Region 4 or 5
         elseif(abs(Imag(X)).gt.abs(dble(X))) then 
            IREG=4 
         else
           IREG=5
         endif
      endif
               
      return
      end
      
C*********************************************************************
C...  Geometric Amplitude GG&MS eq 5.4
      subroutine getabx(abx)


C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)

C...Some parameters from CHARYBDIS commom block are needed
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM

C...KKMASS to be used for KK-mass maximum integration, and MSAT as Atwoods Ms
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
C...Parameters used during the run, to be printed in the data files, therefore needed by WRIPAR
C      DOUBLE PRECISION PTCL,THRMIN,PSCUT,ETCOND,UBMUL,WTPA
C      INTEGER NEVE,NOINT,NOQCD,NCUT,TUNEA
C.. Complex parameters needed to take eikonalization into account
      DOUBLE COMPLEX  X,abx
      DOUBLE PRECISION ks,k
      SAVE /MASSES/,/BLACKH/,/BHPARM/

C...Inline function definitions
      include 'inlines.f'

      abx = (0.0d0,0.0d0)
      PI=3.141593D0
      n=TOTDIM-4
C...  Get shat from Pythia
      SHAT=PARI(14)
C...  Get z=cos(theta) from Pythia
      Z=PARI(41)
C...  Get alpha_s
      t=-(shat*(1.0d0-z)/2.0d0)
      k=sqrt(-t)

      isub=MSTI(1)

      IF ( ISUB.EQ.12 ) THEN
C...  q + qbar -> q' + qbar'
        h=hqapb(z)
        ks=1/36.0D0
      ELSEIF ( ISUB.EQ.11 ) THEN
        IF((IFI1.EQ.IFI2).AND.(IFO1.EQ.IFO2).AND.(IFO1.EQ.IFI1))THEN
C...  If q + q -> q + q (all are equal)
          h=hqqqq(z)
          ks=1/72.0D0 
        ELSEIF((IFI1.EQ.-IFI2).AND.(IFO1.EQ.-IFO2).AND.
     &         (ABS(IFI1).EQ.ABS(IFO1))) THEN
C...  If q + qbar -> q + qbar
          h=hqaqa(z)
          ks=1/36.0D0
        ELSE
C...  (q + q' -> q + q') = (q + qbar' -> q + qbar')
          h=hqpqp(z)
          ks=1/36.0D0
        ENDIF
      ELSEIF ( ISUB.EQ.28 ) THEN
C...  q + g -> q + g
C...  qbar + g -> qbar + g 
        h=hqgqg(z)
        ks=1/96.0D0
      ELSEIF ( (ISUB.EQ.53).OR.(ISUB.EQ.13) ) THEN
C...  g + g -> q + qbar
C...  or q + qbar -> g + g
        h=hggqa(z)
        ks=1/256.0D0
      ELSEIF ( ISUB.EQ.68 ) THEN
C...  g + g -> g + g
        h=hgggg(z)
        ks=1/512.0D0
      ENDIF

C...  Factor X, cf eq 5.5 in MS GG
C...  t and u-channels are the same, as we effectively have a 
C...  \phi^4 -interaction. We treat s-channel similarly to assure
C...  unitarization
      X=1.0D0/(32.0D0*PI**2)*CMPLX(LOG(4.0d0*Ms**2/shat), PI)
C...  Aborn is the result from Atwood multiplied with a factor from the 
C...  fact that we are using a Gaussian cutoff 
C...  The Gaussian cutoff gives this much more in the Amplitude:
C...ERROR      cutfac=Gammafn((2.0D0 + n)/2.0D0)
      cutfac=Gammafn(dble(n)/2.0D0)

C...  Gravity is universally attractive, and for small arguments,
C.... there is no difference in sign between the channels,
C...  GG and MS has calculated the Born amplitude to be negative
C...  (eq 4.3) this is also the right sign to get an attractive
C...  potential (eq 4.5)

C...  Factor F in Atwoods cross section
      IF ((TOTDIM-4).LT.2) THEN
         RETURN
      ELSEIF ((TOTDIM-4).EQ.2) THEN
C...  I now divide Atwoods F (FF) by 2 to make Atwoods results better Agree with Giudice
C...  Starting from Hans (hep-ph/9911350) article I get an expression for D(s) 
C...  (equation 4 in hep-ph/9911231) wich is only half as big as that given by Atwood. 
C...  This factor 2 I now absorb into F 
         FF=LOG(shat/MSAT**2)
      ELSEIF ((TOTDIM-4).GT.2) THEN
         FF=2.0D0/(TOTDIM-4-2)
      ENDIF

C...  Make Aborn depend on s,t,u to ensure continuous transition
C...  From 060727:2, the (spin-dependent) Amplitude from Atwood using
C...  AtwoodMpCutRescale.nb for conversion factors
      AAtw=-((2**(5.0D0 - n)*FF*Sqrt(h)*Sqrt(ks)*Ms**(-2 + n)*
     $     MPLNCK**(-2.0D0-n)*Pi**(1.0D0 - n/2.0D0)*shat**2)/
     $     Gammafn(n/2.0D0))
C...  With t-dependent, gaussian cutoff this gives  
      Aborn=tfac(Ms,t,n)*cutfac*AAtw
C...  Unitarization of cross section, cf eq 5.5 in MS GG
      abx=Aborn*X

      return
      end
