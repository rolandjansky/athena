c     09.03.2005  01.09.2006
                                    
c     ***********************************************
c     *********************************************** 
c     **     THE MATRIX ELEMENT CALCULATIONS       **
c     **                                           **
c     **              Version 1.01                 **
c     **                   for                     **
c     **           B^0_s -> phi^0mu^+mu^-          **
c     **                                           **
c     ***********************************************
c     ***********************************************       
      
      
      subroutine myG2_PhiMuMu(q2,qm2,qm1,mu,mc)
     
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml  
      COMMON /nik3/ n_G2, n_F2, n_Hplus2, n_R, n_R1
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      SAVE /nikff/,/nik1/,/nik2/,/nik3/,/nikwc/
                                
      real q2, M1, qm2, qm1, mu, mc, mt, Mw, ml
      integer model, ias, Nf
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus    ! /nikff/
      real n_gminus, n_g0,n_B0, n_Bplus        ! /nikff/     
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_G2, n_F2, n_Hplus2, n_R, n_R1     ! /nik3/ 
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 	    
                
      real GG2 
      real hat_s 
      real gg, ggplus 
      real c7, RReC9eff, IImC9eff, c10
      
      n_G2 = 0.0
      
      model=n_model
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf
                         
      hat_s=q2/M1**2 
                 
      call myg_PhiMuMu(model,q2,M1,n_M2)
      gg=n_g 
      call mygplus_PhiMuMu(model,q2,n_M1)
      ggplus=n_gplus 
                      
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7 
      c7=2.0*(qm2+qm1)*c7/M1 
                          
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
      call ImC9eff(q2,qm2,mu,mc,IImC9eff) 
                            
      call myC10a(mt,Mw,c10) 
                    
      GG2=(c10*M1*gg)**2 
      GG2=GG2+(RReC9eff*M1*gg-c7*ggplus/hat_s)**2 
      GG2=GG2+(IImC9eff*M1*gg)**2 
      
      n_G2=GG2
 
      return 
      end
      
      
      
      subroutine myF2_PhiMuMu(q2,M2,qm2,qm1,mu,mc)
     
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml    
      COMMON /nik3/ n_G2, n_F2, n_Hplus2, n_R, n_R1
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      SAVE /nikff/,/nik1/,/nik2/,/nik3/,/nikwc/
      
      real q2, M1, M2, qm2, qm1, mu, mc, mt, Mw, ml
      integer model, ias, Nf
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus    ! /nikff/
      real n_gminus, n_g0,n_B0, n_Bplus        ! /nikff/     
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_G2, n_F2, n_Hplus2, n_R, n_R1     ! /nik3/ 
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/   
	     
      real FF2 
      real hat_s, hat_r 
      real ff, BB0 
      real c7, RReC9eff, IImC9eff, c10 
      
      n_F2 = 0.0
      
      model=n_model
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf
          
      hat_s=q2/M1**2 
      hat_r=(M2/M1)**2 
         
      call myf_PhiMuMu(model,q2,M1,n_M2)
      ff=n_f 
      call myB0_PhiMuMu(model,q2,M1,n_M2)
      BB0=n_B0 
          
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7 
      c7=2.0*(qm2-qm1)*c7*(1.0-hat_r)/M1 
          
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
      call ImC9eff(q2,qm2,mu,mc,IImC9eff) 
           
      call myC10a(mt,Mw,c10) 
                            
      FF2=(c10*ff/M1)**2 
      FF2=FF2+(RReC9eff*ff/M1-c7*BB0/hat_s)**2 
      FF2=FF2+(IImC9eff*ff/M1)**2 
            
      n_F2=FF2
                                          
      return
      end
            
      
      
      subroutine myHplus2_PhiMuMu(q2,qm2,qm1,mu,mc)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_G2, n_F2, n_Hplus2, n_R, n_R1
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      SAVE /nikff/,/nik1/,/nik2/,/nik3/,/nikwc/
             	
      real q2, M1, qm2, qm1, mu, mc, mt, Mw, ml
      integer model, ias, Nf      
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus    ! /nikff/
      real n_gminus, n_g0,n_B0, n_Bplus        ! /nikff/     
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_G2, n_F2, n_Hplus2, n_R, n_R1     ! /nik3/
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/    
      
      real HH2 
      real hat_s 
      real aaplus, BBplus 
      real c7, RReC9eff, IImC9eff, c10 
      
      n_Hplus2 = 0.0
      
      model=n_model
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf
                                 
      hat_s=q2/M1**2 
                           
      call myaplus_PhiMuMu(model,q2,M1,n_M2)
      aaplus=n_aplus 
      call myBplus_PhiMuMu(model,q2,M1,n_M2)
      BBplus=n_Bplus 
                      
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7      
      c7=2.0*(qm2-qm1)*c7/M1 
                  
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
      call ImC9eff(q2,qm2,mu,mc,IImC9eff) 
                      
      call myC10a(mt,Mw,c10) 
                   
      HH2=(c10*M1*aaplus)**2 
      HH2=HH2+(RReC9eff*M1*aaplus-c7*BBplus/hat_s)**2 
      HH2=HH2+(IImC9eff*M1*aaplus)**2 
      
      n_Hplus2=HH2
                 
      return 
      end
      
      
      
      subroutine myR_PhiMuMu(q2,M2,qm2,qm1,mu,mc)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus     
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_G2, n_F2, n_Hplus2, n_R, n_R1
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      SAVE /nikff/,/nik1/,/nik2/,/nik3/,/nikwc/
                                	   
      real q2, M1, M2, qm2, qm1, mu, mc, mt, Mw, ml
      integer model, ias, Nf 
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus    ! /nikff/
      real n_gminus, n_g0,n_B0, n_Bplus        ! /nikff/     
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_G2, n_F2, n_Hplus2, n_R, n_R1     ! /nik3/
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 
                                 
      real Re 
      real hat_s, hat_r 
      real ff, BB0, aaplus, BBplus 
      real c7, RReC9eff, IImC9eff, c10 
      
      n_R = 0.0
      
      model=n_model
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf
                          
      hat_s=q2/M1**2 
      hat_r=(M2/M1)**2 
                     
      call myf_PhiMuMu(model,q2,M1,n_M2)
      ff=n_f 
      call myB0_PhiMuMu(model,q2,M1,n_M2)
      BB0=n_B0
      call myaplus_PhiMuMu(model,q2,M1,n_M2)
      aaplus=n_aplus 
      call myBplus_PhiMuMu(model,q2,M1,n_M2)
      BBplus=n_Bplus 
                
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7 
      c7=2.0*(qm2-qm1)*c7/M1 
                        
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
      call ImC9eff(q2,qm2,mu,mc,IImC9eff) 
                      
      call myC10a(mt,Mw,c10) 
                   
      Re=(RReC9eff*ff/M1-c7*BB0*(1.0-hat_r)/hat_s)*
     #(RReC9eff*M1*aaplus-c7*BBplus/hat_s) 
      Re=Re+(IImC9eff**2)*ff*aaplus 
      Re=Re+(c10**2)*ff*aaplus 
      
      n_R=Re
                        
      return
      end
      
      
      
      subroutine myR1_PhiMuMu(q2,M2,qm2,qm1,mu,mc)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus      
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_G2, n_F2, n_Hplus2, n_R, n_R1
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7
      SAVE /nikff/,/nik1/,/nik2/,/nik3/,/nikwc/
                                    
      real q2, M1, M2, qm2, qm1, mu, mc, mt, Mw, ml
      integer model, ias, Nf
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus    ! /nikff/
      real n_gminus, n_g0,n_B0, n_Bplus        ! /nikff/     
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_G2, n_F2, n_Hplus2, n_R, n_R1     ! /nik3/ 
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7 ! /nikwc/ 
                                  	     
      real Re1 
      real hat_s, hat_r 
      real gg, ggplus, ff, BB0 
      real c7, RReC9eff, c10 
      
      n_R1 = 0.0
      Re1=0.0
      
      model=n_model
      ias=n_ias
      M1=n_M1
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      Nf=n_Nf 
      
      hat_s=q2/M1**2 
      hat_r=(M2/M1)**2 
                                                                  
      call myg_PhiMuMu(model,q2,M1,n_M2)
      gg=n_g 
      
      call mygplus_PhiMuMu(model,q2,M1)
      ggplus=n_gplus 
      
      call myf_PhiMuMu(model,q2,M1,n_M2)
      ff=n_f 
      
      call myB0_PhiMuMu(model,q2,M1,n_M2)
      BB0=n_B0  
                      
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7 
      c7=2.0*c7/M1 ! correct formula 
      
                  
      call ReC9eff(q2,qm2,mu,mc,RReC9eff) 
                   
      call myC10a(mt,Mw,c10)
              
      Re1=(RReC9eff*M1*gg-c7*(qm2+qm1)*ggplus/hat_s)*(c10*ff/M1)
      Re1=Re1+(RReC9eff*ff/M1-c7*(qm2-qm1)*(1.0-hat_r)*BB0/hat_s)
     #*c10*M1*gg 
      
      n_R1=Re1
               
      return
      end
      
      
      
      subroutine mybeta1_PhiMuMu(q2, t, M2, qm2, qm1, mu, mc)
     
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_G2, n_F2, n_Hplus2, n_R, n_R1      
      COMMON /nik4/ n_beta1, n_beta2, n_dV, n_Matrix2   
      SAVE /nik2/,/nik3/,/nik4/
                                                        
      real q2, t, M1, M2, qm2, qm1, mu, mc, ml
      
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_G2, n_F2, n_Hplus2, n_R, n_R1     ! /nik3/
      real  n_beta1, n_beta2, n_dV, n_Matrix2  ! /nik4/
                           	  
      real bbeta1 
      real hat_s, hat_r, hat_t, hat_ml 
      real GG2, FF2, HH2, Re 
      real PPi, LLambda 
      
      n_beta1 = 0.0
      
      M1=n_M1
      ml=n_ml 
                             
      hat_s=q2/M1**2 
      hat_t=t/M1**2 
      hat_r=(M2/M1)**2 
      hat_ml=(ml/M1)**2 
                              
      call myG2_PhiMuMu(q2,qm2,qm1,mu,mc)
      GG2=n_G2
       
      call myF2_PhiMuMu(q2,M2,qm2,qm1,mu,mc)
      FF2=n_F2 
      
      call myHplus2_PhiMuMu(q2,qm2,qm1,mu,mc)
      HH2=n_Hplus2 
      
      call myR_PhiMuMu(q2,M2,qm2,qm1,mu,mc)
      Re=n_R
       
      LLambda = 0.0                             
      call myLambda(1.0,hat_s,hat_r,LLambda)

