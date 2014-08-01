c     09.02.2004   17.02.2004   19.02.2004
c     20.02.2004   28.03.2004   29.03.2004
c     30.03.2004   06.05.2004   18.10.2005
c     01.11.2005   15.05.2007
                                      
c     ***************************************************
c     *************************************************** 
c     **       THE MATRIX ELEMENT CALCULATIONS         **
c     **                                               **
c     **   for the decay B^0_{d,s}\to\gamma\mu^+\mu^-  **
c     **                                               **
c     **                Version 1.061                  **
c     **                                               **
c     ***************************************************
c     ***************************************************       
      
      
                           
c       **********************************************
c       *                                            *
c       *  Scaled energy variable "x" on p.034002-6  *
c       *                                            *     
c       *       by F.Kruger and D.Melikhov,          * 
c       *       Phys.Rev. D67, 034002 (2003).        * 
c       *                                            *
c       **********************************************
      subroutine myx(q2,x)
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      SAVE   /nikff2/,/nik2/
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real q2, x, Eg, M1
      
      M1=n_M1
      
      call q2toEg(q2)
      Eg=n_Eg
      
      x=2.0*Eg/M1
      
      return
      end
      
      
c     **********************************************
c     *                                            *
c     *       For \cos\theta calculation           *
c     *                                            *
c     **********************************************
      subroutine myzeta(q2,t,zeta)
      
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      SAVE   /nik2/
      
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real M1, ml, hat_s, hat_t, hat_ml
      real q2,t, zeta
      
      M1=n_M1
      ml=n_ml
      
      hat_ml=ml/M1
      hat_s=q2/(M1**2)
      hat_t=t/(M1**2)
            
      zeta=1.0+2.*(hat_ml**2)-hat_s-2.0*hat_t
      
c      print *, ' hat_s =', hat_s, '  hat_t =', hat_t,
c     #'   hat_ml =', hat_ml,'   zeta =', zeta 
            
      return
      end             
      
      
c     **********************************************
c     *                                            *
c     *   Normalise Mandelstam variable "u/M_1^2"  *
c     *                                            *
c     **********************************************
      subroutine myhatu(hat_s,hat_t,hat_u)
      
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      SAVE   /nik2/
      
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real hat_s, hat_t, hat_u, hat_ml, M1, ml
      
      M1=n_M1
      ml=n_ml
            
      hat_ml=ml/M1            
      hat_u=1+2.0*(hat_ml**2)-hat_s-hat_t
            
      return
      end               
      
c     **********************************************
c     *                                            *
c     *             Subroutine F_1                 *
c     *                                            * 
c     *       by F.Kruger and D.Melikhov,          * 
c     *       Phys.Rev. D67, 034002 (2003),        *
c     *              equation (36).                *   
c     *                                            *
c     **********************************************             
      subroutine myF1_gmm(q2,qm2,mu,mc)
     
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_F1, n_F2, n_B0, n_B1, n_B2  
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, 
     #n_ReTildeFta, n_ImTildeFta
      SAVE /nikff2/,/nik1/,/nik2/,/nik3/,/nikwc/,/nikffreim/
                                
      real q2, M1, qm2, mu, mc, mt, Mw, ml
      integer ias, Nf
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/   
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_F1, n_F2, n_B0, n_B1, n_B2        ! /nik3/ 
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
      real n_ReTildeFtv, n_ImTildeFtv, 
     #n_ReTildeFta, n_ImTildeFta   ! /nikffreim/ 	    
                
      real FF1 
      real hat_s, hat_qm2 
      real Fv, ReTildeFtv, ImTildeFtv 
      real c7, RReC9eff, IImC9eff, c10
      
c      print *, ' In subroutine myF1(q2,qm2,mu,mc) '
      
      FF1 = 0.0
      
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf
      
      hat_qm2=qm2/M1
      hat_s=q2/(M1**2)                   
                    
      call ffs_cqm_km_gmm(q2,1)
      Fv=n_ffs_cqm 
                     
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7                           
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
      call ImC9eff(q2,qm2,mu,mc,IImC9eff)                             
      call myC10a(mt,Mw,c10) 
      
c      print *, ' RReC9eff =', RReC9eff,
c     #' IImC9eff =', IImC9eff
c      print *, ' c10 =', c10, ' c7 =', c7
c      print *, ' Fv =', Fv

      call SubReTildeFtv(q2,qm2)
      ReTildeFtv=n_ReTildeFtv
