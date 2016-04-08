      Subroutine USEGRAV

C     Authors: Pierre-Hugues Beauchemin<pierre-hugues.beauchemin@cern.ch>
C              Georges Azuelos<georges.azuelos@umontreal.ca>
C              


C     Compute graviton production cross section and generate events
C     --------------------------------------------------------------

C     Note: processes = parton + parton -> jet + Graviton, according to eq. (64-66) of hep-ph/9811291



C...Double precision and integer declarations.

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)

c      INTEGER PYK,PYCHGE,PYCOMP
C...PYTHIA Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYDAT4/CHAF(500,2)
      CHARACTER CHAF*16
      COMMON/PYDATR/MRPY(6),RRPY(100)
      COMMON/PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT3/XSFX(2,-40:40),ISIG(1000,3),SIGH(1000)
      COMMON/PYINT4/MWID(500),WIDS(500,5)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYINT6/PROC(0:500)
      CHARACTER PROC*28
      COMMON/PYINT7/SIGT(0:6,0:6,0:5)
      COMMON/PYMSSM/IMSS(0:99),RMSS(0:99)
      COMMON/PYSSMT/ZMIX(4,4),UMIX(2,2),VMIX(2,2),SMZ(4),SMW(2),
     &SFMIX(16,4),ZMIXI(4,4),UMIXI(2,2),VMIXI(2,2)
      COMMON/PYMSRV/RVLAM(3,3,3), RVLAMP(3,3,3), RVLAMB(3,3,3)
      COMMON/PYBINS/IHIST(4),INDX(1000),BIN(20000)
      SAVE /PYDAT1/,/PYDAT2/,/PYDAT3/,/PYDAT4/,/PYDATR/,/PYSUBS/,
     &/PYPARS/,/PYINT1/,/PYINT2/,/PYINT3/,/PYINT4/,/PYINT5/,
     &/PYINT6/,/PYINT7/,/PYMSSM/,/PYSSMT/,/PYMSRV/,/PYBINS/

C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500) 
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
c      DOUBLE PRECISION XWGTUPALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP 
      DOUBLE PRECISION AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP  
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)  
      SAVE /HEPEUP/   

C.. user
      COMMON/USER/SIGMAX, ECM, sigev2, sqrshat
      common /user2/sigtot, ebeam, amgravmin, amgravmax,
     +              ptcut, shatmin, xmin, shatmax, tmin, tmax,
     +              phspv, amd, coupl, ccoupl, g, g5, ndim, ntot, 
     +              iproc, igrav, iq, icutoff
      save /user2/
      dimension xp1(-25:25),xp2(-25:25), sigi(-5:5,-5:5)
      COMMON /USER3/ PT2, Q2, shat, that, uhat, sig0, wgt1, wgt2,
     +               conv, xp1, xp2, sigi, amgrav, amgrav2, costhe,
     +               F1, F2, F3

C.. conversion factor GeV**(-2) --> pb ( = hbarc **2)
      logical ifirst
      DATA CONV/0.389D9/, IFIRST/.true./

C.. first pass? ==> xtraD parameters
      if(ifirst)then
          ifirst=.false.
          Ebeam=ECM/2.


C  Note: sqrt(shat) is min, when p = pTcut
          sqrshatmin=ptcut+sqrt(ptcut**2+amgravmin**2)
          shatmin = max(sqrshatmin**2,AMGRAVmin**2)
          Xmin=shatmin/ECM**2
          shatmax=ECM**2

C.....   phase space volume x1-x2 space
          PHSPV=LOG(XMIN)**2/2.  
      end if
      sigev=0.
      sigev2=sigev

C.. sample ln(x1), ln(x2) = fraction of quark momentum in proton
 20   X1=XMIN**PYR(0)
      X2=XMIN**PYR(0)
      SHAT=X1*X2*ECM**2
      sqrshat=sqrt(shat)
      if(shat.lt.shatmin)go to 20

C.. sample  M**2
C   Note: exclude M1**2 < M**2 
      am1 = shat-2*ptcut*sqrshat
      ammin2=amgravmin**2
      if(am1.lt.ammin2)then
         wgt1=0.
         go to 200
      end if
      ammax2=min(am1,amgravmax**2)