c     . call myPi(hat_s,hat_t,hat_r,hat_ml,PPi) 
      PPi=(hat_t-1.0)*(hat_t-hat_r)+hat_s*hat_t 
      PPi=PPi+hat_ml*(1.0+hat_r+hat_ml-hat_s-2.0*hat_t) 
                                   
      bbeta1=((hat_s+2.0*hat_ml)*LLambda+2.0*hat_s*PPi)*GG2 
      bbeta1=bbeta1+(hat_s+2.0*hat_ml-PPi/(2.0*hat_r))*FF2 
      bbeta1=bbeta1-LLambda*PPi*HH2/(2.0*hat_r) 
      bbeta1=bbeta1+(hat_s-1.0+hat_r)*PPi*Re/hat_r 
      
      n_beta1=bbeta1
       
      return 
      end
      
      
                         
      subroutine mybeta2_PhiMuMu(q2, t, M2, qm2, qm1, mu, mc)
      
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik3/ n_G2, n_F2, n_Hplus2, n_R, n_R1
      COMMON /nik4/ n_beta1, n_beta2, n_dV, n_Matrix2
      SAVE /nik2/,/nik3/,/nik4/
                           	       
      real q2, t, M1, M2, qm2, qm1, mu, mc, ml
      
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real n_G2, n_F2, n_Hplus2, n_R, n_R1     ! /nik3/
      real n_beta1, n_beta2, n_dV, n_Matrix2       ! /nik4/
                                       
      real bbeta2 
      real hat_s, hat_r, hat_t, hat_ml 
      real Re1 
      
      n_beta2 = 0.0
               
      M1=n_M1
      ml=n_ml      
                              
      hat_s=q2/M1**2 
      hat_t=t/M1**2 
      hat_r=(M2/M1)**2 
      hat_ml=(ml/M1)**2
                           
      call myR1_PhiMuMu(q2,M2,qm2,qm1,mu,mc)    
      Re1=n_R1
                    
      bbeta2=2.0*hat_s*(2.0*hat_t+hat_s-hat_r-1.0-2.0*hat_ml)
      bbeta2=bbeta2*Re1 
        
      n_beta2=bbeta2
                  
      return
      end
                           
                     
               
      subroutine mydeltaV_PhiMuMu(q2,M2) 
                   
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik4/ n_beta1, n_beta2, n_dV, n_Matrix2
      SAVE /nikff/,/nik2/,/nik4/
     
      real n_g,n_f,n_aplus,n_aminus,n_gplus    ! /nikff/
      real n_gminus, n_g0,n_B0, n_Bplus        ! /nikff/ 
                                                                    
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real  n_beta1, n_beta2, n_dV, n_Matrix2  ! /nik4/ 
                                 
      real q2, M1, M2, mt, Mw
      integer model      
                              
      real ddV 
      real hat_s, hat_r 
      real gg, ff, aaplus, aaminus 
      real c10 
      real LLambda
      
      n_dV = 0.0
      
      model=n_model
      M1=n_M1
      mt=n_mt
      Mw=n_Mw 
                         
      hat_s=q2/M1**2 
      hat_r=(M2/M1)**2 

