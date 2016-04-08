       SUBROUTINE USEBC
c      SUBROUTINE UPEVNT
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)      

C...Extra commonblock to transfer run info.
      COMMON/PRIV/MODE,NLIM   
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
      SAVE/PRIV/
     
C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500) 
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP  
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)  
      SAVE /HEPEUP/ 

      IF(IABS(MODE).gt.2) THEN
        idprup=661
      ENDIF
C...Call the respective routine to generate event.
      IF(IDPRUP.EQ.661) THEN
        CALL MYBC
      ELSE
        WRITE(*,*) 'Fatal error! Unknown process',IDPRUP 
        STOP  
      ENDIF

      RETURN
      END 
 
C*********************************************************************
C...MYBC
C...Routine to generate g g -> Bc- b~ c events and  g g -> Bc+ b c~ events.
 
       SUBROUTINE MYBC
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      PARAMETER (PI=3.141592653589793D0)
      DIMENSION sigscl(3),sigvct(3),color(3)
C...Extra commonblock to transfer run info.
      COMMON/PRIV/MODE,NLIM
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
      SAVE/PRIV/
      COMMON/fmean/fmean
      SAVE/fmean/
*
      external bccons
*
      real*8 fmean ! mean f(x1)*f(x2)
*
c      integer init/0/,initrun/0/,cf
      integer init/0/,cf
      COMMON/colorflow/cf
*
C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/

C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500) 
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP  
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)  
      SAVE /HEPEUP/   

C...PYTHIA commonblock.
c      INTEGER PYCOMP  
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...Event number counter.
c      DATA ILIM/0/

      real*8 wrk1(5),wrk2(5),wrk3(5),wrk4(5),wrk5(5)
      real*8 wrk(4,20)
*
      real*8 sigscl,sigvct
c   ,gl1,gl2
      real*8 myxgen,mylips
*
      if(init.eq.0)then
        init=1
*
        asqcd=alphas2(ambc)
        scalbc=ambc
        write(6,*)' ALPHA S at QCD scale of M(Bc) =',asqcd
        write(6,*)' PTCUT(Bc)',PTCUT
        write(6,*)'IPAR1: if 0, then IDUP(3)=541 Bc(1S0)'
        write(6,*)'else IDUP(3)=100543 Bc(2S0)+Bc(2S1)',IPAR1
*
        call myxini
        call myfint(fmean)
        write(6,*)' mean pdf = ',fmean
        call inilips
        call inithb
      endif
*
C...CM energy of system.
      ECM=VINT(1)

C...Zero some arrays in common blocks to simplify filling.
      NUP=5
      Do i=1,4
        do j=1,20
           wrk(i,j)=0d0
        enddo
      enddo
      DO 100 I=1,NUP
        MOTHUP(1,I)=0
        MOTHUP(2,I)=0
        ICOLUP(1,I)=0
        ICOLUP(2,I)=0
        SPINUP(I)=9D0
        do j=1,5
          PUP(j,I)=0D0
c          wrk(j,i)=0d0
          wrk1(j)=0d0
          wrk2(j)=0d0
          wrk3(j)=0d0
          wrk4(j)=0d0
          wrk5(j)=0d0
        enddo
        VTIMUP(I)=0D0
  100 CONTINUE
  
  
      PUP(5,3)=ambc
      PUP(5,4)=amb
      PUP(5,5)=amc
      
C...Set up flavour and history of gg --> Bc b~ c
      IDUP(1)=21
      IDUP(2)=21
C       will be set later:
c      IDUP(3)=-541
c      IDUP(4)=-5 
c      IDUP(5)=4 
      ISTUP(1)=-1
      ISTUP(2)=-1 
      ISTUP(3)=1
      ISTUP(4)=1
      ISTUP(5)=1
      MOTHUP(1,3)=1
      MOTHUP(2,3)=2
      MOTHUP(1,4)=1
      MOTHUP(2,4)=2
      MOTHUP(1,5)=1
      MOTHUP(2,5)=2


 11   xwgtup=fmean
      t20=pyr(0)
      x1=myxgen(t20,1)
      t20=pyr(0)
      x2=myxgen(t20,1)
      if(x1*x2.le.tau0)goto 11
*
      pup(3,1)=ebmup(1)*x1
      pup(4,1)=ebmup(1)*x1
      pup(3,2)=-(ebmup(2)*x2)
      pup(4,2)=ebmup(2)*x2
* gg in lab and CM
      wrk(3,1)=pup(3,1)+pup(3,2)
      wrk(4,1)=pup(4,1)+pup(4,2)
*
      wrk(4,2)=sqrt(wrk(4,1)**2-wrk(3,1)**2)
* Bc,b,c in CM
*      call daliz3(wrk(1,2),wrk(1,3),wrk(1,4),wrk(1,5),ambc,amb,amc)
*      call v3lips(wrk(4,2),ambc,amb,amc,vlips)
      call daliz4(wrk(1,2),wrk(1,3),wrk(1,5),wrk(1,4),ambc,amc,amb,
     &            xwgtup)
     
*      write(6,*)'ptcut ',PTCUT
      if(dsqrt(wrk(1,3)**2+wrk(2,3)**2).lt.PTCUT) then
      xwgtup=0.
      goto 123
      endif
      
      vlips=mylips(wrk(4,2))
* differential xs
      call bcgluons(wrk(1,3),sigscl,sigvct)
      
* back to lab
      call dlrnb(wrk(4,2),wrk(1,1),wrk(1,3),pup(1,3))
      call dlrnb(wrk(4,2),wrk(1,1),wrk(1,4),pup(1,4))
      call dlrnb(wrk(4,2),wrk(1,1),wrk(1,5),pup(1,5))
c      print *,pmas(pycomp(5),1),pmas(pycomp(4,1),1)
      
* event weight
      sigtot=sigscl(1)+sigscl(2)+sigscl(3)+
     +       sigvct(1)+sigvct(2)+sigvct(3)
* umnozhil na 2, chtoby generit' chasticu i antichasticu!!!! sejchas 2 net!!!:     
      XWGTUP=XWGTUP*sigtot*vlips*(2d0*pi)**9
* a dirty trick: limiting the diff xsection to current xwgtup
* The current xwgtup is high enough, so the total xsection 
* does not deviate much
      if(xwgtup.gt.xmaxup(1))then
        write(6,*)' MYBC: xs exceeded,',xwgtup
        xwgtup=xmaxup(1)*0.9999999
      endif
      
      ratio=(sigscl(1)+sigscl(2)+sigscl(3))/sigtot

      if(IPAR1.eq.0) then
        if(pyr(0).le.0.5) then
C          if(pyr(0).le.ratio) then
           IDUP(3)=-541
C         else
C           IDUP(3)=-543
C         endif
          IDUP(4)=-5 
          IDUP(5)=4 
        else
C         if(pyr(0).le.ratio) then
           IDUP(3)=541
C         else
C           IDUP(3)=543
C         endif
           IDUP(4)=5 
           IDUP(5)=-4 
        endif         
c      if(IPAR1.eq.0)then
c         IDUP(3)=541
      else
C 100543 eto summa vectora i pseudoscalara 2S 
        if(pyr(0).le.0.5) then
         IDUP(3)=-100543
         IDUP(4)=-5 
         IDUP(5)=4 
        else
         IDUP(3)=100543
         IDUP(4)=5 
         IDUP(5)=-4 
        endif
      endif
*      write(6,*)'IPAR1, IDUP(3)',IPAR1,IDUP(3)
*      IDUP(4)=5
*      IDUP(5)=-4
        
c      if(IDUP(3).eq.-541.or.IDUP(3).eq.541) then
c        color(1)=sigscl(1)/sigtot
c        color(2)=sigscl(2)/sigtot
c        color(3)=sigscl(3)/sigtot
c      else       
c        color(1)=sigvct(1)/sigtot
c        color(2)=sigvct(2)/sigtot
c        color(3)=sigvct(3)/sigtot
c      endif       
 
        color(1)=(sigscl(1)+sigvct(1))/sigtot
        color(2)=(sigscl(2)+sigvct(2))/sigtot
        color(3)=(sigscl(3)+sigvct(3))/sigtot
              
       
      color_rnd=pyr(0)
C...Set up colour of g g --> Bc b~ c
      if(color_rnd.lt.color(1)) then
        if(IDUP(3).lt.0) then
          icolup(1,1)=1101
          icolup(2,1)=1100
          icolup(1,2)=1100
          icolup(2,2)=1102
           icolup(1,3)=0
           icolup(2,3)=0
           icolup(1,4)=0
           icolup(2,4)=1102
           icolup(1,5)=1101
           icolup(2,5)=0
       else
          icolup(1,1)=1100
          icolup(2,1)=1101
          icolup(1,2)=1102
          icolup(2,2)=1100
           icolup(1,3)=0
           icolup(2,3)=0
           icolup(1,4)=1102
           icolup(2,4)=0
           icolup(1,5)=0
           icolup(2,5)=1101
        endif  
       cf=1
       goto 123
      endif
      
      
      if(color_rnd.ge.color(1).and.color_rnd.lt.color(1)+color(2)) then
        if(IDUP(3).lt.0) then
          icolup(1,1)=1100
          icolup(2,1)=1102
          icolup(1,2)=1101
          icolup(2,2)=1100
           icolup(1,3)=0
           icolup(2,3)=0
           icolup(1,4)=0
           icolup(2,4)=1102
           icolup(1,5)=1101
           icolup(2,5)=0
        else
          icolup(1,1)=1102
          icolup(2,1)=1100
          icolup(1,2)=1100
          icolup(2,2)=1101
           icolup(1,3)=0
           icolup(2,3)=0
           icolup(1,4)=1102
           icolup(2,4)=0
           icolup(1,5)=0
           icolup(2,5)=1101
        endif  
       cf=2
       goto 123
      endif
      
      if(color_rnd.gt.color(1)+color(2)) then
        if(IDUP(3).lt.0) then
          icolup(1,1)=1100
          icolup(2,1)=1101
          icolup(1,2)=1101
          icolup(2,2)=1100
           icolup(1,3)=0
           icolup(2,3)=0
           icolup(1,4)=0
           icolup(2,4)=1102
           icolup(1,5)=1102
           icolup(2,5)=0
       else
          icolup(1,1)=1100
          icolup(2,1)=1101
          icolup(1,2)=1101
          icolup(2,2)=1100
           icolup(1,3)=0
           icolup(2,3)=0
           icolup(1,4)=1102
           icolup(2,4)=0
           icolup(1,5)=0
           icolup(2,5)=1102
       endif   
       cf=3
      endif 
      
 123  continue
*
C...Some other compulsory quantities.
      SCALUP=-1d0
*
      RETURN
      END
 

C*********************************************************************

C-----------------------------------------------
      SUBROUTINE daliz3(p,p1,p2,p3,u1,u2,u3)
*
* PURPOSE:
*   generates decay 1-->3 uniformly over phase space
* INPUT:
*   p(4) - 4-momentum of the parent particle, REAL*8
*   u1,u2,u3 - masses of daughter particles, REAL*8
* OUTPUT:
*   p1,p2,p3(4) - 4-momenta of daughter particles, REAL*8
*
      IMPLICIT NONE
c      REAL*8 p(4),p1(4),p2(4),p3(4),f(4),u,u1,u2,u3,enq,s1,s2
      REAL*8 p(4),p1(4),p2(4),p3(4),u,u1,u2,u3
      real*8 a1(4),a2(4),a3(4),b1(4),b2(4),b3(4)
      real*8 o1(3),o2(3),o3(3)
      REAL*8 pyr
      real*8 u12,u12min,u12max
      real*8 u23,u23min,u23max,u23low,u23hig
      real*8 e2t,e3t,u13,q1,q2,q3
      real*8 cost,sint
      real*8 phi,cosphi,sinphi
      real*8 gamma,cosgam,singam
      real*8 xx2,xx3
      real*8 am12
*
      real*8 pi/3.14159265358979/
*
      u=sqrt(p(4)**2-p(1)**2-p(2)**2-p(3)**2)
