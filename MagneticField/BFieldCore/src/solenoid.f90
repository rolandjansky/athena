!-----------------------------------------------------------------------
subroutine solenoid(iploco,x,y,z,hsx,hsy,hsz,bsx,bsy,bsz)
! Autor : T.Nikitina, Universite de Besancon
! main subrotine torsol gives Hs [A/m] and Bs[T] of all coils
! (Toroids+solenoid) for the arbitrary point x,y,z[m]
! Parameters used for toroids(dimentions, position, current) are given in
! the module 'TOROIDINI' in the begining of the program.
! Parameters used for solenoid are given in the begining of the subroutine
! 'solen'.
IMPLICIT REAL(8) (A-H,O-Z)
external func,func1
       
common/istop/iterm
integer iterm
common/zres/zzs,re,rs
real(8) zzs,re,rs
common/ifors/ifor
integer ifor
common/mug/mu0
real(8) mu0
real(8) x,y,z,hsx,hsy,hsz,bsx,bsy,bsz
real(8) xm,ym,zm
real(8) alf,x11,x22,x33,btx,bty,btz,btxr,btyr
integer iploco
xm=x;ym=y;zm=z;
if(IPLOCO == 0.or.IPLOCO == 2) then
   call solen(xm,ym,zm,bsx,bsy,bsz)
!   if(mu0.ge.1.d-4)then
!      bsx=bsx/real(1.d-4);bsy=bsy/real(1.d-4);bsz=bsz/real(1.d-4)
!   endif
   hsx=bsx/mu0;hsy=bsy/mu0;hsz=bsz/mu0;
elseif(IPLOCO == 1 .or.IPLOCO == 3 ) then
    hsx=0.;hsy=0.;hsz=0.
   bsx=0.;bsy=0.;bsz=0.
!   call bg_init
!   call bg_realinit
!   irmod = 2
!   call bg_distort
!   call atlm_init
!   call SolePaul(xm, ym, zm, bsx, bsy, bsz)
elseif(IPLOCO == 4) then
   hsx=0.;hsy=0.;hsz=0.
   bsx=0.;bsy=0.;bsz=0.
endif
          
return
end

!-----------------------------------------------------------------------
subroutine solen(x,y,z,bx,by,bz)
! Autor : T.Nikitina, Universite de Besancon
!	gives B [T] of central solenoid for the arbitrary point x,y,z [m]. 
!   The field of solenoid is calculated used the formulas from the article
!   M.Gyimesi,D.Lavers,T.Pawlak,D.Ostergaard
!  'Biot-Savart Integration for Bars and Arcs'
!   IEEE Transactions on magnetics,vol.29,n6,november 1993 
!   For the integral calculations the adaptive recursive Simpson quadrature
!   was used. The programation is based on the paper
!   W.Gander and W.Gautschi' Adaptive Quadrature - Revisited',BIT,Vol.40,2000
!    pp.84-101  

IMPLICIT REAL(8) (A-H,O-Z)
external func,func1       
common/istop/iterm
integer iterm
common/ifors/ifor
integer ifor
common/zres/zzs,re,rs
real(8) zzs,re,rs
real(8) a,b,tol,j,zr,r1,r2,br,bz
real(8) x,y,z,bx,by,alfa,zend
        a=0.;b=datan(real(1.,8))*real(4.,8)
        tol=1.d-8;
        zend=real(2.6455,8);r2=real(1.2613,8);r1=real(1.2314,8)
               zr=z;rs=r2;zz=zend-zr;re=dsqrt(x*x+y*y)
		! ifor = the switch on the simplified formula for solenoid field
!        in ordre to avoid the cancelation errors away from solenoid
! ifor=1 for complete formula in the the solenoid and nearby
! ifor=0 for simplified formula away from solenoid
		ifor=1
		if(dsqrt(re*re+zr*zr).ge.8.1)ifor=0		         		         
        call sol1(zend,r1,r2,zr,a,b,tol,br,bz)
        alfa = atan2(y,x)
        bx   = br*cos(alfa)
        by   = br*sin(alfa)

        !if(x.eq.0.)then
		 !  alfa=real(89.9999999,8);if(y.lt.real(0.,8))alfa=-alfa
        !else
        !   alfa=datande(y/x) 
		!   if(x.lt.real(0.,8))alfa=real(180.,8)+alfa 	                
        !endif
        !bx=br*dcosde(alfa)
        !by=br*dsinde(alfa)
		
return	       
end
!------------------------------------------------------------------------
subroutine sol1(zend,r1,r2,zr,a,b,tol,br,bz)
! Autor : T.Nikitina, Universite de Besancon
! subroutine called by subroutine solen, performants the first step of
! the adaptive recursive Simpson quadrature.
IMPLICIT REAL(8) (A-H,O-Z)
external func,func1 
common/zres/zzs,re,rs
real(8) zzs,re,rs
common/ifors/ifor
integer ifor
real(8) zend,r1,r2,zr,br,bz,a,b,tol,j,eps
real(8) qr22,qz22,qr12,qz12,qr21,qz21,qr11,qz11
	       eps=1.d-07 
