c     29.10.2002    30.10.2002    31.10.2002
c     01.11.2002    13.05.2003    21.05.2003
c     17.06.2003    02.07.2003    01.09.2006
                        
c     ********************************************************
c     ********************************************************
c     **                                                    ** 
c     **           THE FORM FACTORS CALCULATIONS            **
c     **                                                    **
c     **  for B^0_d\to K^{*0}\mu^+\mu^- decay in 3 models   **
c     **                                                    ** 
c     ********************************************************
c     ******************************************************** 
                                 
                                      
                                             
c     ***************************************************************
c     *                                                             *            
c     *               The form factors parametrisation              *
c     *  by D.Melikhov and B.Stech, Phys.Rev.D62, p.014006 (2000);  * 
c     *                     see the formula (9).                    *
c     *                                                             *
c     ***************************************************************           
      subroutine cqm9_KstMuMu(q2,M,i)
      
      dimension  sf0(7), s1(7), s2(7)
      
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10
      SAVE /nik5/
      
      real n_ffs_cqm, n_cqm9, n_cqm10 
      
      real q2, M
      integer i 
      real cqm 
      real sf0, s1, s2 
                      	    	    
      sf0(1) = 0.36
      sf0(2) = 0.32
      sf0(3) = 0.45
      sf0(4) = 0.44
      sf0(5) = 0.39	    
      sf0(6) = 0.39    
      sf0(7) = 0.27
      
      s1(1) = 0.64
      s1(2) = 1.23
      s1(3) = 0.46
      s1(4) = 0.45
      s1(5) = 0.45
      s1(6) = 0.72
      s1(7) = 1.31
      
      s2(1) = 0.36
      s2(2) = 0.38
      s2(3) = 0.0
      s2(4) = 0.0
      s2(5) = 0.0
      s2(6) = 0.62
      s2(7) = 0.41
                     
      cqm=sf0(i)/(1.0-s1(i)*q2/(M**2)+s2(i)*(q2**2)/M**4) 
      cqm=cqm/(1.0-q2/(M**2)) 
      
      n_cqm9=cqm
      
c      print *,' n_cqm9 =', n_cqm9
                         
      return
      end
      
      
                                       
c     ***************************************************************
c     *                                                             *         
c     *              The form factors parametrisation               * 
c     *  by D.Melikhov and B.Stech, Phys.Rev.D62, p.014006 (2000);  * 
c     *                    see the formula (10).                    *
c     *                                                             *
c     *************************************************************** 
      subroutine cqm10_KstMuMu(q2, M, i) 
      
      dimension sf0(7), s1(7), s2(7)
      
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nik5/
      
      real n_ffs_cqm, n_cqm9, n_cqm10
                
      real q2, M
      integer i
      real cqm       
      real sf0, s1, s2 
               	   	   
      sf0(1) = 0.36
      sf0(2) = 0.32
      sf0(3) = 0.45
      sf0(4) = 0.44
      sf0(5) = 0.39	    
      sf0(6) = 0.39    
      sf0(7) = 0.27
      
      s1(1) = 0.64
      s1(2) = 1.23
      s1(3) = 0.46
      s1(4) = 0.45
      s1(5) = 0.45
      s1(6) = 0.72
      s1(7) = 1.31
      
      s2(1) = 0.36
      s2(2) = 0.38
      s2(3) = 0.0
      s2(4) = 0.0
      s2(5) = 0.0
      s2(6) = 0.62
      s2(7) = 0.41   
                         
      cqm=sf0(i)/(1.0-s1(i)*q2/M**2+s2(i)*q2**2/M**4.0) 
      
      n_cqm10=cqm
                      
      return
      end
      
      