c      print *,'                 '
c      print *,' BEGIN subroutine mydeltaV '
c      print *,' model =',model 
c      print *,'     M1 =', M1
c      print *,'     M2 =', M2
c      print *,'   n_M2 =', n_M2
c      print *,'     mt =', mt
c      print *,'     Mw =', Mw
c      print *,'     q2 =', q2
c      print *, ' hat_s =', hat_s
c      print *, ' hat_r =', hat_r      
       
      LLambda = 0.0                 
      call myLambda(1.0,hat_s,hat_r,LLambda)
                     
      call myg_PhiMuMu(model,q2,M1,n_M2)
      gg=n_g 
      call myf_PhiMuMu(model,q2,M1,n_M2)
      ff=n_f 
      call myaplus_PhiMuMu(model,q2,M1,n_M2)
      aaplus=n_aplus 
      call myaminus_PhiMuMu(model,q2,M1,n_M2)
      aaminus=n_aminus 
                   
      call myC10a(mt,Mw,c10) 
                   
      ddV=(-2.0)*(gg*M1)**2-3.0*((ff/M1)**2)/LLambda 
      ddV=ddV+(2.0*(1.0+hat_r)-hat_s)*(aaplus*M1)**2/(4.0*hat_r) 
      ddV=ddV+hat_s*(aaminus*M1)**2/(4.0*hat_r) 
      ddV=ddV+(ff*aaplus+ff*aaminus)/(2.0*hat_r) 
      ddV=ddV+(1.0-hat_r)*aaplus*aaminus*(M1**2)/(2.0*hat_r) 
      ddV=ddV*LLambda*(c10**2)/2.0 
      