!!!          j=real(5.5438,8)
!!!          j=real(5.5421,8) to be in agremment with Paul calculation a z=0,r=0
!!! 7600amp -> 7730 then 5.5421 * 7730/7600
             j=real(5.6369,8)
            
	      if(ifor.eq.0)j=j*1.8843/1.5119
	       zzs=zend-zr
	       if(dabs(zend-zr).lt.eps)zzs=eps
	       rs=r2
	       if(dabs(r2-re).lt.eps)rs=r2-eps
	       qr22=simp(func,a,b,tol)
	       qz22=simp(func1,a,b,tol)
	       rs=r1
	       if(dabs(r1-re).lt.eps)rs=r1+eps
	       qr12=simp(func,a,b,tol)
	       qz12=simp(func1,a,b,tol)

	       zzs=-zend-zr
	       if(dabs(-zend-zr).lt.eps)zzs=-eps
	       rs=r2
	       if(dabs(r2-re).lt.eps)rs=r2-eps
	       qr21=simp(func,a,b,tol)
	       qz21=simp(func1,a,b,tol)
	       rs=r1
	       if(dabs(r1-re).lt.eps)rs=r1+eps 
	       qr11=simp(func,a,b,tol)
	       qz11=simp(func1,a,b,tol)

	       br=real(2.,8)*(qr22-qr12-qr21+qr11)*j
	       bz=real(2.,8)*(qz22-qz12-qz21+qz11)*j
return
end
!------------------------------------------------------------------
real(8) function func(x)
! This function, integrated by x on [0;2*pi], gives the br component of 
! solenoid field
! 
IMPLICIT REAL(8) (A-H,O-Z)
common/zres/zzs,re,rs
real(8) zzs,re,rs
real(8) ro,x
	       ro=dsqrt(zzs*zzs+re*re+rs*rs-2.*rs*re*cos(x)) 
	       func=(ro+re*dcos(x)*dlog(ro+rs-re*dcos(x)))*dcos(x)
end function
!------------------------------------------------------------------
real(8) function func1(x)
! This function, integrated by x on [0;2*pi], gives the bz component of 
! solenoid field
IMPLICIT REAL(8) (A-H,O-Z)
common/zres/zzs,re,rs
real(8) zzs,re,rs
real(8) ro,g1,g2,g3,b3
               ro=dsqrt(zzs*zzs+re*re+rs*rs-real(2.,8)*rs*re*cos(x))
               g1=zzs*dlog(ro+rs-re*dcos(x))
               g2=re*dcos(x)*dlog(ro+zzs)
               b3=zzs*(rs-re*dcos(x))/(re*ro*sin(x))
               g3=re*dsin(x)*datan(b3)
               func1=g1-g2-g3
end function
!------------------------------------------------------------------
real(8) function simp(func,a,b,tol)
! adaptive recursive Simpson quadrature based on paper
! W.Gander and W.Gautschi' Adaptive Quadrature - Revisited',BIT,Vol.40,2000
!    pp.84-101  
IMPLICIT REAL(8) (A-H,O-Z)
external func  
common/istop/iterm
integer iterm
common/zres/zzs,re,rs
real(8) zzs,re,rs

real(8) a,b,tol
real(8) xa,xm,xb,fa,fm,fb,amc(5),sum,is,eps
	
    amc(1)=real(.9501,8);amc(2)=real(.2311,8);amc(3)=real(.6068,8)
	amc(4)=real(.4860,8);amc(5)=real(.8913,8)

	iterm=0
	eps=real(2.2204d-016,8) 
    xa=a;xb=b;xm=(a+b)/2.
	fa=func(xa);fb=func(xb);fm=func(xm)
	sum=0.
	do i=1,5
	   sum=sum+func(a+amc(i)*(b-a))
    enddo
	is=(b-a)/8.*(fa+fm+fb+sum)
	if(is.eq.0.)is=b-a;
    is=is*tol/eps
	
    simp=simrec(func,a,b,fa,fm,fb,is)

end function
!-------------------------------------------------------------------
real(8) recursive function simrec(func,a,b,fa,fm,fb,is) result(bbb)
! one step of adaptive recursive Simpson quadrature based on paper
! W.Gander and W.Gautschi' Adaptive Quadrature - Revisited',BIT,Vol.40,2000
!    pp.84-101  
IMPLICIT REAL(8) (A-H,O-Z)
external func   
common/istop/iterm
integer iterm
common/zres/zz,re,rs
real(8) zz,re,rs
real(8) a,b,fa,fb,fm,is
real(8) m,h,x1,x2,fm1,fm2,fm3,i1,i2
real(8) dm1,dm2
      
    m=(a+b)/2.;h=(b-a)/4.;x1=a+h;x2=b-h
	fm1=func(x1);fm2=func(x2)
	i1=h/1.5*(fa+4.*fm+fb)
	i2=h/3.*(fa+4.*(fm1+fm2)+2.*fm+fb)

	i1=(16.*i2-i1)/15.
      
	if((is+(i1-i2)).eq.is.or.(m.le.a).or.(b.le.m))then
          if((m.le.a).or.(b.le.m).and.(iterm.eq.0))then
	          !print *,'WARNING!!! Interval contains no more numbers'
	          iterm=1
	      endif
!          simrec=i1
           bbb=i1
	else
!	simrec=simrec(func,a,m,fa,fm1,fm,is)+simrec(func,m,b,fm,fm2,fb,is)
        dm1=simrec(func,a,m,fa,fm1,fm,is)
        dm2=simrec(func,m,b,fm,fm2,fb,is)
        bbb=dm1+dm2
	endif

end function