c     ***************************************************************
c     *                                                             *      
c     *              Form factors aproximation formula              * 
c     *               in the constituent quark model                * 
c     *              for the decay B\to K^*\ell^+\ell^-             * 
c     *                                                             * 
c     *  by D.Melikhov and B.Stech, Phys.Rev.D62, p.014006 (2000).  * 
c     *                                                             * 
c     *    q2    -- the square of transition 4-momentum;            * 
c     *    index -- the index of form factors:                      * 
c     *             index=1 -- A_1(q^2);                            * 
c     *             index=2 -- A_2(q^2);                            * 
c     *             index=3 -- A_0(q^2);                            * 
c     *             index=4 -- V(q^2);                              * 
c     *             index=5 -- T_1(q^2);                            * 
c     *             index=6 -- T_2(q^2);                            * 
c     *             index=7 -- T_3(q^2).                            * 
c     *                                                             *
c     *************************************************************** 
      subroutine ffs_cqm_KstMuMu(q2,index)
      
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10
      SAVE /nik5/
      
      real n_ffs_cqm, n_cqm9, n_cqm10
      
      real q2
      integer index
      real ff
      real M_P
      real M_V
       
      ff=0.0 
      M_P=5.37   ! GeV -- the mass of the B_s-meson   
      M_V=5.42   ! GeV -- the mass of the B^*_s-meson  
               
             
c      print *,'                '
c      print *,' BEGIN subroutine ffs_cqm'
c      print *,'    q2 =', q2
c      print *,' index =', index 
       
      if(index.EQ.1) then
        call cqm10_KstMuMu(q2,M_V,index)
	ff=n_cqm10
      endif
      
      if(index.EQ.2) then
        call cqm10_KstMuMu(q2,M_V,index)
	ff=n_cqm10
      endif
      
      if(index.EQ.3) then
        call cqm9_KstMuMu(q2,M_P,index)
	ff=n_cqm9
      endif
      
      if(index.EQ.4) then
        call cqm9_KstMuMu(q2,M_V,index)
	ff=n_cqm9
      endif
      
      if(index.EQ.5) then
        call cqm9_KstMuMu(q2,M_V,index)
	ff=n_cqm9
      endif	
      
      if(index.EQ.6) then
        call cqm10_KstMuMu(q2,M_V,index)
	ff=n_cqm10
      endif			

      if(index.EQ.7) then
        call cqm10_KstMuMu(q2,M_V,index)
	ff=n_cqm10
      endif

      if(index.GT.7) then
        print *,'         '
        print *,' subroutine ffs_cqm_KstMuMu '
        print *, 'Fatal ERROR: index > 7!'
	print *,' index =', index
	print *,'         '
	STOP
      endif
      
      if(index.LT.1) then
        print *,'         '
        print *,' subroutine ffs_cqm_KstMuMu '
        print *, 'Fatal ERROR: index < 1!'
	print *,' index =', index
	print *,'         '
	STOP
      endif	
	    
      n_ffs_cqm=ff	    
	    	                      
      return 
      end     
      
      
      
c     ************************************************************       
c     *                                                          *      
c     *            Form factors aproximation formula             * 
c     *                 in light-cone sum-rule                   * 
c     *            for the decay B\to\K^*\ell^+\ell^-            * 
c     *                                                          * 
c     *    by P.Ball, V.M.Braun, Phys.Rev. D58, p.094016 (1998)  * 
c     *                   or hep-ph/9805422 v2.                  * 
c     *                                                          * 
c     *  q2    -- the square of transition 4-momentum;           * 
c     *  M1    -- the mass of the B -- meson;                    * 
c     *  M2    -- the mass of the K^* -- meson;                  * 
c     *  index -- the index of form factors:                     * 
c     *           index=1 -- A_1(q^2);                           * 
c     *           index=2 -- A_2(q^2);                           * 
c     *           index=3 -- A_0(q^2);                           * 
c     *           index=4 -- V(q^2);                             * 
c     *           index=5 -- T_1(q^2);                           * 
c     *           index=6 -- T_2(q^2);                           * 
c     *           index=7 -- T_3(q^2);                           * 
c     *                                                          *
c     ************************************************************ 
      subroutine ffs_lcsr_KstMuMu(q2,M1,index,ff)
      
      dimension sf0(7), aF(7), bF(7)
      
      real q2, M1
      integer index 
      real ff
      real sf0, aF, bF	   
                 
      sf0(1) = 0.337
      sf0(2) = 0.283
      sf0(3) = 0.470
      sf0(4) = 0.458
      sf0(5) = 0.379
      sf0(6) = 0.379
      sf0(7) = 0.261
      
      aF(1) = 0.6
      aF(2) = 1.18
      aF(3) = 1.55
      aF(4) = 1.55
      aF(5) = 1.59
      aF(6) = 0.49
      aF(7) = 1.2
      
      bF(1) = -0.023
      bF(2) = 0.281
      bF(3) = 0.68
      bF(4) = 0.575
      bF(5) = 0.615
      bF(6) = -0.241
      bF(7) = 0.098
      	
      aF(index)=aF(index)*q2/M1**2 
      bF(index)=bF(index)*q2**2.0/M1**4 
      ff=sf0(index)/(1.0-aF(index)+bF(index)) 

      return 
      end 
       
      
