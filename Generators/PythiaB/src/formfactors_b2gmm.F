c     02.07.2003   08.07.2003   09.12.2003
c     17.02.2004   19.02.2004   28.03.2004
c     29.03.2004   30.03.2004   06.05.2004
c     17.05.2004   07.06.2004   08.06.2004 
c     03.08.2004   13.08.2004 
                
            
c     *********************************************************************
c     *********************************************************************
c     **                                                                 ** 
c     **  THE FORM FACTORS FOR B^0_{d,s}\to\gamma\mu^+\mu^- CALCULATIONS **
c     **                                                                 **
c     **                                                                 **
c     **                      Version 1.06                               **
c     **                                                                 ** 
c     *********************************************************************
c     *********************************************************************
                                        
                  
                            
c     ***************************************************************
c     *                                                             *
c     *  From q^2 to E_{\gamma} in B-meson rest frame recalculation *
c     *                                                             *
c     *   by F.Kruger and D.Melikhov, Phys.rev.D67, 034002 (2003);  *
c     *                   see the formula (14)                      * 
c     *                                                             *
c     ***************************************************************
      subroutine q2toEg(q2)
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      real q2, Eg, M1
      
      M1=n_M1
      
      Eg=M1*(1.0-q2/(M1**2))/2.0
      
      n_Eg=Eg
          
      return
      end
                                      
                                             
c     ***************************************************************
c     *                                                             *            
c     *               The form factors parametrisation              *
c     *   by F.Kruger and D.Melikhov, Phys.rev.D67, 034002 (2003);  * 
c     *               see the aproximation formula (28).            *
c     *                                                             *
c     *                                                             *
c     *    q2    - the square of transition 4-momentum;             *
c     *    index - the index of form factors:                       *
c     *            index=0 - f_B;                                   *
c     *            index=1 - F_V;                                   *
c     *            index=2 - F_{TV};                                *
c     *            index=3 - F_A;                                   *
c     *            index=4 - F_{TA}.                                *
c     *                                                             *
c     *************************************************************** 
      subroutine ffs_cqm_km_gmm(q2,index)
      
      dimension kmbeta(4), kmdelta(4)
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/    
      real q2, Eg, cqm_km
      real fB, MB
      real kmbeta, kmdelta
      integer index
      
      kmbeta(1)=0.28
      kmbeta(2)=0.30
      kmbeta(3)=0.26
      kmbeta(4)=0.33
      
      kmdelta(1)=0.04
      kmdelta(2)=0.04
      kmdelta(3)=0.30
      kmdelta(4)=0.30
      
      fB=0.2     ! GeV
      MB=5.28    ! GeV
      
      call q2toEg(q2)
      Eg=n_Eg
      
      if(index.EQ.0) then
        n_ffs_cqm=fB        ! f_B > 0 !!!!
      else      
        cqm_km=kmbeta(index)*fB*MB/(kmdelta(index)+Eg)
        n_ffs_cqm=cqm_km
      endif
      
      return
      end
                         
                                    
c     ***************************************************************
c     *                                                             *
c     *      frho - the constant of the decay \rho^0\to e^+e^-      *
c     *                                                             *
c     ***************************************************************
      subroutine subfrho()
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/
      real alpha_em, pi, Mrho, Gam 
      
      alpha_em=1.0/137.0
      pi=3.1415
      Mrho=770000.0  ! KeV - the mass of the \rho^0-meson
      Gam=6.77       ! KeV the width of the decay \rho\to e^+e^-
      
      n_ffs_cqm=sqrt(4.0*pi*Mrho*(alpha_em**2)/(3.0*Gam))
            
c      print *,' f_{rho} =', n_ffs_cqm
          
      return
      end
      
c     ***************************************************************
c     *                                                             *
c     *      fomg - the constant of the decay \omega\to e^+e^-      *
c     *                                                             *
c     ***************************************************************
      subroutine subfomg()
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/
      real alpha_em, pi, Momg, Gam 
      
      alpha_em=1.0/137.0
      pi=3.1415
      Momg=782000.0  ! KeV - the mass of the \omega-meson
      Gam=0.60       ! KeV the width of the decay \rho\to e^+e^-
      
      n_ffs_cqm=alpha_em*sqrt(4.0*pi*Momg/(3.0*Gam))
            
c      print *,' f_{rho} =', n_ffs_cqm
          
      return
      end      
      
      
