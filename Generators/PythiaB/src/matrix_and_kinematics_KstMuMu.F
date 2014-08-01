c     *************************************************************************
c     *************************************************************************
c     **                                                                     **
c     **          The Monte-Karlo PYTHIA generator for the decay             **
c     **                                                                     **
c     **              B^0\to (K^{*0}\to K^+\pi^-)\mu^+\mu^-                  **
c     **                                                                     **
c     **                for Athena 12.0.4 PythiaBModule                      **
c     **                                                                     **
c     **        V JobOptions - file trebuetsia nastroyka:                    **
c     ** c...... 887 channel replaced to the decay B^0_d\to K^{0*}\mu^+\mu^- **
c     **      mdme(887,1) = 1                                                **
c     **      kfdp(887,1) = 13   ! \mu^-                                     **
c     **      kfdp(887,2) = -13  ! \mu^+                                     **
c     **      kfdp(887,3) = 313  ! K^{0*}                                    **
c     **      kfdp(887,4) = 0                                                **
c     **      kfdp(887,5) = 0                                                **
c     **      brat(887)   = DBLE(1.0)                                        **
c     **                                                                     **
c     **              The last modification:   10.10.2006                    **
c     **                                                                     **
c     **         Authors: Nickolai Nikitine, nnikit@mail.cern.ch             **
c     **                                                                     **
c     *************************************************************************
c     *************************************************************************
c                                                                                
c       *******************************************************************  
c       *                                                                 *  
c       *         subroutine MATRIX_and_KINEMATICS_KstMuMu                * 
c       *                      Version 1.05f                              * 
c       *                                                                 *  
c       *    For the calculation the decay kinematic according to the     *  
c       *        decay matrix element M2fortran(...) in FORTRAN           *
c       *              file matrix_for_fortran_v103.F                     *  
c       *                                                                 *  
c       *                                                                 *  
c       *         The parameters of the MATRIX_and_KINEMATICS:            *  
c       *                                                                 *  
c       *     jkf - line number of B-meson in the common block PYJETS.    *
c       *                                                                 *
c       *******************************************************************
                   
      subroutine MATRIX_and_KINEMATICS_KstMuMu (jkf)
            
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
      INTEGER PYCOMP
        
c......Standard COMMON blocs
                              
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
       COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
       COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
       COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
       COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
       COMMON/PYINT1/MINT(400),VINT(400)
       COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
       COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
       COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                                                                           
                                                               
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/,/PYDAT3/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEND.
c           SAVE IFIRST,IIPRNT
c      DATA IFIRST /1/
c      DATA IIPRNT /0/
*KEEP,FAMILY.
       name_self(I)=K(I,2)
       name_selv(I)=abs(K(I,2))
       name_father(I)=K(K(I,3),2)
       name_grandpa(I)=K(K(K(I,3),3),2)
       name_daughter1(I)=K(K(I,4),2)
       name_daughter2(I)=K(K(I,5),2)

       link_self(I)=I
       link_father(I)=K(I,3)
       link_grandpa(I)=K(K(I,3),3)
       link_daughter1(I)=K(I,4)
       link_daughter2(I)=K(I,5)
*KEND.         
                
c......My COMMON blocs
         
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
      COMMON /nik4/ n_beta1, n_beta2, n_dV, n_Matrix2
      SAVE /nik1/,/nik2/,/nik4/
           
c......Begine of the variables declaration
               
c......double precision velocity ov B-meson in LS
       double precision vxb, vyb, vzb       	    
       	    
c......The value of the returned matrix elemet for
c     .the m2fortran_KstMuMu(...) function
      real Matrix_element
          
c......The COMMON blocs parameters
      integer n_model, n_res_swch, n_ias, n_Nf ! /nik1/
      real n_M1, n_M2, n_mt, n_Mw, n_ml        ! /nik2/
      integer model, res_swch, ias, Nf
      real M1, M2, mt, Mw, ml
      real  n_beta1, n_beta2, n_dV, n_Matrix2  ! /nik4/         
                           
c......Decay weight and random mass for VIRTUAL K^* meson (in GeV)
      real MM2, sm2min, sm2max, bwmm2
               
c......The Mandelstam variables
      real s, t  
      
c......The Mandelstam variables/M_1^2
      real hat_s, hat_t
      