c     ********************************************************************
c     *                                                                  *      
c     *              Form factors aproximation formula                   * 
c     *               in the constituent quark model                     * 
c     *              for the decay B\to\K^*\ell^+\ell^-                  * 
c     *                                                                  * 
c     * by D.Melikhov, N.Nikitin, S.Simula, Phys.Rev. D57, p.6814 (1998) * 
c     *                                                                  * 
c     *    q2    -- the square of transition 4-momentum;                 * 
c     *    M1    -- the mass of the B -- meson;                          * 
c     *    M2    -- the mass of the K^* -- meson;                        * 
c     *    index -- the index of form factors:                           * 
c     *             index=1 -- g(q_2);                                   * 
c     *             index=2 -- f(q_2);                                   * 
c     *             index=3 -- a_+(q^2);                                 * 
c     *             index=4 -- a_-(q^2);                                 * 
c     *             index=5 -- g_+(q^2);                                 * 
c     *             index=6 -- g_-(q^2);                                 * 
c     *             index=7 -- g_0=h(q^2).                               * 
c     *                                                                  *
c     ******************************************************************** 
      subroutine ffs_mns_KstMuMu (q2, i, mns)
      
      dimension sf0(7), s1(7), s2(7)
      
      real q2
      integer i 
      real mns 
      real sf0, s1, s2
                                  
      sf0(1) = 0.063
      sf0(2) = 2.01
      sf0(3) = -0.0454
      sf0(4) = 0.053
      sf0(5) = -0.354
      sf0(6) = 0.313
      sf0(7) = 0.0056
      
      s1(1) = 0.0523
      s1(2) = 0.0212
      s1(3) = 0.039	    
      s1(4) = 0.044
      s1(5) = 0.0523
      s1(6) = 0.053
      s1(7) = 0.0657
      
      s2(1) = 0.00066
      s2(2) = 0.00009
      s2(3) = 0.00004
      s2(4) = 0.00023
      s2(5) = 0.0007
      s2(6) = 0.00067
      s2(7) = 0.001
                      	     
      mns=sf0(i)/(1.0-s1(i)*q2+s2(i)*q2**2.0) 
                 
      return 
      end
                          
                           
c     ***************************************************************
c     *                                                             *         
c     *                    Form factor g.                           * 
c     *              For definishion see the paper:                 * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.  * 
c     *                                                             * 
c     *  model -- model for form factor calculations:               * 
c     *           model=1  CQM-MS (Phys.Rev.D62);                   * 
c     *           model=2  LCSR (Phys.Rev.D58);                     * 
c     *           model=3  CQM-MNS (Phys.Rev.D57);                  * 
c     *  q2    -- the square of transition 4-momentum;              * 
c     *  M1    -- the mass of B -- meson;                           * 
c     *  M2    -- the mass of K^* -- meson.                         * 
c     *                                                             *
c     *************************************************************** 
      subroutine myg_KstMuMu(model,q2,M1,M2)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
     
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nikff/,/nik5/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus   ! /nikff/
      real n_gminus, n_g0,n_B0, n_Bplus       ! /nikff/
      real n_ffs_cqm, n_cqm9, n_cqm10         ! /nik5/
       
      real q2, M1, M2
      integer model
      real gg
                      
      gg=0.0
            