c      print *, ' n_ReTildeFtv =', n_ReTildeFtv
      
      call SubImTildeFtv(q2,qm2)
      ImTildeFtv=n_ImTildeFtv
c      print *, ' n_ImTildeFtv =', n_ImTildeFtv
                    
      FF1=(RReC9eff**2+IImC9eff**2+c10**2)*(Fv**2)
      FF1=FF1+(ReTildeFtv**2+ImTildeFtv**2)*(2.0*c7*hat_qm2/hat_s)**2
      FF1=FF1+4.0*(RReC9eff*ReTildeFtv+IImC9eff*ImTildeFtv)
     #*hat_qm2*c7*Fv/hat_s
            
      n_F1=FF1
 
      return 
      end
      


c     **********************************************
c     *                                            *
c     *             Subroutine F_2                 *
c     *                                            * 
c     *       by F.Kruger and D.Melikhov,          * 
c     *       Phys.Rev. D67, 034002 (2003),        *
c     *              equation (36).                *   
c     *                                            *
c     **********************************************             
      subroutine myF2_gmm(q2,qm2,mu,mc)
     
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_F1, n_F2, n_B0, n_B1, n_B2  
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, 
     #n_ReTildeFta, n_ImTildeFta
      SAVE /nikff2/,/nik1/,/nik2/,/nik3/,/nikwc/,/nikffreim/
                                
      real q2, M1, qm2, mu, mc, mt, Mw
      integer ias, Nf
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/      
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_F1, n_F2, n_B0, n_B1, n_B2        ! /nik3/ 
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
       real n_ReTildeFtv, n_ImTildeFtv, 
     #n_ReTildeFta, n_ImTildeFta   ! /nikffreim/	    
                
      real FF2 
      real hat_s, hat_qm2 
      real Fa 
      real ReTildeFta, ImTildeFta 
      real c7, RReC9eff, IImC9eff, c10

c      print *,'  In  subroutine myF2(q2,qm2,mu,mc) '

      FF2 = 0.0
      
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf
      
      hat_qm2=qm2/M1
      hat_s=q2/(M1**2)                   
      
      call ffs_cqm_km_gmm(q2,3)
      Fa=n_ffs_cqm 
                         
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7                           
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
      call ImC9eff(q2,qm2,mu,mc,IImC9eff)                             
      call myC10a(mt,Mw,c10) 
      
c      print *, RReC9eff, IImC9eff, c10
      
      call SubReTildeFta(q2,qm2)
      ReTildeFta=n_ReTildeFta
      call SubImTildeFta(q2,qm2)
      ImTildeFta=n_ImTildeFta
      
c      print *, ReTildeFta, ImTildeFta
                    
      FF2=(RReC9eff**2+IImC9eff**2+c10**2)*(Fa**2)
c      print *, '  FF2 =', FF2
      FF2=FF2+(ReTildeFta**2+ImTildeFta**2)*(2.0*c7*hat_qm2/hat_s)**2
c      print *, '  FF2 =', FF2
      FF2=FF2+4.0*(RReC9eff*ReTildeFta+IImC9eff*ImTildeFta)
     #*hat_qm2*c7*Fa/hat_s
c      print *, '  FF2 =', FF2
            
      n_F2=FF2
 
      return 
      end

      
c     **********************************************
c     *                                            *
c     *             Subroutine B_0                 *
c     *                                            * 
c     *       by F.Kruger and D.Melikhov,          * 
c     *       Phys.Rev. D67, 034002 (2003),        *
c     *              equation (35).                *   
c     *                                            *
c     **********************************************             
      subroutine myB0_gmm(q2,qm2,mu,mc)
     
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_F1, n_F2, n_B0, n_B1, n_B2  
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      SAVE /nikff2/,/nik1/,/nik2/,/nik3/,/nikwc/
                                
      real q2, M1, qm2, mu, mc, mt, Mw, ml
      integer ias, Nf
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/     
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_F1, n_F2, n_B0, n_B1, n_B2        ! /nik3/ 
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 	    
                
      real BB0 
      real hat_s, hat_qm2, hat_ml 
      real F1,F2,Fv,Fa 
      real c10
      

      BB0 = 0.0
      
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf
      
      hat_qm2=qm2/M1
      hat_ml=ml/M1
      hat_s=q2/(M1**2)                    
      