*
    1 u12min=(u1+u2)**2
      u12max=(u -u3)**2
      u12=u12min+(u12max-u12min)*pyr(0)
      am12=sqrt(u12)
*
      e2t=(u12-u1**2+u2**2)/2./am12
      e3t=(u**2-u12-u3**2)/2./am12
      u23min=(u2+u3)**2
      u23max=(u -u1)**2
      u23hig=(e2t+e3t)**2-(sqrt(e2t**2-u2**2)-sqrt(e3t**2-u3**2))**2
      u23low=(e2t+e3t)**2-(sqrt(e2t**2-u2**2)+sqrt(e3t**2-u3**2))**2
      u23=u23min+(u23max-u23min)*pyr(0)
      if(u23.lt.u23low .or. u23.gt.u23hig)goto 1
*
      u13=u**2+u1**2+u2**2+u3**2-u12-u23
*
      q1=sqrt((u**2-(sqrt(u23)+u1)**2)*(u**2-(sqrt(u23)-u1)**2))/2./u
      q2=sqrt((u**2-(sqrt(u13)+u2)**2)*(u**2-(sqrt(u13)-u2)**2))/2./u
      q3=sqrt((u**2-(sqrt(u12)+u3)**2)*(u**2-(sqrt(u12)-u3)**2))/2./u
*
      cost=(q1**2-q2**2+q3**2)/2./q1/q3
      sint=sqrt(1-cost**2)
*
      a1(1)=-(q1*cost)
      a1(2)=q1*sint
      a1(3)=0.
      a1(4)=sqrt(u1**2+q1**2)
*
      a2(1)=-q3-a1(1)
      a2(2)=-a1(2)
      a2(3)=0.
      a2(4)=sqrt(u2**2+q2**2)
*
      a3(1)=q3
      a3(2)=0.
      a3(3)=0.
      a3(4)=sqrt(u3**2+q3**2)
*
      cost=-1+2.*pyr(0)
      sint=sqrt(1-cost**2)
      phi=2.d0*pi*pyr(0)
      cosphi=cos(phi)
      sinphi=sin(phi)
      gamma=2.d0*pi*pyr(0)
      cosgam=cos(gamma)
      singam=sin(gamma)
*
      o1(1)=cost
      o1(2)=sint*cosphi
      o1(3)=sint*sinphi
*
      o2(1)=-sint
      o2(2)=cost*cosphi
      o2(3)=cost*sinphi
*
      call dcross(o1,o2,o3)
*
      xx2=o2(1)*cosgam-o3(1)*singam
      xx3=o2(1)*singam+o3(1)*cosgam
      o2(1)=xx2
      o3(1)=xx3
      xx2=o2(2)*cosgam-o3(2)*singam
      xx3=o2(2)*singam+o3(2)*cosgam
      o2(2)=xx2
      o3(2)=xx3
      xx2=o2(3)*cosgam-o3(3)*singam
      xx3=o2(3)*singam+o3(3)*cosgam
      o2(3)=xx2
      o3(3)=xx3
*
      b1(1)=a1(1)*o1(1)+a1(2)*o2(1)+a1(3)*o3(1)
      b1(2)=a1(1)*o1(2)+a1(2)*o2(2)+a1(3)*o3(2)
      b1(3)=a1(1)*o1(3)+a1(2)*o2(3)+a1(3)*o3(3)
      b1(4)=a1(4)
*
      b2(1)=a2(1)*o1(1)+a2(2)*o2(1)+a2(3)*o3(1)
      b2(2)=a2(1)*o1(2)+a2(2)*o2(2)+a2(3)*o3(2)
      b2(3)=a2(1)*o1(3)+a2(2)*o2(3)+a2(3)*o3(3)
      b2(4)=a2(4)
*
      b3(1)=a3(1)*o1(1)+a3(2)*o2(1)+a3(3)*o3(1)
      b3(2)=a3(1)*o1(2)+a3(2)*o2(2)+a3(3)*o3(2)
      b3(3)=a3(1)*o1(3)+a3(2)*o2(3)+a3(3)*o3(3)
      b3(4)=a3(4)
*
      call dlrnb (u,p,b1,p1)
      call dlrnb (u,p,b2,p2)
      call dlrnb (u,p,b3,p3)
*
      RETURN
      END
*------------------------------------------------------------------------
      SUBROUTINE dcross(a,b,c)
* vektornoe priozvedenie
      IMPLICIT NONE
      REAL*8 a(3),b(3),c(3)
      c(1)    =a(2)*b(3)-a(3)*b(2)
      c(2)    =a(3)*b(1)-a(1)*b(3)
      c(3)    =a(1)*b(2)-a(2)*b(1)
      END
*------------------------------------------------------------------------
      SUBROUTINE dlrnb (u,ps,pi,pf)
* Lorentz boost back
      IMPLICIT NONE
      REAL*8 pf4, fn
      REAL*8 u,ps(4),pi(4),pf(4)
*
      IF (ps(4).eq.u)then
        pf(1)= pi(1)
        pf(2)= pi(2)
        pf(3)= pi(3)
        pf(4)= pi(4)
      else
        pf4  = (pi(4)*ps(4)+pi(3)*ps(3)+pi(2)*ps(2)+pi(1)*ps(1)) / u
        fn   = (pf4+pi(4)) / (ps(4)+u)
        pf(1)= pi(1) + fn*ps(1)
        pf(2)= pi(2) + fn*ps(2)
        pf(3)= pi(3) + fn*ps(3)
        pf(4)= pf4
      endif
*
      END
*------------------------------------------------------------------------
      SUBROUTINE dlrnf (u,ps,pi,pf)
* Lorentz boost forward
      IMPLICIT NONE
      REAL*8 pf4, fn
      REAL*8 u,ps(4),pi(4),pf(4)
*
      IF (ps(4).eq.u)then
        pf(1)= pi(1)
        pf(2)= pi(2)
        pf(3)= pi(3)
        pf(4)= pi(4)
      else
        pf4  = (pi(4)*ps(4)-pi(3)*ps(3)-pi(2)*ps(2)-pi(1)*ps(1)) / u
        fn   = (pf4+pi(4)) / (ps(4)+u)
        pf(1)= pi(1) - fn*ps(1)
        pf(2)= pi(2) - fn*ps(2)
        pf(3)= pi(3) - fn*ps(3)
        pf(4)= pf4
      endif
*
      END
*-------------------------------------------------------------------------
      subroutine inilips
*      implicit none
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
* parametrizacija fazovogo ob"ema Bc+b+c kak funkcija sqrt(s)
      integer mode,nlim,IPAR1,IPAR2
      real*8 amb,amc,ambc,tau0,asqcd,scalbc,PTCUT,ECM
      COMMON/PRIV/MODE,NLIM  
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
*
      COMMON/PYINT1/MINT(400),VINT(400)
*
      integer nlips
      parameter(nlips=2000)
      real*8 xx(0:nlips),yy(0:nlips)
      common/mylipstab/xx,yy
*
      integer i
      real*8 s0
*
      write(6,*)' smart lips initialization'
*
      ECM=VINT(1)
*
      xx(0)=2d0*ambc
      yy(0)=0d0
*      s0=log(2d0*ambc/14000d0) *** FIXED
      s0=log(2d0*ambc/dble(ECM))
      do i=1,nlips
*        xx(i)=14000d0*exp(s0*dble(nlips-i+1)/nlips) *** FIXED
        xx(i)=dble(ECM)*exp(s0*dble(nlips-i+1)/nlips)
        call v3lips(xx(i),amc,ambc,amb,yy(i))
      enddo
*      
      write(6,*)' end'
      end
*-------------------------------------------------------------------------
      real*8 function mylips(shs)
      implicit none
* vychislenie 3-chastichnogo fazovogo ob"ema po parametrizacii
      real*8 shs,divdf2,uulips
*
      integer nlips
      parameter(nlips=2000)
      real*8 xx(0:nlips),yy(0:nlips)
      common/mylipstab/xx,yy
*
      uulips=divdf2(yy(0),xx(0),nlips+1,shs,3)
      if(uulips.lt.0d0)uulips=0d0
      mylips=uulips
      end
*-------------------------------------------------------------------------
      subroutine v3lips(u,u1,u2,u3,s)
*
* PURPOSE: 
* calculates area of Dalitz plot in variables m12**2,m23**2
* INPUT:
* u        - mass of parent particle
* u1,u2,u3 - masses of daughter particles
* OUTPUT:
* s - result, i.e. area of Dalitz plot
*
      implicit none
c      real*8 pi/3.141592653589793/
      real*8 twopi/6.283185307179586/
c      REAL*8 fourpi/12.566370614359172/
      real*8 u,u1,u2,u3,s
      real*8 u12min,u12max
c      real*8 u23,u23min,u23max,u23hig,u23low
      external del23
      real*8 dgauss,del23
      real*8 v,v1,v2,v3
      common/cdalic/v,v1,v2,v3
*
      s=0.d0
      if(u.lt.0.d0.or.u1.lt.0.d0.or.u2.lt.0.d0.or.u3.lt.0.d0)return
      if(u-u1-u2-u3.lt.0.d0)return
*
      v =u **2
      v1=u1**2
      v2=u2**2
      v3=u3**2
*
      u12min=(u1+u2)**2
      u12max=(u -u3)**2
      s=dgauss(del23,u12min,u12max,1.d-8)
      s=s/v/16.d0/twopi**7
*
      end
*-----------------
      real*8 function del23(x)
      real*8 x
      real*8 v,v1,v2,v3
      common/cdalic/v,v1,v2,v3
      real*8 res,sq1,sq2
      real*8 lamd,a,b,c
      lamd(a,b,c)=(a-b-c)**2-4.d0*b*c
*
      sq1=sqrt(lamd(x,v1,v2))
      sq2=sqrt(lamd(x,v ,v3))
      res=sq1*sq2/x /x
      del23=res
      end

*----------------------
      subroutine myxini
      implicit none
* parametrizacija obratnoj pervoobraznoj dlja strukt. funkcii gluona
      real*8 dxstart
      integer maxpnt,npnt
      integer npfact,npteor
      parameter(npnt=10000)
      parameter(maxpnt=npnt)
      parameter(dxstart=2.d-8)
      real*8 xx(0:maxpnt),aa(0:maxpnt)
      common/mypdftab/npfact,npteor,xx,aa
      real*8 dgauss
      real*8 totint,dx0,dx,da
      real*8 mypdfg
      external mypdfg
      integer init/0/
      integer npdfcalls
      common/pdfcalls/npdfcalls
*
      write(6,*)' initializing smart x generation'
      init=1
*
      npdfcalls=0
*
      npteor=npnt
      xx(0)=0d0
      aa(0)=0d0
      xx(1)=dxstart
      da=dxstart*mypdfg(dxstart/2d0)
      aa(1)=aa(0)+da
      npfact=1
*
      dx0=2d0/npnt
*
      totint=aa(1)+dgauss(mypdfg,xx(1),1d0,1.d-9)
      do while(xx(npfact).lt.1d0 .and. da.gt.0d0)
        npfact=npfact+1
        dx=dx0/sqrt(1d0+(mypdfg(xx(npfact-1))/totint)**2)
        xx(npfact)=xx(npfact-1)+dx
        da=dgauss(mypdfg,xx(npfact-1),xx(npfact),1d-9)
        aa(npfact)=aa(npfact-1)+da
      enddo
*
      write(6,*)npfact,' points used'
      if(xx(npfact).gt.1.d0 .and. da.gt.0d0)then
        xx(npfact)=1d0
        aa(npfact)=aa(npfact-1)
     &            +dgauss(mypdfg,xx(npfact-1),xx(npfact),1d-9)
      else
        npfact=npfact-1
      endif
*      
      write(6,*)' end after',npdfcalls,' pdf calls'
      end
*----------------------
      real*8 function myxgen(a,jmd)
      implicit none
* vychislenie obratnoj pervoobraznoj dlja strukt. funkcii gluona
      real*8 a
      integer npnt,maxpnt,jmd
      integer npfact,npteor
      parameter(npnt=10000)
      parameter(maxpnt=npnt)
      real*8 xx(0:maxpnt),aa(0:maxpnt)
      common/mypdftab/npfact,npteor,xx,aa
      real*8 divdf2
