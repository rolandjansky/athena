c   28.10.2002;  29.10.2002;  07.04.2003;  08.04.2003            
c   09.04.2003;  13.04.2003;  14.04.2003;  15.04.2003
c   13.05.2003;  17.05.2003;  21.05.2003;  30.06.2003
c   08.07.2003;
                                                   
                    
                 
c   *     The strong running coupling constant by PDG            * 
c   *                                                            * 
c   * mu - the scale parameter  ( in GeV );                      * 
c   * Nf - number of "effective" flavours ( Nf=5 for b-quark);   * 
c   * the number of the collors = 3;                             * 
c   * ias - the number for Lms[]:                                * 
c   *     = 0              Lms=min Lms,                          * 
c   *     = 1              the mean value,                       * 
c   *     = 2              Lms=max Lms.                          * 
                   
      subroutine myAs(mu,Nf,ias)
            
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/
      
      dimension PLms(3)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
      	                           
      real mu
      integer Nf, ias
      real as, ass, las, mm
      real b0, b1, b2
      real pi
c   * the Lms - parametr, which represented the experimental   * 
c   * value of the As(Mz) in the \bar {MS}-schime              *
      real PLms
      
      n_as=0.0
      
c      print *, ' BEGIN of the subroutine myAs(...)'
                                              
      pi=3.1415927
                                           
      PLms(1)=185.0    ! MeV
      PLms(2)=208.0    ! MeV
      PLms(3)=233.0    ! MeV 
                                    
      b0=11.-2.*(float(Nf))/3. 
      b1=51.-19.*(float(Nf))/3. 
      b2=2857.-5033.*(float(Nf))/9.+325.*(float(Nf**2))/27.      
                             
      mm=(mu*1000.0/PLms(ias+1))**2   
                  
c   * the running coupling constant *
      las=alog(alog(mm))
      ass=(las-0.5)**2+b2*b0/(8.*b1*b1)-5./4.
      ass=ass/((b0**4)*(alog(mm)**2))
      as=1.0-2.0*b1*las/((b0**2)*alog(mm))
      as=as+4.0*b1*b1*ass 
      as=4.0*pi*as/(b0*alog(mm))
              
      n_as=as      
	         
      return          
      end
      
      
      
                                            
c  *********************************************************************                                                              
c  *********************************************************************
c  **     BEGIN THE CALCULATION OF THE WILSON COEFFICIENTS            ** 
c  *********************************************************************
c  ********************************************************************* 
       
           
c    *                  Spenser function                        * 
c    *              in serial representation                    * 
c    *                    ( w <= 1.0 )                          * 
c    *                                                          *                                                
      subroutine myLi2(w,Lii) 
                           
      real w 
      real Lii 
      integer k
                           
      Lii=0.0	
                   
      do k=1,10
        Lii=Lii+w**k/k**2 
      enddo
          	                
      return
      end 
                     
                             
c   *                Coefficient myC1(mu)                      * 
c   *       by A.J.Buras and M.Munz, Phys.Rev. D52, 186.       * 
      subroutine myC1(mu, Mw, Nf, ias)
      
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/
      
      dimension a(8), ak1(8)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
	 
      real mu, Mw 
      integer Nf, ias      	 
      real CC1 
      real eta 
      real asW  ! the strong coupling constant at the scale Mw  
      real asmu ! the strong coupling constant at the scale mu  
      integer i 
           
      real a
      real ak1 
      
      n_c1=0.0
           
      call myAs(Mw,Nf,ias)
      asW=n_as 
      call myAs(mu,Nf,ias)
      asmu=n_as
       
      eta=asW/asmu 
 
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456
                       
      ak1(1) = 0.0
      ak1(2) = 0.0
      ak1(3) = 0.5
      ak1(4) = -0.5
      ak1(5) = 0.0
      ak1(6) = 0.0
      ak1(7) = 0.0
      ak1(8) = 0.0
                              
      CC1=0.0 
      do i=1,8
        CC1=CC1+ak1(i)*eta**a(i) 
      enddo
      
      n_c1=CC1
         
      return
      end 
                              
			      
c   *                Coefficient myC2(mu)                      * 
c   *       by A.J.Buras and M.Munz, Phys.Rev. D52, 186.       * 
      subroutine myC2(mu, Mw, Nf, ias) 
      
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/
      
      dimension a(8), ak2(8)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
       
      real mu, Mw 
      integer Nf, ias      	 
      real CC2 
      real eta 
      real asW  ! the strong coupling constant at the scale Mw  
      real asmu ! the strong coupling constant at the scale mu  
      integer i 
                   
      real a
      real ak2
      
      n_c2=0.0 
                           
      call myAs(Mw,Nf,ias)
      asW=n_as 
      call myAs(mu,Nf,ias)
      asmu=n_as
       
      eta=asW/asmu 
 
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456
                       
      ak2(1) = 0.0
      ak2(2) = 0.0
      ak2(3) = 0.5
      ak2(4) = 0.5
      ak2(5) = 0.0
      ak2(6) = 0.0
      ak2(7) = 0.0
      ak2(8) = 0.0
                              
      CC2=0.0 
      do i=1,8
        CC2=CC2+ak2(i)*eta**a(i) 
      enddo
      
      n_c2=CC2
         
      return
      end 			      
      
      
      