C the above definition of ammax,ensures the allowed kinematics.
      AMGRAV2=ammin2+(ammax2-ammin2)*PYR(0)
C this gives the KK mode,a mass between ammin & ammax.
      AMGRAV=sqrt(AMGRAV2)
      wgt1=ammax2-ammin2

C.. sample t_hat and compute weight
      t1=shat-amgrav2
      determ = t1**2-4.*shat*ptcut**2
      if(determ.lt.0.)then
         write(6,*) ' >>>>> determ < 0 : determ =', determ
         go to 20
      end if
      determ=dsqrt(determ)
      th1 = (-t1-determ)/2.
      th2 = (-t1+determ)/2.
      that = th1 * (th2/th1)**pyr(0)
      wgt2 = (-that) * log(th1/th2)

C shat + that + uhat = M^2
      UHAT=-SHAT-THAT+AMGRAV**2 

C...Subsystem scattering angle in parton cm, defined neglecting quark mass.
      costhe=(that-uhat)/(that+uhat)
      SINTHE=SQRT(MAX(0D0,1D0-COSTHE**2))
      phi=2*paru(1)*PYR(0)

      PT2=THAT*UHAT/SHAT
      Q2=amgrav**2/2+PT2

C...  Parton distributions (multiplied by x) of the protons
      CALL PYPDFU(2212,X1,Q2,XP1)
      CALL PYPDFU(2212,X2,Q2,XP2)



C.--------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
C.. Get cross sections and generate events
C
C     Note: three processes: qqbar->gG, qg->qG, gg->gG
C.--------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

      if(iproc.eq.1110)then
        call qqbar_gGrav
      else if(iproc.eq.1111)then
        call qg_qGrav
      else if(iproc.eq.1112)then
        call gg_gGrav
      else
         write(6,*) ' **** no process **** '
         stop
      end if

C.. particle momenta, after boosting to lab system
      pup(1,1)=0.
      pup(2,1)=0.
      pup(4,1)=x1*ECM/2.d0
      pup(5,1)=pmas(iabs(idup(1)),1)
      pup(3,1)=dsqrt(pup(4,1)**2-pup(5,1)**2)

      pup(1,2)=0.
      pup(2,2)=0.
      pup(4,2)=x2*ECM/2.d0
      pup(5,2)=pmas(iabs(idup(2)),1)
      pup(3,2)=-dsqrt(pup(4,2)**2-pup(5,2)**2)

C.. particle igrav = Graviton; iq = parton( quark or gluon )
C.....velocity of qq c.m. system
C......... gamma = E(qq)/M(qq)
      gamma=(x1+x2)*ECM/(2.d0*sqrshat)
      beta=dsqrt(1.d0-1.d0/gamma**2)
      if(x1.lt.x2)beta=-beta

C..... in cm of qq system, energy and momentum of Grav is:
      if(igrav.eq.4)costhe=-costhe
      e3cm=(shat+amgrav2)/(2.*sqrshat)
      p3cm=dsqrt(e3cm**2-amgrav2)
      p3tcm=p3cm*sinthe
      p3zcm=p3cm*costhe
      pup(3,igrav)=gamma*(p3zcm+beta*e3cm)
      pup(1,igrav)=p3tcm*cos(phi)
      pup(2,igrav)=p3tcm*sin(phi)
      pup(5,igrav)=amgrav
      pup(4,igrav)=dsqrt(
     +    (pup(1,igrav)**2+pup(2,igrav)**2+pup(3,igrav)**2
     +   + pup(5,igrav)**2) )
      e3lab=gamma*(e3cm+beta*p3zcm)

C.. particle iq = light quark or gluon
c      print*, "iq ",iq
c      print*,'idup(iq)',idup(iq)
      e4cm=(shat-amgrav**2)/(2.*sqrshat)
      pup(1,iq)=-pup(1,igrav)
      pup(2,iq)=-pup(2,igrav)
      pup(3,iq)=gamma*(-p3zcm+beta*e4cm)
