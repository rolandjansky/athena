C...   Date:  March 2010
C...   Description:  Routine to weight events based on cross-section
C...   parameterization and/or binning function.
C...
C...   Authors:
C...   - JX dijets weighting functions by S.L. Cheung, P.O. Deviveiros, and E. Feng
C...   - Compositeness weighting functions by P.O. Deviveiros and L. Pribyl

C... Usage instructions:
C    choose a not-yet-used PYPEVWT/IEVWT(1) number
C    implement the weighting function in this subroutine, wraped in the relevant IEVWT(1) loop 
C    please make sure you fully separate your implementation from any other parts of the code; 
C    since different weighting function impementations must coexist in this subrout.
C    in your jO use (replace XX with the selected IEVWT(1) value)
C    "pypars mstp 142 1",     # Enable event weighting
C    "pypevwt ievwt 1 XX",    # Use the weighting function XX 


      SUBROUTINE PYEVWT(WTXS)
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      
C...  PYTHIA params common blocks
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      
C...  ATLAS-SPECIFIC COMMON: 
C     SET IEVWT(1) for weighting fuction   [IEVWT(1)=0 : no weighting func. ] 
C     SET IEVWT(2) for binning             [IEVWT(2)=0 : no binning modul.  ]
      COMMON/PYPEVWT/IEVWT(100),REVWT(100)
      
      SAVE /PYDAT1/,/PYPARS/,/PYINT1/,/PYINT2/,/PYDAT1/,/PYPEVWT/

C set defaults for the ATLAS commons:
      DATA IEVWT/100*0/
      DATA REVWT/100*0.0D0/


C_______________________________________________________________________________________ 
C...Read out subprocess number.
      ISUB=MINT(1)
      ISTSB=ISET(ISUB)
 
C...Read out tau, y*, cos(theta), tau' (where defined, else =0).
      TAU=VINT(21)
      YST=VINT(22)
      CTH=0D0
      IF(ISTSB.EQ.2.OR.ISTSB.EQ.4) CTH=VINT(23)
      TAUP=0D0
      IF(ISTSB.GE.3.AND.ISTSB.LE.5) TAUP=VINT(26)
 
C...Read out x_1, x_2, x_F, shat, that, uhat, p_T^2.
      X1=VINT(41)
      X2=VINT(42)
      XF=X1-X2
      SHAT=VINT(44)
      THAT=VINT(45)
      UHAT=VINT(46)
      PT=VINT(47)
      PT2=VINT(48)


C_______________________________________________________________________________________
C ATLAS-specific body:

C set defaults:

C...  Default:  Unweighted (weight=1)
      WTVAL=1D0

C...  Default:  Fixed bin width
      BNVAL = 1D0

C_______________________________________________________________________________________
C---------------------------------------------------------------------------------------
C binning specific part that only has effect if IEVWT(2) is set in jO
C---------------------------------------------------------------------------------------
C...  Switch for weighting function
      WTFCN=IEVWT(1)

C...  WTFCN=0:  Unweighted
      IF(WTFCN.EQ.0) WTVAL=1D0
         
C...  WTFCN=1:  JX Dijets
      IF(WTFCN.EQ.1)THEN
         VARA = 1.24360E+16
         VARB = 8.91406E+00
         VARC = 4.83110E+00
         VARD = 1.83636E-01
         WTVAL = VARA*(1.0D0-(PT/3500.0D0))**(VARB)
         WTVAL = WTVAL/(PT**(VARC+VARD*LOG(PT/3500.0D0)))
         IF(PT.LT.150) WTVAL = WTVAL * (1 + (1-PT/150.) )
      ENDIF
      
C...  WTFCN=2:  L3 Compositeness
      IF(WTFCN.EQ.2)THEN
         WTVAL=1.94896E+06*EXP(-(PT-392.)/500.)
         WTVAL2=EXP((PT-1100.)*(PT-1100.)/1.3E5)
         WTVAL3=EXP((PT-350.)*(PT-350.)/0.1E5)

         IF(PT.LT.1100)THEN
            WTVAL = WTVAL*WTVAL2
         ENDIF
         IF(PT.LT.350)THEN
            WTVAL = WTVAL*WTVAL3
         ENDIF
      ENDIF