c      print *,'                 '
c      print *,' BEGIN subroutine myg '
c      print *,' model =',model 
c      print *,' M1 =', M1
c      print *,' M2 =', M2
c      print *,' q2 =', q2   
	        
      if(model.EQ.1) then
         call ffs_cqm_KstMuMu(q2,4)
	 gg=n_ffs_cqm
         gg=gg/(M1+M2)
      endif 
           
      if(model.EQ.2) then
         call ffs_lcsr_KstMuMu(q2,M1,4,gg)
	 gg=gg/(M1+M2)
      endif
             
      if(model.EQ.3) then
         call ffs_mns_KstMuMu(q2,1,gg)
      endif
       
      if(model.NE.1.AND.model.NE.2.AND.model.NE.3) then
         print *,'            '
         print *,' subroutine myg_KstMuMu' 
         print *,' Fatal ERROR: model not all the same 1,2 or 3!'
         print *,' model =', model
         print *,'            '
	 STOP
      endif
      
      n_g=gg
      
      return
      end
      
      
              
c     **************************************************************
c     *                                                            *      
c     *                    Form factor f.                          * 
c     *              For definishion see the paper:                * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814. * 
c     *                                                            * 
c     *  model -- model for form factor calculations:              * 
c     *           model=1  CQM-MS (Phys.Rev.D62);                  * 
c     *           model=2  LCSR (Phys.Rev.D58);                    * 
c     *           model=3  CQM-MNS (Phys.Rev.D57);                 * 
c     *  q2    -- the square of transition 4-momentum;             * 
c     *  M1    -- the mass of B -- meson;      	                  * 
c     *  M2    -- the mass of K^* -- meson.                        * 
c     *                                                            *
c     ************************************************************** 
      subroutine myf_KstMuMu(model, q2, M1, M2)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
     
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nikff/,/nik5/
           
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
      real n_ffs_cqm, n_cqm9, n_cqm10
      
      real q2, M1, M2, ff
      integer model 

      ff=0.0
      
      if(model.EQ.1) then
         call ffs_cqm_KstMuMu(q2,1)
	 ff=n_ffs_cqm
	 ff=ff*(M1+M2)
      endif
      
      if(model.EQ.2) then
         call ffs_lcsr_KstMuMu(q2,M1,1,ff)
	 ff=ff*(M1+M2)
      endif 
	     
      if(model.EQ.3) then     
	 call ffs_mns_KstMuMu(q2,2,ff)
      endif
                                 
      if(model.NE.1.AND.model.NE.2.AND.model.NE.3) then
         print *,'            '
         print *,' subroutine myf_KstMuMu ' 
         print *,' Fatal ERROR: model not all the same 1,2 or 3!'
         print *,' model =', model
         print *,'            '
	 STOP
      endif
      
      n_f = ff
      
      return
      end 
      
      
      
c     ***************************************************************      
c     *                                                             *      
c     *                    Form factor a_+.                         * 
c     *              For definishion see the paper:                 * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.  * 
c     *                                                             * 
c     *  model -- model for form factor calculations:               * 
c     *           model=1  CQM-MS (Phys.Rev.D62);                   * 
c     *           model=2  LCSR (Phys.Rev.D58);                     * 
c     *           model=3  CQM-MNS (Phys.Rev.D57);                  * 
c     *  q2    -- the square of transition 4-momentum;              * 
c     *  M1    -- the mass of B -- meson;                           * 
c     *  M2    -- the mass of K^* -- meson.                         * 
c     *                                                             *
c     *************************************************************** 
      subroutine myaplus_KstMuMu(model,q2,M1,M2)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
     
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nikff/,/nik5/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
      real n_ffs_cqm, n_cqm9, n_cqm10
      
      real q2, M1, M2, aaplus
      integer model 
              
      aaplus=0.0
      
      if(model.EQ.1) then
         call ffs_cqm_KstMuMu(q2,2)
	 aaplus=n_ffs_cqm
	 aaplus=(-aaplus)/(M1+M2)
      endif
      
      if(model.EQ.2) then
         call ffs_lcsr_KstMuMu(q2,M1,2,aaplus)
	 aaplus=(-aaplus)/(M1+M2)
      endif 
             
      if(model.EQ.3) then
         call ffs_mns_KstMuMu(q2,3,aaplus)
      endif
      
      if(model.NE.1.AND.model.NE.2.AND.model.NE.3) then
         print *,'            '
         print *,' subroutine myaplus_KstMuMu ' 
         print *,' Fatal ERROR: model not all the same 1,2 or 3!'
         print *,' model =', model
         print *,'            '
         STOP
      endif 
      
      n_aplus=aaplus
                    
      return
      end
                  
                