c      print *, ' In subroutine myB0_gmm(q2,qm2,mu,mc) '
                                   
      call myF1_gmm(q2,qm2,mu,mc)
      F1=n_F1 
c      print *,' F1 =', F1
              
      call myF2_gmm(q2,qm2,mu,mc)
      F2=n_F2
c      print *,' F2 =', F2
      
      call ffs_cqm_km_gmm(q2,1)
      Fv=n_ffs_cqm
c      print *,' Fv =', Fv
      
      call ffs_cqm_km_gmm(q2,3)
      Fa=n_ffs_cqm
c      print *,' Fa =', Fa
            
      call myC10a(mt,Mw,c10)
c      print *,' c10 =', c10
      
      BB0=(hat_s+4.*(hat_ml**2))*(F1+F2)
      BB0=BB0-8.*((c10*hat_ml)**2)*(Fv**2+Fa**2)
             
      n_B0=BB0
c      print *,' n_B0 =', n_B0
       
      return 
      end
      



c     **********************************************
c     *                                            *
c     *             Subroutine B_1                 *
c     *                                            * 
c     *       by F.Kruger and D.Melikhov,          * 
c     *       Phys.Rev. D67, 034002 (2003),        *
c     *              equation (35).                *   
c     *                                            *
c     ********************************************** 
      subroutine myB1_gmm(q2,qm2,mu,mc)
     
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_F1, n_F2, n_B0, n_B1, n_B2  
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta
      SAVE /nikff2/,/nik1/,/nik2/,/nik3/,/nikwc/,/nikffreim/
                                
      real q2, M1, qm2, mu, mc, mt, Mw
      integer ias, Nf
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/       
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_F1, n_F2, n_B0, n_B1, n_B2        ! /nik3/ 
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
      real n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta   ! /nikffreim/ 	    
                
      real BB1 
c      real hat_s, hat_qm2, hat_ml 
      real hat_s, hat_qm2
      real Fv, ReFtv, Fa, ReFta 
      real c7, RReC9eff, c10
      

      BB1 = 0.0
      
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      Nf=n_Nf
      
      hat_qm2=qm2/M1
      hat_s=q2/(M1**2) 
      
      call ffs_cqm_km_gmm(q2,1)
      Fv=n_ffs_cqm
      call SubReTildeFtv(q2,qm2) 
      ReFtv=n_ReTildeFtv 
      call ffs_cqm_km_gmm(q2,3)
      Fa=n_ffs_cqm
      call SubReTildeFta(q2,qm2)
      ReFta=n_ReTildeFta
                      
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7 
      call ReC9eff(q2,qm2,mu,mc,RReC9eff)  
      call myC10a(mt,Mw,c10)
      
      BB1=c10*RReC9eff*hat_s*Fv*Fa
      BB1=BB1+c10*c7*hat_qm2*(Fv*ReFta+Fa*ReFtv)
      BB1=BB1*8.0 
            
      n_B1=BB1
 
      return 
      end



c     **********************************************
c     *                                            *
c     *             Subroutine B_2                 *
c     *                                            * 
c     *       by F.Kruger and D.Melikhov,          * 
c     *       Phys.Rev. D67, 034002 (2003),        *
c     *              equation (35).                *   
c     *                                            *
c     ********************************************** 
      subroutine myB2_gmm(q2,qm2,mu,mc)
     
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_F1, n_F2, n_B0, n_B1, n_B2  
      SAVE /nikff2/,/nik2/,/nik3/
                                
      real q2, M1, qm2, mu, mc, mt, Mw
     
      real n_Eg, n_ffs_cqm                     ! /nikff2/ 
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_F1, n_F2, n_B0, n_B1, n_B2        ! /nik3/ 	    
                
      real hat_s 
      real F1,F2 
            
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      
      hat_s=q2/(M1**2) 
      
      call myF1_gmm(q2,qm2,mu,mc)
      F1=n_F1
      call myF2_gmm(q2,qm2,mu,mc)
      F2=n_F2 
      
      n_B2=hat_s*(F1+F2)
            
      return
      end