c      print *,'  Lambda =', LLambda
c      print *,'     c10 =', c10
c      print *,'      gg =', gg
c      print *,'      ff =', ff
c      print *,'  aaplus =', aaplus
c      print *,' aaminus =', aaminus
        
c      print *,'    model =',model 
c      print *,'       M1 =', M1
c      print *,'       M2 =', M2
c      print *,'     n_M2 =', n_M2
c      print *,'       mt =', mt
c      print *,'       Mw =', Mw
c      print *,'      ddV =', ddV      
c      print *,' END subroutine mydeltaV_PhiMuMu '
c      print *,'                '
      
      n_dV=ddV
               
      return 
      end
      
      
      
c     ******************************************************************** 
c     ******************************************************************** 
c     *                                                                  * 
c     *		       SQUARE of MATRIX ELEMENT                          * 
c     *               for B\to K^* ell^+ell^- decay.                     * 
c     *                                                                  * 
c     *                     by formula (33) in                           * 
c     *    D.Melikhov, N.Nikitin, S.Simula, Phys.rev.D57, p.6814 (1998). * 
c     *                                                                  * 
c     *------------------------------------------------------------------* 
c     *------------------------------------------------------------------* 
c     *                                                                  * 
c     *    model --  switching parametr of the model for form factor     * 
c     *              calculations:                                       * 
c     *     model=1-3 - CQM-MS (D.Melikhov, B.Stech, Phys.Rev.D62,       * 
c     *    				       p.014006, 2000 ),         * 
c     *                                                                  * 
c     * res_swch --  switching parametr:                                 * 
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
c     *     n_M2 -- in COMMON /nik2/ - mass of the REAL K^* - meson, GeV *
c     *       M2 --  mass of the VIRTUAL K^* - meson, GeV;               *
c     *              M2.NE.n_M2 !!!;                                     *
c     *  For the form-factor calculation we use n_M2 mass!!!!!           * 
c     *       mt --  the mass of t-quark, GeV;			         * 
c     *       Mw --  the mass of W-meson, GeV;	                         * 
c     *       ml --  the lepton mass, GeV;                               * 
c     *       Nf --  number of "effective" flavours ( Nf=5 for b-quark); * 
c     *       the number of the collors = 3;                             * 
c     *                                                                  *
c     ********************************************************************
c     ******************************************************************** 
      subroutine m2fortran_PhiMuMu(hat_s,hat_t,M2)
            
      dimension RMM2_max(3), RMM2_max_resonant(3), qm2(3), 
     #qm1(3), qmc(3) 

      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik4/ n_beta1, n_beta2, n_dV, n_Matrix2
      SAVE /nik1/,/nik2/,/nik4/
      
