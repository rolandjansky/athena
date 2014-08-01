*CMZ :          
*-- Author :    James catmore 2006-04-09
      SUBROUTINE uf_BdAnglesCDF(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : uf_BdANgles                                           *
*        (module)                                                      *
*                                                                      *
* Description : User subroutine to define  requirements for event      *
*                 accepting/rejecting additional or                    *
*                 other that in subroutine finsel                      *
*                 for BdJpsi mumu K0* Kpi events 
*                 INCLUDING DECAY ANGLES
*                 additional settings from cufdcp(10) array            *
* Arguments   :  IRET (in/out) accepting(0)/rejecting(1) event         *
*                                                                      *
*                                                                      *
* Author : James Catmore                Date : 2006-04-09              *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
*+CDE, TYPING.
C ----------- end CDE --------------------------------------------------
*
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      double precision par(8), mup(4),mum(4),kp(4),km(4),x(4),bs(4)
c Old angles commented out
c      double precision ct1,ct2,phi,prob1,prob2,R
      double precision ct,cp,phi,prob1,prob2,R
      double precision l1muon,l2muon,muetacut,hadptcut,hadetacut,
     +                 BdPtCut,BdEtaCut
      double precision mu1pt,eta,pt
      integer mupg,mumg,kpg,kmg,J
c      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,LUDAT1.
       COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
*KEEP,LUDAT2.
       COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
*KEEP,PYPARS.
       COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
*KEEP,PYINT1.
       COMMON/PYINT1/MINT(400),VINT(400)
*KEEP,PYINT2.
       COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
*KEEP,PYINT5.
       COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
*KEEP,PYUPPR.
       COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)

*     user finsel decay selection and decay parameters
       common /ufdecay/ cufdcp(10),iufdc
       save   /ufdecay/

*KEND.
*      SAVE /LUJETS/,/LUDAT1/,/LUDAT2/
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEND.
c      SAVE IFIRST,IIPRNT
      SAVE IFIRST
      DATA IFIRST /1/
c      DATA IIPRNT /0/

*KEEP,FAMILY.
       name_self(I)=K(I,2)
       name_selv(I)=abs(K(I,2))
       name_father(I)=K(K(I,3),2)
       name_grandpa(I)=K(K(K(I,3),3),2)
       name_daughter1(I)=K(K(I,4),2)
       name_daughter2(I)=K(K(I,5),2)

       link_self(I)=I
       link_father(I)=K(I,3)
       link_grandpa(I)=K(K(I,3),3)
       link_daughter1(I)=K(I,4)
       link_daughter2(I)=K(I,5)
*KEND.
C ----------- end CDE --------------------------------------------------

       idebug=0

       IF(IFIRST.EQ.1) THEN
          IFIRST =0
       ENDIF

c     return immediately if already rejected
      if ( iret.eq.1 ) return

c
c Add your decay selection here.
c Set iret to 0 (acception) or 1 (rejection) per event.
c 
c Parameters: 0 (off) / 1 (on)
c    cufdcp( 1) : bool: flat (0) or physics angles (1)
c    cufdcp( 2) : muon1 pT cut (LVL1)
c    cufdcp( 3) : muon2 pT cut (LVL2)
c    cufdcp( 4) : muons eta cut
c    cufdcp( 5) : kaons pT cut
c    cufdcp( 6) : kaons eta cut
c    cufdcp( 7) : Bd pT cut
c    cufdcp( 8) : Bd eta cut
c    cufdcp( 9) : Bd lifetime (if zero, extract it from Pythia common blocks)
c    cufdcp(10) : 


c     some counting
      nintree = 0

c     preset
      iret = 1

      if(idebug.eq.1)
     +   print *,'USER_FINSEL: BEGINNING v3'

c     particle loop  -- modify content as needed
c      do  I=1,N
c         
c     if(tree(I).eq.1) then
c
c  accept only Phi KK decays -- code by James Catmore
c
c         if(name_selv(I).eq.333.and.
c     +name_father(I).eq.531.and.
c     +abs(name_daughter1(I)).eq.321.and.
c     +abs(name_daughter2(I)).eq.321) then
c            if(idebug.eq.1) print *,'USER_FINSEL: ',name_self(I)
c            iret=0
c         endif
c     endif
c         
c      enddo

c     evaluation
      
c 1000 continue
c      if(iret.ne.0) then
c     debug output
c         if(idebug.eq.1)
c     +        print *, 'USER_FINSEL: event rejected'
c      endif


