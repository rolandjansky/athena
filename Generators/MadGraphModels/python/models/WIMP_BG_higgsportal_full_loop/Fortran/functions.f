

      double complex function myGGH(ss)
      implicit none     
      include 'input.inc' ! include all model parameter
      INCLUDE 'coupl.inc'
      complex*16 c0
      external c0
      real*8 mb2, mt2,ss
      
      mt2=mdl_mt**2
      mb2=mdl_mb**2
            
      myGGH = 3d0/ss*(mdl_mt*mdl_ymt*(2d0 + (4d0*mt2 - ss)*c0(ss,mt2))
     &   +mdl_mb*mdl_ymb*(2d0 + (4d0*mb2 - ss)*c0(ss,mb2)))
      return 
      end
      
      double complex function myGGA(ss)
      implicit none     
      include 'input.inc' ! include all model parameter
      INCLUDE 'coupl.inc'
      complex*16 c0
      external c0
      real*8 mb2, mt2,ss
      
      mt2=mdl_mt**2
      mb2=mdl_mb**2
            
      myGGA = -2d0*(mdl_mb*mdl_ymb*c0(ss,mb2)+mdl_mt*mdl_ymt*c0(ss,mt2))
      return 
      end
      
      double complex function c0(ss,mass2)
      implicit none
      
      COMPLEX*16 IMAG1
      PARAMETER (IMAG1=(0D0,1D0))      
      real*8 pi,xx,ss,mass2
      parameter (pi=3.141592653589793d0)
      
      xx=4d0*mass2/ss
      
      if (xx.le.1d0) then
        c0=1d0/(2d0*ss)*
     &   (log((sqrt(1d0-xx)+1d0)/(1d0-sqrt(1d0-xx)))-imag1*pi)**2
      else
        c0=-2d0/ss*dasin(1d0/sqrt(xx))**2
      endif  
      
      end

      
      
      