c......The boundary of the mandelstam variables
      real s_min, s_max 
         
c......Kinematics weight of the accepted event
       double precision weight
         
c......The kinematical variables
c     .
c     .PB - B-meson
c     .P1 - \ell^+
c     .P2 - \ell^-
c     .P3 - K^*
c     .
c...... (E, p_x, p_y, p_z)
      double precision PB(4), P1(4), P2(4), P3(4)
      
c......The angle between K^* and \ell^- directions in RF of B-meson:
c     .\cos\theta_{23}
      double precision cos23
      
c......The absolute values for 3-momentum of K^* and \ell^{pm}
      double precision modP1, modP2, modP3  
         
c......Tthe rotation angles
      real phi2, theta1, phi1
          
c......The rotation matrix
      double precision M(4,4)
        
c......The randomase variable
      real rend
         
c......Numbers of the lines for K^*, \mu^+ and \mu^-
      integer kline, mpline, mmline
      
c......For vertex sconservetion after "call pyrobo(...)"      	 
      double precision vk(5), vmp(5), vmm(5)    
      
c......The counters for the accept/reject
c     .of the matrix element    
      integer me_tot, me_accept
        
c......End of the variable declarations
c     .and start of the simulation program
      
      go to 100
                     
c......The errors information and program terminate
110   print *,'Fatal ERROR: Matrix_element < 0 or
     #  Matrix_element > 1 '
      print *,'Matrix_element =', Matrix_element
      print *,'hat_t_min =', t_min/M1**2,
     #'   hat_t =', hat_t,
     #'   hat_t_max =', t_max/M1**2,
     #'   hat_s =', hat_s,
     #'   hat_s_min =', s_min/M1**2,
     #'   hat_s_max =', s_max/M1**2,
     #'           G =', weight,
     #'   M1 =', M1,'   M2 =', M2,
     #'   MM2 =', MM2, '   ml =', ml
      print *, 'By!'
      STOP
120   print *,'Fatal ERROR: s_min > s_max'
      print *,'s_min =', s_min, '   s_max =', s_max
      print *,'   M1 =', M1, '   M2 =', M2,
     #'   MM2 =', MM2,'   ml =', ml
      print *, 'By!'
      STOP
130   print *,'Fatal ERROR: t_min > t_max'
      print *,'t_min =', t_min, '   t_max =', t_max
      print *,'   M1 =', M1, '   M2 =', M2, 
     #'   MM2 =', MM2,'   ml =', ml
      print *, 'By!'
      STOP
150   print *,'Fatal ERROR: modPi^2 < 0'  
      print *,'modP1 =', modP1, '   modP2 =', modP2,
     #'   modP3 =', modP3
      print *,'s =', s, '   t =', t, '   M1 =', M1,
     #'   M2 =', M2,'   MM2 =', MM2, '   ml =', ml
      print *, 'By!'
      STOP
160   print *,'Fatal ERROR: abs(cos23) > 1'
      print *,'cos23 =', cos23
      print *,'modP1 =', modP1, '   modP2 =', modP2,
     #'   modP3 =', modP3
      print *,'s =', s, '   t =', t, '   M1 =', M1,
     #'   M2 =', M2,'   MM2 =', MM2, '   ml =', ml
      print *, 'By!'
      STOP
180   print *,'Fatal ERROR: modP2**2-P2(2)**2) < 0'
       print *,'s =', s, '   t =', t, '   M1 =', M1,
     #'   M2 =', M2,'   MM2 =', MM2, '   ml =', ml
      print *, 'By!'
      STOP     
500   print *,'  '
      print *,'********************************************'
      print *,'**                                        **'
      print *,'**         NO B-MESON DECAY !!!           **'
      print *,'**                                        **'
      print *,'********************************************'
      print *,'   '
      print *,'jkf =', jkf
      print *,'KF = k(jkf,2) =',k(jkf,2)
      print *,'k(jkf,3) =',k(jkf,3)
      print *,'k(jkf,4) =',k(jkf,4)
      print *,'k(jkf,5) =',k(jkf,5)
      print *,'   ' 
      print *,'PxB=p(jkf,1) =',p(jkf,1)
      print *,'PyB=p(jkf,2) =',p(jkf,2)
      print *,'PzB=p(jkf,3) =',p(jkf,3)
      print *,' EB=p(jkf,4) =',p(jkf,4)
      print *,' MB=p(jkf,5) =',p(jkf,5)
      print *,'   '
      go to 600
      