c     ************************************************
c     *                                              *
c     *           Subroutine Bremstrahlung           *
c     *                                              * 
c     *      Kvadrat modula tormoznogo izlucheniya   *
c     *      leptonov v konechnom sostoyaniimuonov   *
c     *                                              *
c     *    q2 - q^2=s - mandelstamovskiy invariant   *
c     *    t          -  mandelstamovskiy invariant  *
c     *    niknum = 1 - tochnaya formula             *
c     *             2 - priblijeniye miagkih fotonov *
c     *                                              *
c     ************************************************ 

      subroutine myBremstrahlung(q2,t,niknum)
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nikbi/ n_Bslg, n_Intrf, n_I1, n_I2 
      SAVE /nikff2/,/nik2/,/nikbi/
                                
      real q2, t, M1, mt, Mw, ml
      integer niknum 
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/      
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_Bslg, n_Intrf, n_I1, n_I2         ! /nikbi/ 	    
                
      real Bslg
      real mult1, mult2
      real hat_s, hat_t, hat_u, hat_ml, xx 
      real c10
      real fB 
      
      Bslg = 0.0
      
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
            
      hat_ml=ml/M1      
      hat_s=q2/(M1**2)                 ! \hat s
      hat_t=t/(M1**2)                  ! \hat t
                  
      call myhatu(hat_s,hat_t,hat_u)   ! \hat u
      call myx(q2,xx)
      
      call myC10a(mt,Mw,c10)
      call ffs_cqm_km_gmm(q2,0)               
      fB=-n_ffs_cqm   ! po moim opredeleniam fB < 0 
      
      mult1=(hat_u-hat_ml**2)*(hat_t-hat_ml**2)
      
c......TOCHNAYA formula dlia kvadrata modula tormoznogo
c      izlucheniya leptonov (m_{ell}) v konechnom sostoyanii 
      if(niknum.EQ.1) then 
c        print *,' Tochnaya formula tormozn. izluch.', niknum    
        
	mult2=mult1/2.0-hat_ml**2
      
        Bslg=hat_s/mult1+mult2*(xx**2)/(mult1**2)      
        Bslg=Bslg*((8.0*hat_ml*fB*c10/M1)**2)
      
        n_Bslg = Bslg
      endif 
      
c......Priblijeniye miagkih fotonov  dlia kvadrata modula tormoznogo
c      izlucheniya leptonov (m_{ell}) v konechnom sostoyanii 
      if(niknum.EQ.2) then      
c        print *,' Priblijeniye miagkih fotonov', niknum
	
	mult2=(xx*hat_ml/mult1)**2
	
	Bslg=hat_s/mult1-mult2
	Bslg=Bslg*((8.0*hat_ml*fB*c10/M1)**2)
	
	n_Bslg = Bslg        
      endif     
           
      return
      end
      
      
c     ************************************************** 
c     *                                                *
c     *            subroutine myI1(q2,t)               *
c     *                                                *
c     *             I1=C^2_{10A}*F_A*\xi               *
c     *                                                *
c     *    q2 - q^2=s - mandelstamovskiy invariant     *
c     *    t          -  mandelstamovskiy invariant    * 
c     *                                                *
c     **************************************************           
      subroutine myI1(q2,t)
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nikbi/ n_Bslg, n_Intrf, n_I1, n_I2 
      SAVE   /nikff2/,/nik2/,/nikbi/
                                
      real q2, t
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/       
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_Bslg, n_Intrf, n_I1, n_I2         ! /nikbi/	    
                
      real I1
      real mt, Mw
      real Fa, c10, zzeta
      
      I1 = 0.0
      
      mt=n_mt
      Mw=n_Mw
            
      call myzeta(q2,t,zzeta) 
      
      call ffs_cqm_km_gmm(q2,3)   
      Fa=n_ffs_cqm
                      
      call myC10a(mt,Mw,c10)
      
      I1=zzeta*Fa*(c10**2)
      
      n_I1=I1
      
      return
      end