c      write(6,*) 'PtJet:', dsqrt( pup(1,igrav)**2 + pup(2,igrav)**2 ) 
      pup(5,iq)=pmas(iabs(idup(iq)),1)
      pup(4,iq)=dsqrt( 
     +      (pup(1,iq)**2+pup(2,iq)**2+pup(3,iq)**2+pup(5,iq)**2))
      e4lab=gamma*(e4cm-beta*p3zcm)

C.. lifetime and spin
      do i=1,4
        VTIMUP(i) = 0.
        SPINUP(I) = 0.
      end do



C.. Get weight 

      ntot = ntot+1
      sigtot=sigtot+sigev2
      return
 200  SIGEV=0.
      xwgtup=sigev
      sigev2=sigev
      sigtot=sigtot+sigev
      ntot=ntot+1


      RETURN
      end subroutine

C ========================================================



      subroutine qqbar_gGrav
C==================================================
C  process 1110: q + qbar -> Grav g
C
C     Note: Compute cross section and define color flow
C==================================================

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)

C...User process event common block.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/

      INTEGER MAXNUP
      PARAMETER (MAXNUP=500) 
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP  
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)  
      SAVE /HEPEUP/   
 
C.. user
      COMMON/USER/SIGMAX, ECM, sigev2, sqrshat
      common /user2/sigtot, ebeam, amgravmin, amgravmax,
     +              ptcut, shatmin, xmin, shatmax, tmin, tmax,
     +              phspv, amd, coupl, ccoupl, g, g5, ndim, ntot, 
     +              iproc, igrav, iq, icutoff
      save /user2/
      dimension xp1(-25:25),xp2(-25:25), sigi(-5:5,-5:5)
      COMMON /USER3/ PT2, Q2, shat, that, uhat, sig0, wgt1, wgt2,
     +               conv, xp1, xp2, sigi, amgrav, amgrav2, costhe,
     +               F1, F2, F3


      data pi/3.141592654/
c   , twopi/6.2832/

C  process 1110: q + g -> Grav g
      IDPRUP = 1110

C.. alpha(em) and alpha_s
c      AEM=PYALEM(Q2)
      ALPS=PYALPS(Q2)



C.. calculate sig0

C compute the unit sphere surface in n extra-d:

        if (ndim.eq.1) then
           omega=2.
        else if (ndim.eq.2) then
           omega=2.*pi
        else if (ndim.eq.3) then
           omega=4.*pi
        else if (ndim.eq.4) then
           omega=2.*pi**2
        else if (ndim.eq.5) then
           omega=(8.*pi**2)/3.
        else if (ndim.eq.6) then
           omega=pi**3
        else if (ndim.ge.7) then
           omega=0.
        end if

C     Constants 
C     {see eq.60 & 64}

      const1 = alps/(36.*shat)
      const2 = amgrav2**((float(ndim)-2.)/2.)/2.
C  ( dm^2 is considered instead of dm. )
      const3 = omega
      const4 = 1/(amd**(ndim+2))


C     Compute the function F1(x,y)

      x=that/shat
      y=(amgrav**2)/shat

      term1 = 1/(x*(y-1-x))
      term2 = (-4)*x*(1+x)*(1+2*x+2*x**2)
      term3 = y*(1+6*x+18*x**2+16*x**3)
      term4 = (-6)*(y**2)*x*(1+2*x)
      term5 = (y**3)*(1+4*x)

      F1= (term2+term3+term4+term5)*term1
      F2=0.
      F3=0.


C     differential cross section(see eq.60)
      
      sig0=const1*const2*const3*const4*F1

C.--------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


      sigev=0.
      sigi(0,0)=0.
      do i1=1,5
         i2=-i1
         sigi(i1,i2)=sig0*xp1(i1)*xp2(i2)
         sigi(i2,i1)=sig0*xp1(i2)*xp2(i1)
         sigev=sigev+sigi(i1,i2)+sigi(i2,i1)
      end do
 
      sigtp=0.
      random=pyr(0)
      do i1=-5,5
         i2=-i1
         sigtp=sigtp+sigi(i1,i2)
         if((sigtp/sigev).gt.random)then
            ia=i1
            ib=i2
            go to 30
         end if
      end do
 30   continue
      sigev=sigev * wgt1 * wgt2 * conv * phspv
      sigev2=sigev