c     ***************************************************************
c     *                                                             *    
c     *                    Form factor a_-.                         * 
c     *              For definishion see the paper:                 * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.  * 
c     *                                                             * 
c     *  model -- model for form factor calculations:               * 
c     *           model=1  CQM-MS (Phys.Rev.D62);                   * 
c     *           model=2  LCSR (Phys.Rev.D58);                     * 
c     *           model=3  CQM-MNS (Phys.Rev.D57);                  * 
c     *  q2    -- the square of transition 4-momentum;              * 
c     *  M1    -- the mass of B -- meson;                           * 
c     *  M2    -- the mass of K^* -- meson.                         * 
c     *                                                             *
c     *************************************************************** 
      subroutine myaminus_KstMuMu(model,q2,M1,M2)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
     
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nikff/,/nik5/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
      real n_ffs_cqm, n_cqm9, n_cqm10
      
      real q2, M1, M2
      integer model
      real aaminus, a1, a2, a3 
	
      aaminus=0.0
      
      if(model.EQ.1) then
         call ffs_cqm_KstMuMu(q2,3)
	 a1=n_ffs_cqm
	 call ffs_cqm_KstMuMu(q2,1)
	 a2=n_ffs_cqm
	 call ffs_cqm_KstMuMu(q2,2)
	 a3=n_ffs_cqm
	 aaminus=2.0*M2*a1-(M1+M2)*a2
	 aaminus=aaminus+(M1-M2)*a3
	 aaminus=aaminus/q2
      endif 
                   
      if(model.EQ.2) then
         call ffs_lcsr_KstMuMu(q2,M1,3,a1)
	 call ffs_lcsr_KstMuMu(q2,M1,1,a2)
	 call ffs_lcsr_KstMuMu(q2,M1,2,a3)
	 aaminus=2.0*M2*a1 
         aaminus=aaminus-(M1+M2)*a2 
	 aaminus=aaminus+(M1-M2)*a3 
	 aaminus=aaminus/q2
      endif
              
      if(model.EQ.3) then
	 call ffs_mns_KstMuMu(q2,4,aaminus) 
      endif	
      
      if(model.NE.1.AND.model.NE.2.AND.model.NE.3) then
         print *,'            '
         print *,' subroutine myaminus_KstMuMu ' 
         print *,' Fatal ERROR: model not all the same 1,2 or 3!'
         print *,' model =', model
         print *,'            '
         STOP
      endif 
      
      n_aminus=aaminus
           
      return
      end 	   
      
      
      
c     ***************************************************************
c     *                                                             *
c     *                    Form factor g_+.                         * 
c     *              For definishion see the paper:                 * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.  * 
c     *                                                             * 
c     *  model -- model for form factor calculations:               * 
c     *           model=1  CQM-MS (Phys.Rev.D62);                   * 
c     *           model=2  LCSR (Phys.Rev.D58);                     * 
c     *           model=3  CQM-MNS (Phys.Rev.D57);                  * 
c     *  q2    -- the square of transition 4-momentum;              * 
c     *  M1    -- the mass of B -- meson;                           * 
c     *                                                             *
c     *************************************************************** 
      subroutine mygplus_KstMuMu(model,q2,M1)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
     
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nikff/,/nik5/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
      real n_ffs_cqm, n_cqm9, n_cqm10 
      
      real q2, M1, ggplus 
      integer model
	
      ggplus=0.0
      
      if(model.EQ.1) then
         call ffs_cqm_KstMuMu(q2,5)
	 ggplus=-n_ffs_cqm
      endif
                              
      if(model.EQ.2) then
         call ffs_lcsr_KstMuMu(q2,M1,5,ggplus)
	 ggplus=-ggplus
      endif 
                           
      if(model.EQ.3) then
         call ffs_mns_KstMuMu(q2,5,ggplus)
      endif
                       
      if(model.NE.1.AND.model.NE.2.AND.model.NE.3) then
         print *,'            '
         print *,' subroutine mygplus_KstMuMu ' 
         print *,' Fatal ERROR: model not all the same 1,2 or 3!'
         print *,' model =', model
         print *,'            '
         STOP
      endif 
      
      n_gplus=ggplus
           
      return
      end
      
      