*
c      print *,'mypdftab', npfact,npteor,xx,aa
      if(jmd.eq.0)then
        myxgen=aa(npfact)
      elseif(jmd.eq.1)then
        if(a.le.0d0)then
          myxgen=0d0
        elseif(a.ge.1d0)then
          myxgen=xx(npfact)
        else
          myxgen=divdf2(xx(0),aa(0),npfact+1,a*aa(npfact),3)
        endif
      elseif(jmd.eq.2)then
        if(a.le.0d0)then
          myxgen=0d0
        elseif(a.ge.xx(npfact))then
          myxgen=aa(npfact)
        else
          myxgen=divdf2(aa(0),xx(0),npfact+1,a,3)
        endif
      else  ! to fix compiler warnings
          myxgen = 999d0
          print*,'myxgen wrong !!! ',myxgen
      endif
*
      end
*------------------------
      real*8 function mypdfg(x)
      implicit double precision (a-h,o-z)
* interfejs k strukt. funk. gluona dlja subr. dgauss (integrirovanie)
C...Extra commonblock to transfer run info.
      COMMON/PRIV/MODE,NLIM  
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
      real*8 x,xx,d1,d2,d3,d4,d5,d6,d7,d8,d9
      integer npdfcalls
      common/pdfcalls/npdfcalls
*
      xx=x
      if(x.lt.0d0 .or. xx.gt.1d0)then
        mypdfg=0d0
        return
      elseif(x.lt.1d-8)then
        xx=1d-8
      elseif(x.gt.0.9999995)then
        xx=0.9999995
      else
        xx=x
      endif
*
      call structm(xx,scalbc,d1,d2,d3,d4,d5,d6,d7,d8,d9)
      mypdfg=d9/xx
      npdfcalls=npdfcalls+1
      end
*-------------------------
      subroutine myfint(fmean)
      implicit none
* interfejs k dgauss dlja vychislenija srednego znachenija
* strukt. funkcii 
      integer mode,nlim,IPAR1,IPAR2
      real*8 amb,amc,ambc,tau0,asqcd,scalbc,PTCUT
      COMMON/PRIV/MODE,NLIM  
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
*
      real*8 fmean
      real*8 dgauss,fix1x2
      external fix1x2
*
      fmean=dgauss(fix1x2,tau0,1d0,1d-8) !integral of f(x1)*f(x2)
     &       /(1d0-tau0*(1d0-log(tau0))) ! area of integration
      end
*-----------
      real*8 function fix1x2(x2)
      implicit none
      real*8 x2,x1min
      real*8 myxgen,mypdfg
*
      integer mode,nlim,IPAR1,IPAR2
      real*8 amb,amc,ambc,tau0,asqcd,scalbc,PTCUT
      COMMON/PRIV/MODE,NLIM  
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
*
      x1min=tau0/x2
      fix1x2=(myxgen(0.d0,0)-myxgen(x1min,2))*mypdfg(x2)
      end
*
*====================================
      subroutine inithb
* vychislenie tablicy Int[Exp[-0.12*sqs*Sqrt[1-x^2]],{x,-1,1}] 
*      implicit none
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
*
      COMMON/PYINT1/MINT(400),VINT(400)
*
      integer nthb
      parameter(nthb=2000)
*      real*8 ambc/6.3d0/ *** FIXED
      integer mode,nlim,IPAR1,IPAR2
      real*8 amb,amc,ambc,tau0,asqcd,scalbc,ptcut,ECM
      COMMON/PRIV/MODE,NLIM  
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
      
      real*8 xx(0:nthb),yy(0:nthb)
      common/thbtab/xx,yy
      real*8 sqsi
      common/cthetab/sqsi
      real*8 funthb,dgauss
      external funthb
      real*8 xlast,ylast
*
      integer i
      real*8 s0
*
      ECM=VINT(1)
*
      write(6,*)'Hello, this is PythiaBc version 2.718(2), your easy solution for the Bc-mesons at ATLAS'
      write(6,*)'M(Bc) =',ambc
      write(6,*)'ECM =',ECM
      xx(0)=2d0*0.02*ambc
      yy(0)=0d0
*      s0=log(2d0*0.02*ambc/0.2/14000d0) *** FIXED
      s0=log(2d0*0.02*ambc/0.2/dble(ECM))
      

      do i=1,nthb
*        xx(i)=0.2*14000d0*exp(s0*dble(nthb-i+1)/nthb) *** FIXED
        xx(i)=0.2*dble(ECM)*exp(s0*dble(nthb-i+1)/nthb)
        if(sqsi.lt.230d0)then
          sqsi=xx(i)
          yy(i)=dgauss(funthb,-1d0,1d0,1d-18)
          xlast=xx(i)
          ylast=yy(i)
        else
          yy(i)=ylast*(xlast/xx(i))**2
        endif
*        write(6,*)i,xx(i),yy(i)
      enddo
*
      end
*----------------------
      real*8 function funthb(x)
* funthb(x)=Exp[-sqs*Sqrt[1-x^2]],{x,-1,1}] 
      real*8 x,xx,v
      real*8 sqsi
      common/cthetab/sqsi
*
      xx=x**2
      if(xx.lt.1d0)then
        v=sqrt(1d0-x**2)
      else
        v=0d0
      endif
      funthb=exp(-(sqsi*v))
*
      end
*----------------------
      real*8 function mythb(shs)
      implicit none
* vychislenie Int[Exp[-0.12*sqs*Sqrt[1-x^2]],{x,-1,1}] interpoljaciej tablicy
      real*8 shs,divdf2,uuthb
*
      integer nthb
      parameter(nthb=2000)
      real*8 xx(0:nthb),yy(0:nthb)
      common/thbtab/xx,yy
*
      if(shs.le.xx(0))then
        uuthb=0d0
      else
        uuthb=divdf2(yy(0),xx(0),nthb+1,shs,3)
      endif
      if(uuthb.lt.0d0)uuthb=0d0
      mythb=uuthb
      end
*-----------------------
      subroutine tstthb
      real*8 sqs,mythb,ECM
      COMMON/PYINT1/MINT(400),VINT(400)
      
      ECM=VINT(1)
      
*      call hbook1(1001,'int',14000,0.,0.2*14000.,0.) *** FIXED
*      call hbook1(1002,'int x s',14000,0.,0.2*14000.,0.) *** FIXED
      call hbook1(1001,'int',ECM,0.,0.2*dble(ECM),0.)
      call hbook1(1002,'int x s',ECM,0.,0.2*dble(ECM),0.)
*
      call inithb
*      do i=1,14000 *** FIXED
      do i=1,int(ECM)
        sqs=(i-0.5d0)*0.2
        call hf1(1001,real(sqs),real(mythb(sqs)))
        call hf1(1002,real(sqs),real(mythb(sqs)*sqs*sqs))
      enddo
      call hrput(0,'thb.hbk',' ')
*
      end
*-------------------------------------------------------
      SUBROUTINE daliz4(p,p1,p2,p3,u1,u2,u3,weight)
*
* PURPOSE:
*   generates decay 1-->3 uniformly over phase space
* INPUT:
*   p(4) - 4-momentum of the parent particle, REAL*8
*   u1,u2,u3 - masses of daughter particles, REAL*8
* OUTPUT:
*   p1,p2,p3(4) - 4-momenta of daughter particles, REAL*8
*
      IMPLICIT NONE
      REAL*8 p(4),p1(4),p2(4),p3(4),u,u1,u2,u3
      real*8 a1(4),a2(4),a3(4),b1(4),b2(4),b3(4)
      real*8 o1(3),o2(3),o3(3)
      REAL*8 pyr
c      real rndm
      real*8 u12,u12min,u12max,u12log1,u12log2
      real*8 u23,u23min,u23max,u23low,u23hig
      real*8 e2t,e3t,u13,q1,q2,q3
      real*8 cost,sint
      real*8 phi,cosphi,sinphi
      real*8 gamma,cosgam,singam
      real*8 xx1,xx2,xx3,sl1,sl2,wwrk
      real*8 am12
      real*8 thb,mythb,weight
*
      real*8 pi/3.14159265358979/
*
      u=sqrt(p(4)**2-p(1)**2-p(2)**2-p(3)**2)
*
    1 u12min=(u1+u2)**2
      u12max=(u -u3)**2
      u12log1=log(u12min)
      u12log2=log(u12max)
      u12=u12log1+(u12log2-u12log1)*pyr(0)
      u12=exp(u12)
      am12=sqrt(u12)
*
      e2t=(u12-u1**2+u2**2)/2./am12
      e3t=(u**2-u12-u3**2)/2./am12
      u23min=(u2+u3)**2
      u23max=(u -u1)**2
      u23hig=(e2t+e3t)**2-(sqrt(e2t**2-u2**2)-sqrt(e3t**2-u3**2))**2
      u23low=(e2t+e3t)**2-(sqrt(e2t**2-u2**2)+sqrt(e3t**2-u3**2))**2
      u23=u23min+(u23max-u23min)*pyr(0)
      if(u23.lt.u23low .or. u23.gt.u23hig)goto 1
      weight=weight * u12
*
      u13=u**2+u1**2+u2**2+u3**2-u12-u23
*
      q1=sqrt((u**2-(sqrt(u23)+u1)**2)*(u**2-(sqrt(u23)-u1)**2))/2./u
      q2=sqrt((u**2-(sqrt(u13)+u2)**2)*(u**2-(sqrt(u13)-u2)**2))/2./u
      q3=sqrt((u**2-(sqrt(u12)+u3)**2)*(u**2-(sqrt(u12)-u3)**2))/2./u
*
      cost=(q1**2-q2**2+q3**2)/2./q1/q3
      sint=sqrt(1-cost**2)
*
      a1(1)=0.
      a1(2)=q1*sint
      a1(3)=-(q1*cost)
      a1(4)=sqrt(u1**2+q1**2)
*
      a2(1)=0.
      a2(2)=-a1(2)
      a2(3)=-q3-a1(3)
      a2(4)=sqrt(u2**2+q2**2)
*
      a3(1)=0.
      a3(2)=0.
      a3(3)=q3
      a3(4)=sqrt(u3**2+q3**2)
* generim cos(theta) b-kvarka soglasno formule
 101  cost=-1+2.*pyr(0)
      sint=sqrt(1-cost**2)
*      sl1=0.12
*      sl1=0.129
      sl1=0.54895/p(4)+0.11937
*      sl2=0.03
*      sl2=0.0522
      sl2=2.9772/p(4)+0.026256
*      xx3=3d0/p(4)**2
*      xx3=-1.91d-4
*      xx3=1d-3
      xx3=1d-4
      xx2=8d0/p(4)
*      xx2=0.065
*      xx2=0d0
      xx1=1d0-xx2-xx3
      thb=xx1*exp(-(sl1*p(4)*sint))+xx2*exp(-(sl2*p(4)*sint))+xx3
      wwrk=pyr(0)
      if(wwrk.gt.thb)goto 101
      weight=weight*
     &   (xx1*mythb(sl1*p(4))+xx2*mythb(sl2*p(4))+2d0*xx3)/2d0/thb
*
      phi=2.d0*pi*pyr(0)
      cosphi=cos(phi)
      sinphi=sin(phi)
      gamma=2.d0*pi*pyr(0)
      cosgam=cos(gamma)
      singam=sin(gamma)
*
      o3(1)=sint*cosphi
      o3(2)=sint*sinphi
      o3(3)=cost
*
      o1(1)=cost*cosphi
      o1(2)=cost*sinphi
      o1(3)=-sint
*
      call dcross(o3,o1,o2)
*
      xx1= o1(1)*cosgam+o2(1)*singam
      xx2=-(o1(1)*singam)+o2(1)*cosgam
      o1(1)=xx1
      o2(1)=xx2
      xx1= o1(2)*cosgam+o2(2)*singam
      xx2=-(o1(2)*singam)+o2(2)*cosgam
      o1(2)=xx1
      o2(2)=xx2
      xx1= o1(3)*cosgam+o2(3)*singam
      xx2=-(o1(3)*singam)+o2(3)*cosgam
      o1(3)=xx1
      o2(3)=xx2
