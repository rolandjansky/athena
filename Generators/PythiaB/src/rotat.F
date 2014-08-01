c **********************************************************************
c *                                                                    *
c *                Functions for 4-vector rotations                    *
c *                        Version 1.03                                *
c *                                                                    *
c *   The last modification:                  21.05.2003               *
c *                                                                    *
c *   Author: Nickolay Nikitin,  nik679@monet.npi.msu.su               *
c *                                                                    *
c **********************************************************************

      subroutine Mzero(RMt)
      dimension RMt(4,4)
      double precision RMt
      do 40 i=1,4
      do 40 j=1,4
40       RMt(i,j)=0.D0
      return
      end


      subroutine Mult(PIn,RMt,PFin)
      dimension PIn(4), PFin(4), RMt(4,4)
      double precision PIn, PFin, RMt
      do 20 i=1,4
      do 20 j=1,4
20       PFin(i)=PFin(i)+RMt(i,j)*PIn(j)
      return
      end

      
      subroutine Pzero(Pt)
      dimension Pt(4)
      double precision Pt
      do 50 i=1,4
50       Pt(i)=0.D0
      return
      end
                    
                     
      subroutine Mov(PIn,PFin)
      dimension PIn(4), PFin(4)
      double precision PIn, PFin
      do 30 i=1,4
30       PFin(i)=PIn(i)
      return
      end
                     
                             
      double precision function Psqr(Pin)
      dimension Pin(4)
      double precision Pin, Pin2
      PIn2=0.D0
      do 60 i=2,4
60       Pin2=Pin2+Pin(i)**2
      Psqr=Pin2
      return
      end
                            
                     
      subroutine transforme(Pin,RMt)                     
c......The rotation variables
      dimension Pin(4), Pout(4), RMt(4,4)
      double precision Pin, Pout, RMt
                         
      call Pzero(Pout)
      call Mult(Pin,RMt,Pout)
      call Pzero(Pin)
      call Mov(Pout,Pin)

      return
      end
                   
                       