c     ***************************************************************
c     *                                                             *
c     *       fphi - the constant of the decay \phi\to e^+e^-       *
c     *                                                             *
c     ***************************************************************
      subroutine subfphi()
      
      COMMON /nikff2/ n_Eg, n_ffs_cqm
      
      real n_Eg, n_ffs_cqm                     ! /nikff2/
      real alpha_em, pi, Mphi, Gam 
      
      alpha_em=1.0/137.0
      pi=3.1415
      Mphi=1019.4           ! MeV - the mass of the \phi-meson
      Gam=4.46*2.9/10000.0  ! MeV the width of the decay \phi\to e^+e^-
      
      n_ffs_cqm=alpha_em*sqrt(4.0*pi*Mphi/(3.0*Gam))
      
c      print *,' f_{phi} =', n_ffs_cqm
          
      return
      end



c     ***************************************************************
c     *                                                             *
c     *            The REAL part of F_{TV} form-factor              *
c     *                                                             *
c     ***************************************************************                  
      subroutine SubReTildeFtv(q2,qm2)
      
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml      
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta
      COMMON /nikwc/ n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7      
      SAVE /nik1/,/nik2/,/nikff2/,/nikffreim/,/nikwc/
      
      integer n_model, n_res_swch, n_ias, n_Nf                      ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml                             ! /nik2/
      real n_Eg, n_ffs_cqm                                          ! /nikff2/
      real n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta   ! /nikffreim/
      real n_as, n_c1, n_c2, n_c3, n_c4, n_c5, n_c6, n_c7           ! /nikwc/ 
      
      integer ias, Nf
      real mt, Mw, mu, M1
      real q2, qm2
      real c1, c2, a2, c7, RR, RRes
      real Ftv, Ftv0, Mres, fres, Gres, Wres, fB, gplus, Qu, WA
      
c      print *, ' In  subroutine SubReTildeFtv(q2,qm2) '
      
      M1=n_M1     ! GeV the mass of the B-meson (B^0_d or B^0_s)
      mu=qm2      ! Gev the mass of the b-quark
      Mw=n_Mw
      mt=n_mt
      Nf=n_Nf
      ias=n_ias
      
      RR=0.0
      RRes=0.0
                  
      if(M1.EQ.5.28) then 
c        print *,'B^0_d - meson decay '
        call ffs_cqm_km_gmm(q2,0)
        fB=-n_ffs_cqm   ! t.k. v formulah predpolagaetsia, chto fB < 0 !!!
c       print *, ' fB =', fB
            
        gplus=-0.27    ! g_+ = -T1, sm. PRD62, 014006, 2000, p.014006-8
c       print *, ' gplus =', gplus

        Qu=2./3.    ! zarad u-quarka
            
c........\rho^0	
        Mres=0.77           ! Gev the mass of rho^0-meson
        Gres=0.151          ! GeV Polnaya shirina raspada \rho^0-mesona
	Wres=(-1.0)/sqrt(2.0) ! izotopicheskiy ves d\bar d v \rho^0-mezone
	call subfrho()
        fres=n_ffs_cqm 
        RR=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
	RRes=gplus*Wres*q2*(q2-Mres**2)*RR/fres
c	print *, ' fres =', fres, '   RRes =', RRes

c........\omega	
	Mres=0.782         ! Gev the mass of \omega-meson
        Gres=0.00844       ! GeV Polnaya shirina raspada \omega-mesona
	Wres=1.0/sqrt(2.0) ! izotopicheskiy ves d\bar d v \omega-mezone
	call subfomg()
        fres=n_ffs_cqm
        RR=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
	RRes=RRes+gplus*Wres*q2*(q2-Mres**2)*RR/fres
c	print *, ' fres =', fres, '   RRes =', RRes		
      endif
      
      if(M1.EQ.5.37) then 
c        print *,'B^0_s - meson decay '
        call ffs_cqm_km_gmm(q2,0)
        fB=-n_ffs_cqm   ! t.k. v formulah predpolagaetsia, chto fB < 0 !!!
c       print *, ' fB =', fB
      
        gplus=-0.38   ! g_+ = -T1, sm. PRD62, 014006, 2000, p.014006-11
c       print *, ' gplus =', gplus

        Qu=0.0    ! zarad u-quarka, t.e. vklad otsutstvuet!!!
	
        Mres=1.020   ! Gev the mass of \phi-meson
        Gres=0.0045  ! GeV Polnaya shirina raspada \phi-mesona
	Wres=-1.0    ! izotopicheskiy ves s\bar s v \phi-mezone
	call subfphi()
        fres=n_ffs_cqm
	RR=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
	RRes=gplus*Wres*q2*(q2-Mres**2)*RR/fres	
      endif      