*
      b1(1)=a1(1)*o1(1)+a1(2)*o2(1)+a1(3)*o3(1)
      b1(2)=a1(1)*o1(2)+a1(2)*o2(2)+a1(3)*o3(2)
      b1(3)=a1(1)*o1(3)+a1(2)*o2(3)+a1(3)*o3(3)
      b1(4)=a1(4)
*
      b2(1)=a2(1)*o1(1)+a2(2)*o2(1)+a2(3)*o3(1)
      b2(2)=a2(1)*o1(2)+a2(2)*o2(2)+a2(3)*o3(2)
      b2(3)=a2(1)*o1(3)+a2(2)*o2(3)+a2(3)*o3(3)
      b2(4)=a2(4)
*
      b3(1)=a3(1)*o1(1)+a3(2)*o2(1)+a3(3)*o3(1)
      b3(2)=a3(1)*o1(2)+a3(2)*o2(2)+a3(3)*o3(2)
      b3(3)=a3(1)*o1(3)+a3(2)*o2(3)+a3(3)*o3(3)
      b3(4)=a3(4)
*
      call dlrnb (u,p,b1,p1)
      call dlrnb (u,p,b2,p2)
      call dlrnb (u,p,b3,p3)
*
      RETURN
      END
      
*=============================================================
*
*--------------------------------------------------
      DOUBLE PRECISION FUNCTION divdf2(f,a,nn,x,mm)
*
* CERNLIB function for interpolation, double precision version
*
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      DIMENSION a(nn),f(nn),t(20),d(20)
      LOGICAL extra
c      LOGICAL mflag,rflag
      DATA mmax/10/
C
C  TABULAR INTERPOLATION USING SYMMETRICALLY PLACED ARGUMENT POINTS.
C
C  START.  FIND SUBSCRIPT IX OF X IN ARRAY A.
      IF( (nn.LT.2) .OR. (mm.LT.1) ) THEN
        PRINT *,' ###### DIVDF2 ERROR   : Invalid dimensions'//
     -          ' received for the arguments.'
        go to 20
      ENDIF
      n=nn
      m=min0(mm,mmax,n-1)
      mplus=m+1
      ix=0
      iy=n+1
      IF(a(1).GT.a(n)) go to 4
*** Search increasing arguments.
    1 mid=(ix+iy)/2
      IF(x.GE.a(mid)) go to 2
      iy=mid
      go to 3
*** If true.
    2 ix=mid
    3 IF(iy-ix.gt.1) go to 1
      go to 7
*** Search decreasing arguments.
    4 mid=(ix+iy)/2
      IF(x.LE.a(mid)) go to 5
      iy=mid
      go to 6
C        (IF TRUE.)
    5 ix=mid
    6 IF(iy-ix.gt.1) go to 4
C
C  Copy reordered interpolation points into (T(I),D(I)), setting
C  *EXTRA* to TRUE if M+2 points to be used.
C
    7 npts=m+2-mod(m,2)
      ip=0
      l=0
      go to 9
    8 l=-l
      IF(l.GE.0) l=l+1
    9 isub=ix+l
      IF((1.LE.isub).AND.(isub.LE.n)) go to 10
*** skip point.
      npts=mplus
      go to 11
*** Insert point.
   10 ip=ip+1
      t(ip)=a(isub)
      d(ip)=f(isub)
   11 IF(ip.LT.npts) go to 8
      extra=npts.ne.mplus
C
C  Replace d by the leading diagonal of a divided-difference table, sup-
C  plemented by an extra line if *EXTRA* is true.
C
      DO 14 l=1,m
        IF(.NOT.extra) go to 12
        isub=mplus-l
        d(m+2)=(d(m+2)-d(m))/(t(m+2)-t(isub))
   12   i=mplus
        DO 13 j=l,m
          isub=i-l
          d(i)=(d(i)-d(i-1))/(t(i)-t(isub))
          i=i-1
   13   CONTINUE
   14 CONTINUE
C
C  Evaluate the Newton interpolation formula at X, averaging two values
C  of last difference if *EXTRA* is TRUE.
C
      sum=d(mplus)
      IF(extra) sum=0.5*(sum+d(m+2))
      j=m
      DO 15 l=1,m
        sum=d(j)+(x-t(j))*sum
        j=j-1
   15 CONTINUE
      divdf2=sum
      RETURN
*** Error processing.
   20 CONTINUE
      divdf2=0
      END
                SUBROUTINE BCGLUONS(Q,SIGSCL,SIGVCT)
*
*
*  This subroutine calculates the gluonic production cross section of
*  Bc-antimeson.
*
*  Q(I,J) are the momenta of Bc-antimeson, b-antiquark and c-quark in
*  the center of mass frame of interacting gluons:
*
*                  I=1 --- x-axis,
*                  I=2 --- y-axis,
*                  I=3 --- z-axis (the momenta of the initial gluons
*                                   direct along this one),
*                  I=4 --- energy;
*
*                  J=1 --- Bc-antimeson,
*                  J=2 --- b-antiquark,
*                  J=3 --- c-quark.
*
*  SIGSCL(K) are the cross-sections of pseudoscalar antiBc-meson state in pb
*  for the different color flows.  
*  SIGVCT(K) are the cross-sections of vector antiBc-meson state in pb
*  for the different color flows.
*
*                       COLOR FLOWS
*
*  K=1:   first gluon color       ---> c quark color,
*         second gluon anti-color ---> b anti-quark anti-color,
*         first gluon anti-color  -><- second gluon color.
*  K=2:   first gluon anti-color  ---> b anti-quark anti-color,
*         second gluon color      ---> c quark color,
*         second gluon anti-color -><- first gluon color.
*  K=3:   c quark color           <--> b anti-quark anti-color,
*         first gluon anti-color  -><- second gluon color,
*         second gluon anti-color -><- first gluon color.
*
*  The subroutine doesn't take into account phase space factors
*  of 1./(2.*energy of a final particle).
*
* Total cross sections of pseudoscalar and vector states gluonic
*               production at energy 20 GeV are
*      9.78+-0.05 pb and 23.74+-0.14 pb correspondingly.
*
*
*                           ATTENTION !!!
*   To insure that the program works right, link the
*   subroutine BCGLUONS.FOR with the main program TSTBCGL.FOR and run.
*   This program prints two columns of values. All values in the first
*   column must be equal to 9.7788450E-07. All values in the second
*   column must be equal to 3.3185381E-06.
*
*
*  Address all the questions to Aleksandr V. Berezhnoy:
*           E-mail: aber@ttk.ru
*           Home phone: 0967-51-29-85
*
*
        REAL*8 P(4,4),ET,MC,MB,P1(4),P2(4),AS,A,Q(4,3),
     + SIGSCL(3),SIGVCT(3),FBC
        COMMON/P/P
        COMMON/M/MB,MC
      integer mode,nlim,IPAR1,IPAR2
      real*8 amb,amc,ambc,tau0,asqcd,scalbc,ptcut,ECM
      COMMON/PRIV/MODE,NLIM  
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2    

        PI=3.14159265358979

*        MB=4.8
*        MC=1.5
        MB=amb
        MC=amc
*        write(6,*)' M(b): ',MB
*        write(6,*)' M(c): ',MC
*       MB is a mass of b-quark.
*       MC is a mass of c-quark.
*
        ET=Q(4,1)+Q(4,2)+Q(4,3)
        P1(4)=ET/2
        P1(3)=P1(4)
        P1(2)=0.
        P1(1)=0.
        P2(4)=P1(4)
        P2(3)=-P1(4)
        P2(2)=0.
        P2(1)=0.


        AS=0.2
*
*       AS is a strong coupling constant.
*


        FBC=0.57
*
*       FBC is a leptonic constant.
*

        A=4./(2*PI)*(AS**4)/(2.*ET*ET)*0.389E+9
        A=A*(MB+MC)**2/MB/MC/12./2.*(FBC**2)/64./3.

        DO J=1,4
          P(J,1)=MB/(MB+MC)*Q(J,1)
          P(J,2)=Q(J,2)
          P(J,3)=Q(J,3)
          P(J,4)=MC/(MB+MC)*Q(J,1)
        END DO

         CALL CALC(P1,P2)
         CALL MESONG_G(P1,P2,SIGSCL,SIGVCT)

       DO K=1,3
        SIGSCL(K)=SIGSCL(K)*A
        SIGVCT(K)=SIGVCT(K)*A
        END DO

       NN=0
       IF(SIGSCL(1).LT.0.) THEN
       SIGSCL(2)=SIGSCL(2)+SIGSCL(1)
       SIGSCL(1)=0.
       ELSE
       CONTINUE
       END IF

       IF(SIGSCL(2).LT.0.) THEN
       SIGSCL(1)=SIGSCL(1)+SIGSCL(2)
       SIGSCL(2)=0.
       ELSE
       CONTINUE
       END IF

       IF(SIGVCT(1).LT.0.) THEN
       SIGVCT(2)=SIGVCT(2)+SIGVCT(1)
       SIGVCT(1)=0.
       ELSE
       CONTINUE
       END IF

       IF(SIGVCT(2).LT.0.) THEN
       SIGVCT(1)=SIGVCT(1)+SIGVCT(2)
       SIGVCT(2)=0.
       ELSE
       CONTINUE
       END IF
       
              
        RETURN
        END




        SUBROUTINE CALC(P1,P2)
**
        REAL*8 P1(4),P2(4),Q1(4),Q2(4),Q3(4),Q4(4),
     +         Q12(4),Q34(4),
     +         P12(4),SQ12,SQ34,SP12,MC,MB,
     +         P_M1(4),P_M2(4),P_M3(4),P_M4(4),P_M5(4),P_M6(4),
     +         KC(4),KB(4),KU(4),SP_M(6)
        COMPLEX*16 UB(4,2),VB(4,2),
     +             UC(4,2),VC(4,2),
     +             JB(4,2,2),JC(4,2,2),
c     +             C1(4,4,2),C2(4,4,2),
     +             H3(4,4,4,2),H4(4,4,4,2),H5(4,4,4,2),H6(4,4,4,2),
     +             C3(4,4,2,2),C4(4,4,2,2),C5(4,4,2,2),C6(4,4,2,2),
     +             DC(2,2),DB(2,2),
     +             SBC(2,2,2,2),G_Q(4,2,2,2,2)
        COMMON/P/Q1,Q2,Q3,Q4
        COMMON/M/MB,MC
        COMMON/SPN/UB,VB,UC,VC
        COMMON/CRNT/JB,JC
        COMMON/G_Q/G_Q,SBC
        COMMON/MOM/Q12,Q34,P12,P_M1,P_M2,P_M3,P_M4,
     +             P_M5,P_M6
        COMMON/S/SQ12,SQ34,SP12,SP_M
        COMMON/H/H3,H6,C3,C4,C5,C6
*
        CALL SPIN(Q1,Q2,MB,UB,VB)
        CALL SPIN(Q3,Q4,MC,UC,VC)
*
        CALL CUR1(UB,VB,JB)
        CALL CUR1(UC,VC,JC)
*
        DO I=1,4
        Q12(I)=-Q1(I)-Q2(I)
        Q34(I)=-Q3(I)-Q4(I)
        P12(I)=P1(I)+P2(I)
        P_M1(I)=P1(I)+Q12(I)
        P_M2(I)=P1(I)+Q34(I)
        P_M3(I)=Q34(I)-Q2(I)
        P_M4(I)=-Q34(I)+Q1(I)
        P_M5(I)=Q3(I)-Q12(I)
        P_M6(I)=Q12(I)-Q4(I)
        KC(I)=P12(I)-Q12(I)
        KU(I)=Q12(I)-Q34(I)
        KB(I)=Q34(I)-P12(I)
        END DO