c **********************************************************************
c
c Code to simulate CP violation
c
c **********************************************************************

c Pythia Parameters
c Bd Lifetime from Pythia
      tau = 0.
      if ( cufdcp(9).le.0.0 ) then
c For Bs lifetime
c        tau = pmas(140,4)/0.299792458
c For Bd lifetime
        tau = pmas(134,4)/0.299792458
        if(idebug.eq.1) print*, 'Bd lifetime extracted: ', tau*0.2997924
      else
        tau = cufdcp(9)/0.299792458
      endif
c Set Physics Parameters
c A0
      par(1) = 0.7661
c Al
      par(2) = 0.5019 
c deltaG
      par(3) = 0.0
c GammaS
      par(4) = 0.659
c Delta M
      par(5) = 0.507 
c phiS
      par(6) = 0.0
c delta1
      par(7) = 0.15
c delta2
      par(8) = 3.02

c Bd kinematic cuts
      muetacut = cufdcp(4)  
      l1muon = cufdcp(2)  
      l2muon = cufdcp(3)  
      hadptcut = cufdcp(5) 
      hadetacut = cufdcp(6)
      BdPtCut = cufdcp(7) 
      BdEtaCut = cufdcp(8)
c **********************************************************************

c Make sure the event is not accepted by lucky initialization
      mupg = -1
      mumg = -2
      kpg  = -3
      kmg  = -4
c Search for the Bd -> J/psi K0* decay
      do  I=1,N
      if(name_self(I).eq.511.and.(name_daughter1(I).eq.443.and.
     +name_daughter2(I).eq.313.or.name_daughter1(I).eq.313.and.
     +name_daughter2(I).eq.443)) then
       do J=1,4
       bs(J) = P(I,J)
       enddo
       continue
      endif
      if(name_self(I).eq.-13.and.name_father(I).eq.443.and.
     +name_grandpa(I).eq.511) then
         mupg = link_grandpa(I)
         do J=1,4
         mup(J) = P(I,J)
         enddo
         continue
      endif
      if(name_self(I).eq.13.and.name_father(I).eq.443.and.
     +name_grandpa(I).eq.511) then
         mumg = link_grandpa(I)
         do J=1,4
         mum(J) = P(I,J)
         enddo
         continue
      endif
      if(name_self(I).eq.321.and.name_father(I).eq.313.and.
     +name_grandpa(I).eq.511) then
         kpg = link_grandpa(I)
         do J=1,4
         kp(J) = P(I,J)
         enddo
         continue
      endif
      if(name_self(I).eq.-211.and.name_father(I).eq.313.and.
     +name_grandpa(I).eq.511) then
         kmg = link_grandpa(I)
         do J=1,4
         km(J) = P(I,J)
         enddo
         continue
      endif
      enddo
c Check that all the particles come from the same Bd
      if(mupg.ne.mumg.or.mupg.ne.kpg.or.mupg.ne.kmg) then
         if(idebug.eq.1) print*,"Muon Plus Grandpa = ",mupg
         if(idebug.eq.1) print*,"Muon Minus Grandpa = ",mumg
         if(idebug.eq.1) print*,"Kaon Plus Grandpa = ",kpg
         if(idebug.eq.1) print*,"Pion Minus Grandpa = ",kmg
         if(idebug.eq.1) print*,"Signal decay not found."
         iret = 1
         return
      endif

      if(idebug.eq.1) print*,"Signal decay was found!"

c      return here for flat angles

      if ( cufdcp(1).ne.0.0 ) then

c If a Bd -> J/psi(mu mu) phi(K K) event has been found
c      print*,"mu+ = (",mup(1),", ",mup(2)," ,",mup(3),", ",mup(4),")"
c      print*,"mu- = (",mum(1),", ",mum(2)," ,",mum(3),", ",mum(4),")"
c      print*,"K+ = (",kp(1),", ",kp(2)," ,",kp(3),", ",kp(4),")"
c      print*,"K- = (",km(1),", ",km(2)," ,",km(3),", ",km(4),")"

c Old angles commented out
c      call cost(mup,mum,kp,km,ct1,ct2)
c      call chi(mup,mum,kp,km,phi)
      call cdfct(mup,mum,kp,km,ct)
      call cdfcp(mup,mum,kp,km,cp)
      call cdfphi(mup,mum,kp,km,phi)
        
      if(idebug.eq.1) print*,"cos(theta) = ",ct
      if(idebug.eq.1) print*,"cos(psi) = ",cp
      if(idebug.eq.1) print*,"phi = ",phi
      if(idebug.eq.1) print*,"Proper Lifetime = ",V(mupg,5)/0.2998

