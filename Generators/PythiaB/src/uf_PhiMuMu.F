      SUBROUTINE uf_PhiMuMu(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : USER_FINSEL                                                   *
*        (module)                                                      *
*                                                                      *
* Description : User subroutine to define  requirements for event      *
*                 accepting/rejecting additional or                    *
*                 other that in subroutine finsel                      *
*               PERFORM decay Bs->phi(KK)mu+mu-                        *  
* Arguments   :  IRET (in/out) accepting(0)/rejecting(1) event         *
*                                                                      *
*                                                                      *
* Author :       Maria Smizanska             Date : 06/10/1996         *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*  Nikolai Nikitine                          Date : 09/03/2005         *
*  S.Sivoklokov                                     01/09/2006         *
* Keywords :                                                           *
*        For the decay B^0_s\to (\phi^0\to K^+K^^-)\mu^+\mu^-          *
*----------------------------------------------------------------------*
*
C...Double precision and integer declarations.                               
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)                           
      IMPLICIT INTEGER(I-N)                                                 
c      INTEGER PYK,PYCHGE,PYCOMP
      INTEGER PYCOMP
       
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
*     user finsel decay selection and decay parameters
      common /ufdecay/ cufdcp(10),iufdc
      save   /ufdecay/
c Parameters: 
c    cufdcp( 1) : KEY_L1 (YES/NO)
c    cufdcp( 2) : pt_trig1
c    cufdcp( 3) : etha_trig1
c    cufdcp( 4) : KEY_L2
c    cufdcp( 5) : pt_trig2
c    cufdcp( 6) : etha_trig2
c    cufdcp( 7) :
c    cufdcp( 8) :
c    cufdcp( 9) :
c    cufdcp(10) :
* S.Sivoklokov: a trick to emulate Lvl1,Lvl2 here (trig1 and trig2.F not used)
*  LVL1 muon cuts
*      common/lvl1/tr1,pt_trig1, etha_trig1
      common/trg1/i_muon_trig1
*      SAVE/lvl1/
      SAVE/trg1/
*  LVL2 muon cut and electron tag cut
*      common/lvl2/tr2,type,pt_trig2, etha_trig2
      common/trg2/i_muon_trig2,i_ele_trig2
*      SAVE/lvl2/
      SAVE/trg2/
                              
                              
              
      integer tree
      COMMON/btree/ntree,tree(4000)                                            
c......My COMMON blocs
      COMMON /nik1/ n_model, n_res_swch, n_ias, n_Nf
      COMMON /nik2/ n_M1, n_M2, n_mt, n_Mw, n_ml
                                                                             
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
      SAVE /btree/
      SAVE /nik1/,/nik2/
                         
c      DATA IFIRST /1/
c      DATA IIPRNT /0/
c      SAVE IFIRST,IIPRNT
                                                               
C ----------- end CDE --------------------------------------------------
c......The COMMON blocs parameters

      integer n_model, n_res_swch, n_ias, n_Nf
      real n_M1, n_M2, n_mt, n_Mw, n_ml
                                 
      integer nbm, nmup, nmum, ikf, imf

      idebug = 0 
      i_muon_trig1=0
      i_muon_trig2=0	
      pt_L1 = cufdcp(2)
      pt_L2 = cufdcp(5)
      eta_L1 = cufdcp(3)
      eta_L2 = cufdcp(6)
      if(idebug.eq.1) print *,'uf_PhiMuMu: LVL1:pt_trig1 etha_trig1',
     +      pt_L1,eta_L1
                
               
      
c......Read the integer parameters
      n_model=1
      n_res_swch=0
      n_ias=1
      n_Nf=5
c      print *,' model =',n_model,'   res_swch =',n_res_swch,
c     #'   ias =',n_ias,'   Nf =', n_Nf

c......to read the meson masses: M1 and M2
      n_M1=PMAS(PYCOMP(531),1)
      n_M2=PMAS(PYCOMP(333),1)
c      print *,' M1 =',n_M1,'   M2 =',n_M2

c......to read the leptonic mass: ml
      n_ml=PMAS(PYCOMP(13),1)
c      print *,' ml =', n_ml

c......to read the mass: of t-quark and W-boson
      n_mt=PMAS(PYCOMP(6),1)
      n_Mw=PMAS(PYCOMP(24),1)
c      print *,' mt =', n_mt,'   Mw =', n_Mw
      
      nmup=0  ! the number of \mu^+ in event
      nmum=0  ! the number of \mu^- in event
      nbm = 0 ! the number of B^0_s-mesons in event
      
      kfKst=333 ! KF - code for \phi^0
      kfmum= 13 ! KF - code for \mu^-
      kfmup=-13 ! KF - code for \mu^+
      
      do ikf=1,N
c	print *, N, '  k(',ikf,',2) =',k(ikf,2)      
        if(k(ikf,2).eq.531) then
	  do imf=k(ikf,4),k(ikf,5)
c	   print *, imf,'  k(',imf,',2) =',k(imf,2)
	   if(k(imf,2).eq.333) then
             nbm = nbm+1
c             print *,' call MATRIX_and_KINEMATICS'
             call MATRIX_and_KINEMATICS_PhiMuMu(ikf)
	   endif
	  enddo
	  if(nbm.EQ.0) then
c	    print *,'FATAL ERROR in user_finsel.F'
c	    print *,'Unknown decay channel of B^0_s: ikf =', ikf
c	    call pylist(2)
	  endif
        endif
	
      enddo
      
      iret=1
      if(nbm.LE.0) return 
      iret = 0
* S.Sivoklokov L1 and L2 emulation:
      if(cufdcp(1).gt.0) then 
         iret = 1
         do I=1,N
            if(IABS(K(I,2)).eq.13) then
               PT = DSQRT(P(I,1)**2+P(I,2)**2)
               TH = DACOS(P(I,3)/DSQRT(PT**2+P(I,3)**2))
               ETA = -DLOG(MAX(.0001,DABS(DTAN(.5*TH))))
               if(PT.gt.pt_L1.and.abs(ETA).lt.eta_L1) then
                  iret=0
                  i_muon_trig1=I
               endif
            endif
         enddo
      endif


      if(cufdcp(4).gt.0) then
         iret = 1
         if(i_muon_trig1.eq.0) return
         do I=1,N
            PT = DSQRT(P(I,1)**2+P(I,2)**2)
            TH = DACOS(P(I,3)/DSQRT(PT**2+P(I,3)**2))
            ETA = -DLOG(MAX(.0001,DABS(DTAN(.5*TH))))
            
            if(IABS(K(I,2)).eq.13.and.
     +           PT.gt.pt_L2.and.
     +           (ETA).lt.eta_L2) then
               if(I.ne.i_muon_trig1) then
                  iret=0
                  i_muon_trig2 = I
               endif
            endif
         enddo 
      endif

                                                                              
c      call pylist(2)
c      print *,'**   nbm =',nbm,'   nmup =',nmup,'   nmum =',nmum, 
c     #'                  **'
      if(idebug.eq.2.and.iret.eq.0) call pylist(2)

      return
*                                                             
      END          ! USER_FINSEL
                                                            