*
        DO K1=1,2
        DO K2=1,2
        DB(K1,K2)=0.
        DC(K1,K2)=0.
        DO I=1,4
        DB(K1,K2)=DB(K1,K2)+JB(I,K1,K2)*KB(I)
        DC(K1,K2)=DC(K1,K2)+JC(I,K1,K2)*KC(I)
        END DO
        DO K3=1,2
        DO K4=1,2
        SBC(K1,K2,K3,K4)=0.
        DO I=1,3
        SBC(K1,K2,K3,K4)=
     +  JB(I,K1,K2)*JC(I,K3,K4)+SBC(K1,K2,K3,K4)
        END DO
        SBC(K1,K2,K3,K4)=
     +  -SBC(K1,K2,K3,K4)+JB(4,K1,K2)*JC(4,K3,K4)
        END DO
        END DO
        END DO
        END DO
*
        DO IUB=1,2
        DO IVB=1,2
        DO IUC=1,2
        DO IVC=1,2
        DO I=1,3
        G_Q(I,IUB,IVB,IUC,IVC)=
     +            +DC(IUC,IVC)*JB(I,IUB,IVB)
     +            -KU(I)*SBC(IUB,IVB,IUC,IVC)
     +            +DB(IUB,IVB)*JC(I,IUC,IVC)
        END DO
        G_Q(4,IUB,IVB,IUC,IVC)=
     +            +DC(IUC,IVC)*JB(4,IUB,IVB)
     +            +KU(4)*SBC(IUB,IVB,IUC,IVC)
     +            +DB(IUB,IVB)*JC(4,IUC,IVC)
        END DO
        END DO
        END DO
        END DO
*
        CALL HLP(P_M3,MB,VB,H3)
        CALL CUR2(UB,H3,C3)
        CALL HLP(P_M4,MB,VB,H4)
        CALL CUR2(UB,H4,C4)
        CALL HLP(P_M5,MC,VC,H5)
        CALL CUR2(UC,H5,C5)
        CALL HLP(P_M6,MC,VC,H6)
        CALL CUR2(UC,H6,C6)
*
        CALL INV(Q12,SQ12)
        CALL INV(Q34,SQ34)
        CALL INV(P12,SP12)
        CALL INV(P_M1,SP_M(1))
        CALL INV(P_M2,SP_M(2))
        CALL INV(P_M3,SP_M(3))
        CALL INV(P_M4,SP_M(4))
        CALL INV(P_M5,SP_M(5))
        CALL INV(P_M6,SP_M(6))
        SP_M(3)=SP_M(3)-MB*MB
        SP_M(4)=SP_M(4)-MB*MB
        SP_M(5)=SP_M(5)-MC*MC
        SP_M(6)=SP_M(6)-MC*MC
        RETURN
        END



        SUBROUTINE SPIN_Z(PU,PV,MQ,U,V)
        COMPLEX*16 U(4,2),V(4,2),IM
        REAL*8 MQ,PU(4),PV(4),EP

        IM=(0.,1.)

        EP=DSQRT(PU(4)+MQ)

        U(1,1)=EP
        U(2,1)=0.
        U(3,1)=-(PU(3)/EP)
        U(4,1)=-((PU(1)-IM*PU(2))/EP)
        U(1,2)=0.
        U(2,2)=U(1,1)
        U(3,2)=DCONJG(U(4,1))
        U(4,2)=-U(3,1)


        EP=DSQRT(PV(4)+MQ)

        V(1,1)=-(PV(3)/EP)
        V(2,1)=-((PV(1)+IM*PV(2))/EP)
        V(3,1)=-EP
        V(4,1)=0.
        V(1,2)=-DCONJG(V(2,1))
        V(2,2)=V(1,1)
        V(3,2)=0.
        V(4,2)=-V(3,1)

        RETURN
        END





        SUBROUTINE CURUQ(P,C)
        REAL*8 P(4)
        COMPLEX*16 C(4,4,2),AS(4,4),SYM(4,4),IM
        DATA AS/16*(0.D0,0.D0)/
        IM=DCMPLX(0.D0,1.D0)
        SYM(1,1)=DCMPLX(P(1),-P(2))
        SYM(2,2)=DCMPLX(-P(1),P(2))
        SYM(3,3)=DCMPLX(-P(1),-P(2))
        SYM(4,4)=DCMPLX(P(1),P(2))
        SYM(1,2)=DCMPLX(P(2),P(1))
        SYM(2,1)=SYM(1,2)
        SYM(2,3)=DCMPLX(0.D0,P(3))
        SYM(3,2)=SYM(2,3)
        SYM(2,4)=DCMPLX(0.D0,P(4))
        SYM(4,2)=SYM(2,4)
        SYM(1,3)=P(3)
        SYM(3,1)=P(3)
        SYM(1,4)=P(4)
        SYM(4,1)=P(4)
        SYM(3,4)=0.
        SYM(4,3)=0.
*
        AS(1,3)=DCMPLX(0.D0,-P(4))
        AS(3,1)=-AS(1,3)
        AS(1,4)=DCMPLX(0.D0,-P(3))
        AS(4,1)=-AS(1,4)
        AS(2,3)=P(4)
        AS(3,2)=-P(4)
        AS(2,4)=P(3)
        AS(4,2)=-P(3)
        AS(3,4)=DCMPLX(-P(2),P(1))
        AS(4,3)=-AS(3,4)
*
        DO I=1,4
        DO J=1,4
        C(I,J,1)=SYM(I,J)+IM*AS(I,J)
        C(I,J,2)=DCONJG(SYM(I,J))-IM*DCONJG(AS(I,J))
        END DO
        END DO
        RETURN
        END
*
*
*
        SUBROUTINE INV(A,C)
        REAL*8 A(4),C
        C=A(4)*A(4)-A(3)*A(3)-A(2)*A(2)-A(1)*A(1)
        RETURN
        END
*
*
*
        SUBROUTINE CUR1(U,V,C)
        COMPLEX*16 U(4,2),V(4,2),C(4,2,2),
     +             IM
        IM=DCMPLX(0.D0,1.D0)
        DO I=1,2
        DO J=1,2
        C(1,I,J)=-(U(1,I)*V(4,J))-(U(2,I)*V(3,J))
     +           +U(3,I)*V(2,J)+U(4,I)*V(1,J)
        C(2,I,J)=IM*(U(1,I)*V(4,J)-(U(2,I)*V(3,J))
     +           -(U(3,I)*V(2,J))+U(4,I)*V(1,J))
        C(3,I,J)=-(U(1,I)*V(3,J))+U(2,I)*V(4,J)
     +           +U(3,I)*V(1,J)-(U(4,I)*V(2,J))
        C(4,I,J)=U(1,I)*V(1,J)+U(2,I)*V(2,J)
     +           -(U(3,I)*V(3,J))-(U(4,I)*V(4,J))
        END DO
        END DO
        RETURN
        END
*
*
*
        SUBROUTINE SPIN(PU,PV,MQ,U,V)
        COMPLEX*16 U(4,2),V(4,2),C1,C2,K1,K2,K3,K4
        REAL*8 MQ,PU(4),PV(4),MODP,NORM,EM,EP
*
        MODP=DSQRT(PU(1)*PU(1)+PU(2)*PU(2)+PU(3)*PU(3))
        NORM=DSQRT(2.*MODP*(MODP+PU(3)))
*
        EM=DSQRT(PU(4)-MQ)
        EP=DSQRT(PU(4)+MQ)
*
        IF(NORM.GT.1.E-18) THEN
        C1=(MODP+PU(3))/NORM
        C2=DCMPLX(PU(1),PU(2))/NORM
*
        K1=EP*C1
        K2=EP*C2
        K3=EM*C1
        K4=EM*C2
*
        U(1,1)=K1
        U(2,1)=DCONJG(K2)
        U(3,1)=-K3
        U(4,1)=-DCONJG(K4)
        U(1,2)=-K2
        U(2,2)=K1
        U(3,2)=-K4
        U(4,2)=K3
*
        ELSE
        U(1,1)=0.
        U(2,1)=EP
        U(3,1)=0.
        U(4,1)=-EM
        U(1,2)=-EP
        U(2,2)=0.
        U(3,2)=-EM
        U(4,2)=0.

        END IF
*
*
        MODP=DSQRT(PV(1)*PV(1)+PV(2)*PV(2)+PV(3)*PV(3))
        NORM=DSQRT(2.*MODP*(MODP+PV(3)))
*
        EM=DSQRT(PV(4)-MQ)
        EP=DSQRT(PV(4)+MQ)
*
        IF(NORM.GT.1.E-18) THEN
        C1=(MODP+PV(3))/NORM
        C2=DCMPLX(PV(1),PV(2))/NORM
*
        K1=EP*C1
        K2=EP*C2
        K3=EM*C1
        K4=EM*C2
*
        V(1,1)=-K3
        V(2,1)=-K4
        V(3,1)=-K1
        V(4,1)=-K2
        V(1,2)=DCONJG(K4)
        V(2,2)=-K3
        V(3,2)=-DCONJG(K2)
        V(4,2)=K1
*
        ELSE
        V(1,1)=0.
        V(2,1)=-EM
        V(3,1)=0.
        V(4,1)=-EP
        V(1,2)=EM
        V(2,2)=0.
        V(3,2)=EP
        V(4,2)=0.
        END IF

        RETURN
        END
*
*
*
*
*
        SUBROUTINE HLP(P,M,V,H)
        COMPLEX*16 V(4,2),H(4,4,4,2),S(4,4,4),
     +  R(4,4),A(4,4,4),D(4),IM
        REAL*8 M,P(4)
        IM=DCMPLX(0.D0,1.D0)
*
        DO IS=1,2
*
        DO I=1,4
        DO J=1,4
        R(I,J)=P(I)*V(J,IS)
        END DO
        END DO
*SYM 12
        S(1,1,2)=-(IM*R(1,4))+R(2,4)
        S(2,1,2)=IM*R(1,3)+R(2,3)
        S(3,1,2)=IM*R(1,2)-R(2,2)
        S(4,1,2)=-(IM*R(1,1))-R(2,1)
*SYM 13
        S(1,1,3)=R(1,3)+R(3,4)
        S(2,1,3)=-R(1,4)+R(3,3)
        S(3,1,3)=-R(1,1)-R(3,2)
        S(4,1,3)=R(1,2)-R(3,1)
*SYM 14
        S(1,1,4)=R(1,1)+R(4,4)
        S(2,1,4)=R(1,2)+R(4,3)
        S(3,1,4)=-R(1,3)-R(4,2)
        S(4,1,4)=-R(1,4)-R(4,1)
*SYM 23
        S(1,2,3)=R(2,3)-IM*R(3,4)
        S(2,2,3)=-R(2,4)+IM*R(3,3)
        S(3,2,3)=-R(2,1)+IM*R(3,2)
        S(4,2,3)=R(2,2)-IM*R(3,1)
*SYM 24
        S(1,2,4)=R(2,1)-IM*R(4,4)
        S(2,2,4)=R(2,2)+IM*R(4,3)
        S(3,2,4)=-R(2,3)+IM*R(4,2)
        S(4,2,4)=-R(2,4)-IM*R(4,1)
*SYM 34
        S(1,3,4)=R(3,1)+R(4,3)
        S(2,3,4)=R(3,2)-R(4,4)
        S(3,3,4)=-R(3,3)-R(4,1)
        S(4,3,4)=-R(3,4)+R(4,2)
*
        DO K=1,4
        S(K,2,1)=S(K,1,2)
        S(K,3,1)=S(K,1,3)
        S(K,4,1)=S(K,1,4)
        S(K,3,2)=S(K,2,3)
        S(K,4,2)=S(K,2,4)
        S(K,4,3)=S(K,3,4)
        END DO
*
        D(1)=-R(1,4)+IM*R(2,4)-R(3,3)+R(4,1)-M*V(1,IS)
        D(2)=-R(1,3)-IM*R(2,3)+R(3,4)+R(4,2)-M*V(2,IS)
        D(3)=R(1,2)-IM*R(2,2)+R(3,1)-R(4,3)-M*V(3,IS)
        D(4)=R(1,1)+IM*R(2,1)-R(3,2)-R(4,4)-M*V(4,IS)
*
*SYM 11
        S(1,1,1)=2*R(1,4)+D(1)
        S(2,1,1)=2*R(1,3)+D(2)
        S(3,1,1)=-(2*R(1,2))+D(3)
        S(4,1,1)=-(2*R(1,1))+D(4)