c   *                Coefficient myC3(mu)                      * 
c   *       by A.J.Buras and M.Munz, Phys.Rev. D52, 186.       * 
      subroutine myC3(mu, Mw, Nf, ias)
      
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/

      dimension a(8), ak3(8)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 
	 
      real mu, Mw 
      integer Nf, ias      	 
      real CC3 
      real eta 
      real asW  ! the strong coupling constant at the scale Mw  
      real asmu ! the strong coupling constant at the scale mu  
      integer i 
                   
      real a
      real ak3
      
      n_c3=0.0 
                           
      call myAs(Mw,Nf,ias)
      asW=n_as 
      call myAs(mu,Nf,ias)
      asmu=n_as
       
      eta=asW/asmu 
 
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456                      

      ak3(1) = 0.0 
      ak3(2) = 0.0 
      ak3(3) = (-1.0)/14.0
      ak3(4) = 1.0/6.0
      ak3(5) = 0.0510
      ak3(6) = -0.1403
      ak3(7) = -0.0113
      ak3(8) = 0.0054
                              
      CC3=0.0 
      do i=1,8
        CC3=CC3+ak3(i)*eta**a(i) 
      enddo
      
      n_c3=CC3
         
      return
      end 	      
      
      
      
c   *                Coefficient myC4(mu)                      * 
c   *       by A.J.Buras and M.Munz, Phys.Rev. D52, 186.       * 
      subroutine myC4(mu, Mw, Nf, ias) 

      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/
      
      dimension a(8), ak4(8)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 
 
      real mu, Mw 
      integer Nf, ias      	 
      real CC4 
      real eta 
      real asW  ! the strong coupling constant at the scale Mw  
      real asmu ! the strong coupling constant at the scale mu  
      integer i 
                   
      real a
      real ak4
      
      n_c4=0.0 
      
      call myAs(Mw,Nf,ias)
      asW=n_as 
      call myAs(mu,Nf,ias)
      asmu=n_as
       
      eta=asW/asmu                      
                     
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456                       
                
      ak4(1) = 0.0
      ak4(2) = 0.0
      ak4(3) = (-1.0)/14.0
      ak4(4) = (-1.0)/6.0
      ak4(5) = 0.0984
      ak4(6) = 0.1214
      ak4(7) = 0.0156
      ak4(8) = 0.0026
                              
      CC4=0.0 
      do i=1,8
        CC4=CC4+ak4(i)*eta**a(i) 
      enddo
      
      n_c4=CC4
         
      return
      end       
      
      
      
c   *                Coefficient myC5(mu)                      * 
c   *       by A.J.Buras and M.Munz, Phys.Rev. D52, 186.       * 
      subroutine myC5(mu, Mw, Nf, ias)
      
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/

      dimension a(8), ak5(8)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/  
             
      real mu, Mw 
      integer Nf, ias      	 
      real CC5 
      real eta 
      real asW  ! the strong coupling constant at the scale Mw  
      real asmu ! the strong coupling constant at the scale mu  
      integer i 
                   
      real a
      real ak5
      
      n_c5=0.0 
                           
      call myAs(Mw,Nf,ias)
      asW=n_as 
      call myAs(mu,Nf,ias)
      asmu=n_as
       
      eta=asW/asmu  
               
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456                       
                 
      ak5(1) = 0.0
      ak5(2) = 0.0
      ak5(3) = 0.0
      ak5(4) = 0.0
      ak5(5) = -0.0397
      ak5(6) = 0.0117
      ak5(7) = -0.0025
      ak5(8) = 0.0304
                              
      CC5=0.0 
      do i=1,8
        CC5=CC5+ak5(i)*eta**a(i) 
      enddo
      
      n_c5=CC5
         
      return
      end       
      
      
      