c......The COMMON blocs parameters
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real  n_beta1, n_beta2, n_dV, n_Matrix2  ! /nik4/
                             	 
      real hat_s, hat_t, M1, M2, mt, Mw, ml 
      integer model, res_swch, ias, Nf 
                  
      real bbeta1, bbeta2, ddV
      integer i 
            
c     .the mandelstam variables; q2=q^2=s 
      real q2, t

c     .m_l^2/M_1^2 */ 
      real hat_ml
c     .mu - scale parameter, GeV  
      real mu 
      
c     .the value of the matrix element 
      real MM2 
c     .the maximum value of the nonresonant matrix elemet MM2
      real RMM2_max
c     .the maximum value of the resonant matrix elemet MM2 
      real RMM2_max_resonant
            
c     .b-quark mass ( in the B -- meson M1), GeV  
      real qm2 
c     .qm1 --  mass of the light quark, on wich m2 transition   
c     .in light meson M2, GeV 				
      real qm1 
c     .mc --  c-quark mass, GeV 
      real qmc 
            
      model=n_model 
      res_swch=n_res_swch
      ias=n_ias
      Nf=n_Nf
      M1=n_M1 
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
      
      n_beta1 = 0.0 
      n_beta2 = 0.0 
      n_dV = 0.0 
      n_Matrix2 = 0.0
           
      q2=hat_s*(M1**2) 
      t=hat_t*(M1**2) 
      hat_ml=(ml/M1)**2 
              
      RMM2_max_resonant(1) = 10000000.0 
      RMM2_max_resonant(2) = 10000000.0
      RMM2_max_resonant(3) = 10000000.0
          
      qm2(1) = 4.85
      qm2(2) = 4.85
      qm2(3) = 4.85
               
      qm1(1) = 0.35
      qm1(2) = 0.35
      qm1(3) = 0.35
                     
      qmc(1) = 1.45
      qmc(2) = 1.45
      qmc(3) = 1.45 
               
      i=model 
      mu=qm2(i)
            
         