c Set Measured Parameters for ML function
c x(1) = proper time V(mupg,5) mm/c (~3.33x10^-12 s^-1)
      x(1) = V(mupg,5)/0.299792458
      x(2) = ct
      x(3) = cp
      x(4) = phi

      call mlfunc_cdf(x,par,prob1)
      call norm(x(1),tau,prob2)

c Accept/Reject Event
c Random number generator
      R = PYR(0)*1.8
c      print*,"Random Number = ",R
c      print*,"Corrected ML Prob = ",prob1*prob2
c      if(idebug.eq.1) print*, "Prob1 ", prob1
c      if(idebug.eq.1) print*, "Prob2 ", prob2
      if(idebug.eq.1) print*, "Random Number = ", R
      if(idebug.eq.1) print*, "Corrected ML Prob = ", prob1*prob2
      if((prob1*prob2).gt.1.8) then 
        print*,"WARNING: Max Prob exceeded!", prob1*prob2
      endif
      if(R.lt.(prob1*prob2)) then 
         iret = 0
         if(idebug.eq.1) print*, 'Accepted prob'
      ELSE
         iret = 1
         if(idebug.eq.1) print*, 'Rejected prob'
         return
      endif
c End of flat-angles switch
      else  
c Flat angles are always accepted
      iret = 0 

      endif

c Apply kinematic cuts

      call kine(bs,eta,pt)
      if(abs(eta).gt.BdEtaCut.or.pt.lt.BdPtCut) then
         iret = 1
c         if(idebug.eq.1) print*,"Rejected because Bd Pt is ",pt
         return
      endif
c      if(idebug.eq.1) print*,"Accepted Bd Pt is ",pt

      call kine(mup,eta,pt)
      mu1pt = pt
      if(abs(eta).gt.muetacut.or.pt.lt.l2muon) then
         iret = 1
         return
      endif
      call kine(mum,eta,pt)
      if(abs(eta).gt.muetacut.or.pt.lt.l2muon) then
         iret = 1
         return
      endif
c Apply Level 1 Cut
      if (mu1pt.lt.pt) then
         mu1pt = pt
      endif
      if(mu1pt.lt.l1muon) then
         iret = 1
         return
      endif
c Apply Hadron cuts
      call kine(kp,eta,pt)
      if(abs(eta).gt.hadetacut.or.pt.lt.hadptcut) then
         iret = 1
         return
      endif
      call kine(km,eta,pt)
      if(abs(eta).gt.hadetacut.or.pt.lt.hadptcut) then
         iret = 1
         return
      endif
c      if(idebug.eq.1) print*,"Proper Lifetime = ",V(mupg,5)/0.299792458
      if(idebug.eq.1) print*,"**** IRET = ",iret

      return
* USER_FINSEL    
      END 