c   *                Coefficient myC6(mu)                      * 
c   *       by A.J.Buras and M.Munz, Phys.Rev. D52, 186.       * 
      subroutine myC6(mu, Mw, Nf, ias) 
      
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/
      
      dimension a(8), ak6(8)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/  
	 
      real mu, Mw 
      integer Nf, ias      	 
      real CC6 
      real eta 
      real asW  ! the strong coupling constant at the scale Mw  
      real asmu ! the strong coupling constant at the scale mu  
      integer i 
                   
      real a
      real ak6
      
      n_c6=0.0 
                           
      call myAs(Mw,Nf,ias)
      asW=n_as 
      call myAs(mu,Nf,ias)
      asmu=n_as
       
      eta=asW/asmu 
               
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456                       
            
      ak6(1) = 0.0
      ak6(2) = 0.0
      ak6(3) = 0.0
      ak6(4) = 0.0
      ak6(5) = 0.0335
      ak6(6) = 0.0239
      ak6(7) = -0.0462
      ak6(8) = -0.0112
                                        
      CC6=0.0 
      do i=1,8
        CC6=CC6+ak6(i)*eta**a(i) 
      enddo
      
      n_c6=CC6
                      
      return
      end       
                        
                  
              
c   * by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  * 
      subroutine myA(z,AA)
                             
      real z, AA 
              
      AA=z*(8.0*z**2+5.0*z-7.0)/(12.0*(z-1.0)**3) 
      AA=AA+z**2*(2.0-3.0*z)*alog(z)/(2.0*(z-1.0)**4.0) 

      return
      end 
                                    
                      
                     	    	      
c   * by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
      subroutine myB(z,BB) 
	
      real z, BB 
 
      BB=z/(4.0*(1.0-z))+z*alog(z)/(4.0*(1.0-z)**2)
      
      return
      end 
                   
            
              
c     .by A.J.Buras and M.Munz, Phys.Rev. D52, 186.   
      subroutine myC_Bur(z,CC)
      
      real z, CC 
 
      CC=z*(z-6.0)/(8.0*(z-1.0)) 
      CC=CC+z*(3.0*z+2.0)*alog(z)/(8.0*(z-1.0)**2) 

      return
      end	  
                        
                           
                            
c     .by A.J.Buras and M.Munz, Phys.Rev. D52, 186.   
      subroutine myD_Bur(z, DD)
      
      real z, DD 
 
      DD=(25.0*z**2-19.0*z**3.0)/(36.0*(z-1.0)**3) 
      DD=DD+z**2*(5.0*z**2-2.0*z-6.0)*alog(z)/(18.0*(z-1.0)**4.0) 
      DD=DD-(4.0/9.0)*alog(z) 
 
      return
      end
                                 
                            
                       
c     .by A.J.Buras and M.Munz, Phys.Rev. D52, 186.   
      subroutine myE(z, EE)
      real z, zlog, EE
      
c      print *, '               ' 
c      print *, ' BEGIN subroutine myE(...)'
c      print *, '    z =', z

      zlog=alog(z)
c      print *, ' zlog =', zlog
 
      EE=z*(18.0-11.0*z-z*z)/(12.0*(1.0-z)**3.0) 
c      print *, 'line 1    EE =', EE      
      EE=EE+z**2*(15.0-16.0*z+4.0*z*z)*zlog/(6.0*(1.0-z)**4)
c      print *, 'line 2    EE =', EE 
      EE=EE-(2.0/3.0)*zlog 
c      print *, 'line 3    EE =', EE
      
c      print *, ' END subroutine myE(...)'
c      print *, '               ' 
      
      return
      end
      
      
      
c     .by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  
      subroutine myF_Bur(z, FF) 
      
      real z, FF 
 
      FF=z*(z**2-5.0*z-2.0)/(4.0*(z-1.0)**3) 
      FF=FF+3.0*z**2*alog(z)/(2.0*(z-1.0)**4) 
 
      return
      end
      
      
            
c     .by A.J.Buras and M.Munz, Phys.Rev. D52, 186.
      subroutine myY(z,YY)
            
      real z, YY
      real BB, CC
      
      call myC_Bur(z,CC)
      call myB(z,BB) 
 
      YY=CC-BB
      
      return
      end 
                      
                
                
c     .by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
      subroutine myZ(z, ZZ)
      
      real z, ZZ
      real CC, DD
      
      call myC_Bur(z,CC)
      call myD_Bur(z,DD) 
 
      ZZ=CC+0.25*DD 
 
      return
      end 
      
      

      
c   *       Coefficient  myC7gamma(mu) in the SM              * 
c   *     by A.J.Buras and M.Munz, Phys.Rev. D52, 186.        * 
      subroutine myC7gamma(mu, Mw, mt, Nf, ias)
      
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/
      
      dimension a(8), ah(8)
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 
      
      real mu, Mw, mt
      integer Nf, ias 

      real C7, C70, C80, sum
      real AA, FF 
      real x, eta 
      real asW, asmu 
      integer i
      real a
      real ah
      
      n_c7=0.0
      
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456
                                      	    
      ah(1) = 2.2996
      ah(2) = -1.0880
      ah(3) = (-3.0)/7.0
      ah(4) = (-1.0)/14.0
      ah(5) = -0.6494
      ah(6) = -0.0380
      ah(7) = -0.0186
      ah(8) = -0.0057	    
      
      C7=0.0
      asW=0.0
      asmu=0.0
                                                                       	    	     
      x=(mt/Mw)**2 
      call myAs(Mw,Nf,ias)
      asW=n_as