c     **************************************************
c     *                                                *
c     *          subroutine myI2(q2,qm2,mu,mc)         *
c     *                                                *
c     *  I2=2*\hat m_b*C_{7\gamma}*C_{10A}*Re(F_{TV})+ *
c     *     C_{10A}*F_V*Re(C_{9V})                     *
c     *                                                *
c     *    q2  - q^2=s - mandelstamovskiy invariant    *
c     *    qm2 - massa b-quarka                        *
c     *    mu  - masshtabniy parametr = 5 GeV          *
c     *    mc  - massa c-quarka                        *  
c     *                                                *
c     **************************************************
      subroutine myI2(q2,qm2,mu,mc)
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nikbi/ n_Bslg, n_Intrf, n_I1, n_I2 
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta
      SAVE /nikff2/,/nik1/,/nik2/,/nikbi/,/nikwc/,/nikffreim/
                                
      real q2, M1, qm2, mu, mc, mt, Mw
      integer ias
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/       
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_Bslg, n_Intrf, n_I1, n_I2         ! /nikbi/
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/
      real n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta  ! /nikffreim	    
                
      real I2 
      real hat_s, hat_qm2 
      real Fv,ReFtv 
      real c7, RReC9eff, c10
      
      I2 = 0.0
      
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      
      hat_qm2=qm2/M1
      hat_s=q2/(M1**2) 
      
      call ffs_cqm_km_gmm(q2,1)
      Fv=n_ffs_cqm
      call SubReTildeFtv(q2,qm2)
      ReFtv=n_ReTildeFtv
                      
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7 
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
      call myC10a(mt,Mw,c10)
      
      I2=2.0*c7*c10*hat_qm2*ReFtv/hat_s+RReC9eff*c10*Fv
      
      n_I2=I2
      
c      print *, '  c7 =', c7
c      print *, ' c10 =', c10
c      print *, ' hat_qm2 =', hat_qm2
c      print *, ' Fv =', Fv
c      print *, ' ReFtv =', ReFtv
c      print *, ' hat_s =', hat_s
c      print *, ' RReC9eff =', RReC9eff
c      print *, ' I2 =', I2
      
      
      
      return
      end


c     ************************************************
c     *                                              *
c     *          Subroutine Interference             *
c     *                                              * 
c     *      Interferencionniy chlen mejdu dvumia    *
c     *                   vkladami                   *
c     *                                              *
c     *    niknum = 1 - tochnaya formula             *
c     *             2 - priblijeniye miagkih fotonov *
c     *                                              *
c     ************************************************ 
      subroutine myInterference(q2,t,qm2,mu,mc,niknum)
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nikbi/ n_Bslg, n_Intrf, n_I1, n_I2 
      SAVE /nikff2/,/nik2/,/nikbi/
                                
      real q2, t, M1, qm2, mu, mc, mt, Mw, ml
      integer niknum 
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/      
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_Bslg, n_Intrf, n_I1, n_I2         ! /nikbi/ 	    
                
      real Intrf, I1, I2
      real mult1
c      real hat_s, hat_t, hat_u, hat_ml, hat_qm2, xx
      real hat_s, hat_t, hat_u, hat_ml, xx
      real fB 
      
      Intrf = 0.0
      
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      
      hat_ml=ml/M1      
      hat_s=q2/(M1**2)
      hat_t=t/(M1**2)
            
      call myhatu(hat_s,hat_t,hat_u)
      call myx(q2,xx)
            
c      print *,' hat_u =', hat_u, ' xx =', xx, '  zzeta  =',  zzeta  

c......TOCHNAYA formula dlia kvadrata modula tormoznogo
c      izlucheniya leptonov (m_{ell}) v konechnom sostoyanii 
      if(niknum.EQ.1) then       
        mult1=(hat_u-hat_ml**2)*(hat_t-hat_ml**2)
            
        call myI1(q2,t)  
        I1=n_I1
c        print *, ' mult1 =', mult1, '  mult2 =',  mult2
c        print *, ' I1 =', I1
            
        call myI2(q2,qm2,mu,mc)   
        I2=n_I2
c        print *, ' mult1 =', mult1, '  mult2 =',  mult2
c        print *, ' I2 =', I2
      
        Intrf=(xx**2)*(xx*I2+I1)/mult1
      
        call ffs_cqm_km_gmm(q2,0)    
        fB=-n_ffs_cqm    ! po moim opredeleniya fB < 0 
      
        Intrf=0.0-Intrf*(hat_ml**2)*(16.0*fB/M1)
      
        n_Intrf = Intrf 
c        print *,' n_Intrf =', n_Intrf 
      endif
      
c......Priblijeniye miagkih fotonov  dlia kvadrata modula tormoznogo
c      izlucheniya leptonov (m_{ell}) v konechnom sostoyanii 
      if(niknum.EQ.2) then      
c        print *,' Priblijeniye miagkih fotonov', niknum	
	n_Intrf = 0.0	
      endif           
           
      return
      end            
      