*SYM 22
        S(1,2,2)=-(2*IM*R(2,4))+D(1)
        S(2,2,2)=2*IM*R(2,3)+D(2)
        S(3,2,2)=2*IM*R(2,2)+D(3)
        S(4,2,2)=-(2*IM*R(2,1))+D(4)
*SYM 33
        S(1,3,3)=2*R(3,3)+D(1)
        S(2,3,3)=-(2*R(3,4))+D(2)
        S(3,3,3)=-(2*R(3,1))+D(3)
        S(4,3,3)=2*R(3,2)+D(4)
*SYM 44
        S(1,4,4)=2*R(4,1)-D(1)
        S(2,4,4)=2*R(4,2)-D(2)
        S(3,4,4)=-(2*R(4,3))-D(3)
        S(4,4,4)=-(2*R(4,4))-D(4)
*
*
*ASYM
*AS 12
        A(1,1,2)=IM*(R(4,1)-R(3,3)-M*V(1,IS))
        A(2,1,2)=IM*(-R(4,2)-R(3,4)+M*V(2,IS))
        A(3,1,2)=IM*(-R(4,3)+R(3,1)-M*V(3,IS))
        A(4,1,2)=IM*(R(4,4)+R(3,2)+M*V(4,IS))
*AS 13
        A(1,1,3)=IM*R(2,3)-R(4,2)+M*V(2,IS)
        A(2,1,3)=IM*R(2,4)+R(4,1)-M*V(1,IS)
        A(3,1,3)=-(IM*R(2,1))+R(4,4)+M*V(4,IS)
        A(4,1,3)=-(IM*R(2,2))-R(4,3)-M*V(3,IS)
*AS 14
        A(1,1,4)=IM*R(2,1)-R(3,2)-M*V(4,IS)
        A(2,1,4)=-(IM*R(2,2))+R(3,1)-M*V(3,IS)
        A(3,1,4)=-(IM*R(2,3))+R(3,4)-M*V(2,IS)
        A(4,1,4)=IM*R(2,4)-R(3,3)-M*V(1,IS)
*AS 23
        A(1,2,3)=IM*(-R(1,3)+R(4,2)-M*V(2,IS))
        A(2,2,3)=IM*(-R(1,4)+R(4,1)-M*V(1,IS))
        A(3,2,3)=IM*(R(1,1)-R(4,4)-M*V(4,IS))
        A(4,2,3)=IM*(R(1,2)-R(4,3)-M*V(3,IS))
*AS 24
        A(1,2,4)=IM*(-R(1,1)+R(3,2)+M*V(4,IS))
        A(2,2,4)=IM*(R(1,2)+R(3,1)-M*V(3,IS))
        A(3,2,4)=IM*(R(1,3)-R(3,4)+M*V(2,IS))
        A(4,2,4)=IM*(-R(1,4)-R(3,3)-M*V(1,IS))
*AS 34
        A(1,3,4)=R(1,2)-IM*R(2,2)-M*V(3,IS)
        A(2,3,4)=-R(1,1)-IM*R(2,1)+M*V(4,IS)
        A(3,3,4)=-R(1,4)+IM*R(2,4)-M*V(1,IS)
        A(4,3,4)=R(1,3)+IM*R(2,3)+M*V(2,IS)
*
        DO L=1,4
        A(L,2,1)=-A(L,1,2)
        A(L,3,2)=-A(L,2,3)
        A(L,4,1)=-A(L,1,4)
        A(L,3,1)=-A(L,1,3)
        A(L,4,2)=-A(L,2,4)
        A(L,4,3)=-A(L,3,4)
        A(L,1,1)=0.
        A(L,2,2)=0.
        A(L,3,3)=0.
        A(L,4,4)=0.
        END DO
*
*
        DO I=1,4
        DO K=1,4
        DO L=1,4
        H(I,K,L,IS)=S(I,K,L)+A(I,K,L)
        END DO
        END DO
        END DO
*
        END DO
        RETURN
        END
*
*
*
        SUBROUTINE CUR2(U,H,C)
        COMPLEX*16 U(4,2),H(4,4,4,2),C(4,4,2,2)
        DO IS=1,2
        DO IH=1,2
        DO K=1,4
        DO M=1,4
        C(M,K,IS,IH)=U(1,IS)*H(1,M,K,IH)
     +              +U(2,IS)*H(2,M,K,IH)
     +              +U(3,IS)*H(3,M,K,IH)
     +              +U(4,IS)*H(4,M,K,IH)
        END DO
        END DO
        END DO
        END DO
*
        RETURN
        END
*
*
*


        SUBROUTINE MESONG_G(P1,P2,SQMATRSCL,SQMATRVCT)
        IMPLICIT REAL*8(S)
        REAL*8 Q1P1(4),P1Q2(4),P2Q4(4),Q3P2(4),
     +         Q1P2(4),P2Q2(4),P1Q4(4),Q3P1(4),
     +         Q1(4),Q2(4),Q3(4),Q4(4),P1(4),P2(4),
     +         Q12(4),Q34(4),P12(4),K1(4),K2(4),
     +         Q1Q34(4),Q34Q2(4),Q12Q4(4),Q3Q12(4),
     +         K_M(38),MB,MC,G(4),D(2,2),
c     +         D(2,2),PB(4),PC(4),PBC(4),
     +         LAMBDA_D,LAMBDA_F,LAMBDA_S,VEC1(38),VEC2(38),VEC3(38),
     +         SQMATRSCL(3),INTER,SQMATRVCT(3)
        COMPLEX*16 M(38,2,2),
     +             UB(4,2),VB(4,2),UC(4,2),VC(4,2),
     +             JB(4,2,2),JC(4,2,2),
     +             B_Q1P1(4,4,2),B_Q1Q34(4,4,2),
     +             B_Q1P2(4,4,2),B_Q3Q12(4,4,2),
     +             B_Q3P1(4,4,2),B_Q3P2(4,4,2),
     +             H_Q1P1(4,4,4,2),H_P1Q2(4,4,4,2),
     +             H_P2Q4(4,4,4,2),H_Q3P2(4,4,4,2),
     +             H_Q1P2(4,4,4,2),H_P2Q2(4,4,4,2),
     +             H_P1Q4(4,4,4,2),H_Q3P1(4,4,4,2),
     +             H_Q12Q4(4,4,4,2),H_Q34Q2(4,4,4,2),
