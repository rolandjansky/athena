c  **************************************************
c  *                                                *
c  *  Breit-Wigner function for intermediate mesons *
c  *                                                *
c  **************************************************
      function BW(s2,Gamma) 

      real s2, Gamma

c......My COMMON bloc
      COMMON /nik2/ M1, M2, mt, Mw, ml
      SAVE /nik2/

c......The nik2 COMMON bloc parameters
      real M1, M2, mt, Mw, ml
      
c......Additional variables      
      real w, wmax
      real BW

      wmax=(M2*Gamma)**2
      
      w=(s2-M2**2)**2+wmax
      w=1.0/w
      w=w*wmax
         
      if(w.GT.1.0) then
        print *,' In function BW(s2,Gamma)'
        print *,' Fatal ERROR!!! w(S2)>1.0'
        print *,' s2 =', s2,'     w =', w
	print *,' M2 =', M2,' Gamma =', Gamma
        STOP
      endif

      BW=w
      return 

      end


c     *********************************************
c     *                                           *
c     *    Tringular function  \lambda (a,b,c)    *
c     *      (for semileptonic decays matrix      * 
c     *           elements calculations)          *
c     *                                           *
c     *********************************************
      subroutine myLambda(a,b,c,L)

      real a, b, c, L

      L=a**2+b**2+c**2
      L=L-2.0*a*b-2.0*b*c-2.0*a*c

      return
      end


c     **********************************************
c     *                                            *
c     *             Function \hat\Pi               *
c     *   by D.Melikhov, N.Nikitin and S.Simula,   *
c     *        Phys.Rev. D57, p.6814 (1998).       *
c     *                                            *
c     **********************************************
      subroutine myPi(a, b, c, d, PPi)

      real a, b, c, d, PPi

      PPi=(b-1.0)*(b-c)+a*b+d*(1.0+c+d-a-2.0*b)

      return
      end



c
c      ****************************************************************
c      *                                                              *
c      *                 Basic kinematics function                    *
c      *                                                              *
c      *   30.06.2003                                                 *
c      *                                                              *
c      *   Author: Nickolai Nikitine,  Nikolai.Nikitine@cern.ch       *
c      *                                                              *
c      ****************************************************************
c
      function gf(x,y,z,u,v,w)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      DOUBLE PRECISION gf 

      gf=x*z*w+x*u*v+y*z*v+y*u*w
     #-x*y*(z+u+v+w-x-y)-z*u*(x+y+v+w-z-u)-v*w*(x+y+z+u-v-w)

      return
      end