c     ***************************************************************
c     *                                                             *      
c     *                    Form factor g_-.                         * 
c     *              For definishion see the paper:                 * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.  * 
c     *                                                             * 
c     *  model -- model for form factor calculations:               * 
c     *           model=1  CQM-MS (Phys.Rev.D62);                   * 
c     *           model=2  LCSR (Phys.Rev.D58);                     * 
c     *           model=3  CQM-MNS (Phys.Rev.D57);                  * 
c     *  q2    -- the square of transition 4-momentum;              * 
c     *  M1    -- the mass of B -- meson;                           * 
c     *  M2    -- the mass of K^* -- meson.                         * 
c     *                                                             *
c     *************************************************************** 
      subroutine mygminus_KstMuMu(model,q2,M1,M2)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
     
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nikff/,/nik5/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
      real n_ffs_cqm, n_cqm9, n_cqm10 
      
      real q2, M1, M2, ggminus 
      integer model
      real gm1, gm2 
      
      ggminus=0.0
      
      if(model.EQ.1) then
         call ffs_cqm_KstMuMu(q2,5)
	 gm1=n_ffs_cqm
	 call ffs_cqm_KstMuMu(q2,6)
	 gm2=n_ffs_cqm
	 ggminus=gm1-gm2 
         ggminus=ggminus*(M1**2-M2**2)/q2
      endif
      
      if(model.EQ.2) then
         call ffs_lcsr_KstMuMu(q2,M1,5,gm1)
	 call ffs_lcsr_KstMuMu(q2,M1,6,gm2)
	 ggminus=gm1-gm2 
         ggminus=ggminus*(M1**2-M2**2)/q2
      endif  
                           
      if(model.EQ.3) then
         call ffs_mns_KstMuMu(q2,6,ggminus)
      endif
                   
      if(model.NE.1.AND.model.NE.2.AND.model.NE.3) then
         print *,'            '
         print *,' subroutine mygminus_KstMuMu' 
         print *,' Fatal ERROR: model not all the same 1,2 or 3!'
         print *,' model =', model
         print *,'            '
         STOP
      endif 
      
      n_gminus=ggminus
           
      return
      end
      
      
      
c     ****************************************************************
c     *                                                              *
c     *                   Form factor h=g_0.                         * 
c     *              For definishion see the paper:                  * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.   * 
c     *                                                              * 
c     *  model -- model for form factor calculations:                * 
c     *           model=1  CQM-MS (Phys.Rev.D62);                    * 
c     *           model=2  LCSR (Phys.Rev.D58);                      * 
c     *           model=3  CQM-MNS (Phys.Rev.D57);                   * 
c     *  q2    -- the square of transition 4-momentum;               * 
c     *  M1    -- the mass of B -- meson;                            * 
c     *  M2    -- the mass of K^* -- meson.                          * 
c     *                                                              *
c     **************************************************************** 
      subroutine myh_KstMuMu(model, q2, M1, M2)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
     
      COMMON /nik5/ n_ffs_cqm, n_cqm9, n_cqm10 
      SAVE /nikff/,/nik5/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
      real n_ffs_cqm, n_cqm9, n_cqm10 
      
      real q2, M1, M2, gg0 
      integer model
      real gg01, gg02, gg03
       
      gg0=0.0
      
      if(model.EQ.1) then
         call ffs_cqm_KstMuMu(q2,5)
	 gg01=n_ffs_cqm
	 call ffs_cqm_KstMuMu(q2,6)
	 gg02=n_ffs_cqm
	 call ffs_cqm_KstMuMu(q2,7)
	 gg03=n_ffs_cqm
	 gg0=(gg01-gg02)/q2 
	 gg0=gg0-gg03/(M1**2-M2**2) 
	 gg0=2.0*gg0      
      endif
       
      if(model.EQ.2) then
         call ffs_lcsr_KstMuMu(q2,M1,5,gg01)
	 call ffs_lcsr_KstMuMu(q2,M1,6,gg02)
	 call ffs_lcsr_KstMuMu(q2,M1,7,gg03)
	 gg0=(gg01-gg02)/q2 
         gg0=gg0-gg03/(M1**2-M2**2) 
	 gg0=2.0*gg0
      endif
               
      if(model.EQ.3) then      
	 call ffs_mns_KstMuMu(q2,7,gg0) 
      endif	
 
      if(model.NE.1.AND.model.NE.2.AND.model.NE.3) then
         print *,'            '
         print *,' subroutine myh_KstMuMu ' 
         print *,' Fatal ERROR: model not all the same 1,2 or 3!'
         print *,' model =', model
         print *,'            '
         STOP
      endif 
           
      n_g0=gg0	   
	   
      return
      end   
	
 