C weight, in pb!
      if (icutoff.eq.0) then
        XWGTUP = sigev
      else if (icutoff.eq.1 .AND. shat.le.aMD**2) then
        XWGTUP = sigev
      else if (icutoff.eq.1 .AND. shat.gt.aMD**2) then
        XWGTUP = sigev * (aMD**4/shat**2)  
      else if (icutoff.eq.2 .AND. shat.le.aMD**2) then
        XWGTUP = sigev
      else if (icutoff.eq.2 .AND. shat.gt.aMD**2) then
        XWGTUP = 0
      end if
 

C.. fill event
      NUP=4
      SCALUP=q2
      AQEDUP=-1.
      AQCDUP=-1.

      
C.. particle ID's
      igrav=3
      iq=4
      IDUP(1)=ia
      IDUP(2)=ib
      IDUP(3)=39
      IDUP(4)=21
      
C.. status codes ( -1:an incoming particle of the hard scattering process,
C                   1:an outgoing final state particle)
      ISTUP(1)=-1
      ISTUP(2)=-1
      ISTUP(3)=1
      ISTUP(4)=1


C.. MOTHUP(1,i),MOTHUP(2,i):positions of first and last mother of particle i.
      do i=3,4
         MOTHUP(1,i)=i-2
         MOTHUP(2,i)=0
      end do

C..   colour tags( ICOLUP(1,i):the colour of particle i
C                  ICOLUP(2,i):the colour of anti-particle i)   

      if(ia .gt.0)then
         ICOLUP(1,1)=101
         ICOLUP(2,1)=0
         ICOLUP(1,2)=0
         ICOLUP(2,2)=102
      else
         ICOLUP(1,1)=0
         ICOLUP(2,1)=102
         ICOLUP(1,2)=101
         ICOLUP(2,2)=0
      end if
      ICOLUP(1,4)=101
      ICOLUP(2,4)=102


      return
      end subroutine

C ========================================================

      subroutine qg_qGrav
C==================================================
C  process 1111: q + g -> Grav q
C
C     Note: Compute cross section and define color flow
C==================================================

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
C...User process event common block.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/

      INTEGER MAXNUP
      PARAMETER (MAXNUP=500) 
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP  
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)  
      SAVE /HEPEUP/   
 
C.. user
      COMMON/USER/SIGMAX, ECM, sigev2, sqrshat
      common /user2/sigtot, ebeam, amgravmin, amgravmax,
     +              ptcut, shatmin, xmin, shatmax, tmin, tmax,
     +              phspv, amd, coupl, ccoupl, g, g5, ndim, ntot, 
     +              iproc, igrav, iq, icutoff
      save /user2/
      dimension xp1(-25:25),xp2(-25:25), sigi(-5:5,-5:5)
      COMMON /USER3/ PT2, Q2, shat, that, uhat, sig0, wgt1, wgt2,
     +               conv, xp1, xp2, sigi, amgrav, amgrav2, costhe,
     +               F1, F2, F3



      data pi/3.141592654/
c  , twopi/6.2832/

C  process 1111: q + g -> Grav q
      IDPRUP = 1111

C.. alpha(em) and alpha_s
c      AEM=PYALEM(Q2)
      ALPS=PYALPS(Q2)


C.. calculate sig0

C compute the unit sphere surface in n extra-d:

        if (ndim.eq.1) then
           omega=2.
        else if (ndim.eq.2) then
           omega=2.*pi
        else if (ndim.eq.3) then
           omega=4.*pi
        else if (ndim.eq.4) then
           omega=2.*pi**2
        else if (ndim.eq.5) then
           omega=(8.*pi**2)/3.
        else if (ndim.eq.6) then
           omega=pi**3
        else if (ndim.ge.7) then
           omega=0.
        end if

C     Constants

      const1 = alps/(96.*shat)
      const2 = amgrav2**((float(ndim)-2.)/2.)/2.
      const3 = omega
      const4 = 1/(amd**(ndim+2))