c **********************************************************************
*
* Subroutine to calculate cos(theta)
*
c23456******************************************************************
c     subroutine cost(d1,d2,h1,h2,ct1,ct2)
c     implicit none
c     double precision d1(4),d2(4),h1(4),h2(4),jpsi(4),kns(4),b(4)
c     double precision jpsib(4),d1b(4),knsb(4),h1b(4)
c     double precision ct1,ct2
c     integer i
c
c     do i=1,4 
c        jpsi(i) = d1(i)+d2(i)
c        kns(i) = h1(i)+h2(i)
c        b(i) = jpsi(i)+kns(i)
c     enddo
c
c     call loren4(b, jpsi, jpsib)
c     call loren4(jpsi, d1, d1b)
c     call ang3(jpsib,d1b,ct1)
c
c     call loren4(b, kns, knsb)
c     call loren4(kns,h1,h1b)
c     call ang3(knsb,h1b,ct2)
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to calculate chi
*
c23456******************************************************************
c     subroutine chi(d1,d2,h1,h2,phi)
c     implicit none
c     double precision p(4),d1(4),d2(4),h1(4),h2(4),jpsi(4),kns(4)
c     double precision d1b(4),h1b(4),jpsib(4),knsb(4)
c     double precision dperp(3), hperp(3), dpara(3)
c     double precision phi, signum
c     integer i
c
c     do i=1,4
c        jpsi(i) = d1(i)+d2(i)
c        kns(i) = h1(i)+h2(i)
c        p(i) = jpsi(i)+kns(i)
c     enddo
c
c     call loren4(p,d1,d1b)
c     call loren4(p,h1,h1b)
c     call loren4(p,jpsi,jpsib)
c     call loren4(p,kns,knsb)
c
c     call cross(jpsib,d1b,dperp)
c     call cross(knsb,h1b,hperp)
c     call cross(dperp,hperp,dpara)
c
c     call ang3(jpsib,dpara,signum)
c     call ang3(dperp,hperp,phi)
c     phi = signum * dacos(phi)  
c      if (phi.LT.0) phi=phi+2*3.1415926536
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to calculate CDF cos(theta)
*
c23456******************************************************************
c     subroutine cdfct(d1,d2,h1,h2,ct)
c     implicit none
c     double precision d1(4),d2(4),h1(4),h2(4),jpsi(4),kns(4)
c     double precision d1b(4),h1b(4),knsb(4)
c     double precision x(3), y(3), z(3), h1bu(3), d1bu(3), knso(3)
c     double precision ct, prod
c     integer i
c
c Calculate 4 vectors of J/psi and phi/K0*
c     do i=1,4
c        jpsi(i) = d1(i)+d2(i)
c        kns(i) = h1(i)+h2(i)
c     enddo
c 
c Boost into rest frame of J/psi
c     call loren4(jpsi,d1,d1b)
c     call loren4(jpsi,h1,h1b)
c     call loren4(jpsi,kns,knsb)
c 
c Calcuate coordinate system  
c     call unit3(knsb, x)
c     call unit3(h1b, h1bu) 
c     call unit3(d1b, d1bu)
c     call dot3(h1bu,x,prod)
c 
c     do i=1,3
c        knso(i) = h1bu(i) - x(i)*prod
c     enddo
c 
c     call unit3(knso,y)
c     call cross(x,y,z)
c
c Calculate cos(theta)
c     call dot3(d1bu,z,ct)
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to calculate CDF cos(Psi)
*
c23456******************************************************************
c     subroutine cdfcp(d1,d2,h1,h2,cp)
c     implicit none
c     double precision d1(4),d2(4),h1(4),h2(4),jpsi(4),kns(4)
c     double precision h1b(4),jpsib(4)
c     double precision h1bu(3),jpsibu(3)
c     double precision cp
c     integer i
c
c Calculate 4 vectors of J/psi and phi/K0*
c     do i=1,4
c        jpsi(i) = d1(i)+d2(i)
c        kns(i) = h1(i)+h2(i)
c     enddo
c 
c Boost into rest frame of phi/K0*
c     call loren4(kns,jpsi,jpsib)
c     call loren4(kns,h1,h1b)
c 
c Calculate cos(Psi)
c     call ang3(h1b,jpsib,cp)
c     cp = cp*(-1.0)
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to calculate CDF Phi
*
c23456******************************************************************
c     subroutine cdfphi(d1,d2,h1,h2,phi)
c     implicit none
c     double precision d1(4),d2(4),h1(4),h2(4),jpsi(4),kns(4)
c     double precision d1b(4),h1b(4),knsb(4)
c     double precision x(3), y(3), z(3), h1bu(3), d1bu(3), knso(3)
c     double precision phi, prod, xdot, ydot
c     integer i
c
c Calculate 4 vectors of J/psi and phi/K0*
c     do i=1,4
c        jpsi(i) = d1(i)+d2(i)
c        kns(i) = h1(i)+h2(i)
c     enddo
c
c Boost into rest frame of J/psi
c     call loren4(jpsi,d1,d1b)
c     call loren4(jpsi,h1,h1b)
c     call loren4(jpsi,kns,knsb)
c 
c Calcuate coordinate system
c     call unit3(knsb, x)
c     call unit3(h1b, h1bu) 
c     call unit3(d1b, d1bu)
c     call dot3(h1bu,x,prod)
c 
c     do i=1,3
c        knso(i) = h1bu(i) - x(i)*prod
c      enddo
c 
c     call unit3(knso,y)
c
c Calculate Phi
c     call dot3(d1bu,y,ydot)
c     call dot3(d1bu,x,xdot)
c     phi = datan2(ydot,xdot)
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to normalize exponential distribution to flat distribution 
* between 0 and tmax
* t is the proper decay time of the Bd in ps
* tau is the lifetime of the Bd
* p is the normalization factor
* tmax is the maximum allowed time in ps
*
c23456******************************************************************
c     subroutine norm(t,tau,p)
c     double precision t, tau, p
c t is in pico seconds
c     if(t.Gt.20.0) print*, "WARNING: proper decay time > 20ps"
c     p = exp(t/tau)*tau
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to calculate ML function
*
c23456******************************************************************
c     subroutine mlfunc_cdf(x, par, prob)
c     double precision x(4), par(8), a(6), b1(6), b2(6), c(6)
c     double precision prob, time, cost, cospsi, phi, a0, apara,
c    +                 aperp, deltag, gammas, deltam, phis, delta1,
c    +                 delta2, gammah, gammal, expgl, expgh, expgs,
c    +                 sinsqt, sisqps, sin2t, sin2ps, eta
c     integer i
c
c Measured Variables
c     time = x(1)
c Old angles commented out
c      cost1 = x(2)
c      cost2 = x(3)
c     cost = x(2)
c     cospsi = x(3)
c     phi = x(4)
c
c Fitted Variables
c     a0 = par(1)
c     apara = par(2)
c     aperp = sqrt(1 - a0**2 - apara**2)
c     deltag = par(3)
c     gammas = par(4)
c     deltam = par(5)
c     phis = par(6)
c     delta1 = par(7)
c     delta2 = par(8)
c
c The tabulated function is Sum of: a(i) * (b1(i) +/- b2(i)) * c(i)
c Define a cells
c
c     a(1) = 0.5 * a0**2
c     a(2) = 0.5 * apara**2
c     a(3) = 0.5 * aperp**2
c     a(4) = 0.5 * a0*apara*cos(delta2 - delta1)
c     a(5) = apara*aperp
c     a(6) = a0*aperp
c
c Define B cells
c Input variables are gammas, deltag, deltam, phis, delta1, delta2, time
c
c     gammah = gammas - deltag/2.0
c     gammal = gammas + deltag/2.0
c     expgl = exp((-gammal)*time)
c     expgh = exp((-gammah)*time)
c     expgs = exp((-gammas)*time)
c
c     b1(1) = (1.0 + cos(phis))*expgl + (1.0 - cos(phis))*expgh
c     b1(2) = b1(1)
c     b1(3) = (1.0 - cos(phis))*expgl + (1.0 + cos(phis))*expgh
c     b1(4) = b1(1)
c     b1(5) = (-0.5)*(expgh - expgl)*cos(delta1)*sin(phis)
c     b1(6) = (-0.5)*(expgh - expgl)*cos(delta2)*sin(phis)
c
c     b2(1) = 2.0*expgs*sin(deltam*time)*sin(phis)
c     b2(2) = b2(1)
c     b2(3) = -b2(1)
c     b2(4) = b2(1)
c     b2(5) = expgs*(sin(delta1)*cos(deltam*time) -
c    +        cos(delta1)*sin(deltam*time)*cos(phis))
c     b2(6) = expgs*(sin(delta2)*cos(deltam*time) -
c    +        cos(delta2)*sin(deltam*time)*cos(phis))
c
c Define C cells
c Old angles commented out
c Input variables are cost1, cost2 and phi
c
c      sisqt1 = 1.0 - cost1**2
c      sisqt2 = 1.0 - cost2**2
c      sin2t1 = 2.0*sqrt(sisqt1)*cost1
c      sin2t2 = 2.0*sqrt(sisqt2)*cost2
c
c      c(1) = 4.0*sisqt1*cost2**2
c      c(2) = (1.0 + cost1**2)*sisqt2 - sisqt1*sisqt2*cos(2.0*phi)
c      c(3) = (1.0 + cost1**2)*sisqt2 + sisqt1*sisqt2*cos(2.0*phi)
c      c(4) = (-sqrt(2.0))*sin2t1*sin2t2*cos(phi)
c      c(5) = 2.0*sisqt1*sisqt2*sin(2.0*phi)
c      c(6) = sqrt(2.0)*sin2t1*sin2t2*sin(phi)
c
c Input variables are cost, cosp and phi
c
c     sinsqt = 1.0 - cost**2
c     sisqps = 1.0 - cospsi**2
c     sin2t = 2.0*sqrt(sinsqt)*cost
c     sin2ps = 2.0*sqrt(sisqps)*cospsi
c
c     c(1) = 2.0*cospsi**2 * (1.0 - sinsqt*cos(phi)**2)
c     c(2) = sisqps * (1.0 - sinsqt*sin(phi)**2)
c     c(3) = sisqps * sinsqt
c     c(4) = (-1.0/sqrt(2.0))*sin2ps * sinsqt * sin(2.0*phi)
c     c(5) = sisqps * sin2t * sin(phi)
c     c(6) = (1.0/sqrt(2.0))*sin2ps * sin2t * cos(phi)
c
c
c Sum the tabulated formula
c Currently just calculate bs decay
c     prob = 0.0
c eta is +1 for particle -1 for antiparticle
c adam changed eta to meet B meson production
c     eta = 1.0 
c     do i=1,6
c        prob = prob + a(i)*(b1(i) + eta*b2(i))*c(i)
c     enddo
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to calculate eta and pt of particle
*
c23456******************************************************************
c     subroutine kine(p, eta, pt)
c     double precision p(4)
c     double precision eta,pt,th
c     pt = sqrt(p(1)**2+p(2)**2)
c     th = acos(p(3)/sqrt(pt**2+p(3)**2))
c     eta = -log(max(0.0001,abs(dtan(0.5*th))))
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to convert vector into unit vector
*
c23456******************************************************************
c     subroutine unit3(v, norm)
c     double precision v(3), norm(3)
c
c     do i=1,3
c        norm(i) = v(i) / dsqrt(v(1)**2+v(2)**2+v(3)**2)
c     enddo
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to perform dot product of 3 momentum
*
c23456******************************************************************
c     subroutine dot3(v1, v2, prod)
c     double precision v1(3), v2(3)
c     double precision prod
c
c     prod = v1(1)*v2(1)+v1(2)*v2(2)+v1(3)*v2(3)
c
c     return
c     end