c     .inicialisation of the MM2_max()          
      if(model.EQ.1) then
         RMM2_max(1) = 7000000.0   ! for e^+e^-
         RMM2_max(2) = 70.0        ! for \mu^+\mu^-
         RMM2_max(3) = 15.0        ! for \tau^+\tau^-
      endif      
      if(model.EQ.2) then
         RMM2_max(1) = 7000000.0   ! for e^+e^-
         RMM2_max(2) = 70.0        ! for \mu^+\mu^-
         RMM2_max(3) = 15.0        ! for \tau^+\tau^-
      endif
      if(model.EQ.3) then
         RMM2_max(1)= 7000000.0    ! for e^+e^-
         RMM2_max(2)= 70.0         ! for \mu^+\mu^-
         RMM2_max(3)= 15.0         ! for \tau^+\tau^-
      endif      
            
c     .The matrix element calculation                   
      call mybeta1_PhiMuMu(q2,t,M2,qm2(i),qm1(i),mu,qmc(i))
      bbeta1=n_beta1
      
      call mybeta2_PhiMuMu(q2,t,M2,qm2(i),qm1(i),mu,qmc(i))
      bbeta2=n_beta2

      call mydeltaV_PhiMuMu(q2,M2)
      ddV=n_dV
                  
      MM2=bbeta1+bbeta2+4.0*hat_ml*ddV
         
c     .what RMM2_max[i] to selected?  
      if(ml.LT.0.09) then
         i=1
      endif
      if(ml.LT.0.2) then
         i=2
      endif	 
      if(ml.GE.0.2) then
         i=3
      endif		 
                     
c     .The normalisation of the matrix element
      if(res_swch.EQ.0) then
         MM2=MM2/RMM2_max(i)
      endif
      if(res_swch.EQ.1) then
         MM2=MM2/RMM2_max_resonant(i)
      endif
                     
      if(MM2.LT.0.0) then
         print *,'             '
         print *,' subroutine m2fortran_PhiMuMu'
         print *,' Fatal ERROR: the square of the normalise 
     #matrix element < 0'
         print *,' hat_s =', hat_s, '   hat_t =', hat_t
         print *,' MM2 =', MM2
         print *,' By!'
         print *,'             '
         MM2=-1.0
         STOP 
      endif	  
                   
      if(MM2.GT.1.0) then
         print *,'             '
         print *,' subroutine m2fortran_PhiMuMu'      		   
         print *,' Fatal ERROR: the square of the normalise 
     #matrix element > 1'
         print *,' hat_s =', hat_s, '   hat_t =', hat_t
         print *,' MM2 =', MM2
         print *,' By!'
         print *,'             '
         MM2=-2.0
         STOP
      endif	      	 
      
      n_Matrix2=MM2
      
c      print *,' ******************************** '
c      print *,' *                              * '
c      print *,' *   n_Matrix2 =', n_Matrix2,  '* '    
c      print *,' *   END subroutine m2fortran   * '
c      print *,' *                              * '
c      print *,' ******************************** '
c      print *,'                                  '
                
      return
      end 