c      print *, '   asW =', asW 
      call myAs(mu,Nf,ias)
      asmu=n_as
c      print *,' asmu =', asmu
       
      eta=asW/asmu 
      
      call myA(x,AA)
      call myF_Bur(x,FF)
      
c      print *, '   x =', x
c      print *, ' eta =', eta
c      print *, '  AA =', AA
c      print *, '  FF =', FF 
                             
      C70=(-0.5)*AA 
      C80=(-0.5)*FF 
                        
      C7=eta**0.7*C70 
      C7=C7+(8.0/3.0)*(eta**0.61-eta**0.7)*C80 
                         
      sum=0.0
      do i=1,8 	    
        sum=sum+ah(i)*eta**a(i)
c	print *, i, '  sum =', sum
      enddo 
                      	
      C7=C7+sum 
      n_c7=C7
                 
      return
      end
                                                          
                                    
                                                     
                                                      
c     *             Coefficient P_E                   * 
c     * by A.J.Buras and M.Munz, Phys.Rev. D52, 186;  * 
c     *            see formula (2.12).                * 
      subroutine myPe(eta, Pee)
      
      dimension a(8), q(8)
      
      real eta 
      real sum 
      real Pee 
      integer i
      
      real a, q
      
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456       
 
      q(1) = 0.0
      q(2) = 0.0
      q(3) = 0.0
      q(4) = 0.0
      q(5) = 0.0318
      q(6) = 0.0918
      q(7) = -0.2700
      q(8) = 0.0059
	    
      sum=0.0
      do i=1,8 
        sum=sum+q(i)*eta**(a(i)+1.0) 
      enddo	
	
      Pee=0.1405+sum 
 
      return
      end
                         
                          
                            
c     *           Coefficient P^{NDR}_0                * 
c     * by A.J.Buras and M.Munz, Phys.Rev. D52, 186;   * 
c     *	          see formula (2.11).                  * 
      subroutine myP0ndr(asW, eta, P00ndr)
      
      dimension a(8), p(8), r(8), s(8)
      
      real asW, eta 
      real P00ndr 
      real sum 
      integer i 
      
      real pi
      real a, p, r, s
             
      pi=3.1415927
       
      a(1) = 14.0/23.0
      a(2) = 16.0/23.0
      a(3) = 6.0/23.0
      a(4) = (-12.0)/23.0
      a(5) = 0.4086
      a(6) = -0.4230
      a(7) = -0.8994
      a(8) = 0.1456          
                     
      p(1) = 0.0
      p(2) = 0.0
      p(3) = (-80.0)/203.0
      p(4) = 8.0/33.0
      p(5) = 0.0433
      p(6) = 0.1384
      p(7) = 0.1648
      p(8) = -0.0073		     
      		      
      r(1) = 0.0
      r(2) = 0.0
      r(3) = 0.8966
      r(4) = -0.1960
      r(5) = -0.2011
      r(6) = 0.1328
      r(7) = -0.0292
      r(8) = -0.1858
      
      s(1) = 0.0
      s(2) = 0.0
      s(3) = -0.2009
      s(4) = -0.3579
      s(5) = 0.0490
      s(6) = -0.3616
      s(7) = -0.3554
      s(8) = 0.0072
      
      sum=0.0
      do i=1,8
        sum=sum+p(i)*eta**(a(i)+1.0)
      enddo 
      P00ndr=pi*(-0.1875+sum)/asW 
      P00ndr=P00ndr+1.2468 
		
      sum=0.0
      do i=1,8 
         sum=sum+(r(i)+s(i)*eta)*eta**a(i)
      enddo 
      P00ndr=P00ndr+sum 
 
      return
      end
      

      
c     *     Coefficient  C_{9V} (in the NDR schime)      * 
c     *   by A.J.Buras and M.Munz, Phys.Rev. D52, 186;   * 
c     *	           see formula (2.10).                   * 
      subroutine myC9v(mu, Mw, mt, Nf, ias, C9)
      
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7  
      SAVE /nikwc/
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 
                                                      
      real mu, Mw, mt
      integer Nf, ias   
                              
      real C9 
      real x, eta 
      real asW, asmu 
      real sin2W        ! the square of the weak angle 
      real P00ndr, YY, ZZ, Pee, EE 
                                     
      sin2W=0.224
                          
      x=(mt/Mw)**2 
      call myAs(Mw,Nf,ias)
      asW=n_as 
      call myAs(mu,Nf,ias)
      asmu=n_as 
      eta=asW/asmu 
      