c......Start of the generation procedure
100   continue
c      print *,'      '
c      print *,' In MATRIX_and_KINEMATICS_KstMuMu '
c      print *,'      '      
      
      model=n_model 
      res_swch=n_res_swch
      ias=n_ias
      Nf=n_Nf
      M1=n_M1 
      M2=n_M2
      mt=n_mt
      Mw=n_Mw
      ml=n_ml
            
      me_tot=0
      me_accept=0
      
10    continue
c      print *,' 10    continue '
      
c      print *,' Nf =', Nf
c      print *,' M1 =', M1
c      print *,' M2 =', M2
c      print *,' mt =', mt
c      print *,' Mw =', Mw
c      print *,' ml =', ml
      
      
c      print *,' n_Nf =', n_Nf
c      print *,' n_M1 =', n_M1
c      print *,' n_M2 =', n_M2
c      print *,' n_mt =', n_mt
c      print *,' n_Mw =', n_Mw
c      print *,' n_ml =', n_ml
c      print *,'             '
      
      
c      .random virtual K^*-meson mass
c      .
      sm2min=(M2-5.*0.051)**2
      sm2max=(M2+5.*0.051)**2
20    continue
      rend=PYR(0)
      MM2=(sm2max-sm2min)*rend+sm2min
      
      bwmm2=BW(MM2, 0.051)
      rend=PYR(0)
c      print *,' before bwmm2:  rend =', rend
c      print *,'               bwmm2 =', bwmm2 
      if(rend.GT.bwmm2) goto 20
      MM2=sqrt(MM2)
       
c      print *,'               '
c      print *,' bwmm2 =', bwmm2
c      print *,'  rend =', rend    
c      print *,'    M2 =', M2, ' GeV'
c      print *,'   MM2 =', MM2, ' GeV'
c      print *,'                '      
c     .End the virtual K^*-meson mass generation     
      
      s_min=4.*(ml**2)
      s_max=(M1-MM2)**2
      if(s_min.GT.s_max) go to 120
      
c......Chose of the random "s"
      rend=PYR(0)   
      s=(s_max-s_min)*rend+s_min
      hat_s=s/M1**2   
      
      t_min=(MM2+ml)**2
      t_max=(M1-ml)**2
      if(t_min.GT.t_max) go to 130
                  
c......Chose of the random "t"
      rend=PYR(0)
      t=(t_max-t_min)*rend+t_min
      hat_t=t/M1**2
        
      me_tot=me_tot+1      
        