c     +             H_Q3Q12(4,4,4,2),H_Q1Q34(4,4,4,2),
     +             C_Q12Q4(4,4,2,2),C_Q34Q2(4,4,2,2),
     +             C_Q3Q12(4,4,2,2),C_Q1Q34(4,4,2,2),
     +             C_Q1P1(4,4,2,2),C_P1Q2(4,4,2,2),
     +             C_P2Q4(4,4,2,2),C_Q3P2(4,4,2,2),
     +             C_Q1P2(4,4,2,2),C_P2Q2(4,4,2,2),
     +             C_P1Q4(4,4,2,2),C_Q3P1(4,4,2,2),
     +             C_M23(4,4,4,2,2),C_M24(4,4,4,2,2),
     +             C_M25(4,4,4,2,2),C_M26(4,4,4,2,2),
     +             C_M27(4,4,4,2,2),C_M28(4,4,4,2,2),
     +             C_M29(4,4,4,2,2),C_M30(4,4,4,2,2),
     +             C_M31(4,4,4,2,2),C_M32(4,4,4,2,2),
     +             C_M33(4,4,4,2,2),C_M34(4,4,4,2,2),
     +             B_C(2,2,2,2),
     +             XXVCT1(3),XXVCT2(3),XXVCT3(3),
     +             XXSCL1,XXSCL2,XXSCL3,
     +             G_P1Q34(4,2,2,2),G_P2Q34(4,2,2,2),
     +             G_P1Q12(4,2,2,2),G_P2Q12(4,2,2,2),
     +             G_P(4,2,2),G_Q(4,2,2,2,2)
        COMMON/SPN/UB,VB,UC,VC
        COMMON/CRNT/JB,JC
        COMMON/M/MB,MC
        COMMON/P/Q1,Q2,Q3,Q4
        COMMON/G_Q/G_Q,B_C
        COMMON/MOM/Q12,Q34,P12,K1,K2,Q34Q2,Q1Q34,
     +             Q3Q12,Q12Q4
        COMMON/S/SQ12,SQ34,SP12,SK1,SK2,SQ34Q2,SQ1Q34,
     +             SQ3Q12,SQ12Q4
        COMMON/H/H_Q34Q2,H_Q12Q4,C_Q34Q2,C_Q1Q34,C_Q3Q12,
     +           C_Q12Q4
        COMMON/C_GG/LAMBDA_D,LAMBDA_F,LAMBDA_S,VEC1,VEC2,VEC3
        DATA G/-1.,-1.,-1.,1./
        DATA D/-1.,0.,0.,-1./

        DO J=1,4
        Q1P1(J)=Q1(J)-P1(J)
        P1Q2(J)=P1(J)-Q2(J)
        P2Q4(J)=P2(J)-Q4(J)
        Q3P2(J)=Q3(J)-P2(J)
        Q1P2(J)=Q1(J)-P2(J)
        P2Q2(J)=P2(J)-Q2(J)
        P1Q4(J)=P1(J)-Q4(J)
        Q3P1(J)=Q3(J)-P1(J)
        END DO

        CALL INV(Q1P1,SQ1P1)
        CALL INV(P1Q2,SP1Q2)
        CALL INV(P2Q4,SP2Q4)
        CALL INV(Q3P2,SQ3P2)
        CALL INV(Q1P2,SQ1P2)
        CALL INV(P2Q2,SP2Q2)
        CALL INV(P1Q4,SP1Q4)
        CALL INV(Q3P1,SQ3P1)

        SQ1P1=SQ1P1-MB*MB
        SP1Q2=SP1Q2-MB*MB
        SQ3P1=SQ3P1-MC*MC
        SP1Q4=SP1Q4-MC*MC
        SQ1P2=SQ1P2-MB*MB
        SP2Q2=SP2Q2-MB*MB
        SQ3P2=SQ3P2-MC*MC
        SP2Q4=SP2Q4-MC*MC

        K_M(1)=1./SQ12/SQ34/SK1
        K_M(2)=1./SQ12/SQ34/SK2
        K_M(3)=1./SQ12/SQ34/SP12
        K_M(4)=1./SQ12/SQ34
        K_M(5)=K_M(4)
        K_M(6)=K_M(4)
        K_M(7)=1./SQ1P1/SQ34/SK1
        K_M(8)=1./SP1Q2/SQ34/SK1
        K_M(9)=1./SQ3P1/SQ12/SK2
        K_M(10)=1./SP1Q4/SQ12/SK2
        K_M(11)=1./SP2Q4/SQ12/SK1
        K_M(12)=1./SQ3P2/SQ12/SK1
        K_M(13)=1./SP2Q2/SQ34/SK2
        K_M(14)=1./SQ1P2/SQ34/SK2
        K_M(15)=1./SQ1P1/SK1/SP2Q4
        K_M(16)=1./SP1Q2/SK1/SP2Q4
        K_M(17)=1./SQ1P1/SK1/SQ3P2
        K_M(18)=1./SP1Q2/SK1/SQ3P2
        K_M(19)=1./SP2Q2/SK2/SQ3P1
        K_M(20)=1./SP2Q2/SK2/SP1Q4
        K_M(21)=1./SQ1P2/SK2/SQ3P1
        K_M(22)=1./SQ1P2/SK2/SP1Q4
        K_M(23)=1./SQ1Q34/SP2Q2/SQ34
        K_M(24)=1./SQ1P1/SP2Q2/SQ34
        K_M(25)=1./SQ1P1/SQ34Q2/SQ34
        K_M(26)=1./SQ1Q34/SP1Q2/SQ34
        K_M(27)=1./SQ1P2/SP1Q2/SQ34
        K_M(28)=1./SQ1P2/SQ34Q2/SQ34
        K_M(29)=1./SQ3Q12/SP2Q4/SQ12
        K_M(30)=1./SQ3P1/SP2Q4/SQ12
        K_M(31)=1./SQ3P1/SQ12Q4/SQ12
        K_M(32)=1./SQ3Q12/SP1Q4/SQ12
        K_M(33)=1./SQ3P2/SP1Q4/SQ12
        K_M(34)=1./SQ3P2/SQ12Q4/SQ12
        K_M(35)=1./SQ1Q34/SP12/SQ34
        K_M(36)=1./SQ34Q2/SP12/SQ34
        K_M(37)=1./SQ3Q12/SP12/SQ12
        K_M(38)=1./SQ12Q4/SP12/SQ12


       DO IG1=1,2
       DO IG2=1,2
       DO I=1,4
       G_P(I,IG1,IG2)=0.
       END DO
       END DO
       END DO
       
        DO IG1=1,2
        G_P(1,IG1,IG1)=P1(1)-P2(1)
        G_P(2,IG1,IG1)=P1(2)-P2(2)
        G_P(3,IG1,IG1)=P1(3)-P2(3)
        G_P(4,IG1,IG1)=-P1(4)+P2(4)
        DO IG2=1,2
        G_P(IG1,IG1,IG2)=G_P(IG1,IG1,IG2)-2*P1(IG2)-P2(IG2)
        G_P(IG2,IG1,IG2)=G_P(IG2,IG1,IG2)+2*P2(IG1)+P1(IG1)
        END DO
        END DO

        CALL GLUON(P1,Q34,JC,G_P1Q34)
        CALL GLUON(P2,Q34,JC,G_P2Q34)
        CALL GLUON(P1,Q12,JB,G_P1Q12)
        CALL GLUON(P2,Q12,JB,G_P2Q12)

        CALL HLP(Q1P1,MB,VB,H_Q1P1)
        CALL CUR2(UB,H_Q1P1,C_Q1P1)
        CALL HLP(P1Q2,MB,VB,H_P1Q2)
        CALL CUR2(UB,H_P1Q2,C_P1Q2)
        CALL HLP(P2Q4,MC,VC,H_P2Q4)
        CALL CUR2(UC,H_P2Q4,C_P2Q4)
        CALL HLP(Q3P2,MC,VC,H_Q3P2)
        CALL CUR2(UC,H_Q3P2,C_Q3P2)

        CALL HLP(Q1P2,MB,VB,H_Q1P2)
        CALL CUR2(UB,H_Q1P2,C_Q1P2)
        CALL HLP(P2Q2,MB,VB,H_P2Q2)
        CALL CUR2(UB,H_P2Q2,C_P2Q2)
        CALL HLP(P1Q4,MC,VC,H_P1Q4)
        CALL CUR2(UC,H_P1Q4,C_P1Q4)
        CALL HLP(Q3P1,MC,VC,H_Q3P1)
        CALL CUR2(UC,H_Q3P1,C_Q3P1)

        CALL H_BAR(UB,Q1P1,MB,B_Q1P1)
        CALL H_BAR(UB,Q1Q34,MB,B_Q1Q34)
        CALL H_BAR(UB,Q1P2,MB,B_Q1P2)
        CALL H_BAR(UC,Q3Q12,MC,B_Q3Q12)
        CALL H_BAR(UC,Q3P1,MC,B_Q3P1)
        CALL H_BAR(UC,Q3P2,MC,B_Q3P2)

        CALL CUR3(B_Q1P1,H_P2Q2,C_M24)
        CALL CUR3(B_Q1Q34,H_P2Q2,C_M23)
        CALL CUR3(B_Q1P1,H_Q34Q2,C_M25)
        CALL CUR3(B_Q1Q34,H_P1Q2,C_M26)
        CALL CUR3(B_Q1P2,H_P1Q2,C_M27)
        CALL CUR3(B_Q1P2,H_Q34Q2,C_M28)
        CALL CUR3(B_Q3Q12,H_P2Q4,C_M29)
        CALL CUR3(B_Q3P1,H_P2Q4,C_M30)
        CALL CUR3(B_Q3P1,H_Q12Q4,C_M31)
        CALL CUR3(B_Q3Q12,H_P1Q4,C_M32)
        CALL CUR3(B_Q3P2,H_P1Q4,C_M33)
        CALL CUR3(B_Q3P2,H_Q12Q4,C_M34)

        DO I=1,3
        SQMATRVCT(I)=0.
        SQMATRSCL(I)=0.
       END DO
       INTERVCT=0.
       INTERSCL=0.

        DO IG1=1,2
        DO IG2=1,2
        DO IVB=1,2
        DO IUC=1,2


        DO IUB=1,2
        DO IVC=1,2

        DO I=1,38
        M(I,IUB,IVC)=0.
        END DO

        M(4,IUB,IVC)=JB(IG2,IUB,IVB)*JC(IG1,IUC,IVC)
     +      -D(IG1,IG2)*B_C(IUB,IVB,IUC,IVC)
        M(5,IUB,IVC)=-(JB(IG1,IUB,IVB)*JC(IG2,IUC,IVC))
     +       +D(IG1,IG2)*B_C(IUB,IVB,IUC,IVC)
        M(6,IUB,IVC)=JB(IG1,IUB,IVB)*JC(IG2,IUC,IVC)
     +      -JB(IG2,IUB,IVB)*JC(IG1,IUC,IVC)

        DO J=1,4
        M(1,IUB,IVC)=
     +        G_P2Q34(J,IG2,IUC,IVC)*G_P1Q12(J,IG1,IUB,IVB)*G(J)
     +       +M(1,IUB,IVC)
        M(2,IUB,IVC)=
     +        G_P1Q34(J,IG1,IUC,IVC)*G_P2Q12(J,IG2,IUB,IVB)*G(J)
     +       +M(2,IUB,IVC)
        M(3,IUB,IVC)=
     +        G_P(J,IG1,IG2)*G_Q(J,IUB,IVB,IUC,IVC)*G(J)
     +       +M(3,IUB,IVC)
        M(7,IUB,IVC)=
     +       -(C_Q1P1(IG1,J,IUB,IVB)*G_P2Q34(J,IG2,IUC,IVC))
     +       +M(7,IUB,IVC)
        M(8,IUB,IVC)=
     +       -(C_P1Q2(J,IG1,IUB,IVB)*G_P2Q34(J,IG2,IUC,IVC))
     +       +M(8,IUB,IVC)
        M(9,IUB,IVC)=
     +       -(C_Q3P1(IG1,J,IUC,IVC)*G_P2Q12(J,IG2,IUB,IVB))
     +       +M(9,IUB,IVC)
        M(10,IUB,IVC)=
     +       -(C_P1Q4(J,IG1,IUC,IVC)*G_P2Q12(J,IG2,IUB,IVB))
     +       +M(10,IUB,IVC)
        M(11,IUB,IVC)=
     +       -(C_P2Q4(J,IG2,IUC,IVC)*G_P1Q12(J,IG1,IUB,IVB))
     +       +M(11,IUB,IVC)
        M(12,IUB,IVC)=
     +       -(C_Q3P2(IG2,J,IUC,IVC)*G_P1Q12(J,IG1,IUB,IVB))
     +       +M(12,IUB,IVC)
        M(13,IUB,IVC)=
     +       -(C_P2Q2(J,IG2,IUB,IVB)*G_P1Q34(J,IG1,IUC,IVC))
     +       +M(13,IUB,IVC)
        M(14,IUB,IVC)=
     +       -(C_Q1P2(IG2,J,IUB,IVB)*G_P1Q34(J,IG1,IUC,IVC))
     +       +M(14,IUB,IVC)
        M(15,IUB,IVC)=
     +        C_Q1P1(IG1,J,IUB,IVB)*C_P2Q4(J,IG2,IUC,IVC)*G(J)
     +       +M(15,IUB,IVC)
        M(16,IUB,IVC)=
     +        C_P1Q2(J,IG1,IUB,IVB)*C_P2Q4(J,IG2,IUC,IVC)*G(J)
     +       +M(16,IUB,IVC)
        M(17,IUB,IVC)=
     +        C_Q1P1(IG1,J,IUB,IVB)*C_Q3P2(IG2,J,IUC,IVC)*G(J)
     +       +M(17,IUB,IVC)
        M(18,IUB,IVC)=
     +        C_P1Q2(J,IG1,IUB,IVB)*C_Q3P2(IG2,J,IUC,IVC)*G(J)
     +       +M(18,IUB,IVC)
        M(19,IUB,IVC)=
     +        C_P2Q2(J,IG2,IUB,IVB)*C_Q3P1(IG1,J,IUC,IVC)*G(J)
     +       +M(19,IUB,IVC)
        M(20,IUB,IVC)=
     +        C_P2Q2(J,IG2,IUB,IVB)*C_P1Q4(J,IG1,IUC,IVC)*G(J)
     +       +M(20,IUB,IVC)
        M(21,IUB,IVC)=
     +        C_Q1P2(IG2,J,IUB,IVB)*C_Q3P1(IG1,J,IUC,IVC)*G(J)
     +       +M(21,IUB,IVC)
        M(22,IUB,IVC)=
     +        C_Q1P2(IG2,J,IUB,IVB)*C_P1Q4(J,IG1,IUC,IVC)*G(J)
     +       +M(22,IUB,IVC)
        M(23,IUB,IVC)=C_M23(J,IG1,IG2,IUB,IVB)*JC(J,IUC,IVC)
     +               +M(23,IUB,IVC)
        M(24,IUB,IVC)=C_M24(IG1,J,IG2,IUB,IVB)*JC(J,IUC,IVC)
     +               +M(24,IUB,IVC)
        M(25,IUB,IVC)=C_M25(IG1,IG2,J,IUB,IVB)*JC(J,IUC,IVC)
     +               +M(25,IUB,IVC)
        M(26,IUB,IVC)=C_M26(J,IG2,IG1,IUB,IVB)*JC(J,IUC,IVC)
     +               +M(26,IUB,IVC)
        M(27,IUB,IVC)=C_M27(IG2,J,IG1,IUB,IVB)*JC(J,IUC,IVC)
     +               +M(27,IUB,IVC)
        M(28,IUB,IVC)=C_M28(IG2,IG1,J,IUB,IVB)*JC(J,IUC,IVC)
     +               +M(28,IUB,IVC)
        M(29,IUB,IVC)=C_M29(J,IG1,IG2,IUC,IVC)*JB(J,IUB,IVB)
     +               +M(29,IUB,IVC)
        M(30,IUB,IVC)=C_M30(IG1,J,IG2,IUC,IVC)*JB(J,IUB,IVB)
     +               +M(30,IUB,IVC)
        M(31,IUB,IVC)=C_M31(IG1,IG2,J,IUC,IVC)*JB(J,IUB,IVB)
     +               +M(31,IUB,IVC)
        M(32,IUB,IVC)=C_M32(J,IG2,IG1,IUC,IVC)*JB(J,IUB,IVB)
     +               +M(32,IUB,IVC)
        M(33,IUB,IVC)=C_M33(IG2,J,IG1,IUC,IVC)*JB(J,IUB,IVB)
     +               +M(33,IUB,IVC)
        M(34,IUB,IVC)=C_M34(IG2,IG1,J,IUC,IVC)*JB(J,IUB,IVB)
     +               +M(34,IUB,IVC)

        DO I=1,4
        M(35,IUB,IVC)=
     +      C_Q1Q34(I,J,IUB,IVB)*JC(I,IUC,IVC)*G_P(J,IG1,IG2)
     +     +M(35,IUB,IVC)
        M(36,IUB,IVC)=
     +      C_Q34Q2(J,I,IUB,IVB)*JC(I,IUC,IVC)*G_P(J,IG1,IG2)
     +     +M(36,IUB,IVC)
        M(37,IUB,IVC)=
     +      C_Q3Q12(I,J,IUC,IVC)*JB(I,IUB,IVB)*G_P(J,IG1,IG2)
     +     +M(37,IUB,IVC)
        M(38,IUB,IVC)=
     +      C_Q12Q4(J,I,IUC,IVC)*JB(I,IUB,IVB)*G_P(J,IG1,IG2)
     +     +M(38,IUB,IVC)
        END DO
        END DO

        END DO
        END DO

        DO J=1,38
        M(J,1,1)=K_M(J)*(M(J,1,1)+M(J,2,2))
        M(J,1,2)=K_M(J)*M(J,1,2)
        M(J,2,1)=K_M(J)*M(J,2,1)
        M(J,2,2)=M(J,1,1)-2.*K_M(J)*M(J,2,2)
        END DO

        DO I=1,3
        XXVCT1(I)=0.
        XXVCT2(I)=0.
        XXVCT3(I)=0.
        END DO

        XXSCL1=0.
        XXSCL2=0.
        XXSCL3=0.

        DO J=1,38
        XXVCT1(1)=VEC1(J)*M(J,1,1)+XXVCT1(1)
        XXVCT1(2)=VEC1(J)*M(J,1,2)+XXVCT1(2)
        XXVCT1(3)=VEC1(J)*M(J,2,1)+XXVCT1(3)

        XXVCT2(1)=VEC2(J)*M(J,1,1)+XXVCT2(1)
        XXVCT2(2)=VEC2(J)*M(J,1,2)+XXVCT2(2)
        XXVCT2(3)=VEC2(J)*M(J,2,1)+XXVCT2(3)

        XXVCT3(1)=VEC3(J)*M(J,1,1)+XXVCT3(1)
        XXVCT3(2)=VEC3(J)*M(J,1,2)+XXVCT3(2)
        XXVCT3(3)=VEC3(J)*M(J,2,1)+XXVCT3(3)

        XXSCL1=VEC1(J)*M(J,2,2)+XXSCL1
        XXSCL2=VEC2(J)*M(J,2,2)+XXSCL2
        XXSCL3=VEC3(J)*M(J,2,2)+XXSCL3
        END DO

        INTER=
     +   (LAMBDA_D-LAMBDA_F)*(
     +                        XXVCT1(1)*DCONJG(XXVCT2(1))/2.
     +                       +XXVCT1(2)*DCONJG(XXVCT2(2))
     +                       +XXVCT1(3)*DCONJG(XXVCT2(3))
     +                                                   )
     +  +(LAMBDA_D-LAMBDA_F)*(
     +                        XXVCT2(1)*DCONJG(XXVCT1(1))/2.
     +                       +XXVCT2(2)*DCONJG(XXVCT1(2))
     +                       +XXVCT2(3)*DCONJG(XXVCT1(3))
     +                                                   )
 
        SQMATRVCT(1)=SQMATRVCT(1)+INTER/2.
     +  +(LAMBDA_D+LAMBDA_F)*(
     +                        XXVCT1(1)*DCONJG(XXVCT1(1))/2.
     +                       +XXVCT1(2)*DCONJG(XXVCT1(2))
     +                       +XXVCT1(3)*DCONJG(XXVCT1(3))  
     +                                                   )
        
        SQMATRVCT(2)=SQMATRVCT(2)+INTER/2.
     +  +(LAMBDA_D+LAMBDA_F)*(
     +                        XXVCT2(1)*DCONJG(XXVCT2(1))/2.
     +                       +XXVCT2(2)*DCONJG(XXVCT2(2))
     +                       +XXVCT2(3)*DCONJG(XXVCT2(3))
     +                                                   )

        SQMATRVCT(3)=SQMATRVCT(3)
     +  +LAMBDA_S*(
     +             XXVCT3(1)*DCONJG(XXVCT3(1))/2.
     +            +XXVCT3(2)*DCONJG(XXVCT3(2))
     +            +XXVCT3(3)*DCONJG(XXVCT3(3))
     +                                        )

       INTER=
     +   ((LAMBDA_D-LAMBDA_F)*XXSCL1*DCONJG(XXSCL2)
     +   +(LAMBDA_D-LAMBDA_F)*XXSCL2*DCONJG(XXSCL1))/2.                                         
               
        SQMATRSCL(1)=SQMATRSCL(1)+INTER/2.
     +   +(LAMBDA_D+LAMBDA_F)*XXSCL1*DCONJG(XXSCL1)/2.
     
        SQMATRSCL(2)=SQMATRSCL(2)+INTER/2.
     +   +(LAMBDA_D+LAMBDA_F)*XXSCL2*DCONJG(XXSCL2)/2.
     
        SQMATRSCL(3)=SQMATRSCL(3)
     +   + LAMBDA_S*XXSCL3*DCONJG(XXSCL3)/2.

        END DO
        END DO
        END DO
        END DO

        RETURN
        END


        BLOCK DATA BCCONS
        REAL*8 LAMBDA_D,LAMBDA_F,LAMBDA_S,VEC1(38),VEC2(38),VEC3(38)
        COMMON/C_GG/LAMBDA_D,LAMBDA_F,LAMBDA_S,VEC1,VEC2,VEC3
        DATA LAMBDA_D,LAMBDA_F,LAMBDA_S,VEC1,VEC2,VEC3/
     +  6.66666667, 12., 24.,