c     ******************************************************************** 
c     ******************************************************************** 
c     *                                                                  * 
c     *		       SQUARE of MATRIX ELEMENT                          * 
c     *             for B\to\gamma\ell^+\ell^- decay.                    * 
c     *                                                                  * 
c     *                     by formula (34) in                           * 
c     *   by F.Kruger and D.Melikhov, Phys.Rev. D67, 034002 (2003).      * 
c     *                                                                  * 
c     *------------------------------------------------------------------* 
c     *------------------------------------------------------------------* 
c     *                                                                  * 
c     *    model -- not used in this matrix element;                     *
c     *                                                                  * 
c     * res_swch -- switching parametr:                                  * 
c     *          res_swch=0 -- switch OFF the resonant contribution,     * 
c     *          res_swch=1 -- switch ON the resonant contribution;      * 
c     *							                 * 
c     *      ias -- switching parametr for for Lms[]                     * 
c     *                                   in the function myAs(...):     * 
c     *          ias = 0   --  Lms=min Lms,                              * 
c     *          ias = 1   --  the mean value,                           * 
c     *          ias = 2   --  Lms=max Lms;                              * 
c     *                                                                  * 
c     *    hat_s --  q^2/M1^2=s/M1^2;                                    * 
c     *    hat_t --  t/M1^{2};                                           * 
c     *       M1 --  mass of the B -- meson, GeV;                        *
c     *       mt --  the mass of t-quark, GeV;			         * 
c     *       Mw --  the mass of W-meson, GeV;	                         * 
c     *       ml --  the lepton mass, GeV;                               * 
c     *       Nf --  number of "effective" flavours ( Nf=5 for b-quark); * 
c     *       the number of the collors = 3;                             * 
c     *                                                                  *
c     ********************************************************************
c     ******************************************************************** 
      subroutine m2fortran_gmm(hat_s,hat_t)
      
      dimension RMM2_max(3), RMM2_max_resonant(3)
      
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_F1, n_F2, n_B0, n_B1, n_B2
      COMMON /nik4/ n_Matrix2
      COMMON /nikbi/ n_Bslg, n_Intrf, n_I1, n_I2 
      COMMON /nikff2/ n_Eg, n_ffs_cqm
      SAVE /nik1/,/nik2/,/nik3/,/nik4/,/nikbi/,/nikff2/
            
c......The COMMON blocs parameters      
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_F1, n_F2, n_B0, n_B1, n_B2        ! /nik3/
      real n_Matrix2                           ! /nik4/
      real n_Bslg, n_Intrf, n_I1, n_I2         ! /nikbi/
      real n_Eg, n_ffs_cqm                     ! /nikff2/ 
                            	 
      real hat_s, hat_t, M1, M2, mt, Mw, ml, mu 
      integer res_swch, ias, Nf
      real BB0, BB1, BB2, Bslg, Intrf 
      real xx, zzeta 
                  
            
c     .the mandelstam variables; q2=q^2=s 
      real q2, t
      
c     .the maximum value of the nonresonant matrix elemet MM2
      real RMM2_max
c     .the maximum value of the resonant matrix elemet MM2 
      real RMM2_max_resonant
            
c     .b-quark mass ( in the B -- meson M1), GeV  
      real qm2 
c     .mc --  c-quark mass, GeV 
      real qmc 

c      print *, ' In subroutine m2fortran(hat_s,hat_t) '
            
      res_swch=n_res_swch
      ias=n_ias
      Nf=n_Nf
      M1=n_M1
      M2=n_M2 
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      
      n_B0=0.0 
      n_B1=0.0
      n_B2=0.0 
      n_Matrix2=-13.0
                     
      q2=hat_s*(M1**2) 
      t=hat_t*(M1**2) 
      hat_ml=ml/M1 
      
c......The cut on the foton mass (min. energy)
      call q2toEg(q2)
      if(n_Eg.LT.M2) go to 13
                  
      RMM2_max_resonant(1) = 10000000.0 
      RMM2_max_resonant(2) = 100000.0
      RMM2_max_resonant(3) = 100000.0
          
      qm2 = 4.85
      qmc = 1.45
      mu=qm2
                     
c     .inicialisation of the MM2_max()      
      RMM2_max(1) = 8900000.0    ! for e^+e^-       (ot baldi!!!)
      RMM2_max(2) = 25.0         ! for \mu^+\mu^-   (provereno!!!)
      RMM2_max(3) = 4000.0       ! for \tau^+\tau^- (ot baldi!!!)
      
      if(M2.NE.0.035) then