c      print *, '                         '
c      print *, 'BEGIN Subroutine myC9v(...)'
      
c      print *,'   asW =', asW
c      print *,'  asmu =', asmu
c      print *,'   eta =', eta       
             
      call myP0ndr(asW,eta,P00ndr)
      call myY(x,YY)
      call myZ(x,ZZ)
      call myPe(eta,Pee)
      call myE(x,EE)      
      
c      print *,' P00nd =', P00ndr
c      print *,'     YY =', YY
c      print *,'     ZZ =', ZZ
c      print *,'    Pee =', Pee
c      print *,'     EE =', EE
                                                     
      C9=P00ndr+(YY/sin2W)-4.0*ZZ+Pee*EE 
      
c      print *, '     C9 =', C9
c      print *, 'END Subroutine myC9v(...)'
c      print *, '                         '
                                                                         
      return
      end
                                                
                                                         
                                          
c     *                Coefficient C_{10A}               * 
c     *   by A.J.Buras and M.Munz, Phys.Rev. D52, 186;   * 
c     *	                see formula (2.8).               */ 
      subroutine myC10a(mt,Mw,C10)
                                                   
      real mt, Mw  
      real C10 
      real x 
      real sin2W
      real YY
                                              
      sin2W=0.224     ! the square of the weak angle  
                                
      x=(mt/Mw)**2 
                  
      call myY(x,YY)	         
      C10=(-YY)/sin2W 
 
      return
      end
                  
                                  
                                       
c     *          Re(h(z,\hat s)) by Buras           * 
      subroutine Reh(mu,m2,z,s,rreh)
      
      real mu, m2, z, s 
      real rreh, swh 
      real x             ! Buras variable "x" in (2.29)  
	   
      x=4.0*z**2/s 
 
      rreh=(-8.0)*alog(m2/mu)/9.0-8.0*alog(z)/9.0 
      rreh=rreh+8.0/27.0+4.0*x/9.0 
 
      swh=2.0*(2.0+x)*sqrt(abs(1.0-x))/9.0 
      
      if(x.LE.1.0) then
         swh=swh*alog(abs((sqrt(1.0-x)+1.0)/(sqrt(1.0-x)-1.0)))  
      else 
	 swh=swh*2.0*atan(1.0/sqrt(x-1.0)) 
      endif
 
      rreh=rreh-swh 
 
      return
      end 
      
      
      
c     *     Im(h(z,\hat s)) by Buras       * 
      subroutine Imh(z,s,iimh)
      
      real z, s  
      real x         ! Buras variable "x" in (2.29)  
      real iimh 
      real pi
                                                
      pi=3.141593
                             
      x=4.0*z**2/s
                               
      if(x.LE.1.0) then 
	 iimh=2.0*pi*(2.0+x)*sqrt(abs(1.0-x))/9.0 
      else 
         iimh=0.0 
      endif	   
                                         
      return
      end
                                       
                                                 
                                                              
c     *     Re(h(0,\hat s)) by Buras       * 
      subroutine Reh0(mu, m2, s, rreh0)
      
      real mu, m2,s 
      real rreh0
       
      rreh0=8.0/27.0-8.0*alog(m2/mu)/9.0-4.0*alog(s)/9.0
      
      return
      end
                                                      	    			   
                   
                     
c     *     Im(h(0,\hat s)) by Buras	   * 
      subroutine Imh0(iimh0) 
                        
      real iimh0 
      real pi 
                    
      pi=3.141593 
                      
      iimh0=4.0*pi/9.0 
          
      return
      end					   
                 
      

c     *******************************************************************
c     *                                                                 *      
c     *                        REAL PART                                * 
c     * of the resonant hadronic contribution to the C_{9V} coefficient * 
c     *                                                                 * 
c     *       res_swch=0 -- switch OFF for the resonant contribution;   * 
c     *       res_swch=1 -- switch ON for the resonant contribution.    * 
c     *                                                                 *
c     *******************************************************************
      subroutine ReHadd(res_swch, q2, ml, RReHadd)
      
      dimension RGamma_ll(2), RGamma(2), RGamma_ll_e_mu(2),
     #RGamma_ll_tau(2), RM(2)
      
      integer res_swch 
      real q2, ml
      
      integer i
      real RReHadd 
      real a,b,Znam 
      real RGamma_ll
 
      real alpha_ked 
      real pi 
 