c     ****************************************************************
c     *                                                              *
c     *      Combination of the different form factors B_0.          * 
c     *              For definishion see the paper:                  * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.   * 
c     *                                                              * 
c     *  model -- model for form factor calculations:                * 
c     *           model=1  CQM;                                      * 
c     *           model=2  LCSR;                                     * 
c     *  q2    -- the square of transition 4-momentum;               * 
c     *  M1    -- the mass of B_s - meson;                           * 
c     *  M2    -- the mass of \phi - meson.                          * 
c     *                                                              *
c     ****************************************************************
      subroutine myB0_KstMuMu(model, q2, M1, M2)
      
      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
      SAVE /nikff/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
      
      real q2, M1, M2, BB0  
      integer model   
      real hat_s, hat_r
      real BB01, BB02 
 
      hat_s=q2/M1**2 
      hat_r=(M2/M1)**2
      
      call mygplus_KstMuMu(model,q2,M1)
      BB01=n_gplus
      call mygminus_KstMuMu(model,q2,M1,M2)
      BB02=n_gminus 
                                    
      BB0=BB01+hat_s*BB02/(1.0-hat_r) 
                        
      n_B0=BB0
                    
      return
      end 




c     ****************************************************************
c     *                                                              *
c     *       Combination of the different form factors B_+.         * 
c     *              For definishion see the paper:                  * 
c     *  D.Melikhov, N.Nikitin and S.Simula, Phys.Rev.D57, p.6814.   * 
c     *                                                              * 
c     *  model -- model for form factor calculations:                * 
c     *           model=1  CQM;                                      * 
c     *           model=2  LCSR;                                     * 
c     *  q2    -- the square of transition 4-momentum;               * 
c     *  M1    -- the mass of B_s - meson;                           * 
c     *  M2    -- the mass of \phi - meson.                          * 
c     *                                                              *
c     **************************************************************** 
      subroutine myBplus_KstMuMu(model, q2, M1, M2) 

      COMMON /nikff/ n_g,n_f,n_aplus,n_aminus,n_gplus,
     #n_gminus, n_g0,n_B0, n_Bplus
      SAVE /nikff/
      
      real n_g,n_f,n_aplus,n_aminus,n_gplus
      real n_gminus, n_g0,n_B0, n_Bplus
 
      real q2, M1, M2, BBplus
      real BBplus1, BBplus2
      integer model
      real hat_s 
 
      hat_s=q2/M1**2
      call myh_KstMuMu(model,q2,M1,M2)
      BBplus1=n_g0
      call mygplus_KstMuMu(model,q2,M1)
      BBplus2=n_gplus 
              
      BBplus=0.5*hat_s*(M1**2)*BBplus1 
      BBplus=BBplus+BBplus2 
      BBplus=-BBplus 
      
      n_Bplus=BBplus
      
      return
      end 
   