C     Compute the function F2(x,y)

      x=that/shat
      y=(amgrav**2)/shat

      term1 = 1/(x*(y-1-x))
      term2 = (-4)*x*(1+x**2)
      term3 = y*(1+x)*(1+8*x+x**2)
      term4 = (-3)*(y**2)*(1+4*x+x**2)
      term5 = 4*(y**3)*(1+x)
      term6 = (-2)*(y**4)
 
      F2 = (term2+term3+term4+term5+term6)*term1
      F1=0.
      F3=0.

C     differential cross section(eq 60):
      
      sig0=const1*const2*const3*const4*F2

C.--------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


C.. different cases of quark-gluon scattering
      sigev=0.
      sigi(0,0)=0.
      do i1=-5,5
         if(i1.ne.0)then
            i2=21
            sigi(i1,0)=sig0*xp1(i1)*xp2(i2)
            sigi(0,i1)=sig0*xp1(i2)*xp2(i1)
            sigev=sigev+sigi(i1,0)+sigi(0,i1)
         end if
      end do

      sigtp=0.
      random=pyr(0)
      do i1=-5,5
         if(i1.ne.0)then
            i2=21
            sigtp=sigtp+sigi(i1,0)
            if((sigtp/sigev).gt.random)then
               ia=i1
               ib=i2
               go to 30
            end if
            sigtp=sigtp+sigi(0,i1)
            if((sigtp/sigev).gt.random)then
               ia=i2
               ib=i1
               go to 30
            end if
         end if
      end do
 30   continue

        sigev=sigev * wgt1 * wgt2 * conv * phspv
        sigev2=sigev

C weight, in pb!
         
      if (icutoff.eq.0) then
        XWGTUP = sigev
      else if (icutoff.eq.1 .AND. shat.le.aMD**2) then
        XWGTUP = sigev
      else if (icutoff.eq.1 .AND. shat.gt.aMD**2) then
        XWGTUP = sigev * (aMD**4/shat**2)  
      else if (icutoff.eq.2 .AND. shat.le.aMD**2) then
        XWGTUP = sigev
      else if (icutoff.eq.2 .AND. shat.gt.aMD**2) then
        XWGTUP = 0 
      end if


C.. fill event
      NUP=4
      SCALUP=q2
      AQEDUP=-1.
      AQCDUP=-1.

C.. particle ID's
c   Note:    here, "that" is (p_q - p_G)**2, so put 
c            quark in position 1 and G in position 3
c        OR  quark in position 2 and G in position 4

      IDUP(1)=ia
      IDUP(2)=ib
      if(ia.eq.21)then
         igrav=4
         iq=3
      else
         igrav=3
         iq=4
      end if
      IDUP(igrav)=39
      IDUP(iq)=min(ia,ib)

C.. status codes
      ISTUP(1)=-1
      ISTUP(2)=-1
      ISTUP(3)=1
      ISTUP(4)=1

C.. positions of first and last mother
      do i=3,4
         MOTHUP(1,i)=0
         MOTHUP(2,i)=0
      end do

C.. colour tags
      do i=1,4
         icolup(1,i)=0
         icolup(2,i)=0
      end do
      if(ia.eq.21)then
         icolup(1,1)=101
         icolup(2,1)=102
         if(ib.gt.0)then
            icolup(1,2)=102
            icolup(2,2)=0
            icolup(1,iq)=101
            icolup(2,iq)=0
         else
            icolup(1,2)=0
            icolup(2,2)=101
            icolup(1,iq)=0
            icolup(2,iq)=102
         end if
      else
         icolup(1,2)=101
         icolup(2,2)=102
         if(ia.gt.0)then
            icolup(1,1)=102
            icolup(2,1)=0
            icolup(1,iq)=101
            icolup(2,iq)=0
         else
            icolup(1,1)=0
            icolup(2,1)=101
            icolup(1,iq)=0
            icolup(2,iq)=102
         end if
      end if
      return
      end subroutine


C ========================================================


      subroutine gg_gGrav

C==================================================
C  process 1112: g + g -> Grav g
C
C     Note: Compute cross section and define color flow
C==================================================

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
C...User process event common block.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/

      INTEGER MAXNUP
      PARAMETER (MAXNUP=500) 
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP  
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)  
      SAVE /HEPEUP/   
 