c......F_{TV}(q^2)      
      call ffs_cqm_km_gmm(q2,2)
      Ftv=n_ffs_cqm
c......F_{TV}(0)      
      call ffs_cqm_km_gmm(0.0,2)
      Ftv0=n_ffs_cqm
      
      call myC1(mu,Mw,Nf,ias)
      c1=n_c1
      call myC2(mu,Mw,Nf,ias)
      c2=n_c2
      a2=c1+c2/3.0  ! t.k. v programme scheta koefficientov opr.: c1=-0,24, c2=1,1       
      call myC7gamma(mu,Mw,mt,Nf,ias)
      c7=n_c7
c      print *, ' c1 =', c1, '   c2 =', c2, '   a2 =', a2, '   c7 =', c7
      
c      weak annihilation
      WA=4.0*3.0*a2*(Qu**2)*fB/(c7*qm2)
                             
c      prinato, chto dlia B^0_d\to\ro^0: V_{ub}V^*_{ud}/V_{tb}V^*_{td}=1
c      print *,'     '	          
      n_ReTildeFtv=Ftv+Ftv0+WA-2.0*RRes   
c      print *, '  n_ReTildeFtv =', n_ReTildeFtv
c      print *,'     '
      
      return
      end

      
c     ***************************************************************
c     *                                                             *
c     *         The IMAGINARY part of F_{TV} form-factor            *
c     *                                                             *
c     ***************************************************************             
      subroutine SubImTildeFtv(q2,qm2)
      
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml      
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta
      SAVE /nik2/,/nikff2/,/nikffreim/ 
      
      real n_M1, n_M2, n_mt, n_Mw, n_ml                             ! /nik2/
      real n_Eg, n_ffs_cqm                                          ! /nikff2/
      real n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta   ! /nikffreim/

      
      real q2, qm2
      real II
      real Mres, fres, Wres, gplus, Gres, M1
      
c      print *,' subroutine SubImTildeFtv(q2,qm2) '
      
      M1=n_M1
      
      if(M1.EQ.5.28) then 
c        print *,'B^0_d - meson decay '
      
        call ffs_cqm_km_gmm(q2,0)
        fB=-n_ffs_cqm   ! t.k. v formulah predpolagaetsia, chto fB < 0 !!!
c       print *, ' fB =', fB
      
        gplus=-0.27    ! g_+ = -T1, sm. PRD62, 014006, 2000, p.014006-8
c       print *, ' gplus =', gplus

c........\rho^0
        Mres=0.77           ! Gev the mass of rho^0-meson
        Gres=0.151          ! GeV Polnaya shirina raspada \rho^0-mesona
	Wres=(-1.0)/sqrt(2.0) ! izotopicheskiy ves d\bar d v \rho^0-mezone
	call subfrho()
        fres=n_ffs_cqm
c       print *, ' fres =', fres  
        II=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
        n_ImTildeFtv=2.0*gplus*Wres*q2*Mres*Gres*II/fres

c........\omega
	Mres=0.782         ! Gev the mass of \omega-meson
        Gres=0.00844       ! GeV Polnaya shirina raspada \omega-mesona
	Wres=1.0/sqrt(2.0) ! izotopicheskiy ves d\bar d v \omega-mezone
	call subfomg()
        fres=n_ffs_cqm
c       print *, ' fres =', fres  
        II=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
        n_ImTildeFtv=n_ImTildeFtv+2.0*gplus*Wres*q2*Mres*Gres*II/fres
      endif
      
      if(M1.EQ.5.37) then 
c        print *,'B^0_s - meson decay '
        Mres=1.020   ! Gev the mass of phi-meson
        Gres=0.0045  ! GeV Polnaya shirina raspada \rho^0-mesona
	Wres=-1.0    ! izotopicheskiy ves s\bar s v \phi-mezone
      
        call ffs_cqm_km_gmm(q2,0)
        fB=-n_ffs_cqm   ! t.k. v formulah predpolagaetsia, chto fB < 0 !!!
c       print *, ' fB =', fB
      
        call subfphi()
        fres=n_ffs_cqm
c       print *, ' fres =', fres  
      
        gplus=-0.38  ! g_+ = -T1, sm. PRD62, 014006, 2000, p.014006-11
c       print *, ' gplus =', gplus
      
        II=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)         
        n_ImTildeFtv=2.0*gplus*Wres*q2*Mres*Gres*II/fres
      endif          
            