C...  WTFCN=3:  L4 Compositeness
      IF(WTFCN.EQ.3)THEN
         WTVAL=1.94896E+06*EXP(-(PT-392.)/420.)
         WTVAL2=EXP((PT-1200.)*(PT-1200.)/1.2E5)
         WTVAL3=EXP((PT-350.)*(PT-350.)/0.1E5)

         IF(PT.LT.1200)THEN
            WTVAL = WTVAL*WTVAL2
         ENDIF
         IF(PT.LT.350)THEN
            WTVAL = WTVAL*WTVAL3
         ENDIF
      ENDIF

C...  WTFCN=4:  L5 Compositeness
      IF(WTFCN.EQ.4)THEN
         WTVAL=1.94896E+06*EXP(-(PT-392.)/220.)
         WTVAL2=EXP((PT-1200.)*(PT-1200.)/1.5E5)
         WTVAL3=EXP((PT-350.)*(PT-350.)/0.1E5)

         IF(PT.LT.1200)THEN
            WTVAL = WTVAL*WTVAL2
         ENDIF
         IF(PT.LT.350)THEN
            WTVAL = WTVAL*WTVAL3
         ENDIF
      ENDIF


C...  WTFCN=5:  bug fix for J/psi + Upsilon recommended by T. Sjostrand
      IF(WTFCN.EQ.5)THEN      
C...  Copy form for pT0 as used in multiple interactions.
C...  You are not forced to use this pT0 expression, .e.g. the studies
C...  of Marianne Bargiotti indicates that a value larger than the
C...  normal PARP(82) is required. Rather than then changing PARP(82)
C...  itself, which would mess up the multiple interactions, you may
C...  use another value, hardcoded or as a tunable parameter.
         PT0=PARP(82)*(VINT(1)/PARP(89))**PARP(90)
         PT20=PT0**2
         
C...  Introduce dampening factor.
         WTXS=(PT2/(PT20+PT2))**2
         
C...  Also dampen alpha_strong by using larger Q2 scale.
         Q2=VINT(52)
         WTXS=WTXS*(PYALPS(PT20+Q2)/PYALPS(Q2))**3
         
C     ignore subseq. code parts:
         RETURN
         
      ENDIF         

C---------------------------------------------------------------------------------------
C  end of weighting functions imple. that only have effect if IEVWT(1) is set in jO
C---------------------------------------------------------------------------------------
C---------------------------------------------------------------------------------------

         
C_______________________________________________________________________________________
C---------------------------------------------------------------------------------------
C binning specific part that only has effect if IEVWT(2) is set in jO
C---------------------------------------------------------------------------------------
C...  Switch for binning function
      BNFCN=IEVWT(2)

C...  BNFCN=0:  Fixed bin width
      IF(BNFCN.EQ.0) BNVAL = 1D0
         
C...  BNFCN=1:  Linearly increasing bin width, yields flat distribution per log(pT):
C...  dN/d(log pT) = dN/( 1/pT * d(pT) ) = pT * (dN/dpT)
C...  Thus dN/d(log pT) = 1  -->  dN/dpT = pT  --> dpT = pT
      IF(BNFCN.EQ.1)THEN
         BNVAL = PT
      ENDIF

C...  BNFCN=2:  Jet energy resolution ~ sqrt(E)
      IF(BNFCN.EQ.2)THEN
         BNVAL = SQRT(PT)
      ENDIF

C---------------------------------------------------------------------------------------
C  end of binning specific part that only has effect if IEVWT(2) is set in jO
C---------------------------------------------------------------------------------------
C---------------------------------------------------------------------------------------

C     NB: BNVAL is set to 1 by default and is only changed if binn.-specific fuction is called using IEVWT(2) in jO
C...  Finally compute efficiency (inverse of effective event weight) from weighting and binning functions
      WTXS = 1./ (WTVAL * BNVAL)
 
      RETURN
      END