C.. user
      COMMON/USER/SIGMAX, ECM, sigev2, sqrshat
      common /user2/sigtot, ebeam, amgravmin, amgravmax,
     +              ptcut, shatmin, xmin, shatmax, tmin, tmax,
     +              phspv, amd, coupl, ccoupl, g, g5, ndim, ntot, 
     +              iproc, igrav, iq, icutoff
      save /user2/
      dimension xp1(-25:25),xp2(-25:25), sigi(-5:5,-5:5)
      COMMON /USER3/ PT2, Q2, shat, that, uhat, sig0, wgt1, wgt2,
     +               conv, xp1, xp2, sigi, amgrav, amgrav2, costhe,
     +               F1, F2, F3



      data pi/3.141592654/
c  , twopi/6.2832/

C  process 1112: g + g -> Grav g
      IDPRUP = 1112

C.. alpha(em) and alpha_s
C      AEM=PYALEM(Q2)
      ALPS=PYALPS(Q2)


C.. calculate sig0

C compute the unit sphere surface in n extra-d:

        if (ndim.eq.1) then
           omega=2.
        else if (ndim.eq.2) then
           omega=2.*pi
        else if (ndim.eq.3) then
           omega=4.*pi
        else if (ndim.eq.4) then
           omega=2.*pi**2
        else if (ndim.eq.5) then
           omega=(8.*pi**2)/3.
        else if (ndim.eq.6) then
           omega=pi**3
        else if (ndim.ge.7) then
           omega=0.
        end if

C     Constants

      const1 = 3*alps/(16.*shat)
      const2 = amgrav2**((float(ndim)-2.)/2.)/2.
      const3 = omega
      const4 = 1/(amd**(ndim+2))


C     Compute the function F3(x,y) (see eq.92)

      x=that/shat
      y=(amgrav**2)/shat


      term1 = 1/(x*(y-1-x))

      term2 = 1+2*x+3*x**2+2*x**3+x**4
      term3 = (-2)*y*(1+x**3)
      term4 = 3*(y**2)*(1+x**2)
      term5 = (-2)*(y**3)*(1+x)
      term6 = y**4

      F3= (term2+term3+term4+term5+term6)*term1
      F1=0.
      F2=0.

c      write(6,*) F1, F2, F3


C     differential cross section
      
      sig0=const1*const2*const3*const4*F3

C.--------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


C.. different cases of parton-antiparton scattering
      sigev=0.
      sigi(0,0)=0.
      i1=21
      i2=21
      sigev=sig0*xp1(i1)*xp2(i2)
 
      ia=i1
      ib=i2
        sigev=sigev * wgt1 * wgt2 * conv * phspv
        sigev2=sigev

C weight, in pb!
         
      if (icutoff.eq.0) then
        XWGTUP = sigev
      else if (icutoff.eq.1 .AND. shat.le.aMD**2) then
        XWGTUP = sigev
      else if (icutoff.eq.1 .AND. shat.gt.aMD**2) then
        XWGTUP = sigev * (aMD**4/shat**2)  
      else if (icutoff.eq.2 .AND. shat.le.aMD**2) then
        XWGTUP = sigev
      else if (icutoff.eq.2 .AND. shat.gt.aMD**2) then
        XWGTUP = 0 
      end if


C.. fill event
      NUP=4
      SCALUP=q2
      AQEDUP=-1.
      AQCDUP=-1.

C.. particle ID's
      igrav=3
      iq=4
      IDUP(1)=ia
      IDUP(2)=ib
      IDUP(3)=39
      IDUP(4)=21

C.. status codes
      ISTUP(1)=-1
      ISTUP(2)=-1
      ISTUP(3)=1
      ISTUP(4)=1

C.. positions of first and last mother
      do i=3,4
         MOTHUP(1,i)=i-2
         MOTHUP(2,i)=0
      end do

C.. colour tags
      icolup(1,1)=101
      icolup(2,1)=102
      icolup(1,2)=102
      icolup(2,2)=103
      icolup(1,4)=101
      icolup(2,4)=103
      return
      end subroutine