c        print *, 'M2 ne ravno 0.035 !!!'
        RMM2_max(1) = 8900000.0      ! for e^+e^-       (ot baldi!!!)
        RMM2_max(2) = 25.0*200.0     ! for \mu^+\mu^-   (garantiya maximuma)
        RMM2_max(3) = 80000.0        ! for \tau^+\tau^- (ot baldi!!!)
      endif   

c      print *, '  hat_s =', hat_s
c      print *, '  hat_t =', hat_t
      
      call myB0_gmm(q2,qm2,mu,qmc)      
      BB0=n_B0
c      print *, ' BB0 =', BB0     
      
      call myB1_gmm(q2,qm2,mu,qmc)
      BB1=n_B1
c      print *, ' BB1 =', BB1
      
      call myB2_gmm(q2,qm2,mu,qmc)
      BB2=n_B2
c      print *, ' BB2 =', BB2
      
      call myx(q2,xx)
c      print *, ' xx =', xx
      
      call myzeta(q2,t,zzeta)
c      print *, ' zzeta =', zzeta
      
c     1 - tochnaya formula (stavitsia po umolchaniyu!!!)
c     2 - priblijeniye miagkih fotonov       
      call myBremstrahlung(q2,t,1)
      Bslg=n_Bslg
c      print *, ' Bslg =', Bslg
      
c     1 - tochnaya formula (stavitsia po umolchaniyu!!!)
c     2 - priblijeniye miagkih fotonov       
      call myInterference(q2,t,qm2,mu,qmc,1)
      Intrf=n_Intrf
c      print *, ' Intrf =', Intrf 
                  
		   
c......The matrix element calculation            
      n_Matrix2=(xx**2)*BB0+xx*zzeta*BB1+(zzeta**2)*BB2            
      n_Matrix2=n_Matrix2+Bslg+Intrf
               
c     .what RMM2_max[i] to selected?  
      if(ml.LT.0.09) then
         i=1
      endif
      if(ml.LT.0.2.AND.ml.GE.0.09) then
         i=2
      endif	 
      if(ml.GE.0.2) then
         i=3
      endif		 
                     
c     .The normalisation of the matrix element
      if(res_swch.EQ.0) then
         n_Matrix2=n_Matrix2/RMM2_max(i)
      endif
      if(res_swch.EQ.1) then
         n_Matrix2=n_Matrix2/RMM2_max_resonant(i)
      endif
                     
      if(n_Matrix2.LT.0.0) then
         print *,'             '
         print *,' subroutine m2fortran_gmm'
         print *,' Fatal ERROR: the square of the normalise 
     #matrix element < 0'
         print *,' hat_s =', hat_s, '   hat_t =', hat_t
	 print *,'  s =', q2, '  t =', t
	 print *,' xx =', xx, ' zzeta =', zzeta	 
	 print *, n_Bslg, n_Intrf, n_I1, n_I2 
	 print *, n_F1, n_F2, n_B0, n_B1, n_B2
         print *,'n_Matrix2  =', n_Matrix2
         print *,' By!'
         print *,'             '
         n_Matrix2=-1.0
         STOP 
      endif	  
                   
      if(n_Matrix2.GT.1.0) then
         print *,'             '
         print *,' subroutine m2fortran_gmm'      		   
         print *,' Fatal ERROR: the square of the normalise 
     #matrix element > 1'
         print *,' hat_s =', hat_s, '   hat_t =', hat_t
	 print *,'  s =', q2, '  t =', t
	 print *,' xx =', xx, ' zzeta =', zzeta
	 print *, n_Bslg, n_Intrf, n_I1, n_I2 
	 print *, n_F1, n_F2, n_B0, n_B1, n_B2
         print *,' n_Matrix2 =', n_Matrix2
         print *,' By!'
         print *,'             '
c         n_Matrix2=-2.0
c         STOP
      endif	      	 
      
      
c      print *,' ******************************** '
c      print *,' *                              * '
c      print *,' *   n_Matrix2 =', n_Matrix2,  '* '    
c      print *,' *   END subroutine m2fortran   * '
c      print *,' *                              * '
c      print *,' ******************************** '
c      print *,'                                  '

13    continue
                
      return
      end 