c     .Total branching ratios of J/\psi and \psi^{'}--mesons in GeV-s 
      real RGamma 

c     .The branching fractions for decays V->l^+l^-, i={e,\mu} in GeV--s 
      real RGamma_ll_e_mu 

c     .The branching fractions for decays V->tau^+tau^- in GeV--s 
      real RGamma_ll_tau 

c     .Masses of J/\psi and \psi^{'}--mesons */ 
      real RM 
	  
      RReHadd=0.0
      alpha_ked=1.0/137.0
      pi=3.141593
      
      RGamma(1) = 0.000088   ! GeV
      RGamma(2) = 0.000277   ! GeV
      
      RGamma_ll_e_mu(1) = 0.000005  ! GeV
      RGamma_ll_e_mu(2) = 0.000002  ! GeV
      
      RGamma_ll_tau(1) = 0.0        ! GeV
      RGamma_ll_tau(2) = 0.000002   ! GeV
      
      RM(1) = 3.09688
      RM(2) = 3.686
                      
                    	  
      if(ml.LE.1.0) then 
	 do i=1,2 
     	    RGamma_ll(i)=RGamma_ll_e_mu(i)
     	 enddo 
      else 
	 do i=1,2
	    RGamma_ll(i)=RGamma_ll_tau(i) 
	 enddo
      endif
      
      if(res_swch.EQ.0) then
         RReHadd=0.0
      endif
      
      if(res_swch.EQ.1) then
         RReHadd=0.0
	 do i=1,2
	    a=RM(i)**2-q2 
	    b=RM(i)*RGamma(i) 
	    Znam=a**2+b**2 
	    Znam=RGamma_ll(i)*RM(i)*a/Znam 
	    RReHadd=RReHadd+Znam
	 enddo
      endif
 
c     .plus is the consequence of relation:  
c     .       \kappa*(3C_1+C_2)=1            
c     .    with sign of Wilson coefficien    
c     .      C_2(M_W)=+1 as at work          
c     .        A.J.Buras and M.Munz,         
c     .         Phys.Rev. D52, 186             
      RReHadd=RReHadd*3.0*pi/alpha_ked**2 
 
      return
      end 



c    *****************************************************************
c    *                                                               *
c    *                   IMAGINARY PART                              * 
c    * of the resonant hadronic contribution to the C_9V coefficient * 
c    *                                                               * 
c    *  res_swch -- switching parametr:                              * 
c    *       res_swch=0 -- switch OFF for the resonant contribution; * 
c    *       res_swch=1 -- switch ON for the resonant contribution.  * 
c    *                                                               *
c    *****************************************************************
      subroutine ImHadd(res_swch, q2, ml, IImHadd) 
      
      dimension RGamma_ll(2), RGamma(2), RGamma_ll_e_mu(2), 
     #RGamma_ll_tau(2), RM(2)
             
      integer res_swch 
      real q2, ml
                          
      integer i
      real IImHadd 
      real a,b,Znam 
      real RGamma_ll
                  
      real alpha_ked 
      real pi 
                         
c     .Total branching ratios of J/\psi and \psi^{'}--mesons in GeV-s 
      real RGamma 
                 
c     .The branching fractions for decays V->l^+l^-, i={e,\mu} in GeV--s 
      real RGamma_ll_e_mu 
                
c     .The branching fractions for decays V->tau^+tau^- in GeV--s 
      real RGamma_ll_tau 
                
c     .Masses of J/\psi and \psi^{'}--mesons */ 
      real RM 
                               	  
      IImHadd=0.0
      alpha_ked=1.0/137.0
      pi=3.141593
      
      RGamma(1) = 0.000088   ! GeV
      RGamma(2) = 0.000277   ! GeV
      
      RGamma_ll_e_mu(1) = 0.000005  ! GeV
      RGamma_ll_e_mu(2) = 0.000002  ! GeV
      
      RGamma_ll_tau(1) = 0.0        ! GeV
      RGamma_ll_tau(2) = 0.000002   ! GeV
      
      RM(1) = 3.09688
      RM(2) = 3.686 
            
      if(ml.LE.1.0) then 
        do i=1,2 
          RGamma_ll(i)=RGamma_ll_e_mu(i) 
        enddo 
      else 
        do i=1,2
          RGamma_ll(i)=RGamma_ll_tau(i) 
        enddo
      endif  
            
      if(res_swch.EQ.0) then
         IImHadd=0.0
      endif
      
      if(res_swch.EQ.1) then
         IImHadd=0.0
	 do i=1,2
            a=RM(i)**2-q2 
            b=RM(i)*RGamma(i) 
            Znam=a**2+b**2 
            Znam=RGamma_ll(i)*RM(i)*b/Znam 
            IImHadd=IImHadd+Znam
	 enddo
      endif	    
                              	
                 
              
c     . plus is the consequence of relation:  
c     .       \kappa*(3C_1+C_2)=1             
c     .   with sign of Wilson coefficien      
c     .      C_2(M_W)=+1 as at work           
c     .        A.J.Buras and M.Munz,          
c     .         Phys.Rev. D52, 186            
      IImHadd=IImHadd*3.0*pi/alpha_ked**2 
           
      return
      end 
                
                      

c     *****************************************************
c     *                                                   * 
c     *            Function omega(s)                      * 
c     * by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189. *
c     *                                                   *
c     ***************************************************** 
      subroutine omega(s,oomega)
      
      real s 
      real oomega 
      real pi
      real Lii
       
      pi=3.141593
      
      call myLi2(s,Lii)
      oomega=(-2.0)*pi**2/9.0-4.0*Lii/3.0 
      oomega=oomega-2.0*alog(s)*alog(1.0-s)/3.0 
      oomega=oomega-(5.0+4.0*s)*alog(1.0-s)/(3.0*(1.0+2.0*s)) 
      oomega=oomega-2.0*s*(1.0+s)*(1.0-2.0*s)*alog(s)/
     #      (3.0*(1.0-s)**2*(1.0+2.0*s)) 
      oomega=oomega+(5.0+9.0*s-6.0*s*s)/(6.0*(1.0-s)*(1.0+2.0*s))
      
      return
      end
                             

c     *************************************************************                  
c     *                                                           *             
c     *         REAL PART of the coefficient C_9V:                * 
c     *           C9 -> C9eff=ReC9eff+i*ImC9eff                   * 
c     *  by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189         * 
c     *                                                           * 
c     *   res_swch -- switching parametr:                         * 
c     * res_swch=0 -- switch OFF for the resonant contribution,   * 
c     * res_swch=1 -- switch ON for the resonant contribution;    * 
c     *                                                           * 
c     *         q2 -- the square of transition 4-momentum;        * 
c     *         m2 -- b-quark mass (in the heavy meson M1), GeV;  * 
c     *         mu -- scale parameter, GeV;                       * 
c     *         mc -- c-quark mass, GeV;                          * 
c     *         mt -- t-quark mass, GeV;                          * 
c     *         Mw -- mass of the W--meson, GeV;                  * 
c     *         ml -- leptonic mass, GeV;                         * 
c     *         Nf -- number of "effective" flavors               * 
c     *                          (for b-quark Nf=5);              * 
c     *        ias -- switching parametr for Lms[]                * 
c     *                          in the myAs(..) subroutine.      * 
c     *                                                           *
c     ************************************************************* 
      subroutine ReC9eff(q2,m2,mu,mc,RReC9eff)
     
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml  
      SAVE /nikwc/,/nik1/,/nik2/
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
	       
      real q2, m2, mu, mc, mt, Mw, ml, M1
      integer res_swch, Nf, ias

      real s          ! Buras variable "\hat s" in (2.26)      
      real z          ! Buras variable "z"      in (2.26)
      real tilde_eta  ! Buras variable " \tilde\eta" in (2.33)  
      real c1,c2,c3,c4,c5,c6,c9,RReC9eff
      real as, oomega, RReHadd, RReh 
 
      real pi
      
      pi=3.141593
      
      res_swch=n_res_swch
      Nf=n_Nf
      ias=n_ias
      
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
 
c      print *, '                      '
c      print *, ' BEGIN subroutine ReC9eff(...) '
c      print *, '  mu =', mu
c      print *, '  Mw =', Mw 
c      print *, '  mt =', mt
c      print *, '  Nf =', Nf 
c      print *, ' ias =', ias

      s=q2/(M1**2) 
      z=mc/m2
      call myAs(mu,Nf,ias)
      as=n_as
      
      if(s.GE.1.0) then
        s=0.99999999
      endif
            
      call omega(s,oomega) 
      tilde_eta=1.0+as*oomega/pi 
      
c      print *, '         s =', s
c      print *, '         z =', z 
c      print *, '        as =', as
c      print *, '    oomega =', oomega 
c      print *, ' tilde_eta =', tilde_eta
 
c     .Initialisation of the Wilson coefficients  
      call myC1(mu,Mw,Nf,ias)
      c1=n_c1 
      call myC2(mu,Mw,Nf,ias)
      c2=n_c2 
      call myC3(mu,Mw,Nf,ias)
      c3=n_c3 
      call myC4(mu,Mw,Nf,ias)
      c4=n_c4 
      call myC5(mu,Mw,Nf,ias)
      c5=n_c5 
      call myC6(mu,Mw,Nf,ias)
      c6=n_c6 
      call myC9v(mu,Mw,mt,Nf,ias,c9)
      
c      print *, '  c1 =', c1
c      print *, '  c2 =', c2 
c      print *, '  c3 =', c3
c      print *, '  c4 =', c4 
c      print *, '  c5 =', c5
c      print *, '  c6 =', c6
c      print *, '  c9 =', c9 
 
      RReC9eff=c9*tilde_eta

      RReh=0.0
      call Reh(mu,m2,z,s,RReh) 
      RReC9eff=RReC9eff+(3.0*c1+c2+3.0*c3+c4+3.0*c5+c6)*RReh
c      print *, ' Reh(mu,m2,z,s) =', RReh

      RReh=0.0
      call Reh(mu,m2,1.0,s,RReh) 
      RReC9eff=RReC9eff-0.5*(4.0*c3+4.0*c4+3.0*c5+c6)*RReh
c      print *, ' Reh(mu,m2,1.0,s) =', RReh

      RReh=0.0
      call Reh0(mu,m2,s,RReh) 
c      print *, ' Reh0(mu,m2,s) =', RReh

      RReC9eff=RReC9eff-0.5*(c3+3.0*c4)*RReh 
      RReC9eff=RReC9eff+2.0*(3.0*c3+c4+3.0*c5+c6)/9.0 
 
      call ReHadd(res_swch,q2,ml,RReHadd)
c      print *,' ReHadd(res_swch,q2,ml) =', RReHadd
      RReC9eff=RReC9eff+RReHadd 
      
c      print *, ' RReC9eff =', RReC9eff
c      print *, ' END subroutine ReC9eff(...) '
c      print *, '                      '
                 
      return
      end 
                
                                          
					  
c     *************************************************************  
c     *                                                           *             
c     *       IMAGINARY PART of the coefficient C_9V:             * 
c     *           C9 -> C9eff=ReC9eff+i*ImC9eff                   * 
c     *  by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189         * 
c     *                                                           * 
c     *   res_swch -- switching parametr:                         * 
c     * res_swch=0 -- switch OFF for the resonant contribution;   * 
c     * res_swch=1 -- switch ON for the resonant contribution;    * 
c     *                                                           * 
c     *         q2 -- the square of transition 4-momentum;        * 
c     *         m2 -- b-quark mass ( in the heavy meson M1), GeV; * 
c     *         mu -- scale parameter, GeV;                       * 
c     *         mc -- c-quark mass, GeV;                          * 
c     *         Mw -- mass of the W--meson, GeV;                  * 
c     *         ml -- leptonic mass, GeV;                         * 
c     *         Nf -- number of "effective" flavors               * 
c     *                          (for b-quark Nf=5);              * 
c     *        ias -- switching parametr for Lms[]                * 
c     *                          in the myAs(..) subroutine.      * 
c     *                                                           *
c     ************************************************************* 
      subroutine ImC9eff(q2,m2,mu,mc,IImC9eff)
     
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml 
      SAVE /nikwc/,/nik1/,/nik2/
      
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
                     	       
      integer res_swch, Nf, ias  
      real q2, m2, mu, mc, Mw, ml   	        
                                           
      real s      ! Buras variable "\hat s"in (2.26) 
      real z      ! Buras variable "z" in (2.26)
      real c1,c2,c3,c4,c5,c6,IImC9eff
      real IImh, IImhadd 
      
      res_swch=n_res_swch
      Nf=n_Nf
      ias=n_ias
      
      Mw=n_Mw
      ml=n_ml
                                      
      s=q2/m2**2 
      z=mc/m2 
                                     
c     .Initialisation of the Wilson coefficients  
      call myC1(mu,Mw,Nf,ias)
      c1=n_c1 
      call myC2(mu,Mw,Nf,ias)
      c2=n_c2 
      call myC3(mu,Mw,Nf,ias)
      c3=n_c3 
      call myC4(mu,Mw,Nf,ias)
      c4=n_c4 
      call myC5(mu,Mw,Nf,ias)
      c5=n_c5 
      call myC6(mu,Mw,Nf,ias)
      c6=n_c6 
       
      call Imh(z,s,IImh)                           
      IImC9eff=(3.0*c1+c2+3.0*c3+c4+3.0*c5+c6)*IImh
      call Imh(1.0,s,IImh)
      IImC9eff=IImC9eff-0.5*(4.0*c3+4.0*c4+3.0*c5+c6)*IImh
      call Imh0(IImh) 
      IImC9eff=IImC9eff-0.5*(c3+3.0*c4)*IImh 
                        
      call ImHadd(res_swch,q2,ml,IImhadd)		  
      IImC9eff=IImC9eff+IImHadd 
                 
      return
      end 
               
 
             		                          
		             		   
			   







            