c   g1->c g2->anti-b
     +  0.,
     + -0.75,
     +  0.75,
     +  0.,
     + -0.75,
     +  0.75,
     +  0.,
     +  0.,
     +  0.75,
     +  0.,
     +  0.,
     +  0.,
     + -0.75,
     +  0.,
     +  0.,
     +  0.,
     +  0.,
     +  0.,
     +  0.666666667,
     + -0.0833333333,
     + -0.0833333333,
     + -0.0833333333,
     +  0.666666667,
     + -0.0833333333,
     + -0.0833333333,
     +  0.,
     +  0.,
     +  0.,
     + -0.0833333333,
     + -0.0833333333,
     +  0.666666667,
     +  0.,
     +  0.,
     +  0.,
     +  0.666666667,
     + -0.0833333333,
     + -0.0833333333,
     +  0.666666667,
c   g1->anti-b g2->c
     + -0.75,
     +  0.,
     + -0.75,
     +  0.75,
     +  0.,
     + -0.75,
     +  0.,
     + -0.75,
     +  0.,
     +  0.,
     +  0.,
     +  0.75,
     +  0.,
     +  0.,
     + -0.0833333333,
     + -0.0833333333,
     + -0.0833333333,
     +  0.666666667,
     +  0.,
     +  0.,
     +  0.,
     +  0.,
     +  0.,
     +  0.,
     +  0.,
     +  0.666666667,
     + -0.0833333333,
     + -0.0833333333,
     +  0.,
     +  0.,
     +  0.,
     + -0.0833333333,
     + -0.0833333333,
     +  0.666666667,
     + -0.666666667,
     +  0.0833333333,
     +  0.0833333333,
     + -0.666666667,
c   singlet
     + -0.5,
     + -0.5,
     +  0.,
     +  0.5,
     + -0.5,
     +  0.,
     +  0.25,
     + -0.25,
     +  0.25,
     + -0.25,
     + -0.25,
     +  0.25,
     + -0.25,
     +  0.25,
     +  0.222222222,
     + -0.027777778,
     + -0.027777778,
     +  0.222222222,
     +  0.222222222,
     + -0.027777778,
     + -0.027777778,
     +  0.222222222,
     +  0.222222222,
     + -0.027777778,
     +  0.222222222,
     +  0.222222222,
     + -0.027777778,
     +  0.222222222,
     +  0.222222222,
     + -0.027777778,
     +  0.222222222,
     +  0.222222222,
     + -0.027777778,
     +  0.222222222,
     +  0.,
     +  0.,
     +  0.,
     +  0./
        END


        SUBROUTINE H_BAR(U,P,M,HLP)
        COMPLEX*16 U(4,2),HLP(4,4,2),R(4,4),IM
        REAL*8 M,P(4)
        IM=DCMPLX(0.D0,1.D0)
*
        DO IS=1,2
*
        DO I=1,4
        DO J=1,4
        R(I,J)=P(I)*U(J,IS)
        END DO
        END DO
*1*
        HLP(1,1,IS)=R(1,1)+IM*R(2,1)+R(3,2)-R(4,4)-M*U(4,IS)
        HLP(2,1,IS)=R(1,2)-IM*R(2,2)-R(3,1)-R(4,3)-M*U(3,IS)
        HLP(3,1,IS)=R(1,3)+IM*R(2,3)+R(3,4)-R(4,2)+M*U(2,IS)
        HLP(4,1,IS)=R(1,4)-IM*R(2,4)-R(3,3)-R(4,1)+M*U(1,IS)
*2*
        HLP(1,2,IS)=-(IM*R(1,1))+R(2,1)+IM*(R(3,2)-R(4,4)-M*U(4,IS))
        HLP(2,2,IS)=+IM*R(1,2)+R(2,2)+IM*(R(3,1)+R(4,3)+M*U(3,IS))
        HLP(3,2,IS)=-(IM*R(1,3))+R(2,3)+IM*(R(3,4)-R(4,2)+M*U(2,IS))
        HLP(4,2,IS)=+IM*R(1,4)+R(2,4)+IM*(R(3,3)+R(4,1)-M*U(1,IS))
*3*
        HLP(1,3,IS)=-R(1,2)-IM*R(2,2)+R(3,1)-R(4,3)-M*U(3,IS)
        HLP(2,3,IS)=+R(1,1)-IM*R(2,1)+R(3,2)+R(4,4)+M*U(4,IS)
        HLP(3,3,IS)=-R(1,4)-IM*R(2,4)+R(3,3)-R(4,1)+M*U(1,IS)
        HLP(4,3,IS)=+R(1,3)-IM*R(2,3)+R(3,4)+R(4,2)-M*U(2,IS)
*4*
        HLP(1,4,IS)=-R(1,4)-IM*R(2,4)-R(3,3)+R(4,1)+M*U(1,IS)
        HLP(2,4,IS)=-R(1,3)+IM*R(2,3)+R(3,4)+R(4,2)+M*U(2,IS)
        HLP(3,4,IS)=-R(1,2)-IM*R(2,2)-R(3,1)+R(4,3)-M*U(3,IS)
        HLP(4,4,IS)=-R(1,1)+IM*R(2,1)+R(3,2)+R(4,4)-M*U(4,IS)
*
        END DO
*
        RETURN
        END
*
*
*
        SUBROUTINE CUR3(B,H,C)
        COMPLEX*16 B(4,4,2),H(4,4,4,2),C(4,4,4,2,2)
        DO IS=1,2
        DO JS=1,2
        DO L1=1,4
        DO L2=1,4
        DO L3=1,4
        C(L1,L2,L3,IS,JS)=B(1,L1,IS)*H(1,L2,L3,JS)
     +                   +B(2,L1,IS)*H(2,L2,L3,JS)
     +                   +B(3,L1,IS)*H(3,L2,L3,JS)
     +                   +B(4,L1,IS)*H(4,L2,L3,JS)
        END DO
        END DO
        END DO
        END DO
        END DO
        RETURN
        END
*
*
*
        SUBROUTINE GLUON(P1,P2,J,G)
        REAL*8 P1(4),P2(4),P(4),Q(4),K(4)
        COMPLEX*16 J(4,2,2),G(4,2,2,2),R(2,2)
*
        DO I=1,4
        P(I)=2*P1(I)+P2(I)
        Q(I)=P1(I)-P2(I)
        K(I)=2*P2(I)+P1(I)
        END DO
*
        DO K1=1,2
        DO K2=1,2
        R(K1,K2)=0.
        DO I=1,4
        R(K1,K2)=R(K1,K2)+P(I)*J(I,K1,K2)
        END DO
        END DO
        END DO
*
        DO IG=1,2
        DO IU=1,2
        DO IV=1,2
        G(1,IG,IU,IV)=(-Q(1))*J(IG,IU,IV)-K(IG)*J(1,IU,IV)
        G(2,IG,IU,IV)=(-Q(2))*J(IG,IU,IV)-K(IG)*J(2,IU,IV)
        G(3,IG,IU,IV)=(-Q(3))*J(IG,IU,IV)-K(IG)*J(3,IU,IV)
        G(4,IG,IU,IV)=(+Q(4))*J(IG,IU,IV)-K(IG)*J(4,IU,IV)
        G(IG,IG,IU,IV)=G(IG,IG,IU,IV)+R(IU,IV)
        END DO
        END DO
        END DO
*
        RETURN
        END