c **********************************************************************

c **********************************************************************
*
* Subroutine to calculate angle between 2 vectors
*
c23456******************************************************************
c     subroutine ang3(v1, v2, prod)
c     double precision v1(3), v2(3)
c     double precision prod, v1mag, v2mag
c
c     call dot3(v1,v2,prod)
c     v1mag = v1(1)**2+v1(2)**2+v1(3)**2
c     v2mag = v2(1)**2+v2(2)**2+v2(3)**2
c
c     prod = prod/dsqrt(v1mag*v2mag)
c
c     return
c     end

c **********************************************************************

*
* $Id: cross.F,v 1.1.1.1 1996/02/15 17:49:51 mclareni Exp $
*
c     SUBROUTINE CROSS(X,Y,Z)
C
C CERN PROGLIB# F117    CROSS           .VERSION KERNFOR  2.02  730125
C ORIG. 01/01/64
C 
c     IMPLICIT DOUBLE PRECISION (A-H,O-Z)
c     DIMENSION X(3),Y(3),Z(3)
C
C
c     Z1=X(2)*Y(3)-X(3)*Y(2)
c     Z2=X(3)*Y(1)-X(1)*Y(3)
c     Z(3)=X(1)*Y(2)-X(2)*Y(1)
c     Z(1)=Z1
c     Z(2)=Z2
c     RETURN
c     END