c      print *, q2, II, Mres, Gres, gplus, fres
      
c      print *,' SubImTildeFtv(q2,qm2) =', n_ImTildeFtv
      
      return
      end      
      

c     ***************************************************************
c     *                                                             *
c     *            The REAL part of F_{TA} form-factor              *
c     *                                                             *
c     ***************************************************************  
      subroutine SubReTildeFta(q2,qm2)
      
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml             
      COMMON /nikff2/ n_Eg, n_ffs_cqm 
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta      
      SAVE /nik2/,/nikff2/,/nikffreim/ 
      
      real n_M1, n_M2, n_mt, n_Mw, n_ml                             ! /nik2/      
      real n_Eg, n_ffs_cqm                                          ! /nikff2/
      real n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta   ! /nikffreim/

      
      real q2, qm2
      real Fta, Fta0, RR, RRes
      real Mres, fres, Wres, gplus, Gres, M1
      
      M1=n_M1     ! GeV the mass of the B-meson (B^0_d or B^0_s)

      if(M1.EQ.5.28) then 
c        print *,'B^0_d - meson decay '
        call ffs_cqm_km_gmm(q2,0)
        fB=-n_ffs_cqm   ! t.k. v formulah predpolagaetsia, chto fB < 0 !!!
c       print *, ' fB =', fB
            
        gplus=-0.27    ! g_+ = -T1, sm. PRD62, 014006, 2000, p.014006-8
c       print *, ' gplus =', gplus
	
        Mres=0.77           ! Gev the mass of rho^0-meson
        Gres=0.151          ! GeV Polnaya shirina raspada \rho^0-mesona
	Wres=(-1.0)/sqrt(2.0) ! izotopicheskiy ves d\bar d v \rho^0-mezone
	call subfrho()
        fres=n_ffs_cqm
c       print *, ' fres =', fres  
        RR=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
	RRes=gplus*Wres*q2*(q2-Mres**2)*RR/fres
	
	Mres=0.782         ! Gev the mass of \omega-meson
        Gres=0.00844       ! GeV Polnaya shirina raspada \omega-mesona
	Wres=1.0/sqrt(2.0) ! izotopicheskiy ves d\bar d v \omega-mezone
	call subfomg()
        fres=n_ffs_cqm
c       print *, ' fres =', fres  
        RR=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
	RRes=RRes+gplus*Wres*q2*(q2-Mres**2)*RR/fres		
      endif
      
      if(M1.EQ.5.37) then 
c        print *,'B^0_s - meson decay '
        call ffs_cqm_km_gmm(q2,0)
        fB=-n_ffs_cqm   ! t.k. v formulah predpolagaetsia, chto fB < 0 !!!
c       print *, ' fB =', fB
      
        call subfphi()
        fres=n_ffs_cqm
c       print *, ' fres =', fres  
      
        gplus=-0.38   ! g_+ = -T1, sm. PRD62, 014006, 2000, p.014006-11
c       print *, ' gplus =', gplus
	
        Mres=1.020   ! Gev the mass of \phi-meson
        Gres=0.0045  ! GeV Polnaya shirina raspada \phi-mesona
	Wres=-1.0    ! izotopicheskiy ves s\bar s v \phi-mezone
	RR=1.0/((q2-Mres**2)**2+(Mres*Gres)**2)
	RRes=gplus*Wres*q2*(q2-Mres**2)*RR/fres	
      endif 
      
c......F_{TA}(q^2)                 
      call ffs_cqm_km_gmm(q2,4)
      Fta=n_ffs_cqm
c......F_{TA}(0)                 
      call ffs_cqm_km_gmm(0.0,4)
      Fta0=n_ffs_cqm         
             
      n_ReTildeFta=Fta+Fta0-2.0*RRes
      
c      print *, ' n_ReTildeFta =', n_ReTildeFta
      
      return
      end      
      

c     ***************************************************************
c     *                                                             *
c     *         The IMAGINARY part of F_{TA} form-factor            *
c     *                                                             *
c     ***************************************************************     
      subroutine SubImTildeFta(q2,qm2)
            
      COMMON /nikffreim/ n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta
      SAVE /nikffreim/ 
      
      real n_ReTildeFtv, n_ImTildeFtv, n_ReTildeFta, n_ImTildeFta   ! /nikffreim/      
      real q2, qm2
      
      call SubImTildeFtv(q2,qm2)
    
      n_ImTildeFta=n_ImTildeFtv
      
c      print *, '  n_ImTildeFta =', n_ImTildeFta
      
      return
      end           