c......Accept or reject this case in (s,t)-plane?
      weight=gf(DBLE(s),DBLE(t),DBLE(M1**2),
     #DBLE(ml**2),DBLE(ml**2),DBLE(MM2**2))
      if(weight.GT.0.D0) then
c         print *,' s =', s, '   t =', t
c         print *,'  weight =', weight
         go to 10
      endif
            
          
         
c ...........................................................................
c     .......................................................................
c     ..                                                                   ..
c     ..              THE NORMALISE MATRIX ELEMENT                         ..
c     ..	      for B\to K^* ell^+ell^- decay			   ..
c     ..								   ..
c     ..           See the file matrix_for_fortran_v103.F 	           ..
c     ..								   ..
c     .......................................................................
c     .......................................................................
c     ..                                                                   ..
c     ..     model   -  switching parametr of the model for form factor    ..
c     ..                calculations:                                      ..
c     ..     model=1 - CQM-MS (D.Melikhov, B.Stech, Phys.Rev.D62,          ..
c     ..                                       p.014006, 2000 ),           ..
c     ..     model=2 - LCSR (P.Ball,V.M.Braun, Phys.Rev.D58,               ..
c     ..                                          p.094016, 1998),         ..
c     ..     model=3 - CQM-MNS (D.Melikhov, N.Nikitin and S.Simula,        ..
c     ..                              Phys.Rev.D57, p.6814, 1998);         ..
c     ..                                                                   ..
c     .. res_swch --  switching parametr:                                  ..
c     ..          res_swch=0 -- switch OFF the resonant contribution,      ..
c     ..          res_swch=1 -- switch ON the resonant contribution;       ..
c     ..                                                                   ..
c     ..      ias -- switching parametr for for Lms[]                      ..
c     ..                                   in the function As(...):        ..
c     ..          ias = 0   --  Lms=min Lms,                               ..
c     ..          ias = 1   --  the mean value,                            ..
c     ..          ias = 2   --  Lms=max Lms;                               ..
c     ..							           ..
c     ..   hat_s --  q^{2}/M1^{2}=s/M1^{2};				   ..
c     ..   hat_t --  t/M1^{2};  					   ..
c     ..      M1 --  mass of the B -- meson, GeV;			   ..
c     ..      MM2 --  mass of the VIRTUAL K^* - meson, GeV;		   ..
c     ..      mt --  the mass of t-quark, GeV;  			   ..
c     ..      Mw --  the mass of W-meson, GeV;  			   ..
c     ..      ml --  the lepton mass, GeV;				   ..
c     ..      Nf --  number of "effective" flavours ( Nf=5 for b-quark);   ..
c     ..             the number of the collors = 3;			   ..
c     .. 								   ..
c     .......................................................................
c............................................................................
      call m2fortran_KstMuMu(hat_s, hat_t, MM2)
      Matrix_element=n_Matrix2
c......The errors control
      if(Matrix_element.LT.0.0) go to 110
      if(Matrix_element.GT.1.0) go to 110
        
c......Accept or reject this case according matrix element?
      rend=PYR(0)
      if(rend.GT.Matrix_element) then
c        print *,'  Matrix_element =', Matrix_element
c        print *,'  PYR(0) =', rend
c        print *,'  s =', s, '   t =', t
c        print *,'  weight =', weight     
        go to 10
      endif	
      me_accept=me_accept+1
         
c      print *,'         ' 
c      print *,'         '
c      print *,'*************************************************'
c      print *,'*************************************************'
c      print *,'**                                             **'
c      print *,'**   THE ACCSEPTION OF THE MATRIX ELEMENT      **'
c      print *,'**                                             **'
c      print *,'*************************************************'
c      print *,'*************************************************'
c      print *,'          '
c      print *,'M1 =', M1,'   M2 =', M2,'   MM2 =', MM2,
c     #'   ml =', ml
c      print *,'          '
c      print *,'s =', s,'   t =', t
c      print *,'s_min =', s_min,'   s_max =', s_max
c      print *,'t_min(s) =', t_min,'   t_max(s) =', t_max
c      print *,'          '      
c      print *,'bwmm2          =', bwmm2
c      print *,'weight         =', weight
c      print *,'Matrix_element =', Matrix_element
c      print *,'          '
c      print *,'The number of the total events    =', me_tot
c      print *,'The number of the accepted events =', me_accept   
c      print *,'The mumber of the rejected events =', me_tot-me_accept
c      print *,'           '
      
c......***********************************************************************
c      *                                                                     *
c      *                  Start of the kinematics bloc                       *
c      *                                                                     *
c......***********************************************************************
      
c.......Initial configuration in the (X,Y) plane
      
c      . for B - meson
      PB(1)=DBLE(M1)
      PB(2)=0.D0
      PB(3)=0.D0
      PB(4)=0.D0
      
c      . for K^* - meson

      P3(1)=DBLE((M1**2+MM2**2-s)/(2.*M1))
      P3(2)=DSQRT(P3(1)**2-DBLE(MM2**2))
      P3(3)=0.D0
      P3(4)=0.D0
      modP3=P3(2)
        
c      . for \ell^+ and \ell^-
      P1(1)=DBLE((M1**2+ml**2-t)/(2.*M1))
      P2(1)=DBLE((s+t-MM2**2-ml**2)/(2.*M1))
      
      if((P1(1)**2-DBLE(ml**2)).LT.0.D0) go to 150
      if((P2(1)**2-DBLE(ml**2)).LT.0.D0) go to 150
      if((P3(1)**2-DBLE(MM2**2)).LT.0.D0) go to 150
      
      modP1=DSQRT(P1(1)**2-DBLE(ml**2))
      modP2=DSQRT(P2(1)**2-DBLE(ml**2))        
      
c      . \cos\theta_{23}
      cos23=DBLE(MM2**2+ml**2+2.*P2(1)*P3(1)-t)/(2.D0*modP2*modP3)
      if(DABS(cos23).GT.1.D0) go to 160
        
c      . for \ell^-
      P2(2)=modP2*cos23
      if((modP2**2-P2(2)**2).LT.0.D0) go to 180
      P2(3)=-DSQRT(modP2**2-P2(2)**2) 
      P2(4)=0.D0
      
c      . for \ell^+
      P1(2)=-P2(2)-P3(2)
      P1(3)=-P2(3)
      P1(4)=0.D0
      
c......Rotations
      
c     .\phi2

      phi2=2.0*3.1415*PYR(0)
      
      call Mzero(M)
      M(1,1)=1.D0
      M(2,2)=1.D0
      M(3,3)=DBLE(cos(phi2))
      M(3,4)=-DBLE(sin(phi2)) 
      M(4,3)=DBLE(sin(phi2))
      M(4,4)=DBLE(cos(phi2))

      call transforme(P1,M)
      call transforme(P2,M)
      call transforme(P3,M)     
      
        
c     .\theta1
      
      theta1=3.1415*PYR(0)
      
      call Mzero(M)   
      M(1,1)=1.D0
      M(2,2)=DBLE(cos(theta1))
      M(2,3)=-DBLE(sin(theta1))
      M(3,2)=DBLE(sin(theta1))
      M(3,3)=DBLE(cos(theta1))  
      M(4,4)=1.D0
      
      call transforme(P1,M)
      call transforme(P2,M)
      call transforme(P3,M)

c      .\phi1
      
      phi1=2.0*3.1415*PYR(0)
      
      call Mzero(M)  
      M(1,1)=1.D0
      M(2,2)=1.D0
      M(3,3)=DBLE(cos(phi1)) 
      M(3,4)=-DBLE(sin(phi1))
      M(4,3)=DBLE(sin(phi1)) 
      M(4,4)=DBLE(cos(phi1))
      
      call transforme(P1,M)
      call transforme(P2,M)
      call transforme(P3,M)
      
c      print *,'          '
c      print *, 'The examinaton of the 4-moments conservation',
c     #' after rotations'
c      print *, 'EB =', pb(1),'  Etot =', p1(1)+p2(1)+p3(1)
c      print *, 'pxB =', pb(2),'   pxtot =', p1(2)+p2(2)+p3(2)
c      print *, 'pyB =', pb(3),'   pytot =', p1(3)+p2(3)+p3(3)
c      print *, 'pzB =', pb(4),'   pztot =', p1(4)+p2(4)+p3(4)
c      print *, '              '
      
c......*********************************************************
c      *                                                       *
c      *                   The PYTHIA part                     *
c      *                                                       *
c .....*********************************************************
      
c      print *,'       '
c      print *,' B-meson caracteristics '
c      print *,jkf,'  KF =',k(jkf,2)
c      print *,jkf,'  B-vertex: V =', (v(jkf,i),i = 1,5)
c      print *,'     '
       
c......the verifikation of the B-meson decay
      if(k(jkf,4).EQ.k(jkf,5)) go to 500


c......Moderniziruem zapis' raspada B^0_d\to K^{*0}\mu^+\mu^-
      do jd = k(jkf,4), k(jkf,5)
          	     
c.........K^{*0} - meson (313) 
         if(k(jd,2).eq.313)  then   
	   
	   kline = jd
c	   print *,'kline ident =', kline 
	     
c	   print *,jd,'  KF_old =',k(jd,2)  
	   
           k(jd,1)=1
           k(jd,2)=313
	   k(jd,3)=k(jd,3)
	   	   
	   if(k(jd,4).NE.0) then
             call treedel(jd)
           endif
	   k(jd,4)=0 
           k(jd,5)=0
	   
           do i=1,3
             p(jd,i) = P3(i+1)
c	     print *,'        i =', i
c	     print *,'       jd =', jd
c	     print *,'  P3(i+1) =', P3(i+1)
c	     print *,'  p(jd,i) =', p(jd,i)
           enddo
           p(jd,4) = P3(1)
           p(jd,5) = DBLE(MM2)
	          
	   do i=1,4
	     v(jd,i)=v(jd,i)
	   enddo
	     	   
c...........Calculation metod:
c          . MeV^{-1} = 6.58 * 10^{-19} s;
c          . \tau (K^{*0}) = 1/51 MeV^{-1} = 1.29 * 10^{-20} s;
c          . \tau (B^0) = 1.55 * 10^{-12} s
c          . or \tau (B^0)= pmas(pycomp(511),4) mm/s;
c          . that is \tau (K^{*0}) in mm/s equal:
c          . pmas(pycomp(511),4)[\tau (K^{*0}) s]/[\tau (B^0) s]	   
	   v(jd,5)=pmas(pycomp(511),4)*1.29D0/1.55D0
	   v(jd,5)=v(jd,5)/10**8
c	   print *,' tau =', v(jd,5), ' mm/s'
              	   	   
           do i=1,5
	     vk(i)=v(jd,i)
	   enddo		   
           
c           print *,jd,'  KF_new =',k(jd,2)	      		   
c           print *,jd,'  K^*-vertex: =', (v(jd,i),i = 1,5)	   	   	              
c           print *,'     '
	   
         endif
                         
             
c.........\mu^+ (-13) 
         if(k(jd,2).eq.(-13))   then
	 	   
           mpline = jd
c           print *,'mpline ident =', mpline 
                      
c           print *,jd,'  KF_old =',k(jd,2)
           k(jd,1)=1
           k(jd,2)=-13
           k(jd,3)=k(jd,3)
	   
	   if(k(jd,4).NE.0) then
             call treedel(jd)
           endif
	   k(jd,4)=0 
           k(jd,5)=0 
           
	   do i=1,3
             p(jd,i) = P1(i+1)
           enddo
           p(jd,4) = P1(1)
           p(jd,5) = DBLE(ml)
	   
	   do i=1,4
	     v(jd,i)=v(jd,i)
	   enddo
	   v(jd,5)=0.D0
	   
	   do i=1,5
	     vmp(i)=v(jd,i)
	   enddo 
	   
c           print *,jd,'  KF_new =',k(jd,2)
c           print *,jd,'  mu^+ vertex: =', (v(jd,i),i = 1,5)                        
c           print *,'     '
	   
         endif
               
c.........\mu^- (13) 
         if(k(jd,2).eq.13)   then
	 
	   mmline = jd
c           print *,'mmline ident =', mmline
                	         
c           print *,jd,'  KF_old =',k(jd,2)
           k(jd,1)=1
           k(jd,2)=13
           k(jd,3)=k(jd,3)
	   
	   if(k(jd,4).NE.0) then
             call treedel(jd)
           endif
	   k(jd,4)=0 
           k(jd,5)=0 
           
	   do i=1,3
             p(jd,i) = P2(i+1)
           enddo
           p(jd,4) = P2(1)
           p(jd,5) = DBLE(ml)
	   
	   do i=1,4
	     v(jd,i)=v(jd,i)
	   enddo
	   v(jd,5)=0.D0
	   
	   do i=1,5
	     vmm(i)=v(jd,i)
	   enddo
	   
c           print *,jd,'  KF_new =',k(jd,2) 
c           print *,jd,'  mu^- vertex: =', (v(jd,i),i = 1,5)                        
c           print *,'     '
	   
         endif
                 	                     	  
      enddo
      
c      print *, 'The examinaton of the 4-moments conservation',
c     #' BEFORE the Lorentz boost'
c      print *, 'EB =',  pb(1),
c     #'  Etot =',       p(k(jkf,4),4)+p(k(jkf,4)+1,4)+p(k(jkf,4)+2,4)
c      print *, 'pxB =', pb(2),
c     #'   pxtot =',     p(k(jkf,4),1)+p(k(jkf,4)+1,1)+p(k(jkf,4)+2,1)
c      print *, 'pyB =', pb(3),
c     #'   pytot =',     p(k(jkf,4),2)+p(k(jkf,4)+1,2)+p(k(jkf,4)+2,2)
c      print *, 'pzB =', pb(4),
c     #'   pztot =',     p(k(jkf,4),3)+p(k(jkf,4)+1,3)+p(k(jkf,4)+2,3)
c      print *, '               '
            
	    
c......perform Lorentz boost in the direction
c     .of B - meson momentum (EB,PxB,PyB,PzB);
c     .only for final K^{*}-meson and leptons !!!

      vxb = p(jkf,1)/p(jkf,4)
      vyb = p(jkf,2)/p(jkf,4)
      vzb = p(jkf,3)/p(jkf,4)
c      print *,'  vxb =', vxb
c      print *,'  vyb =', vyb
c      print *,'  vzb =', vzb
      
      call pyrobo(k(jkf,4),k(jkf,5),0D0,0D0,vxb,vyb,vzb)
      

c......vertexes determination after "call pyrobo(...)"
c     .K^*
      do i=1,5
        v(kline,i)=vk(i)
      enddo
c     .\mu^+
      do i=1,5
        v(mpline,i)=vmp(i)
      enddo
c     .\mu^-
      do i=1,5
        v(mmline,i)=vmm(i)
      enddo            
                        
c      print *, 'The examinaton of the 4-moments conservation',
c     #' AFTER Lorentz boost'
c      print *, 'EB =',  p(jkf,4),
c     #'  Etot =',       p(k(jkf,4),4)+p(k(jkf,4)+1,4)+p(k(jkf,4)+2,4)
c      print *, 'pxB =', p(jkf,1),
c     #'   pxtot =',     p(k(jkf,4),1)+p(k(jkf,4)+1,1)+p(k(jkf,4)+2,1)
c      print *, 'pyB =', p(jkf,2),
c     #'   pytot =',     p(k(jkf,4),2)+p(k(jkf,4)+1,2)+p(k(jkf,4)+2,2)
c      print *, 'pzB =', p(jkf,3),
c     #'   pztot =',     p(k(jkf,4),3)+p(k(jkf,4)+1,3)+p(k(jkf,4)+2,3)
c      print *, '              '
 
c      print *, '            ***         '
c      print *, '                        '
c      print *,'The decay of the K^{*0}-meson'
c......Perform the K^{*0}-meson decay
c     .on the only charged particles;
c.    .K^{*0}-meson have in "PYTHIA" only 3 decay channels:
c     .IDC=617 corresponds to the decay channel K^{*0}\to K^{+}\pi^{-}
c     .IDC=618 corresponds to the decay channel K^{*0}\to K^0\pi^0   
c     .IDC=619 corresponds to the decay channel K^{*0}\to K^0\gamma

      do i=617,619
        mdme(i,1) = 3      ! switched off the current decay channel
      enddo
      mdme(617,1) = 1      ! switched on K^*\to K^+\pi^- decay      
      call pydecy(kline)   ! perform decay only on the K^{+}\pi^{-}
      do i=617,619
        mdme(i,1) = 1      ! switched on the current decay channel
      enddo
       
c      print *, 'The vertex examinaton after K^*-decay'
c      print *,kline,     '  K^*-vertex: =', (v(kline,i),i = 1,5)
c      print *,p(k(kline,4),5),
c     #'    1-vertex: =', (v(k(kline,4),i),i = 1,5)
c      print *,p(k(kline,5),5),
c     #'    2-vertex: =', (v(k(kline,5),i),i = 1,5)
c      print *,p(mpline,5),
c     #'  mu^+ vertex: =', (v(mpline,i),i = 1,5)
c      print *,p(mmline,5),
c     #'  mu^- vertex: =', (v(mmline,i),i = 1,5)
c      print *,'        '
      
                
c      print *, 'The examinaton of the 4-moments conservation',
c     #' after K^{*0}-decay'

c      print *, ' EK^* =', p(kline,4),
c     #'   EKpitot =',      p(k(kline,4),4)+p(k(kline,5),4)
c      print *, 'pxK^* =', p(kline,1),
c     #'   pxKpitot =',     p(k(kline,4),1)+p(k(kline,5),1)
c      print *, 'pyK^* =', p(kline,2),
c     #'   pyKpitot =',     p(k(kline,4),2)+p(k(kline,5),2)
c      print *, 'pzK^* =', p(kline,3),
c     #'   pzKpitot =',     p(k(kline,4),3)+p(k(kline,5),3)
c      print *, '              '


c      call pylist(2)      
     
600   continue 
c      print *,'*************************************************'
c      print *,'*************************************************'
c      print *,'**                                             **'
c      print *,'**   THE END OF THE ACCEPTED INFORMATION       **'
c      print *,'**                                             **'
c      print *,'*************************************************'
c      print *,'*************************************************'
c      print *,'           '
c      print *,'           '
c      print *,'           '
c      print *,'           '
c      print *,'           '
      
      end
           
          
       