c     SUBROUTINE LOREN4  (DIR,P4IN,P4OUT)
C
C CERN PROGLIB# U101    LOREN4          .VERSION KERNFOR  4.07  830624
C ORIG. 20/02/68
C
c
c     DOUBLE PRECISION PCM2, ONMCM, EPBETA, PROD
c
c     DOUBLE PRECISION DIR(4),P4IN(4),P4OUT(4)
C
C--                VN(A) MEANS N-VECTOR A
C--                GAMMA=ECM/MCM
C--                EPBETA=ECM*V3(PCM)*V3(BETA)
C--                V3(BETA)=V3(PCM)/ECM
C
c     PCM2=DIR(1)*DIR(1)+DIR(2)*DIR(2)+DIR(3)*DIR(3)
c
c
c     ONMCM=1.D0/DSQRT (DIR(4)*DIR(4)-PCM2)
c
c     EPBETA=P4IN(1)*DIR(1)+P4IN(2)*DIR(2)+P4IN(3)*DIR(3)
c     PROD=EPBETA*(DIR(4)*ONMCM-1.D0)/PCM2-P4IN(4)*ONMCM
c     P4OUT(4)=ONMCM*(P4IN(4)*DIR(4)-EPBETA)
c        DO 50 I=1,3
c  50 P4OUT(I)=P4IN(I)+DIR(I)*PROD
c     RETURN
c     END

