#define PYTHIA

C CDECK  ID>, UPINIT.
C *CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
C *-- Author :    Chris Harris and Peter Richardson
C----------------------------------------------------------------------
      SUBROUTINE INITCHARYBDIS
C----------------------------------------------------------------------
C     Les Houches initialisation routine
C----------------------------------------------------------------------
      INCLUDE 'GravADD_i/charybdis1001.inc'
C--Les Houches run common block
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)

C--event common block
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,
     &              IDUP(MAXNUP),ISTUP(MAXNUP),MOTHUP(2,MAXNUP),
     &              ICOLUP(2,MAXNUP),PUP(5,MAXNUP),VTIMUP(MAXNUP),
     &              SPINUP(MAXNUP)
      SAVE /HEPRUP/,/HEPEUP/
C...Pythia switches 
      DOUBLE PRECISION PARP,PARI
      INTEGER MSTP,MSTI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
    
C--local variables
      INTEGER NSEARCH,I
      DOUBLE PRECISION WGTSUM,WGTSQ,SPNZRO,SPNHLF,SPNONE,PTOTAL
      DOUBLE PRECISION PPROBS(6,3)
      DATA PPROBS /1.0d0, 1.0d0, 1.0d0, 1.0d0, 1.0d0, 1.0d0,
     &     0.773d0, 0.776d0, 0.761d0, 0.743d0, 0.726d0, 0.711d0,
     &     0.688d0, 0.827d0, 0.909d0, 0.959d0, 0.990d0, 1.011d0/
C--beam particles and energies (only proton=2212 and antiproton=-2212)
c      IDBMUP(1) = 2212
c      IDBMUP(2) = 2212
c      EBMUP(1)  = 7000.0D0
c      EBMUP(2)  = 7000.0D0
c      EBMUP(1)  = 20000.0D0
c      EBMUP(2)  = 20000.0D0

 
C--Set MPLNCK and define what is meant by it:
C--MSSDEF=1 means M_GT, MSSDEF=2 means M_DL and MSSDEF=3 means M_D
C      if( MPLNCK.eq.0.0D0)  MPLNCK=1000.0D0
ccc      MPLNCK=5000.0D0
C      if(MSSDEF.eq.0)  MSSDEF=2
ccc      MSSDEF=2
C--Set number of dimensions (number of EXTRA dimensions is n=D-4)
C--TOTDIM can be in the range 6-11
C      if(TOTDIM.eq.0)  TOTDIM=6 
ccc      TOTDIM=6
C--Use Giddings+Thomas momentum scale for calling pdfs?
C--(See page 12 of hep-ph/0106219)
ccc      GTSCA=.TRUE.
C--Set mass window for black holes produced
C      if(MINMSS.eq.0.0D0)  MINMSS=5000.0D0
ccc      MINMSS=5000.0D0
C...MAXMSS default value is set here since it depends on EBMUP(1)+EBMUP(2)
      if(MAXMSS.eq.0.0D0)  MAXMSS=EBMUP(1)+EBMUP(2)
ccc      MAXMSS=EBMUP(1)+EBMUP(2)
C--Set NBODY decay of BH remnant - NBODY can be in range 2-5.
C      if(NBODY.eq.0) NBODY=2
ccc      NBODY=2
C--Turn time variation of BH Hawking temperature in decay on or off
ccc      TIMVAR=.TRUE.
C--Set which decay products are to be used:
C--MSSDEC=1 gives no heavy particles
C--MSSDEC=2 gives t, W and Z as well
C--MSSDEC=3 gives t, W, Z and Higgs as well  
C      if(MSSDEC.eq.0) MSSDEC=3
ccc      MSSDEC=3
C--Turn grey-body factors on/off
ccc      GRYBDY=.TRUE.
C--Turn kinematic cut-off of decay on (rather than M=MPLANCK cut-off)
ccc      KINCUT=.FALSE.
C--print out option (0=no printout, 1 =errors only, 2=errors+info)
      IPRINT = 1
C--parameters for the initial maximum weight search
      NSEARCH=10000
C--pdf's for the beams (use the generator default)
C--MUST BE THE SAME FOR BOTH BEAMS
      PDFGUP(1) = -1
      PDFGUP(2) = -1
      PDFSUP(1) = -1
      PDFSUP(2) = -1
C--------------------------------------------------------------------
C     END OF USER VARIABLES DON'T TOUCH ANYTHING BELOW HERE
C--------------------------------------------------------------------
C--probabilities for particles of different spin (arbitrary normalisation)
      IF (GRYBDY) THEN
         SPNZRO = PPROBS(TOTDIM-5,1)
         SPNHLF = PPROBS(TOTDIM-5,2)
         SPNONE = PPROBS(TOTDIM-5,3)
      ELSE
         SPNZRO = 1d0
         SPNHLF = 0.750d0
         SPNONE = 1d0
      ENDIF
C--calculation of probability of emission for different particle types
C--only light SM particles
      IF (MSSDEC.EQ.1) THEN
         PTOTAL = (78.0d0*SPNHLF)+(18.0d0*SPNONE)
         PQUARK = (60.0d0*SPNHLF)/PTOTAL
         PLEPT  = (12.0d0*SPNHLF)/PTOTAL
         PNEUT  = (6.0d0*SPNHLF)/PTOTAL
         PGLUON = (16.0d0*SPNONE)/PTOTAL
         PGAMMA = (2.0d0*SPNONE)/PTOTAL
         PWBOSN = 0.0D0
         PZBOSN = 0.0D0
         PHIGGS = 0.0D0   
C--light SM partcles + top W/Z      
      ELSEIF (MSSDEC.EQ.2) THEN
         PTOTAL = (3.0d0*SPNZRO)+(90.0d0*SPNHLF)+(24.0d0*SPNONE)
         PQUARK = (72.0d0*SPNHLF)/PTOTAL
         PLEPT  = (12.0d0*SPNHLF)/PTOTAL
         PNEUT  = (6.0d0*SPNHLF)/PTOTAL
         PGLUON = (16.0d0*SPNONE)/PTOTAL
         PGAMMA = (2.0d0*SPNONE)/PTOTAL
         PZBOSN = (SPNZRO+(2.0d0*SPNONE))/PTOTAL
         PWBOSN = 2.0d0*PZBOSN
         PHIGGS = 0.0D0 
C--light SM particles +top W/Z and Higgs
      ELSE
         PTOTAL = (4.0d0*SPNZRO)+(90.0d0*SPNHLF)+(24.0d0*SPNONE)
         PQUARK = (72.0d0*SPNHLF)/PTOTAL
         PLEPT  = (12.0d0*SPNHLF)/PTOTAL
         PNEUT  = (6.0d0*SPNHLF)/PTOTAL
         PGLUON = (16.0d0*SPNONE)/PTOTAL
         PGAMMA = (2.0d0*SPNONE)/PTOTAL
         PZBOSN = (SPNZRO+(2.0d0*SPNONE))/PTOTAL
         PWBOSN = 2.0d0*PZBOSN
         PHIGGS = SPNZRO/PTOTAL
      ENDIF
C--what do do with the weights(here are generating weighted events)
      IDWTUP = 1
C--only one process
      NPRUP  = 1
C--communication code
      LPRUP(1) = 1
C--calculate the maximum weight
      WGTSUM = 0.0D0
      WGTSQ = 0.0D0
      XMAXUP(1) = 0.0D0
      DO I=1,NSEARCH
         CALL CHEVNT(.FALSE.)
         WGTSUM = WGTSUM+XWGTUP
         WGTSQ  = WGTSQ+XWGTUP**2
         XMAXUP(1) = MAX(XMAXUP(1),XWGTUP)
      ENDDO
      IF(MSTP(121).EQ.1) XMAXUP(1)=PARP(121)*XMAXUP(1)
      WGTSUM = WGTSUM/DBLE(NSEARCH)
      WGTSQ  = MAX(WGTSQ/DBLE(NSEARCH)-WGTSUM**2,0.0D0)
      WGTSQ  = SQRT(WGTSQ/ DBLE(NSEARCH))
C--cross section
      XSECUP(1) = wgtsum
C--error on the cross section
      XERRUP(1) = wgtsq

      END
CDECK  ID>, UPEVNT.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Chris Harris and Peter Richardson
C----------------------------------------------------------------------
      SUBROUTINE USEGRAVADD
C----------------------------------------------------------------------
C     Les Houches event routine
C----------------------------------------------------------------------
      IMPLICIT NONE
C     write(*,*) 'in useGravADD'
      CALL CHEVNT(.TRUE.)
      END
CDECK  ID>, CHDFIV.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Ian Knowles
C-----------------------------------------------------------------------
      SUBROUTINE CHDFIV(P0,P1,P2,P3,P4,P5,*)
C-----------------------------------------------------------------------
C     Generates 5-body decay 0->1+2+3+4+5 using pure phase space
C     (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHRGEN,P0(5),P1(5),P2(5),P3(5),P4(5),P5(5),B,C,
     & AA,BB,CC,DD,EE,FF,TT,S1,RS1,GG,S2,RS2,HH,S3,PP,QQ,RR,SS,P1CM,
     & P2345(5),P2CM,P345(5),P3CM,P45(5),P4CM
      DOUBLE PRECISION TWO
      PARAMETER (TWO=2.D0)
      EXTERNAL CHRGEN
      B=P0(5)-P1(5)
      C=P2(5)+P3(5)+P4(5)+P5(5)
      IF (B.LT.C) RETURN 1
      AA=(P0(5)+P1(5))**2
      BB=B**2
      CC=C**2
      DD=(P3(5)+P4(5)+P5(5))**2
      EE=(P4(5)+P5(5))**2
      FF=(P4(5)-P5(5))**2
      TT=(B-C)*P0(5)**11/729
C Select squared masses S1, S2 and S3 of 2345, 345 and 45 subsystems
  10  S1=BB+CHRGEN(1)*(CC-BB)
      RS1=SQRT(S1)
      GG=(RS1-P2(5))**2
      S2=DD+CHRGEN(2)*(GG-DD)
      RS2=SQRT(S2)
      HH=(RS2-P3(5))**2
      S3=EE+CHRGEN(3)*(HH-EE)
      PP=(AA-S1)*(BB-S1)
      QQ=((RS1+P2(5))**2-S2)*(GG-S2)/S1
      RR=((RS2+P3(5))**2-S3)*(HH-S3)/S2
      SS=(S3-EE)*(S3-FF)/S3
      IF (PP*QQ*RR*SS*((GG-DD)*(HH-EE))**2.LT.TT*S1*S2*S3*CHRGEN(4)**2)
     & GOTO 10
C Do two body decays: 0-->1+2345, 2345-->2+345, 345-->3+45 and 45-->4+5
      P1CM=SQRT(PP/4)/P0(5)
      P2345(5)=RS1
      P2CM=SQRT(QQ/4)
      P345(5)=RS2
      P3CM=SQRT(RR/4)
      P45(5)=SQRT(S3)
      P4CM=SQRT(SS/4)
      CALL CHDTWO(P0   ,P1,P2345,P1CM,TWO,.TRUE.)
      CALL CHDTWO(P2345,P2,P345 ,P2CM,TWO,.TRUE.)
      CALL CHDTWO(P345 ,P3,P45  ,P3CM,TWO,.TRUE.)
      CALL CHDTWO(P45  ,P4,P5   ,P4CM,TWO,.TRUE.)
      END
CDECK  ID>, CHDFOR.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Ian Knowles
C-----------------------------------------------------------------------
      SUBROUTINE CHDFOR(P0,P1,P2,P3,P4,*)
C-----------------------------------------------------------------------
C     Generates 4-body decay 0->1+2+3+4 using pure phase space
C     (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHRGEN,P0(5),P1(5),P2(5),P3(5),P4(5),B,C,AA,BB,
     & CC,DD,EE,TT,S1,RS1,FF,S2,PP,QQ,RR,P1CM,P234(5),P2CM,P34(5),P3CM
      DOUBLE PRECISION TWO
      PARAMETER (TWO=2.D0)
      EXTERNAL CHRGEN
      B=P0(5)-P1(5)
      C=P2(5)+P3(5)+P4(5)
      IF (B.LT.C) RETURN 1
      AA=(P0(5)+P1(5))**2
      BB=B**2
      CC=C**2
      DD=(P3(5)+P4(5))**2
      EE=(P3(5)-P4(5))**2
      TT=(B-C)*P0(5)**7/16
C Select squared masses S1 and S2 of 234 and 34 subsystems
  10  S1=BB+CHRGEN(1)*(CC-BB)
      RS1=SQRT(S1)
      FF=(RS1-P2(5))**2
      S2=DD+CHRGEN(2)*(FF-DD)
      PP=(AA-S1)*(BB-S1)
      QQ=((RS1+P2(5))**2-S2)*(FF-S2)/S1
      RR=(S2-DD)*(S2-EE)/S2
      IF (PP*QQ*RR*(FF-DD)**2.LT.TT*S1*S2*CHRGEN(3)**2) GOTO 10
C Do two body decays: 0-->1+234, 234-->2+34 and 34-->3+4
      P1CM=SQRT(PP/4)/P0(5)
      P234(5)=RS1
      P2CM=SQRT(QQ/4)
      P34(5)=SQRT(S2)
      P3CM=SQRT(RR/4)
      CALL CHDTWO(P0  ,P1,P234,P1CM,TWO,.TRUE.)
      CALL CHDTWO(P234,P2,P34 ,P2CM,TWO,.TRUE.)
      CALL CHDTWO(P34 ,P3,P4  ,P3CM,TWO,.TRUE.)
      END
CDECK  ID>, CHDTHR.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHDTHR(P0,P1,P2,P3,*)
C-----------------------------------------------------------------------
C     GENERATES THREE-BODY DECAY 0->1+2+3 DISTRIBUTED
C     ACCORDING TO PHASE SPACE * WEIGHT
C     (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHRGEN,CHRUNI,A,B,C,D,AA,BB,CC,DD,EE,FF,PP,QQ,WW,
     & RR,PCM1,PC23,P0(5),P1(5),P2(5),P3(5),P23(5),TWO,ONE
      EXTERNAL CHRGEN,CHRUNI
      PARAMETER (ONE=1.0D0,TWO=2.D0)
      A=P0(5)+P1(5)
      B=P0(5)-P1(5)
      C=P2(5)+P3(5)
      IF (B.LT.C) RETURN 1
      D=ABS(P2(5)-P3(5))
      AA=A*A
      BB=B*B
      CC=C*C
      DD=D*D
      EE=(B-C)*(A-D)
      A=0.5D0*(AA+BB)
      B=0.5D0*(CC+DD)
      C=4.0D0/(A-B)**2
C
C  CHOOSE MASS OF SUBSYSTEM 23 WITH PRESCRIBED DISTRIBUTION
C
   10 FF=CHRUNI(0,BB,CC)
      PP=(AA-FF)*(BB-FF)
      QQ=(CC-FF)*(DD-FF)
      WW=ONE
      RR=EE*FF*CHRGEN(0)
      IF (PP*QQ*WW.LT.RR*RR) GOTO 10
C
C  FF IS MASS SQUARED OF SUBSYSTEM 23.
C
C  DO 2-BODY DECAYS 0->1+23, 23->2+3
C
      P23(5)=SQRT(FF)
      PCM1=SQRT(PP)*0.5D0/P0(5)
      PC23=SQRT(QQ)*0.5D0/P23(5)
      CALL CHDTWO(P0,P1,P23,PCM1,TWO,.TRUE.)
      CALL CHDTWO(P23,P2,P3,PC23,TWO,.TRUE.)
      END
CDECK  ID>, CHDTWO.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber & Mike Seymour
C-----------------------------------------------------------------------
      SUBROUTINE CHDTWO(P0,P1,P2,PCM,COSTH,ZAXIS)
C-----------------------------------------------------------------------
C     GENERATES DECAY 0 -> 1+2
C
C     PCM IS CM MOMENTUM
C
C     COSTH = COS THETA IN P0 REST FRAME (>1 FOR ISOTROPIC)
C     IF ZAXIS=.TRUE., COS THETA IS MEASURED FROM THE ZAXIS
C     IF .FALSE., IT IS MEASURED FROM P0'S DIRECTION 
C     (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHRUNI,ONE,ZERO,PCM,COSTH,C,S,P0(5),P1(5),P2(5),
     & PP(5),R(9)
      LOGICAL ZAXIS
      EXTERNAL CHRUNI
      PARAMETER (ZERO=0.D0, ONE=1.D0)
C--CHOOSE C.M. ANGLES
      C=COSTH
      IF (C.GT.ONE) C=CHRUNI(0,-ONE,ONE)
      S=SQRT(ONE-C*C)
      CALL CHRAZM(PCM*S,PP(1),PP(2))
C--PP IS MOMENTUM OF 2 IN C.M.
      PP(3)=-(PCM*C)
      PP(4)=SQRT(P2(5)**2+PCM**2)
      PP(5)=P2(5)
C--ROTATE IF NECESSARY
      IF (COSTH.LE.ONE.AND..NOT.ZAXIS) THEN
        CALL CHUROT(P0,ONE,ZERO,R)
        CALL CHUROB(R,PP,PP)
      ENDIF
C--BOOST FROM C.M. TO LAB FRAME
      CALL CHULOB(P0,PP,P2)
      CALL CHVDIF(4,P0,P2,P1)
      END
CDECK  ID>, CHEVNT.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Chris Harris
C----------------------------------------------------------------------
      SUBROUTINE CHEVNT(GENEVT)
C----------------------------------------------------------------------
C     Black Hole Production and Decay
C----------------------------------------------------------------------
      INCLUDE 'GravADD_i/charybdis1001.inc'
C--Les Houches run common block
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)
C--event common block
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,
     &              IDUP(MAXNUP),ISTUP(MAXNUP),MOTHUP(2,MAXNUP),
     &              ICOLUP(2,MAXNUP),PUP(5,MAXNUP),VTIMUP(MAXNUP),
     &              SPINUP(MAXNUP)
      SAVE /HEPRUP/,/HEPEUP/
C...Pythia switches
      DOUBLE PRECISION PARP,PARI
      INTEGER MSTP,MSTI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
     
C--Local Variables
      DOUBLE PRECISION ECMS,ONE,TWO,PIFAC,THREE,ZERO,
     &     CHRGEN,RPOW,A0,A1,EMJ,FACT,QSQ,HCS,prob,
     &     FACTR,RCS,BHPOW,DISF1,DISF2,OMEGAD,DL2GT,
     &     RHORSQ,PLPOW,MPFACT,PA(5),PB(5),PC(5),PCM,CHUPCM,PCMF(5),
     &     XXMIN,XLMIN,EMSCA,XX(2),DISF(13,2),GEV2PB,CHFMAS
      INTEGER STAT,IBARYN,IQRK(MAXPUP),IGLU(MAXPUP),IQBR(MAXPUP),MTRY,
     &     NQRK,NGLU,NQBR,JQRK,JGLU,JQBR,J,ISTART,CHST,iline,FSTAT(5),
     &     IP,IQ,PT,I,BHCHRG,IDN(2),NTRY,NHTRY,SAVCHR,SAVBRN,ID(5),
     &     MHTRY,LTRY,LHTRY,CHFCHG,SPIN,FSPIN(5)
      LOGICAL GENEVT,CHRLOG,FIRST
      EXTERNAL CHFMAS,CHRGEN,CHUPCM,CHRLOG,CHFCHG
      PARAMETER(ONE=1.0D0,TWO=2.0D0,THREE=3.0D0,ZERO=0.0D0,NHTRY=200)
      PARAMETER(MHTRY=20,LHTRY=200,GEV2PB=389379.D3)
      SAVE HCS,BHPOW,PLPOW,RPOW,A0,A1,FACT,MPFACT
      DATA FIRST/.TRUE./
      PIFAC = ACOS(-ONE)
      IF(FIRST) THEN
C--Convert MPLNCK to M_GT if above value is M_DL
         IF (MSSDEF.EQ.2) THEN
            DL2GT=(TWO**(TOTDIM-6)*PIFAC**(TOTDIM-5))**(1/(TOTDIM-TWO))
            INTMPL=DL2GT*MPLNCK
C--Convert MPLNCK to M_GT if above value is M_D
         ELSEIF (MSSDEF.EQ.3) THEN
            INTMPL=MPLNCK*TWO**(1/(TOTDIM-TWO))
         ELSE
            INTMPL=MPLNCK
         ENDIF
C--Calculate other parameters
         PLPOW=TWO/(TOTDIM-THREE)
         BHPOW=PLPOW-GENPOW
         RPOW=1/BHPOW
         A0=MINMSS**BHPOW
         A1=(MAXMSS**BHPOW-A0)
         IF (TOTDIM.EQ.11) THEN
            OMEGAD=(PIFAC**5)/12
         ELSEIF (TOTDIM.EQ.10) THEN
            OMEGAD=(32*PIFAC**4)/105
         ELSEIF (TOTDIM.EQ.9) THEN
            OMEGAD=(PIFAC**4)/3
         ELSEIF (TOTDIM.EQ.8) THEN
            OMEGAD=(16*PIFAC**3)/15
         ELSEIF (TOTDIM.EQ.7) THEN
            OMEGAD=PIFAC**3
         ELSEIF(TOTDIM.EQ.6) THEN
            OMEGAD=(8*PIFAC**2)/3
         ELSEIF(TOTDIM.EQ.5) THEN
            OMEGAD=2*PIFAC**2
         ENDIF
         MPFACT=INTMPL**(TOTDIM-2)
         RHFACT=(4*(2*PIFAC)**(TOTDIM-4))/((TOTDIM-2)*OMEGAD*MPFACT)
         FIRST = .FALSE.
      ENDIF      
      IF ( MINMSS.GE.MAXMSS ) THEN
        XWGTUP=0.0D0
        RETURN
      ENDIF
C--Select a mass for the produced black hole
      ECMS = 2.0D0*SQRT(EBMUP(1)*EBMUP(2))
      EMSCA=(A0+A1*CHRGEN(1))**RPOW
      BHMASS=EMSCA
      QSQ=EMSCA**2
      EMJ=EMSCA**(1-BHPOW)/BHPOW*A1
C--Calculate (r_h)**2 and initial T_H
      RHORSQ=(RHFACT*EMSCA)**PLPOW
C--Select initial momentum fractions
      XXMIN=QSQ/ECMS**2
      XLMIN=LOG(XXMIN)
      FACT=-(GEV2PB*PIFAC*RHORSQ*EMJ*XLMIN*TWO/EMSCA)
C--Change momentum scale for calling pdfs (see comment above)
      IF (GTSCA) EMSCA=1/SQRT(RHORSQ*FACTOR)
      SCALUP = EMSCA
      CALL CHPDF(XXMIN,XLMIN,EMSCA,XX,DISF)
      HCS=ZERO
      DISF1=ZERO
      DISF2=ZERO
      DO 30 IP=1,13
      DISF1=DISF1+DISF(IP,1)
 30   DISF2=DISF2+DISF(IP,2)
      FACTR=FACT*DISF1*DISF2
      HCS=FACTR
      XWGTUP = HCS
      IF ( MSTP(142).EQ.1 ) THEN
C        PARP(200)=(BHMASS**4)/(5000.0**4)
C        FACT=FACT*PARP(200)
C        PARP(200)=1.0D0
        if (DISF1*DISF2.ne.0d0) PARP(200)=0.05D0/(DISF1*DISF2)
C       write(*,*) 'in charybdis, parp(200): ', PARP(200)
        XWGTUP=XWGTUP*PARP(200)
      ENDIF
      IF(.NOT.GENEVT) RETURN
      RCS=HCS*CHRGEN(0)
      HCS = 0.0D0
      DO 20 IP=1,13
      DO 10 IQ=1,13
      FACTR=FACT*DISF(IP,1)*DISF(IQ,2)
      HCS=HCS+FACTR
      IF (HCS.GT.RCS) GOTO 99
 10   CONTINUE
 20   CONTINUE
C--Generate event
 99   IDN(1)=IP
      IDN(2)=IQ
C--put the incoming particles in the Les Houches common block
      NUP = 2
      DO I=1,2
         IF(IDN(I).GT.6.AND.IDN(I).LE.12) THEN
            IDN(I) = -MOD(IDN(I)-1,6)-1
         ELSEIF(IDN(I).EQ.13) THEN
            IDN(I) = 21
         ENDIF
         IDUP(I) = IDN(I)
         PUP(1,I) = 0.0D0
         PUP(2,I) = 0.0D0
         PUP(3,I) = XX(I)*EBMUP(I)
         PUP(4,I) = XX(I)*EBMUP(I)
         ISTUP(I) = -1
      ENDDO
      PUP(3,2) = -PUP(3,2)
      CALL CHVSUM(4,PUP(1,1),PUP(1,2),PCMF)
      CALL CHUMAS(PCMF)
C--Calculate BH charge (actually 3*charge)
      BHCHRG=CHFCHG(IDN(1))+CHFCHG(IDN(2))
C--Black Hole decay
      MTRY = 0
      CHST = BHCHRG
 35   MTRY = MTRY+1
      BHCHRG = CHST
      NUP = 2
      CALL CHVEQU(5,PCMF,PA)
 40   NUP = NUP+1
C--Check that BH mass > MPLNCK if not using KINCUT      
      IF ((.NOT.KINCUT).AND.(PA(5).LT.MPLNCK)) GOTO 55
C--Set BHMASS to correct value (necessary in time varying case)
      IF (TIMVAR) BHMASS=PA(5)
C--Select the decay product
      LTRY=0
 45   IF (MSSDEC.EQ.1) THEN
         CALL CHHBH1(PT,STAT,SPIN,BHCHRG)
      ELSEIF (MSSDEC.EQ.2) THEN
         CALL CHHBH2(PT,STAT,SPIN,BHCHRG)
      ELSE
         CALL CHHBH3(PT,STAT,SPIN,BHCHRG)
      ENDIF
C--Obtain energy of emitted parton
      CALL CHUBHS(PC(4),STAT,SPIN)
      PC(5)=CHFMAS(PT)
C--Check energy > mass for selected parton
      IF (PC(4).LT.PC(5)) THEN
         BHCHRG=BHCHRG+CHFCHG(PT)
         IF(IPRINT.EQ.2) WRITE (*,*) 'Not enough energy'
         GOTO 45
      ENDIF
C--Check that emission is kinematically allowed
      IF (PC(4).GT.(PA(5)**2+PC(5)**2)/(TWO*PA(5))) THEN
         IF (KINCUT.OR.LTRY.GT.LHTRY) THEN 
            GOTO 50
         ELSE
            LTRY=LTRY+1
            BHCHRG=BHCHRG+CHFCHG(PT)
            GOTO 45
         ENDIF
      ENDIF
C--Use 2-body phase space decay 
      PB(5)=SQRT(PA(5)**2+PC(5)**2-TWO*PA(5)*PC(4))
C--Calculate centre of mass momentum for decay
      PCM=SQRT(PC(4)**2-PC(5)**2)
C--PA is BH 5-momentum before, PB after decay
      CALL CHDTWO(PA,PB,PC,PCM,TWO,.TRUE.)
C--Add emitted particle to event record
      CALL CHVEQU(5,PC,PUP(1,NUP))
      IDUP(NUP)=PT
      ISTUP(NUP) = 1
      MOTHUP(1,NUP) = 1
      MOTHUP(2,NUP) = 2
C--Change BH 5-momentum back to PA
      CALL CHVEQU(5,PB,PA)
      GOTO 40
 50   BHCHRG=BHCHRG+CHFCHG(PT)
 55   NUP = NUP-1
C--Calculate the baryon number violation
C--Find the baryon number of the inital state
      IBARYN = 0
      DO I=1,2
        IF(ABS(IDUP(I)).LE.6) IBARYN = IBARYN-SIGN(1,IDUP(I))
      ENDDO
C--and the final state
      DO I=3,NUP
        IF(ABS(IDUP(I)).LE.6) IBARYN = IBARYN+SIGN(1,IDUP(I))
      ENDDO
C--Try to balance this out
      NTRY = 0
      SAVCHR = BHCHRG
      SAVBRN = IBARYN
 60   NTRY = NTRY+1
      PCM = PA(5)
      DO I=1,NBODY
         IF (MSSDEC.EQ.1) THEN
            CALL CHHBH1(ID(I),FSTAT(I),FSPIN(I),BHCHRG)
         ELSEIF (MSSDEC.EQ.2) THEN
            CALL CHHBH2(ID(I),FSTAT(I),FSPIN(I),BHCHRG)
         ELSE
            CALL CHHBH3(ID(I),FSTAT(I),FSPIN(I),BHCHRG)
         ENDIF
         IF(ABS(ID(I)).LE.6) THEN
            IBARYN = IBARYN+SIGN(1,ID(I))
         ENDIF
         PCM = PCM-CHFMAS(ID(I))
      ENDDO
      IF(((IBARYN.NE.0.OR.BHCHRG.NE.0).OR.
     &    PCM.LT.0.0D0).AND.NTRY.LE.NHTRY) THEN
         BHCHRG = SAVCHR
         IBARYN = SAVBRN
         GOTO 60
      ENDIF
C--Go back if needed
      IF(NTRY.GT.NHTRY) THEN
         IF(MTRY.LE.MHTRY) THEN
            IF(IPRINT.EQ.2) THEN
               WRITE (*,*) 'Attempt',MTRY,' failed.'
               WRITE (*,*) 'Starting whole decay again'
            ENDIF
            GOTO 35
         ELSE
            IF(IPRINT.GT.1) 
     &           PRINT *,'WARNING TOO MANY TRIES NEEDED'
            XWGTUP = 0.0D0
            RETURN
         ENDIF
      ENDIF
C--Perform the NBODY decay of the remnant
      DO I=1,NBODY
         PUP(5,NUP+I) = CHFMAS(ID(I))
         ISTUP(NUP+I) = 1
         IDUP(NUP+I) = ID(I)
         MOTHUP(1,NUP+I) = 1
         MOTHUP(2,NUP+I) = 2
      ENDDO
C--check if decay is allowed
      IF (NBODY.EQ.2) THEN
         PCM = CHUPCM(PA(5),PUP(5,NUP+1),PUP(5,NUP+2))
         CALL CHDTWO(PA,PUP(1,NUP+1),PUP(1,NUP+2),PCM,TWO,.TRUE.)
      ELSEIF (NBODY.EQ.3) THEN
         CALL CHDTHR(PA,PUP(1,NUP+1),PUP(1,NUP+2),PUP(1,NUP+3),*100)
      ELSEIF (NBODY.EQ.4) THEN
         CALL CHDFOR(PA,PUP(1,NUP+1),PUP(1,NUP+2),PUP(1,NUP+3),
     &                  PUP(1,NUP+4),*100)
      ELSEIF (NBODY.EQ.5) THEN
         CALL CHDFIV(PA,PUP(1,NUP+1),PUP(1,NUP+2),PUP(1,NUP+3),
     &                  PUP(1,NUP+4),PUP(1,NUP+5),*100)
      ENDIF
      GOTO 110
 100  IF(IPRINT.GE.1) 
     &     PRINT *,'FAILED TO PERFORM ',NBODY,' DECAY OF REMNENT'
      GOTO 35
 110  NUP = NUP+NBODY
C--Now sort out the colour connections
C--First the initial state
      NQRK = 0
      NGLU = 0
      NQBR = 0
      DO I=1,2
        IF(IDUP(I).GT.0.AND.IDUP(I).LE.6) THEN
           NQBR = NQBR+1
           IQBR(NQBR) = I
        ELSEIF(IDUP(I).LT.0.AND.IDUP(I).GE.-6) THEN
           NQRK = NQRK+1
           IQRK(NQRK) = I
        ELSEIF(IDUP(I).EQ.21) THEN
           NGLU = NGLU+1
           IGLU(NGLU) = I
        ENDIF
      ENDDO
C--Then the final state
      DO I=3,NUP
        IF(IDUP(I).GT.0.AND.IDUP(I).LE.6) THEN
           NQRK = NQRK+1
           IQRK(NQRK) = I
        ELSEIF(IDUP(I).LT.0.AND.IDUP(I).GE.-6) THEN
           NQBR = NQBR+1
           IQBR(NQBR) = I
        ELSEIF(IDUP(I).EQ.21) THEN
           NGLU = NGLU+1
           IGLU(NGLU) = I
        ENDIF
      ENDDO
C--zero the colour connections
      DO I=1,NUP
         DO J=1,2
            ICOLUP(J,I) = 0
         ENDDO
      ENDDO
C--Now make the colour connections
      JGLU = 0
      JQBR = 0
      JQRK = 0
      IF(NQRK.GT.0) THEN 
         I = IQRK(1)
         JQRK = 1
         J = 1
      ELSEIF(NGLU.GT.0) THEN
         I = IGLU(1)
         JGLU = 1
         J = 2
      ELSE
         GOTO 200
      ENDIF
      ISTART = I
      ILINE  = 500
C--Find an antiquark or gluon to pair this with
 150   IF(J.EQ.1.OR.J.EQ.2) THEN
         IF(JQBR.EQ.NQBR.and.JGLU.EQ.NGLU) THEN
            GOTO 250
         ELSE
            PROB = DBLE(NQBR-JQBR)/DBLE(NQBR+NGLU-JQBR-JGLU)
C--Pair with an antiquark
            IF(CHRLOG(PROB)) THEN
               IF(JQBR.NE.NQBR) THEN
                 IF(NGLU.NE.JGLU.and.NQBR-JQBR.EQ.1) GOTO 150
                  JQBR = JQBR+1
                  IF(I.LE.2) THEN
                     ICOLUP(2,I         ) = ILINE
                  ELSE
                     ICOLUP(1,I         ) = ILINE
                  ENDIF
                  IF(IQBR(JQBR).LE.2) THEN
                     ICOLUP(1,IQBR(JQBR)) = ILINE
                  ELSE
                     ICOLUP(2,IQBR(JQBR)) = ILINE
                  ENDIF
                  ILINE = ILINE+1
                  I = IQBR(JQBR)
                  J = 3
                  GOTO 150
               ELSE
                  GOTO 250
               ENDIF
C--Pair with a gluon
            ELSE
               IF(JGLU.NE.NGLU) THEN
                  JGLU = JGLU+1
                  IF(I.LE.2) THEN
                     ICOLUP(2,I         ) = ILINE 
                  ELSE
                     ICOLUP(1,I         ) = ILINE 
                  ENDIF
                  IF(IGLU(JGLU).LE.2) THEN
                     ICOLUP(1,IGLU(JGLU)) = ILINE
                  ELSE
                     ICOLUP(2,IGLU(JGLU)) = ILINE
                  ENDIF
                  ILINE = ILINE+1
                  I = IGLU(JGLU)
                  J = 2
                  GOTO 150
               ELSE
                  GOTO 250
               ENDIF
            ENDIF
         ENDIF
C--Find a quark to pair this with
      ELSEIF(J.EQ.3) THEN
         IF(JQRK.NE.NQRK) THEN
            JQRK = JQRK+1
            IF(I.LE.2) THEN
               ICOLUP(2,I         ) = ILINE
            ELSE
               ICOLUP(1,I         ) = ILINE
            ENDIF
            IF(IQRK(JQRK).LE.2) THEN
               ICOLUP(1,IQRK(JQRK)) = ILINE
            ELSE
               ICOLUP(2,IQRK(JQRK)) = ILINE
            ENDIF
            ILINE = ILINE+1
            I = IQRK(JQRK)
            J = 1
            GOTO 150
         ELSE
            GOTO 250
         ENDIF
      ENDIF
 250  IF(I.LE.2) THEN
         ICOLUP(2,I)      = ILINE
      ELSE
         ICOLUP(1,I)      = ILINE
      ENDIF
      IF(ISTART.LE.2) THEN
         ICOLUP(1,ISTART) = ILINE
      ELSE
         ICOLUP(2,ISTART) = ILINE
      ENDIF
C--zero the flavours
 200  DO I=1,NUP
         IF(IDUP(I).GT.0.AND.IDUP(I).LE.6) THEN
            ICOLUP(2,I) = 0
         ELSEIF(IDUP(I).GE.-6.AND.IDUP(I).lt.0) THEN
            ICOLUP(1,I) = 0
         ELSEIF(IDUP(I).NE.21) THEN
            ICOLUP(1,I) = 0
            ICOLUP(2,I) = 0
         ENDIF
      ENDDO
      END
CDECK  ID>, CHFCHG.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Peter Richardson
C----------------------------------------------------------------------
      FUNCTION CHFCHG(ID)
C----------------------------------------------------------------------
C     Function to return the charge of a SM particle input is PDG code
C----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER CHFCHG,ID
      IF(ABS(ID).LE.6) THEN
         IF(MOD(ID,2).EQ.0) THEN
            CHFCHG = 2
         ELSE
            CHFCHG = -1
         ENDIF
      ELSEIF(ABS(ID).GE.11.AND.ABS(ID).LE.16) THEN
         IF(MOD(ID,2).EQ.0) THEN
            CHFCHG = 0
         ELSE
            CHFCHG = -3
         ENDIF
      ELSEIF((ID.GE.21.AND.ID.LE.23).OR.ID.EQ.25) THEN
         CHFCHG = 0
      ELSEIF(ABS(ID).EQ.24) THEN
         CHFCHG = 3
      ELSE
         print *,'CHFCHG WARNING UNKNOWN PARTICLE',ID
         STOP
      ENDIF
      IF(ID.LT.0) CHFCHG = -CHFCHG
      END
CDECK  ID>, CHFMAS.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Peter Richardson
C----------------------------------------------------------------------
      FUNCTION CHFMAS(ID)
C----------------------------------------------------------------------
C     Function to return the mass of a SM particle input is PDG code
C     This is generator dependent
C----------------------------------------------------------------------
C--get the masses from the HERWIG common block
#if !defined(PYTHIA)
      INCLUDE 'GravADD_i/HERWIG64.INC'
      INTEGER ID
      DOUBLE PRECISION CHFMAS
      IF(ABS(ID).LE.6) THEN
         CHFMAS = RMASS(ABS(ID))
      ELSEIF(ABS(ID).GE.11.AND.ABS(ID).LE.16) THEN
         CHFMAS = RMASS(110+ABS(ID))
      ELSEIF(ID.EQ.21) THEN
         CHFMAS = RMASS(13)
      ELSEIF(ID.EQ.22) THEN
         CHFMAS = 0.0D0
      ELSEIF(ID.EQ.23) THEN
         CHFMAS = RMASS(200)
      ELSEIF(ABS(ID).EQ.24) THEN
         CHFMAS = RMASS(198)
      ELSEIF(ID.EQ.25) THEN
         CHFMAS = RMASS(201)
      ELSE
         PRINT *,'CHFMAS WARNING UNKNOWN PARTICLE',ID
         STOP
      ENDIF
#endif
C--get the masses from the PYTHIA common block
#if defined(PYTHIA)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      INTEGER PYCOMP,ID
      DOUBLE PRECISION CHFMAS
      EXTERNAL PYCOMP
      CHFMAS  = PMAS(PYCOMP(ID),1)
#endif
      END
CDECK  ID>, CHHBH1.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :   Chris Harris
C---------------------------------------------------------------------
      SUBROUTINE CHHBH1(PT,STAT,SPIN,BHCHRG)
C---------------------------------------------------------------------
C     Subroutine to select the type of the next particle
C---------------------------------------------------------------------
      INCLUDE 'GravADD_i/charybdis1001.inc'
      INTEGER PT,STAT,SPIN,BHCHRG,CHFCHG,I
      DOUBLE PRECISION PTYPE,CHRGEN,EPS
      LOGICAL FIRST
      DATA FIRST/.TRUE./
      EXTERNAL CHRGEN,CHFCHG
      PARAMETER(EPS=1.0D-6)
C--initialisation on first call
      IF(FIRST) THEN
         PFERM (1) = PQUARK
         PFERM (2) = PFERM(1)+PLEPT
         PFERM (3) = PFERM(2)+PNEUT
         PBOSON(1) = PFERM(3)+PGLUON
         PBOSON(2) = PBOSON(1)+PGAMMA
C--check the sum
         IF(ABS(PBOSON(2)-1.0D0).GT.EPS) THEN
            IF(IPRINT.GE.1) THEN
               print *,'WARNING PROBABILITIES DO NOT SUM TO ONE'
               print *,'RENORMALISING'
            ENDIF
            DO I=1,3
               PFERM(I) = PFERM(I)/PBOSON(2)
            ENDDO
            PBOSON(1) = PBOSON(1)/PBOSON(2)
            PQUARK = PQUARK/PBOSON(2)
            PLEPT  = PLEPT /PBOSON(2)
            PNEUT  = PNEUT /PBOSON(2)
            PGLUON = PGLUON/PBOSON(2)
            PGAMMA = PGAMMA/PBOSON(2)
            PBOSON(2) = 1.0D0
         ENDIF
         FIRST = .FALSE.
      ENDIF
C--Assume decay product is a fermion
      STAT=-1
      SPIN=1
C--Choose decay product
      PTYPE=CHRGEN(2)
C--Change STAT to 1 and SPIN to 2 if we have a boson instead
      IF (PTYPE.GE.PFERM(3)) THEN 
         STAT=1
         SPIN=2
      ENDIF
C--Choose particle type
      IF (PTYPE.LT.PFERM(1)) THEN
C--Quarks
         PT=1+INT(5*PTYPE/PFERM(1))
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PFERM(2)) THEN
C--Leptons
         PT=11+2*INT(3*(PTYPE-PFERM(1))/PLEPT)
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PFERM(3)) THEN
C--Neutrinos
         PT=12+2*INT(3*(PTYPE-PFERM(2))/PNEUT)
         IF(CHRGEN(4).LT.0.5D0) PT = -PT
      ELSEIF (PTYPE.LT.PBOSON(1)) THEN
C--Gluons
         PT=21
      ELSE
C--Photons
         PT=22
      ENDIF
C--Calculate new charge of BH
      BHCHRG=BHCHRG-CHFCHG(PT)
      END
CDECK  ID>, CHHBH2.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :   Chris Harris
C---------------------------------------------------------------------
      SUBROUTINE CHHBH2(PT,STAT,SPIN,BHCHRG)
C---------------------------------------------------------------------
C     Subroutine to select the type of the next particle
C---------------------------------------------------------------------
      INCLUDE 'GravADD_i/charybdis1001.inc'
      INTEGER PT,STAT,SPIN,BHCHRG,CHFCHG,I
      DOUBLE PRECISION PTYPE,CHRGEN,EPS
      LOGICAL FIRST
      DATA FIRST/.TRUE./
      EXTERNAL CHRGEN,CHFCHG
      PARAMETER(EPS=1.0D-6)
C--initialisation on first call
      IF(FIRST) THEN
         PFERM (1) = PQUARK
         PFERM (2) = PFERM(1)+PLEPT
         PFERM (3) = PFERM(2)+PNEUT
         PBOSON(1) = PFERM(3)+PGLUON
         PBOSON(2) = PBOSON(1)+PWBOSN
         PBOSON(3) = PBOSON(2)+PZBOSN
         PBOSON(4) = PBOSON(3)+PGAMMA
C--check the sum
         IF(ABS(PBOSON(4)-1.0D0).GT.EPS) THEN
            IF(IPRINT.GE.1) THEN
               print *,'WARNING PROBABILITIES DO NOT SUM TO ONE'
               print *,'RENORMALISING'
            ENDIF
            DO I=1,3
               PFERM(I) = PFERM(I)/PBOSON(4)
               PBOSON(I) = PBOSON(I)/PBOSON(4)
            ENDDO
            PQUARK = PQUARK/PBOSON(4)
            PLEPT  = PLEPT /PBOSON(4)
            PNEUT  = PNEUT /PBOSON(4)
            PGLUON = PGLUON/PBOSON(4)
            PGAMMA = PGAMMA/PBOSON(4)
            PZBOSN = PZBOSN/PBOSON(4)
            PWBOSN = PWBOSN/PBOSON(4)
            PBOSON(4) = 1.0D0
         ENDIF
         FIRST = .FALSE.
      ENDIF
C--Assume decay product is a fermion
      STAT=-1
      SPIN=1
C--Choose decay product
      PTYPE=CHRGEN(2)
C--Change STAT to 1 and SPIN to 2 if we have a boson instead except for one
C--degree of freedom of massive gauge bosons
      IF (PTYPE.GE.PFERM(3)) THEN 
         STAT=1
         SPIN=2
         IF ((PTYPE.GE.PBOSON(1)).AND.(PTYPE.LT.PBOSON(3))) THEN
            IF (3*CHRGEN(3).LT.1.0d0) SPIN=0
         ENDIF
      ENDIF
C--Choose particle type
      IF (PTYPE.LT.PFERM(1)) THEN
C--Quarks
         PT=1+INT(6*PTYPE/PFERM(1))
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PFERM(2)) THEN
C--Leptons
         PT=11+2*INT(3*(PTYPE-PFERM(1))/PLEPT)
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PFERM(3)) THEN
C--Neutrinos
         PT=12+2*INT(3*(PTYPE-PFERM(2))/PNEUT)
         IF(CHRGEN(4).LT.0.5D0) PT = -PT
      ELSEIF (PTYPE.LT.PBOSON(1)) THEN
C--Gluons
         PT=21
      ELSEIF (PTYPE.LT.PBOSON(2)) THEN
C--W+/W-
         PT=24
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PBOSON(3)) THEN
C--Z0
         PT=23
      ELSE
C--Photons
         PT=22
      ENDIF
C--Calculate new charge of BH
      BHCHRG=BHCHRG-CHFCHG(PT)
      END
CDECK  ID>, CHHBH3.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :   Chris Harris
C---------------------------------------------------------------------
      SUBROUTINE CHHBH3(PT,STAT,SPIN,BHCHRG)
C---------------------------------------------------------------------
C     Subroutine to select the type of the next particle
C---------------------------------------------------------------------
      INCLUDE 'GravADD_i/charybdis1001.inc'
      INTEGER PT,STAT,SPIN,BHCHRG,CHFCHG,I
      DOUBLE PRECISION PTYPE,CHRGEN,EPS
      LOGICAL FIRST
      DATA FIRST/.TRUE./
      EXTERNAL CHRGEN,CHFCHG
      PARAMETER(EPS=1.0D-6)
C--initialisation on first call
      IF(FIRST) THEN
         PFERM (1) = PQUARK
         PFERM (2) = PFERM(1)+PLEPT
         PFERM (3) = PFERM(2)+PNEUT
         PBOSON(1) = PFERM(3)+PGLUON
         PBOSON(2) = PBOSON(1)+PWBOSN
         PBOSON(3) = PBOSON(2)+PZBOSN
         PBOSON(4) = PBOSON(3)+PHIGGS
         PBOSON(5) = PBOSON(4)+PGAMMA
C--check the sum
         IF(ABS(PBOSON(5)-1.0D0).GT.EPS) THEN
            IF(IPRINT.GE.1) THEN
               print *,'WARNING PROBABILITIES DO NOT SUM TO ONE'
               print *,'RENORMALISING'
            ENDIF
            DO I=1,3
               PFERM(I) = PFERM(I)/PBOSON(5)
               PBOSON(I) = PBOSON(I)/PBOSON(5)
            ENDDO
            PBOSON(4) = PBOSON(4)/PBOSON(5)
            PQUARK = PQUARK/PBOSON(5)
            PLEPT  = PLEPT /PBOSON(5)
            PNEUT  = PNEUT /PBOSON(5)
            PGLUON = PGLUON/PBOSON(5)
            PGAMMA = PGAMMA/PBOSON(5)
            PZBOSN = PZBOSN/PBOSON(5)
            PWBOSN = PWBOSN/PBOSON(5)
            PHIGGS = PHIGGS/PBOSON(5)
            PBOSON(5) = 1.0D0
         ENDIF
         FIRST = .FALSE.
      ENDIF
C--Assume decay product is a fermion
      STAT=-1
      SPIN=1
C--Choose decay product
      PTYPE=CHRGEN(2)
C--Change STAT to 1 and SPIN to 2 if we have a boson instead except for one
C--degree of freedom of massive gauge bosons
      IF (PTYPE.GE.PFERM(3)) THEN
         STAT=1
         SPIN=2
         IF ((PTYPE.GE.PBOSON(1)).AND.(PTYPE.LT.PBOSON(3))) THEN
            IF (3*CHRGEN(3).LT.1.0d0) SPIN=0
         ENDIF
      ENDIF
C--Choose particle type
      IF (PTYPE.LT.PFERM(1)) THEN
C--Quarks
         PT=1+INT(6*PTYPE/PFERM(1))
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PFERM(2)) THEN
C--Leptons
         PT=11+2*INT(3*(PTYPE-PFERM(1))/PLEPT)
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PFERM(3)) THEN
C--Neutrinos
         PT=12+2*INT(3*(PTYPE-PFERM(2))/PNEUT)
         IF(CHRGEN(4).LT.0.5D0) PT = -PT
      ELSEIF (PTYPE.LT.PBOSON(1)) THEN
C--Gluons
         PT=21
      ELSEIF (PTYPE.LT.PBOSON(2)) THEN
C--W+/W-
         PT=24
C--Ensure BH charge remains close to 0, and antiparticles half time
         IF (((CHFCHG(PT)*BHCHRG).LT.0).OR.
     &       ((BHCHRG.EQ.0).AND.(CHRGEN(4).LT.0.5D0))) THEN 
            PT=-PT
         ENDIF
      ELSEIF (PTYPE.LT.PBOSON(3)) THEN
C--Z0
         PT=23
      ELSEIF (PTYPE.LT.PBOSON(4)) THEN
C--Higgs
         PT=25
         SPIN=0
      ELSE
C--Photons
         PT=22
      ENDIF
C--Calculate new charge of BH
      BHCHRG=BHCHRG-CHFCHG(PT)
      END
CDECK  ID>, CHPDF.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Peter Richardson
C----------------------------------------------------------------------
      SUBROUTINE CHPDF(XXMINB,XLMINB,EMSCAB,XXB,DISFB)
C----------------------------------------------------------------------
C     Subroutine to calculate the pdfs
C     generator dependent
C----------------------------------------------------------------------
#if !defined(PYTHIA)
      INCLUDE 'GravADD_i/HERWIG64.INC'
      DOUBLE PRECISION EMSCAB,XXB(2),DISFB(13,2),XXMINB,XLMINB,
     &     CHRUNI,UPV,DNV,USEA,DSEA,STR,CHM,BTM,TOP,GLU
      INTEGER IDB(2),I,J
      CHARACTER *8 DUMMY
      DOUBLE PRECISION VALUE(20)
      CHARACTER*20 PARM(20)
      LOGICAL FIRST
      EXTERNAL CHRUNI
      DATA VALUE/20*0D0/,PARM/20*' '/
      DATA FIRST/.TRUE./
C--Les Houches run common block
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)
C--use generator internal pdfs
      IF(PDFGUP(1).LT.0) THEN
C---SET UP INITIAL STATE
         CALL HWUIDT(1,IDBMUP(1),IDB(1),DUMMY)
         PART1=DUMMY
         CALL HWUIDT(1,IDBMUP(2),IDB(2),DUMMY)
         PART2=DUMMY
         IPART1 = IDB(1)
         IPART2 = IDB(2)
         NHEP=1
         ISTHEP(NHEP)=101
         PHEP(1,NHEP)=0.0D0
         PHEP(2,NHEP)=0.0D0
         PHEP(3,NHEP)=EBMUP(1)
         PHEP(4,NHEP)=EBMUP(1)
         PHEP(5,NHEP)=RMASS(IDB(1))
         JMOHEP(1,NHEP)=0
         JMOHEP(2,NHEP)=0
         JDAHEP(1,NHEP)=0
         JDAHEP(2,NHEP)=0
         IDHW(NHEP)=IPART1
         IDHEP(NHEP)=IDPDG(IDB(1))
         NHEP=NHEP+1
         ISTHEP(NHEP)=102
         PHEP(1,NHEP)=0.0D0
         PHEP(2,NHEP)=0.0D0
         PHEP(3,NHEP)=-EBMUP(2)
         PHEP(4,NHEP)=EBMUP(2)
         PHEP(5,NHEP)=RMASS(IDB(2))
         JMOHEP(1,NHEP)=0
         JMOHEP(2,NHEP)=0
         JDAHEP(1,NHEP)=0
         JDAHEP(2,NHEP)=0
         IDHW(NHEP)=IPART2
         IDHEP(NHEP)=IDPDG(IPART2)
C---NEXT ENTRY IS OVERALL CM FRAME
         NHEP=NHEP+1
         IDHW(NHEP)=14
         IDHEP(NHEP)=0
         ISTHEP(NHEP)=103
         JMOHEP(1,NHEP)=NHEP-2
         JMOHEP(2,NHEP)=NHEP-1
         JDAHEP(1,NHEP)=0
         JDAHEP(2,NHEP)=0
         CALL CHVSUM(4,PHEP(1,NHEP-1),PHEP(1,NHEP-2),PHEP(1,NHEP))
         CALL CHUMAS(PHEP(1,NHEP))
         XLMIN = XLMINB
         EMSCA = EMSCAB
         XXMIN = XXMINB
         CALL HWSGEN(.TRUE.)
         DO I=1,2
            XXB(I) = XX(I)
            DO J=1,13
               DISFB(J,I) = DISF(J,I)
            ENDDO
         ENDDO
      ELSE
C--initialisation
         IF(FIRST) THEN
            IF(PDFGUP(1).NE.PDFGUP(2).OR.PDFSUP(1).NE.PDFSUP(2)) THEN
               print *,'MUST HAVE SAME PDF FOR BOTH BEAMS'
               STOP
            ENDIF
            PARM(1)='NPTYPE'
            VALUE(1)=1
            PARM(2)='NGROUP'
            VALUE(2)=PDFGUP(1)
            PARM(3)='NSET'
            VALUE(3)=PDFSUP(1)
            CALL PDFSET(PARM,VALUE)
            FIRST = .FALSE.
         ENDIF
         XXB(1)=EXP(CHRUNI(0,0.0D0,XLMINB))
         XXB(2)=XXMINB/XXB(1)
         DO I=1,2
            CALL STRUCTM(XXB(I),EMSCAB,
     &           UPV,DNV,USEA,DSEA,STR,CHM,BTM,TOP,GLU)
            DISFB( 3,I) = STR
            DISFB( 4,I) = CHM
            DISFB( 5,I) = BTM
            DISFB( 6,I) = TOP
            DISFB( 9,I) = STR
            DISFB(10,I) = CHM
            DISFB(11,I) = BTM
            DISFB(12,I) = TOP
            DISFB(13,I) = GLU
            IF(IDBMUP(I).GT.0) THEN
               DISFB(1,I) = DNV+DSEA
               DISFB(2,I) = UPV+USEA
               DISFB(7,I) = DSEA
               DISFB(8,I) = USEA
            ELSE
               DISFB(1,I) = DSEA
               DISFB(2,I) = USEA
               DISFB(7,I) = DNV+DSEA
               DISFB(8,I) = UPV+USEA
            ENDIF
         ENDDO
      ENDIF
#endif
#if defined(PYTHIA)
      IMPLICIT NONE
C--Les Houches run common block
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)
      SAVE /HEPRUP/
C      DOUBLE PRECISION EMSCAB,XXB(2),DISFB(13,2),I,J,XXMINB,
C     &     XLMINB,CHRUNI,XPQ(-25:25),Q2,UPV,DNV,USEA,DSEA,STR,CHM,BTM,
C     &     TOP,GLU
      DOUBLE PRECISION EMSCAB,XXB(2),DISFB(13,2),XXMINB,
     &     XLMINB,CHRUNI,XPQ(-25:25),Q2,UPV,DNV,USEA,DSEA,STR,CHM,BTM,
     &     TOP,GLU
      INTEGER I
      EXTERNAL CHRUNI
      DOUBLE PRECISION VALUE(20)
      CHARACTER*20 PARM(20)
      LOGICAL FIRST
      DATA VALUE/20*0D0/,PARM/20*' '/
      DATA FIRST/.TRUE./
      IF(PDFGUP(1).LT.0) THEN
      XXB(1)=EXP(CHRUNI(0,0.0D0,XLMINB))
      XXB(2)=XXMINB/XXB(1)
      Q2 = EMSCAB**2
      CALL PYPDFU(IDBMUP(1),XXB(1),Q2,XPQ)
      DO I=1,6
         DISFB(I,1) = XPQ(I)
         DISFB(I+6,1) = XPQ(-I)
      ENDDO
      DISFB(13,1) = XPQ(21)
      CALL PYPDFU(IDBMUP(2),XXB(2),Q2,XPQ)
      DO I=1,6
         DISFB(I,2) = XPQ(I)
         DISFB(I+6,2) = XPQ(-I)
      ENDDO
      DISFB(13,2) = XPQ(21)
      ELSE
C--initialisation
         IF(FIRST) THEN
            IF(PDFGUP(1).NE.PDFGUP(2).OR.PDFSUP(1).NE.PDFSUP(2)) THEN
               print *,'MUST HAVE SAME PDF FOR BOTH BEAMS'
               STOP
            ENDIF
            PARM(1)='NPTYPE'
            VALUE(1)=1
            PARM(2)='NGROUP'
            VALUE(2)=PDFGUP(1)
            PARM(3)='NSET'
            VALUE(3)=PDFSUP(1)
            CALL PDFSET(PARM,VALUE)
            FIRST = .FALSE.
         ENDIF
         XXB(1)=EXP(CHRUNI(0,0.0D0,XLMINB))
         XXB(2)=XXMINB/XXB(1)
         DO I=1,2
            CALL STRUCTM(XXB(I),EMSCAB,
     &           UPV,DNV,USEA,DSEA,STR,CHM,BTM,TOP,GLU)
            DISFB( 3,I) = STR
            DISFB( 4,I) = CHM
            DISFB( 5,I) = BTM
            DISFB( 6,I) = TOP
            DISFB( 9,I) = STR
            DISFB(10,I) = CHM
            DISFB(11,I) = BTM
            DISFB(12,I) = TOP
            DISFB(13,I) = GLU
            IF(IDBMUP(I).GT.0) THEN
               DISFB(1,I) = DNV+DSEA
               DISFB(2,I) = UPV+USEA
               DISFB(7,I) = DSEA
               DISFB(8,I) = USEA
            ELSE
               DISFB(1,I) = DSEA
               DISFB(2,I) = USEA
               DISFB(7,I) = DNV+DSEA
               DISFB(8,I) = UPV+USEA
            ENDIF
         ENDDO
      ENDIF
#endif
      END
CDECK  ID>, CHRAZM.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHRAZM(PT,PX,PY)
C-----------------------------------------------------------------------
C     RANDOMLY ROTATED 2-VECTOR (PX,PY) OF LENGTH PT
C     (taken from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHRGEN,PT,PX,PY,C,S,CS,QT,ONE,ZERO
      PARAMETER(ONE=1.0D0, ZERO=0.0D0)
      EXTERNAL CHRGEN
   10 C=2.0D0*CHRGEN(1)-1.0D0
      S=2.0D0*CHRGEN(2)-1.0D0
      CS=C*C+S*S
      IF (CS.GT.ONE .OR. CS.EQ.ZERO) GOTO 10
      QT=PT/CS
      PX=(C*C-S*S)*QT
      PY=2.0D0*C*S*QT
      END
CDECK  ID>, CHRGEN.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Peter Richardson
C-----------------------------------------------------------------------
      FUNCTION CHRGEN(I)
C-----------------------------------------------------------------------
C     random number generator (just calls HERWIG or PYTHIA one)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHRGEN
      INTEGER I
C--use HERWIG random number generator
#if !defined(PYTHIA)
      DOUBLE PRECISION HWRGEN
      EXTERNAL HWRGEN
      CHRGEN = HWRGEN(I)
#endif
C--use the PYTHIA one
#if defined(PYTHIA)
      DOUBLE PRECISION PYR
      EXTERNAL PYR
      CHRGEN =  PYR(I)
#endif
      END
CDECK  ID>, CHRLOG.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      FUNCTION CHRLOG(A)
C-----------------------------------------------------------------------
C     Returns .TRUE. with probability A
C     (taken from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHRGEN,A,R
      LOGICAL CHRLOG
      EXTERNAL CHRGEN
      CHRLOG=.TRUE.
      R=CHRGEN(0)
      IF(R.GT.A) CHRLOG=.FALSE.
      END
CDECK  ID>, CHRUNI.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      FUNCTION CHRUNI(I,A,B)
C-----------------------------------------------------------------------
C     Uniform random random number in range [A,B]
C     (taken from HERWIG)
C-----------------------------------------------------------------------
      DOUBLE PRECISION CHRUNI,CHRGEN,A,B,RN
      INTEGER I
      EXTERNAL CHRGEN
      RN=CHRGEN(I)
      CHRUNI=A+RN*(B-A)
      END
CDECK  ID>, CHUBHS.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :   Chris Harris
C---------------------------------------------------------------------
      SUBROUTINE CHUBHS(ENERGY,STAT,SPIN)
C---------------------------------------------------------------------
C     Calculates energy spectrum for a black hole of given radius
C---------------------------------------------------------------------
      INCLUDE 'GravADD_i/charybdis1001.inc'
      DOUBLE PRECISION ENERGY,SPCMAX,SPCVAL,RADHOR,HWKTMP,ETRAT
      DOUBLE PRECISION MXARRY(6,3),CHRGEN,CHRUNI,CHUTAB
      DOUBLE PRECISION FLUX(200,6,3),ETVALS(200,6)
      INTEGER NARRAY(6),STAT,SPIN,POINTS,I
      EXTERNAL CHRGEN,CHRUNI,CHUTAB
      DOUBLE PRECISION ONE,TWO,THREE,ZERO,PIFAC,HALF
      PARAMETER(ONE=1.0D0,TWO=2.0D0,THREE=3.0D0,ZERO=0.0D0,HALF=0.5D0)
      DATA NARRAY /151,121,131,141,119,151/
      DATA MXARRY /0.021d0,0.032d0,0.045d0,0.061d0,0.081d0,0.11d0,
     &     0.017d0,0.026d0,0.036d0,0.045d0,0.056d0,0.068d0,
     &     0.017d0,0.033d0,0.053d0,0.076d0,0.11d0,0.13d0/
      DATA (ETVALS(I,1),I=1,151) /0.00837758, 0.0837758, 0.167552, 
     &0.251327, 0.335103, 0.418879, 0.502655, 0.586431, 0.670206, 
     &0.753982, 0.837758, 0.921534,  1.00531,  1.08909,  1.17286,  
     &1.25664,  1.34041,  1.42419,  1.50796,  1.59174,  1.67552,  
     &1.75929,  1.84307,  1.92684,  2.01062,   2.0944,  2.17817,  
     &2.26195,  2.34572,   2.4295,  2.51327,  2.59705,  2.68083,   
     &2.7646,  2.84838,  2.93215,  3.01593,   3.0997,  3.18348,  
     &3.26726,  3.35103,  3.43481,  3.51858,  3.60236,  3.68614,  
     &3.76991,  3.85369,  3.93746,  4.02124,  4.10501,  4.18879,  
     &4.27257,  4.35634,  4.44012,  4.52389,  4.60767,  4.69145,  
     &4.77522,    4.859,  4.94277,  5.02655,  5.11032,   5.1941,  
     &5.27788,  5.36165,  5.44543,   5.5292,  5.61298,  5.69675,  
     &5.78053,  5.86431,  5.94808,  6.03186,  6.11563,  6.19941,  
     &6.28319,  6.36696,  6.45074,  6.53451,  6.61829,  6.70206,  
     &6.78584,  6.86962,  6.95339,  7.03717,  7.12094,  7.20472,  
     &7.28849,  7.37227,  7.45605,  7.53982,   7.6236,  7.70737,  
     &7.79115,  7.87493,   7.9587,  8.04248,  8.12625,  8.21003,   
     &8.2938,  8.37758,  8.46136,  8.54513,  8.62891,  8.71268,  
     &8.79646,  8.88024,  8.96401,  9.04779,  9.13156,  9.21534,  
     &9.29911,  9.38289,  9.46667,  9.55044,  9.63422,  9.71799,  
     &9.80177,  9.88554,  9.96932,  10.0531,  10.1369,  10.2206,  
     &10.3044,  10.3882,   10.472,  10.5558,  10.6395,  10.7233,  
     &10.8071,  10.8909,  10.9746,  11.0584,  11.1422,   11.226,  
     &11.3097,  11.3935,  11.4773,  11.5611,  11.6448,  11.7286,  
     &11.8124,  11.8962,  11.9799,  12.0637,  12.1475,  12.2313,   
     &12.315,  12.3988,  12.4826,  12.5664/
      DATA (FLUX(I,1,1),I=1,151) /0.000302416, 0.00291566, 0.00558027, 
     &0.00800197, 0.0101859, 0.0121371, 0.0138614, 0.015365, 0.0166554, 
     &0.0177408, 0.0186311, 0.0193371, 0.0198712, 0.0202464, 0.0204769, 
     &0.0205776, 0.0205633, 0.0204495, 0.0202512, 0.0199826, 0.0196575, 
     &0.0192887, 0.0188876, 0.0184644, 0.018028, 0.017586, 0.0171441, 
     &0.0167072, 0.0162784, 0.0158598, 0.0154524, 0.0150562, 0.0146703, 
     &0.0142933, 0.0139233, 0.0135581, 0.0131954, 0.0128329, 0.0124687, 
     &0.012101, 0.0117287, 0.0113511, 0.0109679, 0.0105796, 0.010187, 
     &0.0097913, 0.00939432, 0.00899787, 0.00860399, 0.00821478, 
     &0.00783223, 0.00745829, 0.00709462, 0.00674274, 0.00640377, 
     &0.00607866, 0.005768, 0.00547213, 0.00519109, 0.00492474, 
     &0.00467268, 0.00443436, 0.0042091, 0.0039961, 0.00379453, 
     &0.0036035, 0.00342213, 0.00324959, 0.00308509, 0.00292792, 
     &0.00277747, 0.00263322, 0.00249476, 0.00236175, 0.00223397, 
     &0.00211127, 0.00199352, 0.0018807, 0.00177276, 0.00166969, 
     &0.0015715, 0.00147816, 0.00138963, 0.00130586, 0.00122677, 
     &0.00115224, 0.00108214, 0.00101631, 0.000954559, 0.000896698, 
     &0.000842512, 0.000791779, 0.000744269, 0.000699762, 0.000658029, 
     &0.000618861, 0.000582052, 0.000547413, 0.000514771, 0.000483972, 
     &0.000454876, 0.000427365, 0.000401333, 0.000376696, 0.000353375, 
     &0.000332, 0.000310443, 0.00029073, 0.000272125, 0.000254591, 
     &0.000238088, 0.000222578, 0.000208022, 0.000194382, 0.000181617, 
     &0.000169684, 0.0001588, 0.000148148, 0.000138454, 0.0001295,
     &0.000121, 0.000113152, 0.000105834, 9.90067e-05, 9.26315e-05, 
     &8.66729e-05, 8.10985e-05, 7.5878e-05, 7.0984e-05, 6.63922e-05, 
     &6.20809e-05, 5.80306e-05, 5.42246e-05, 5.06479e-05, 4.72874e-05, 
     &4.41314e-05, 4.11692e-05, 3.83912e-05, 3.57884e-05, 3.33521e-05, 
     &3.10742e-05, 2.89466e-05, 2.69615e-05, 2.51111e-05, 2.33879e-05, 
     &2.17842e-05, 2.02926e-05, 1.8906e-05, 1.76171e-05, 1.64192e-05,
     &1.52e-05/
      DATA (FLUX(I,1,2),I=1,151) /9.9924e-07, 9.62357e-05, 0.000370872, 
     &0.000803897, 0.00137345, 0.0020564, 0.00283305, 0.00368963, 
     &0.00461454, 0.00559229, 0.00660209, 0.00762302, 0.00864035, 
     &0.00964574, 0.0106307, 0.0115808, 0.0124765, 0.0133006, 0.014044, 
     &0.0147037, 0.0152769, 0.0157563, 0.0161325, 0.0164003, 0.0165624, 
     &0.0166267, 0.0166009, 0.0164899, 0.0162981, 0.0160326, 0.0157045, 
     &0.0153266, 0.0149102, 0.0144638, 0.0139948, 0.0135109, 0.0130204, 
     &0.0125304, 0.0120465, 0.0115724, 0.0111114, 0.0106658, 0.0102376, 
     &0.00982739, 0.00943497, 0.00905983, 0.00870158, 0.00835955, 
     &0.0080324, 0.00771795, 0.00741412, 0.00711948, 0.00683327, 
     &0.00655479, 0.00628292, 0.0060163, 0.00575402, 0.00549595, 
     &0.00524262, 0.00499461, 0.00475223, 0.00451558, 0.00428494, 
     &0.00406101, 0.00384461, 0.00363646, 0.00343697, 0.00324628, 
     &0.00306453, 0.00289183, 0.00272826, 0.00257377, 0.00242813, 
     &0.00229105, 0.00216213, 0.00204095, 0.00192704, 0.00181987, 
     &0.00171899, 0.00162396, 0.00153434, 0.00144968, 0.0013695, 
     &0.00129338, 0.00122102, 0.0011522, 0.00108673, 0.0010244, 
     &0.000964981, 0.000908321, 0.00085433, 0.000802968, 0.000754197, 
     &0.000707961, 0.000664175, 0.000622759, 0.000583662, 0.000546835, 
     &0.000512222, 0.00047975, 0.000449325, 0.000420846, 0.00039421, 
     &0.000369318, 0.000346067, 0.000324354, 0.000304076, 0.000285131, 
     &0.000267413, 0.000250822, 0.000235264, 0.000220659, 0.000206942, 
     &0.000194046, 0.000181911, 0.000170475, 0.00015969, 0.000149516, 
     &0.000139925, 0.00013089, 0.000122383, 0.000114375, 0.000106842, 
     &9.97618e-05, 9.31189e-05, 8.68953e-05, 8.10729e-05, 7.56304e-05, 
     &7.05476e-05, 6.5805e-05, 6.13842e-05, 5.72669e-05, 5.3435e-05, 
     &4.98696e-05, 4.65523e-05, 4.34649e-05, 4.05893e-05, 3.79096e-05, 
     &3.54108e-05, 3.30793e-05, 3.09023e-05, 2.88673e-05, 2.69624e-05, 
     &2.51775e-05, 2.35042e-05, 2.19355e-05, 2.04652e-05, 1.90868e-05, 
     &1.77945e-05, 1.6583e-05, 1.54479e-05/
      DATA (FLUX(I,1,3),I=1,151) /2.36467e-09, 2.27485e-06, 1.74831e-05, 
     &5.67478e-05, 0.000129498, 0.00024372, 0.000406154, 0.000622444, 
     &0.000897231, 0.00123422, 0.00163621, 0.00210503, 0.00264154, 
     &0.00324549, 0.00391542, 0.00464852, 0.00544043, 0.00628508, 
     &0.00717462, 0.00809913, 0.00904674, 0.0100036, 0.010954, 
     &0.0118809, 0.0127664, 0.0135921, 0.0143404, 0.014995, 0.015542, 
     &0.0159706, 0.0162738, 0.0164486, 0.0164966, 0.0164228, 0.016236, 
     &0.0159478, 0.0155716, 0.015122, 0.0146141, 0.0140622, 0.0134802, 
     &0.0128807, 0.0122745, 0.011671, 0.011078, 0.0105017, 0.00994673, 
     &0.00941661, 0.00891352, 0.00843877, 0.00799277, 0.00757527, 
     &0.00718537, 0.00682182, 0.00648292, 0.00616677, 0.00587134, 
     &0.00559445, 0.00533398, 0.00508784, 0.0048541, 0.00463095, 
     &0.00441687, 0.00421054, 0.00401094, 0.00381727, 0.00362905, 
     &0.00344597, 0.00326794, 0.003095, 0.00292736, 0.00276523, 
     &0.0026089, 0.00245866, 0.00231475, 0.0021774, 0.00204674, 
     &0.00192284, 0.00180574, 0.00169536, 0.00159159, 0.00149424, 
     &0.0014031, 0.00131786, 0.00123824, 0.00116391, 0.00109451, 
     &0.00102969, 0.000969093, 0.000912375, 0.000859198, 0.000809247, 
     &0.000762232, 0.00071789, 0.000675986, 0.000636318, 0.000598712, 
     &0.000563024, 0.000529139, 0.000496956, 0.000466402, 0.000437415, 
     &0.000409939, 0.000383934, 0.000359358, 0.000336172, 0.000314336, 
     &0.00029381, 0.000274549, 0.000256505, 0.000239628, 0.000223864, 
     &0.000209157, 0.000195449, 0.00018268, 0.000170793, 0.000159724, 
     &0.000149417, 0.000139815, 0.000130862, 0.000122505, 0.000114698, 
     &0.000107393, 0.000100551, 9.41333e-05, 8.81074e-05, 8.24435e-05, 
     &7.7117e-05, 7.21049e-05, 6.73879e-05, 6.29497e-05, 5.87753e-05, 
     &5.48516e-05, 5.11669e-05, 4.77097e-05, 4.44701e-05, 4.14379e-05, 
     &3.86033e-05, 3.59566e-05, 3.34882e-05, 3.11885e-05, 2.9048e-05, 
     &2.70572e-05, 2.52068e-05, 2.34873e-05, 2.189e-05, 2.0406e-05, 
     &1.90271e-05, 1.7745e-05, 1.65523e-05, 1.54419e-05/      
      DATA (ETVALS(I,2),I=1,121) /0.00628319, 0.0628319, 0.125664, 
     &0.188496, 0.251327, 0.314159, 0.376991, 0.439823, 0.502655, 
     &0.565487, 0.628319,  0.69115, 0.753982, 0.816814, 0.879646, 
     &0.942478,  1.00531,  1.06814,  1.13097,  1.19381,  1.25664,  
     &1.31947,   1.3823,  1.44513,  1.50796,   1.5708,  1.63363,  
     &1.69646,  1.75929,  1.82212,  1.88496,  1.94779,  2.01062,  
     &2.07345,  2.13628,  2.19911,  2.26195,  2.32478,  2.38761,  
     &2.45044,  2.51327,  2.57611,  2.63894,  2.70177,   2.7646,  
     &2.82743,  2.89027,   2.9531,  3.01593,  3.07876,  3.14159,  
     &3.20442,  3.26726,  3.33009,  3.39292,  3.45575,  3.51858,  
     &3.58142,  3.64425,  3.70708,  3.76991,  3.83274,  3.89557,  
     &3.95841,  4.02124,  4.08407,   4.1469,  4.20973,  4.27257,   
     &4.3354,  4.39823,  4.46106,  4.52389,  4.58673,  4.64956,  
     &4.71239,  4.77522,  4.83805,  4.90088,  4.96372,  5.02655,  
     &5.08938,  5.15221,  5.21504,  5.27788,  5.34071,  5.40354,  
     &5.46637,   5.5292,  5.59203,  5.65487,   5.7177,  5.78053,  
     &5.84336,  5.90619,  5.96903,  6.03186,  6.09469,  6.15752,  
     &6.22035,  6.28319,  6.59734,   6.9115,  7.22566,  7.53982,  
     &7.85398,  8.16814,   8.4823,  8.79646,  9.11062,  9.42478,  
     &9.73894,  10.0531,  10.3673,  10.6814,  10.9956,  11.3097,  
     &11.6239,  11.9381,  12.2522,  12.5664/
      DATA (FLUX(I,2,1),I=1,121) /0.000404009, 0.00392351, 0.00758112, 
     &0.0109642, 0.0140671, 0.0168871, 0.019425, 0.0216845, 0.0236723, 
     &0.0253982, 0.0268742, 0.0281145, 0.0291353, 0.0299539, 0.0305884, 
     &0.0310576, 0.0313803, 0.0315749, 0.0316592, 0.0316499, 0.0315629, 
     &0.0314126, 0.0312118, 0.0309721, 0.0307032, 0.0304133, 0.0301092, 
     &0.0297962, 0.0294781, 0.0291573, 0.0288354, 0.0285125, 0.0281883, 
     &0.0278615, 0.0275302, 0.0271924, 0.0268457, 0.0264877, 0.0261159, 
     &0.0257285, 0.0253236,   0.0249, 0.0244569, 0.0239942, 0.0235121, 
     &0.0230115, 0.0224938, 0.0219609, 0.0214148, 0.0208582, 0.0202936, 
     &0.0197239, 0.019152, 0.0185805, 0.0180121, 0.0174493, 0.0168942, 
     &0.0163489, 0.0158149, 0.0152936, 0.0147861, 0.014293, 0.0138149, 
     &0.0133518, 0.0129037, 0.0124703, 0.0120513, 0.0116459, 0.0112534, 
     &0.0108733, 0.0105045, 0.0101465, 0.00979838, 0.00945948, 
     &0.0091292, 0.00880703, 0.0084925, 0.00818535, 0.0078853, 
     &0.00759225, 0.00730615, 0.00702705, 0.006755, 0.00649014, 
     &0.00623262, 0.00598259, 0.00574022, 0.00550565, 0.00527895, 
     &0.00506023, 0.00484951, 0.00464678, 0.00445198, 0.00426498, 
     &0.00408567, 0.00391383, 0.00374925, 0.00359166, 0.00344078, 
     &0.00329631, 0.00315796, 0.0025469, 0.00204557, 0.00163075, 
     &0.0012906, 0.00101732, 0.000801626, 0.000632102, 0.00049774, 
     &0.000390215, 0.000304261, 0.000236379, 0.000183518, 0.000142612, 
     &0.000110818, 8.5892e-05, 6.62998e-05, 5.10081e-05, 3.92052e-05, 
     &3.01586e-05, 2.32131e-05/
      DATA (FLUX(I,2,2),I=1,121) /1.26e-06, 0.000122431, 0.000476022,  
     &0.00104, 0.00178939, 0.00269598, 0.003735, 0.00488878, 0.00614213, 
     &0.00747465, 0.0088588, 0.0102671, 0.0116812, 0.0130912, 0.0144878, 
     &0.0158537, 0.0171651, 0.018402, 0.0195553, 0.0206245, 0.0216077, 
     &0.0224957, 0.0232756, 0.0239401, 0.0244915, 0.0249384, 0.0252869, 
     &0.0255375, 0.0256892, 0.0257458, 0.0257172, 0.0256153, 0.0254495, 
     &0.0252251, 0.0249472, 0.0246228, 0.0242616, 0.0238724, 0.0234616, 
     &0.0230335, 0.0225927, 0.0221441, 0.0216922, 0.0212403, 0.0207899, 
     &0.0203422, 0.0198992, 0.0194625, 0.0190324, 0.0186078, 0.0181871, 
     &0.0177698, 0.0173567, 0.016948, 0.0165423, 0.0161373, 0.0157316, 
     &0.0153252, 0.0149193, 0.0145143, 0.0141098, 0.0137047, 0.0132989, 
     &0.0128937, 0.0124907, 0.0120913, 0.011696, 0.0113049, 0.0109185, 
     &0.0105384, 0.0101659, 0.00980213, 0.00944741, 0.00910205, 
     &0.00876643, 0.00844109, 0.00812641, 0.00782259, 0.00752948, 
     &0.007247, 0.00697494, 0.00671296, 0.0064607, 0.0062176, 
     &0.00598332, 0.00575754, 0.00553982, 0.00532963, 0.00512633, 
     &0.00492941, 0.00473864, 0.00455387, 0.00437493, 0.00420146, 
     &0.00403308, 0.00386959, 0.00371103, 0.00355747, 0.00340894, 
     &0.00326531, 0.00312645, 0.0025038, 0.00199701, 0.00159259, 
     &0.00127038, 0.0010106, 0.000799155, 0.000627867, 0.000491358, 
     &0.000384293, 0.000300776, 0.000235218, 0.000183312, 0.000142194, 
     &0.000109933, 8.49344e-05, 6.56842e-05, 5.08066e-05, 3.92136e-05, 
     &3.01523e-05, 2.31118e-05/
      DATA (FLUX(I,2,3),I=1,121) /3.71013e-09, 3.60137e-06, 2.79421e-05, 
     &9.15076e-05, 0.000210575, 0.000399441, 0.000670605, 0.0010349, 
     &0.00150158, 0.00207833, 0.00277124, 0.00358477, 0.00452156, 
     &0.00558228, 0.00676548, 0.00806727, 0.00948115, 0.0109977, 
     &0.0126046, 0.0142862, 0.0160239, 0.017796, 0.0195783, 0.0213443, 
     &0.0230663, 0.024716, 0.0262659,  0.02769, 0.0289652, 0.0300722, 
     &0.0309962, 0.0317278, 0.0322628, 0.0326024, 0.0327526, 0.0327237, 
     &0.0325297, 0.0321871, 0.0317142,  0.03113, 0.0304537, 0.0297042, 
     &0.028899, 0.0280543, 0.027185, 0.0263039, 0.0254221, 0.0245488, 
     &0.0236917, 0.0228568, 0.0220487, 0.0212707, 0.0205247, 0.0198119, 
     &0.0191324, 0.0184857, 0.0178703, 0.0172848, 0.016727, 0.0161945, 
     &0.0156848, 0.0151952, 0.0147233, 0.0142665, 0.0138225, 0.0133894, 
     &0.0129653, 0.0125488, 0.0121387, 0.0117343, 0.0113352, 0.010941, 
     &0.010552, 0.0101684, 0.00979058, 0.00941927, 0.0090551, 
     &0.00869877, 0.00835099, 0.00801245, 0.0076837, 0.00736531, 
     &0.00705769, 0.00676116, 0.00647592, 0.00620205, 0.00593957, 
     &0.00568833, 0.00544816, 0.00521873, 0.00499969, 0.00479062, 
     &0.00459105, 0.0044005, 0.00421842, 0.00404429, 0.00387761, 
     &0.00371786, 0.00356457, 0.00341728, 0.00327558, 0.00263989, 
     &0.00210751, 0.00166763, 0.00131391, 0.00103573, 0.000818073, 
     &0.000645765, 0.000507411, 0.000396169, 0.000307892, 0.000239034, 
     &0.000185837, 0.000144605, 0.000112303, 8.68411e-05, 6.68717e-05, 
     &5.14003e-05, 3.95356e-05, 3.04482e-05, 2.3439e-05/
      DATA (ETVALS(I,3),I=1,131) /0.00502655, 0.0502655, 0.100531, 
     &0.150796, 0.201062, 0.251327, 0.301593, 0.351858, 0.402124, 
     &0.452389, 0.502655,  0.55292, 0.603186, 0.653451, 0.703717, 
     &0.753982, 0.804248, 0.854513, 0.904779, 0.955044,  1.00531,  
     &1.05558,  1.10584,  1.15611,  1.20637,  1.25664,   1.3069,  
     &1.35717,  1.40743,   1.4577,  1.50796,  1.55823,   1.6085,  
     &1.65876,  1.70903,  1.75929,  1.80956,  1.85982,  1.91009,  
     &1.96035,  2.01062,  2.06088,  2.11115,  2.16142,  2.21168,  
     &2.26195,  2.31221,  2.36248,  2.41274,  2.46301,  2.51327,  
     &2.56354,  2.61381,  2.66407,  2.71434,   2.7646,  2.81487,  
     &2.86513,   2.9154,  2.96566,  3.01593,  3.06619,  3.11646,  
     &3.16673,  3.21699,  3.26726,  3.31752,  3.36779,  3.41805,  
     &3.46832,  3.51858,  3.56885,  3.61911,  3.66938,  3.71965,  
     &3.76991,  3.82018,  3.87044,  3.92071,  3.97097,  4.02124,   
     &4.0715,  4.12177,  4.17204,   4.2223,  4.27257,  4.32283,   
     &4.3731,  4.42336,  4.47363,  4.52389,  4.57416,  4.62442,  
     &4.67469,  4.72496,  4.77522,  4.82549,  4.87575,  4.92602,  
     &4.97628,  5.02655,  5.27788,   5.5292,  5.78053,  6.03186,  
     &6.28319,  6.53451,  6.78584,  7.03717,  7.28849,  7.53982,  
     &7.79115,  8.04248,   8.2938,  8.54513,  8.79646,  9.04779,  
     &9.29911,  9.55044,  9.80177,  10.0531,  10.3044,  10.5558,  
     &10.8071,  11.0584,  11.3097,  11.5611,  11.8124,  12.0637,   
     &12.315,  12.5664/
      DATA (FLUX(I,3,1),I=1,131) /0.000505327, 0.0049346, 0.00959093, 
     &0.0139491, 0.0179937, 0.0217143, 0.0251057, 0.0281676, 0.0309046, 
     &0.0333259, 0.0354444, 0.0372764, 0.038841, 0.0401591, 0.0412527, 
     &0.0421447, 0.0428579, 0.0434145, 0.0438363, 0.0441431, 0.044354, 
     &0.0444859, 0.0445537, 0.0445708, 0.0445487, 0.0444965, 0.0444215,  
     &0.04433, 0.0442256, 0.0441111, 0.0439875, 0.0438552, 0.0437129, 
     &0.043559, 0.043391, 0.0432061, 0.0430015, 0.042774, 0.0425203, 
     &0.0422381, 0.0419248, 0.0415786, 0.0411982, 0.0407828, 0.0403324, 
     &0.0398476, 0.0393294, 0.0387796, 0.0382003, 0.0375943, 0.0369644, 
     &0.0363139, 0.0356461, 0.0349645, 0.0342726, 0.0335736, 0.0328707, 
     &0.0321668, 0.0314647, 0.0307669, 0.0300755, 0.0293922, 0.0287185, 
     &0.0280556, 0.0274042, 0.0267649, 0.026138, 0.0255234, 0.024921, 
     &0.0243305, 0.0237512, 0.0231829, 0.0226246, 0.0220758, 0.0215358, 
     &0.021004, 0.0204799, 0.019963, 0.0194529, 0.0189494, 0.0184521, 
     &0.0179612, 0.0174766, 0.0169985, 0.0165271, 0.0160627, 0.0156055, 
     &0.0151559, 0.0147145, 0.0142814, 0.013857, 0.0134419, 0.0130361, 
     &0.0126401, 0.012254, 0.0118779, 0.011512, 0.0111562, 0.0108106, 
     &0.010475, 0.0101493, 0.00866104, 0.00737802, 0.00626206, 
     &0.00528708, 0.00444089, 0.00371704, 0.00310645, 0.00259473, 
     &0.00216484, 0.00180149, 0.00149375, 0.00123447, 0.00101818, 
     &0.000839256, 0.00069163, 0.000569436, 0.000467843, 0.000383349, 
     &0.000313423, 0.000255983, 0.000209043, 0.00017069, 0.000139245, 
     &0.000113386, 9.21391e-05, 7.47678e-05, 6.06441e-05, 4.91935e-05, 
     &3.98982e-05, 3.23278e-05/
      DATA (FLUX(I,3,2),I=1,131) /1.44805e-06, 0.000141467, 0.00055311, 
     &0.00121462, 0.00209962, 0.00317696, 0.00441866, 0.00580455, 
     &0.00731715, 0.00893248, 0.0106181, 0.0123416, 0.0140814, 0.015827, 
     &0.0175681, 0.0192849, 0.0209504, 0.0225421, 0.024051, 0.0254782, 
     &0.0268224, 0.0280734, 0.0292158, 0.0302412, 0.0311526,  0.03196, 
     &0.032669, 0.0332779, 0.0337827, 0.0341856, 0.0344962, 0.0347265, 
     &0.0348843, 0.0349723, 0.0349925, 0.0349516, 0.0348587, 0.0347233, 
     &0.0345502, 0.0343425, 0.0341041, 0.0338403, 0.033557, 0.0332574, 
     &0.0329433, 0.0326158, 0.0322779, 0.0319328, 0.0315816, 0.0312234, 
     &0.0308568, 0.0304819, 0.0301012, 0.0297154, 0.0293238, 0.0289232, 
     &0.0285119, 0.0280904, 0.027661, 0.0272247, 0.0267802, 0.0263253, 
     &0.0258593, 0.0253843, 0.0249027, 0.0244162, 0.0239244, 0.0234269, 
     &0.0229242, 0.0224188, 0.0219133, 0.0214091, 0.0209067, 0.0204063, 
     &0.0199091, 0.0194168, 0.0189307, 0.0184522, 0.0179811, 0.017518, 
     &0.0170634, 0.0166178, 0.0161817, 0.0157549, 0.0153376, 0.0149296, 
     &0.0145312, 0.0141418, 0.0137611, 0.0133886, 0.013024, 0.0126676, 
     &0.012319, 0.0119777, 0.011643, 0.0113145, 0.0109922, 0.0106765, 
     &0.0103672, 0.010064, 0.00976629, 0.00836625, 0.00711991, 
     &0.00603413, 0.00510372, 0.00431049, 0.00363159, 0.00304777, 
     &0.00254654, 0.00212009, 0.0017616, 0.00146279, 0.00121399, 
     &0.0010058, 0.000830862, 0.00068412, 0.000561979, 0.000461173, 
     &0.000378353, 0.000310252, 0.000254053, 0.000207585, 0.000169256, 
     &0.000137826, 0.000112199, 9.13435e-05, 7.43267e-05, 6.03912e-05, 
     &4.89718e-05, 3.96479e-05, 3.20759e-05/
      DATA (FLUX(I,3,3),I=1,131) /5.0498e-09, 4.92905e-06, 3.84715e-05, 
     &0.000126715, 0.00029321, 0.000559154, 0.000943548, 0.00146328, 
     &0.00213315, 0.00296582, 0.00397172, 0.00515891, 0.00653277, 
     &0.00809581, 0.00984729, 0.0117829, 0.0138944, 0.0161692, 
     &0.0185905, 0.0211371, 0.0237826, 0.0264973, 0.0292472, 0.0319957, 
     &0.0347041, 0.0373332, 0.0398443, 0.0422007, 0.0443696, 0.0463222, 
     &0.0480361, 0.0494951, 0.0506894, 0.0516166,  0.05228, 0.0526887, 
     &0.0528572, 0.0528034, 0.0525478, 0.0521132, 0.0515233, 0.0508015, 
     &0.0499707, 0.0490532, 0.0480688, 0.0470364, 0.0459726, 0.0448917, 
     &0.0438063, 0.0427271, 0.0416623, 0.0406194, 0.0396032, 0.0386175, 
     &0.0376649, 0.0367467, 0.035863, 0.0350132, 0.0341959, 0.0334092, 
     &0.0326506, 0.0319176, 0.0312069, 0.0305158, 0.0298411, 0.0291798, 
     &0.0285297, 0.0278879, 0.0272524, 0.0266217, 0.0259942, 0.0253692, 
     &0.024746, 0.0241244, 0.0235045, 0.022887, 0.0222723, 0.0216615, 
     &0.0210554, 0.0204554, 0.0198626, 0.019278, 0.018703, 0.0181386, 
     &0.0175858, 0.0170454, 0.0165183, 0.0160049, 0.0155058, 0.0150213, 
     &0.0145514, 0.0140963, 0.0136557, 0.0132296, 0.0128175, 0.012419, 
     &0.0120336, 0.0116609, 0.0113001, 0.0109506, 0.010612, 0.00905965,   
     &0.0077, 0.00650119, 0.00545587, 0.00456275, 0.00381295, 
     &0.00318752, 0.00266297, 0.00221879, 0.00184112, 0.00152188, 
     &0.00125524, 0.00103485, 0.000853342, 0.000703293, 0.000578464, 
     &0.000474399, 0.000388042, 0.000316975, 0.000258894, 0.000211499,
     &0.000172685, 0.000140756, 0.000114472, 9.29277e-05, 7.53814e-05, 
     &6.11556e-05, 4.96218e-05, 4.02384e-05, 3.258e-05/
      DATA (ETVALS(I,4),I=1,141) /0.00418879, 0.0418879, 0.0837758, 
     &0.125664, 0.167552,  0.20944, 0.251327, 0.293215, 0.335103, 
     &0.376991, 0.418879, 0.460767, 0.502655, 0.544543, 0.586431, 
     &0.628319, 0.670206, 0.712094, 0.753982,  0.79587, 0.837758, 
     &0.879646, 0.921534, 0.963422,  1.00531,   1.0472,  1.08909,  
     &1.13097,  1.17286,  1.21475,  1.25664,  1.29852,  1.34041,   
     &1.3823,  1.42419,  1.46608,  1.50796,  1.54985,  1.59174,  
     &1.63363,  1.67552,   1.7174,  1.75929,  1.80118,  1.84307,  
     &1.88496,  1.92684,  1.96873,  2.01062,  2.05251,   2.0944,  
     &2.13628,  2.17817,  2.22006,  2.26195,  2.30383,  2.34572,  
     &2.38761,   2.4295,  2.47139,  2.51327,  2.55516,  2.59705,  
     &2.63894,  2.68083,  2.72271,   2.7646,  2.80649,  2.84838,  
     &2.89027,  2.93215,  2.97404,  3.01593,  3.05782,   3.0997,  
     &3.14159,  3.18348,  3.22537,  3.26726,  3.30914,  3.35103,  
     &3.39292,  3.43481,   3.4767,  3.51858,  3.56047,  3.60236,  
     &3.64425,  3.68614,  3.72802,  3.76991,   3.8118,  3.85369,  
     &3.89557,  3.93746,  3.97935,  4.02124,  4.06313,  4.10501,   
     &4.1469,  4.18879,  4.39823,  4.60767,  4.81711,  5.02655,  
     &5.23599,  5.44543,  5.65487,  5.86431,  6.07375,  6.28319,  
     &6.49262,  6.70206,   6.9115,  7.12094,  7.33038,  7.53982,  
     &7.74926,   7.9587,  8.16814,  8.37758,  8.58702,  8.79646,   
     &9.0059,  9.21534,  9.42478,  9.63422,  9.84366,  10.0531,  
     &10.2625,   10.472,  10.6814,  10.8909,  11.1003,  11.3097,  
     &11.5192,  11.7286,  11.9381,  12.1475,  12.3569,  12.5664/
      DATA (FLUX(I,4,1),I=1,141) /0.000606647, 0.00594603, 0.0116033, 
     &0.016942, 0.0219381, 0.0265737, 0.0308375, 0.0347254, 0.0382389, 
     &0.0413862, 0.0441804, 0.0466395, 0.0487846, 0.0506398, 0.0522308, 
     &0.0535844, 0.0547275, 0.0556864, 0.0564866, 0.057152, 0.0577045, 
     &0.0581642, 0.0585486, 0.0588735, 0.0591516, 0.0593938, 0.0596084, 
     &0.0598017, 0.0599782, 0.0601405, 0.0602892, 0.0604241, 0.0605434, 
     &0.0606449, 0.0607249, 0.0607799, 0.0608058, 0.0607984, 0.0607539, 
     &0.0606688, 0.0605397, 0.060364, 0.0601398, 0.0598657, 0.0595413, 
     &0.0591669, 0.0587431, 0.0582717, 0.057755, 0.0571957, 0.0565971, 
     &0.0559623, 0.0552955, 0.0546007, 0.0538817, 0.0531424, 0.052387, 
     &0.0516188, 0.0508418, 0.0500589, 0.049273, 0.0484868, 0.0477025, 
     &0.0469218, 0.0461463, 0.0453773, 0.0446153, 0.0438612, 0.0431151, 
     &0.0423769, 0.0416466, 0.040924, 0.0402085, 0.0394994, 0.0387965, 
     &0.0380988, 0.0374063, 0.0367179, 0.0360334, 0.0353527, 0.0346753, 
     &0.0340011, 0.0333301, 0.0326625, 0.0319982, 0.031338, 0.030682, 
     &0.0300307, 0.0293846, 0.0287446, 0.0281108, 0.0274843, 0.0268655, 
     &0.0262549, 0.0256533, 0.0250609, 0.0244783, 0.023906, 0.0233442, 
     &0.0227931, 0.0222527, 0.0197156, 0.0174381, 0.0153863, 0.0135276, 
     &0.0118437, 0.0103304, 0.00898649, 0.00780568, 0.00677302, 
     &0.00586881, 0.00507424, 0.00437536, 0.00376301, 0.00323036, 
     &0.0027702, 0.00237405, 0.00203276, 0.00173797, 0.00148308, 
     &0.00126318, 0.00107444, 0.000913232, 0.000775886, 0.000658809, 
     &0.0005588, 0.000473283, 0.000400276, 0.000338187, 0.000285581, 
     &0.000241095, 0.000203455, 0.000171549, 0.00014448, 0.000121544, 
     &0.000102169, 8.58513e-05, 7.21281e-05, 6.058e-05, 5.0847e-05, 
     &4.2637e-05/
      DATA (FLUX(I,4,2),I=1,141) /1.58875e-06, 0.000155777, 0.000611358, 
     &0.00134723, 0.0023364, 0.00354585, 0.00494547, 0.00651345, 
     &0.00823075, 0.0100709, 0.0119976, 0.013975, 0.0159792, 0.0179987, 
     &0.0200228,  0.02203, 0.0239905, 0.0258797, 0.027689, 0.0294203, 
     &0.0310734, 0.0326368, 0.0340936, 0.0354346, 0.0366637, 0.0377918, 
     &0.0388251, 0.0397602, 0.0405913, 0.0413194, 0.0419549,  0.04251, 
     &0.0429916, 0.0434002, 0.0437363, 0.0440055, 0.0442177, 0.0443814, 
     &0.0445006, 0.0445768, 0.0446126, 0.0446138, 0.0445866, 0.0445339, 
     &0.0444561, 0.0443542, 0.0442311, 0.0440908, 0.043935, 0.0437623, 
     &0.0435705, 0.0433601, 0.043134, 0.0428947, 0.0426409, 0.0423686, 
     &0.0420758, 0.0417632, 0.0414342, 0.0410905, 0.0407298, 0.0403491, 
     &0.0399471, 0.0395261, 0.0390897, 0.0386397, 0.0381749, 0.0376934, 
     &0.0371956, 0.0366852, 0.0361651, 0.0356373, 0.0351016, 0.0345574, 
     &0.0340063, 0.033451, 0.0328941, 0.0323369, 0.0317793, 0.0312216, 
     &0.0306655, 0.0301121, 0.0295629, 0.029018, 0.0284776, 0.0279423, 
     &0.0274126, 0.026889, 0.0263711, 0.025859, 0.0253526, 0.0248524, 
     &0.0243586, 0.0238707, 0.0233881, 0.0229102, 0.0224376, 0.0219708, 
     &0.0215097, 0.0210537, 0.0206023,  0.01842, 0.0163737, 0.014485, 
     &0.0127713, 0.0112347, 0.00986244, 0.00863503, 0.00753484, 
     &0.00655095, 0.0056776, 0.00491003, 0.00424072, 0.00365878, 
     &0.00315218, 0.00271025, 0.00232498, 0.00199046, 0.00170169, 
     &0.00145362, 0.00124088, 0.00105822, 0.00090105, 0.000765812, 
     &0.000649816, 0.000550817, 0.000466651, 0.000395186, 0.00033444, 
     &0.000282732, 0.00023872, 0.000201334, 0.000169683, 0.000142962, 
     &0.000120422, 0.000101383, 8.52738e-05, 7.16414e-05, 6.01294e-05, 
     &5.04382e-05, 4.22984e-05/
      DATA (FLUX(I,4,3),I=1,141) /6.3726e-09, 6.24338e-06, 4.89273e-05, 
     &0.000161789, 0.000375805, 0.000719346, 0.00121828, 0.001896, 
     &0.00277341, 0.00386878, 0.00519756, 0.00677209, 0.00860124,  
     &0.01069, 0.0130391, 0.0156443, 0.0184961, 0.0215794, 0.024873, 
     &0.0283496, 0.0319762, 0.0357137, 0.0395186, 0.0433433, 0.0471377, 
     &0.0508503, 0.0544308, 0.0578313, 0.0610079, 0.0639223, 0.0665436, 
     &0.0688485, 0.0708218, 0.0724567, 0.0737543, 0.0747229, 0.0753767, 
     &0.0757352, 0.0758217, 0.0756625, 0.075285, 0.0747175, 0.0739879, 
     &0.0731235, 0.0721494, 0.0710899, 0.069966, 0.0687977, 0.0676016, 
     &0.0663924, 0.0651828, 0.0639829, 0.0628008, 0.0616432, 0.0605145, 
     &0.0594179, 0.0583553, 0.0573268, 0.0563321, 0.0553697, 0.0544376, 
     &0.0535328, 0.0526524, 0.0517929, 0.0509508, 0.0501227, 0.0493051, 
     &0.0484947, 0.0476888, 0.0468848, 0.0460805, 0.0452743, 0.0444649, 
     &0.0436515, 0.042834, 0.042012, 0.0411865, 0.0403581, 0.0395277, 
     &0.0386968, 0.0378669, 0.0370395, 0.0362164, 0.035399, 0.0345891, 
     &0.0337884, 0.0329982, 0.0322198, 0.0314545, 0.0307031, 0.0299666, 
     &0.0292456, 0.0285406, 0.0278519, 0.0271795, 0.0265236, 0.0258838, 
     &0.0252599, 0.0246516, 0.0240583, 0.0234793, 0.0207791, 0.0183468, 
     &0.0161285, 0.0141068, 0.0122877, 0.0106782, 0.00927219, 
     &0.00804831, 0.00697838, 0.00603684, 0.00520644, 0.00447737, 
     &0.00384297, 0.0032956, 0.00282521, 0.00242045, 0.00207078, 
     &0.00176801, 0.00150635, 0.00128146, 0.00108933, 0.000925749, 
     &0.000786426, 0.00066745, 0.000565638, 0.000478595, 0.000404457, 
     &0.000341594, 0.000288443, 0.000243503, 0.000205427, 0.000173112, 
     &0.000145696, 0.000122502, 0.000102951, 8.65082e-05, 7.26804e-05, 
     &6.10319e-05, 5.12045e-05, 4.29157e-05/
      DATA (ETVALS(I,5),I=1,119) /0.00359039, 0.0359039, 0.0718078, 
     &0.107712, 0.143616,  0.17952, 0.215423, 0.251327, 0.287231, 
     &0.323135, 0.359039, 0.394943, 0.430847, 0.466751, 0.502655, 
     &0.538559, 0.574463, 0.610367,  0.64627, 0.682174, 0.718078, 
     &0.753982, 0.789886,  0.82579, 0.861694, 0.897598, 0.933502, 
     &0.969406,  1.00531,  1.04121,  1.07712,  1.11302,  1.14893,  
     &1.18483,  1.22073,  1.25664,  1.29254,  1.32844,  1.36435,  
     &1.40025,  1.43616,  1.47206,  1.50796,  1.54387,  1.57977,  
     &1.61568,  1.65158,  1.68748,  1.72339,  1.75929,   1.7952,   
     &1.8311,    1.867,  1.90291,  1.93881,  1.97472,  2.01062,  
     &2.04652,  2.08243,  2.11833,  2.15423,  2.33375,  2.51327,  
     &2.69279,  2.87231,  3.05183,  3.23135,  3.41087,  3.59039,  
     &3.76991,  3.94943,  4.12895,  4.30847,  4.48799,  4.66751,  
     &4.84703,  5.02655,  5.20607,  5.38559,  5.56511,  5.74463,  
     &5.92415,  6.10367,  6.28319,   6.4627,  6.64222,  6.82174,  
     &7.00126,  7.18078,   7.3603,  7.53982,  7.71934,  7.89886,  
     &8.07838,   8.2579,  8.43742,  8.61694,  8.79646,  8.97598,   
     &9.1555,  9.33502,  9.51454,  9.69406,  9.87358,  10.0531,  
     &10.2326,  10.4121,  10.5917,  10.7712,  10.9507,  11.1302,  
     &11.3097,  11.4893,  11.6688,  11.8483,  12.0278,  12.2073,  
     &12.3869,  12.5664/
      DATA (FLUX(I,5,1),I=1,119) /0.000707742, 0.00696306, 0.013622, 
     &0.0199438, 0.0258963, 0.0314543, 0.0366008, 0.0413272, 0.0456326, 
     &0.0495235, 0.0530132, 0.0561204, 0.0588688, 0.061285, 0.0633984, 
     &0.0652395, 0.0668391, 0.0682279, 0.0694351, 0.0704883, 0.0714134, 
     &0.0722334, 0.0729684, 0.0736363, 0.0742525, 0.0748286, 0.0753746, 
     &0.0758971, 0.0764006, 0.0768889, 0.0773613, 0.0778171, 0.0782538, 
     &0.0786693, 0.0790574, 0.0794139, 0.0797338, 0.0800119, 0.0802432, 
     &0.0804228, 0.0805463, 0.0806106, 0.0806132, 0.0805515, 0.0804249, 
     &0.080233, 0.0799768, 0.0796578, 0.0792777, 0.0788402, 0.0783479, 
     &0.0778052, 0.0772162, 0.0765849, 0.0759168, 0.0752153, 0.074486, 
     &0.0737326, 0.0729594,  0.07217, 0.0713688, 0.0672747, 0.0631895, 
     &0.0591664, 0.0551559, 0.0511155, 0.0470694, 0.0431013, 0.0393084, 
     &0.0357573, 0.0324669, 0.0294194, 0.0266, 0.0239432, 0.0215, 
     &0.0192369, 0.0171849, 0.0153311, 0.0136605, 0.012153, 0.0107899, 
     &0.00955817, 0.00844947, 0.00745744, 0.00657441, 0.0057905, 
     &0.00509428, 0.00447488, 0.00392336, 0.00345015, 0.00301986, 
     &0.00264098, 0.00230825, 0.00201616, 0.00175951, 0.00153379, 
     &0.00133545, 0.00116159, 0.00100965, 0.000877169, 0.000761744, 
     &0.000661112, 0.000573298, 0.000496682, 0.000429944, 0.000371949, 
     &0.000321654, 0.000278075, 0.000240297, 0.000207523, 0.000179083, 
     &0.000154432, 0.000133102, 0.000114682, 9.87872e-05, 8.5069e-05, 
     &7.3221e-05, 6.29832e-05, 5.41427e-05/
      DATA (FLUX(I,5,2),I=1,119) /1.69752e-06, 0.000166876, 0.000656692, 
     &0.00145079, 0.0025219, 0.00383575, 0.00536073, 0.0070739, 
     &0.00895512, 0.010976, 0.0130975, 0.0152809, 0.0175005, 0.0197445, 
     &0.0220015, 0.0242486, 0.0264539, 0.0285917, 0.030653, 0.0326409, 
     &0.0345556, 0.036385, 0.038111, 0.0397236, 0.0412279, 0.0426358, 
     &0.0439534, 0.0451766, 0.0462981, 0.0473188, 0.0482497, 0.0491032, 
     &0.0498854, 0.0505955, 0.0512329, 0.0518027, 0.0523152, 0.0527787, 
     &0.053196, 0.053567, 0.053894, 0.0541827, 0.0544391, 0.0546662, 
     &0.0548626, 0.0550287, 0.0551678, 0.0552839, 0.0553792, 0.0554513, 
     &0.0554971, 0.0555166, 0.0555143, 0.0554926, 0.0554502, 0.0553821, 
     &0.0552856, 0.055162, 0.0550154, 0.0548479, 0.0546569, 0.0532957, 
     &0.051347, 0.0489391, 0.0462389, 0.0434023, 0.0405365, 0.0376939, 
     &0.0348892, 0.0321298, 0.0294357, 0.026842, 0.0243855, 0.022091, 
     &0.0199665, 0.0180056, 0.0161949, 0.0145232, 0.0129848, 0.0115789, 
     &0.0103044, 0.00915607, 0.00812422, 0.00719702, 0.00636366, 
     &0.00561573, 0.00494694, 0.0043518, 0.00382449, 0.00335844, 
     &0.0029466, 0.00258226, 0.00225987, 0.00197508, 0.0017243, 
     &0.00150417, 0.00131132, 0.00114241, 0.000994366, 0.000864575, 
     &0.000750893, 0.000651563, 0.00056502, 0.000489769, 0.000424368, 
     &0.00036749, 0.000318001, 0.000274961, 0.000237584, 0.000205188, 
     &0.000177153, 0.000152904, 0.000131918, 0.000113742, 9.79971e-05, 
     &8.43751e-05, 7.26128e-05, 6.24714e-05, 5.3733e-05/
      DATA (FLUX(I,5,3),I=1,119) /7.67881e-09, 7.54357e-06, 5.92886e-05, 
     &0.000196611, 0.00045797, 0.000879024, 0.0014927, 0.00232918, 
     &0.00341578, 0.00477676, 0.00643302, 0.00840168, 0.0106956, 
     &0.0133228, 0.0162858, 0.0195811, 0.0231985, 0.0271202, 0.0313215, 
     &0.0357694, 0.0404231, 0.0452356, 0.0501528, 0.0551159, 0.0600627,  
     &0.06493, 0.0696545, 0.0741767, 0.0784408, 0.082399, 0.086011, 
     &0.0892462, 0.0920832, 0.0945115, 0.0965297, 0.0981441, 0.0993701, 
     &0.100229, 0.100746,  0.10095, 0.100875, 0.100552, 0.100014, 
     &0.0992936, 0.0984218, 0.097427, 0.0963357, 0.0951719, 0.093957, 
     &0.0927098, 0.0914464, 0.0901803, 0.0889224, 0.0876818, 0.0864652, 
     &0.0852772, 0.0841213, 0.0829988,  0.08191, 0.0808538, 0.0798285, 
     &0.0750509, 0.0705011, 0.0657913, 0.0608118, 0.0557087, 0.0507283, 
     &0.0460694, 0.0418151, 0.0379423, 0.0343766, 0.0310499, 0.0279327, 
     &0.025033, 0.0223728, 0.0199657, 0.0178044, 0.0158648, 0.0141164, 
     &0.0125338, 0.0111013, 0.00981121, 0.00865793, 0.00763366, 
     &0.00672658, 0.00592251, 0.00520797, 0.00457206, 0.00400705, 
     &0.00350824, 0.0030683, 0.00268218, 0.00234346, 0.00204591, 
     &0.00178415, 0.00155392, 0.0013519, 0.00117523, 0.00102119, 
     &0.000887018, 0.000770095, 0.000668061, 0.000578992, 0.000501331, 
     &0.000433784, 0.000375185, 0.000324413, 0.000280419, 0.000242259, 
     &0.000209136, 0.000180402, 0.00015552, 0.000134018, 0.000115463, 
     &9.9454e-05, 8.56321e-05, 7.36885e-05, 6.33696e-05, 5.44653e-05/
      DATA (ETVALS(I,6),I=1,151) /0.00314159, 0.0314159, 0.0628319, 
     &0.0942478, 0.125664,  0.15708, 0.188496, 0.219911, 0.251327, 
     &0.282743, 0.314159, 0.345575, 0.376991, 0.408407, 0.439823, 
     &0.471239, 0.502655, 0.534071, 0.565487, 0.596903, 0.628319, 
     &0.659734,  0.69115, 0.722566, 0.753982, 0.785398, 0.816814,  
     &0.84823, 0.879646, 0.911062, 0.942478, 0.973894,  1.00531,  
     &1.03673,  1.06814,  1.09956,  1.13097,  1.16239,  1.19381,  
     &1.22522,  1.25664,  1.28805,  1.31947,  1.35088,   1.3823,  
     &1.41372,  1.44513,  1.47655,  1.50796,  1.53938,   1.5708,  
     &1.60221,  1.63363,  1.66504,  1.69646,  1.72788,  1.75929,  
     &1.79071,  1.82212,  1.85354,  1.88496,  1.91637,  1.94779,   
     &1.9792,  2.01062,  2.04204,  2.07345,  2.10487,  2.13628,   
     &2.1677,  2.19911,  2.23053,  2.26195,  2.29336,  2.32478,  
     &2.35619,  2.38761,  2.41903,  2.45044,  2.48186,  2.51327,  
     &2.54469,  2.57611,  2.60752,  2.63894,  2.67035,  2.70177,  
     &2.73319,   2.7646,  2.79602,  2.82743,  2.85885,  2.89027,  
     &2.92168,   2.9531,  2.98451,  3.01593,  3.04734,  3.07876,  
     &3.11018,  3.14159,  3.29867,  3.45575,  3.61283,  3.76991,  
     &3.92699,  4.08407,  4.24115,  4.39823,  4.55531,  4.71239,  
     &4.86947,  5.02655,  5.18363,  5.34071,  5.49779,  5.65487,  
     &5.81195,  5.96903,  6.12611,  6.28319,  6.44026,  6.59734,  
     &6.75442,   6.9115,  7.06858,  7.22566,  7.38274,  7.53982,   
     &7.6969,  7.85398,  8.01106,  8.16814,  8.32522,   8.4823,  
     &8.63938,  8.79646,  8.95354,  9.11062,   9.2677,  9.42478,  
     &9.73894,  10.0531,  10.3673,  10.6814,  10.9956,  11.3097,  
     &11.6239,  11.9381,  12.2522,  12.5664/
      DATA (FLUX(I,6,1),I=1,151) /0.000809285, 0.0079693, 0.0156311, 
     &0.022938, 0.0298497, 0.0363344, 0.0423699, 0.0479426, 0.0530494, 
     &0.0576952, 0.0618929, 0.0656626, 0.0690295, 0.0720232, 0.0746762, 
     &0.0770231, 0.0790985, 0.0809368, 0.0825719, 0.0840346, 0.0853549, 
     &0.0865583, 0.0876687, 0.088706, 0.0896874, 0.0906267, 0.0915347, 
     &0.0924189, 0.0932841, 0.0941337, 0.0949672, 0.0957836, 0.0965798, 
     &0.0973506, 0.0980911, 0.0987955, 0.099457, 0.100069, 0.100626,  
     &0.10112, 0.101549, 0.101906, 0.102188, 0.102393, 0.102519, 
     &0.102566, 0.102533, 0.102423, 0.102238, 0.101979, 0.101652, 
     &0.101261, 0.100809, 0.100302, 0.0997452, 0.0991436, 0.0985024, 
     &0.0978265, 0.0971203, 0.0963888, 0.0956363, 0.094866, 0.0940814, 
     &0.0932857, 0.092481, 0.0916692, 0.090853, 0.0900324, 0.0892086, 
     &0.0883826, 0.087554, 0.0867231, 0.0858899, 0.0850532, 0.0842132, 
     &0.0833692, 0.0825207, 0.0816667, 0.080807, 0.0799407, 0.079068, 
     &0.0781883, 0.0773019, 0.0764082, 0.0755076, 0.0746006, 0.0736877, 
     &0.0727694, 0.0718463, 0.0709193, 0.0699895, 0.0690574, 0.068124, 
     &0.0671912, 0.0662592, 0.065329, 0.0644021, 0.0634791, 0.0625611, 
     &0.061649, 0.060743, 0.0563301, 0.0521382, 0.0481676, 0.0443965, 
     &0.0408061, 0.037396,  0.03418, 0.0311759, 0.0283927, 0.0258257, 
     &0.0234595, 0.0212756, 0.0192593, 0.0174016, 0.0156973, 0.014142, 
     &0.0127284, 0.0114458, 0.0102822, 0.00922554, 0.00826637, 
     &0.00739746, 0.00661279, 0.00590658, 0.00527226, 0.00470284, 
     &0.0041915, 0.00373215, 0.00331982, 0.00295038, 0.0026202, 
     &0.00232572, 0.00206331, 0.00182949, 0.00162103, 0.0014352, 
     &0.00126972, 0.00112262, 0.000992099, 0.000876413, 0.000683056, 
     &0.000531116, 0.000412083, 0.000319294, 0.000247034, 0.000190749, 
     &0.000147057, 0.00011326, 8.71096e-05, 6.6887e-05/
      DATA (FLUX(I,6,2),I=1,151) /1.78396e-06, 0.000175716, 0.000692887, 
     &0.00153366, 0.0026707, 0.00406879, 0.00569523, 0.00752629, 
     &0.00954095, 0.0117094, 0.0139905, 0.0163431, 0.0187403, 0.0211698,  
     &0.02362, 0.0260668, 0.0284768, 0.030823, 0.0330963, 0.035301, 
     &0.0374375, 0.0394932, 0.0414492, 0.043295, 0.0450364, 0.0466863, 
     &0.0482513, 0.0497262, 0.051103, 0.0523826, 0.053576, 0.0546968, 
     &0.0557502, 0.0567343, 0.0576477, 0.0584957, 0.0592887, 0.0600346, 
     &0.0607356, 0.0613902, 0.0620003, 0.0625719, 0.0631111, 0.0636198, 
     &0.0640962, 0.0645394, 0.0649529, 0.0653413, 0.0657063, 0.0660451, 
     &0.0663531, 0.0666306, 0.0668822, 0.0671113, 0.0673159, 0.0674904, 
     &0.0676309, 0.0677389, 0.0678196, 0.0678756, 0.0679033, 0.0678972, 
     &0.0678551, 0.0677799, 0.0676778, 0.0675501, 0.0673939, 0.0672048, 
     &0.066983, 0.066733, 0.0664597, 0.0661647, 0.0658456,   0.0655, 
     &0.0651299, 0.0647396, 0.064333, 0.0639106, 0.0634715, 0.0630148, 
     &0.062543, 0.0620595, 0.0615657, 0.0610623, 0.0605486, 0.0600253, 
     &0.0594937, 0.0589563, 0.0584128, 0.057863, 0.0573069, 0.0567457, 
     &0.0561806, 0.0556118, 0.0550388, 0.0544604, 0.053878, 0.0532926, 
     &0.0527051, 0.0521146, 0.0515202, 0.0485068, 0.0454498, 0.0423943, 
     &0.0393923, 0.036488, 0.0337105, 0.0310715, 0.0285714, 0.0262057, 
     &0.0239722, 0.0218738, 0.0199158, 0.018101, 0.0164264, 0.0148842, 
     &0.0134648, 0.0121592, 0.0109605, 0.00986385, 0.00886488, 
     &0.00795832, 0.00713765, 0.00639526, 0.0057237, 0.00511655, 
     &0.00456864, 0.00407545, 0.00363267, 0.00323588, 0.0028806, 
     &0.00256244, 0.00227744, 0.00202224, 0.00179408, 0.00159053, 
     &0.00140929, 0.00124804, 0.00110459, 0.000976957, 0.000863437, 
     &0.000673092, 0.000523742, 0.000406926, 0.000315524, 0.000244201, 
     &0.000188768, 0.000145714, 0.000112263, 8.63566e-05, 6.6358e-05/
      DATA (FLUX(I,6,3),I=1,151) /8.97104e-09, 8.83026e-06, 6.95537e-05, 
     &0.000231151, 0.000539569, 0.00103781, 0.00176596, 0.00276113, 
     &0.00405724, 0.0056848, 0.00767043, 0.0100363, 0.0127997, 0.015972, 
     &0.019558, 0.0235552, 0.0279528, 0.0327314, 0.0378621, 0.0433069, 
     &0.049018, 0.0549392, 0.0610064, 0.0671499, 0.0732951, 0.0793657, 
     &0.0852866, 0.0909847, 0.0963937, 0.101454, 0.106117, 0.110344, 
     &0.114107, 0.117392, 0.120194, 0.122518, 0.124381, 0.125804, 
     &0.126815, 0.127449, 0.127742, 0.127729,  0.12745, 0.126942,  
     &0.12624, 0.125378, 0.124389, 0.123299, 0.122136, 0.120921, 
     &0.119676, 0.118415, 0.117153, 0.115902, 0.114669, 0.113462, 
     &0.112286, 0.111142, 0.110031, 0.108953, 0.107906, 0.106888, 
     &0.105894, 0.104921, 0.103964, 0.103018, 0.102079, 0.101142, 
     &0.100203, 0.0992566, 0.0983002, 0.0973301, 0.0963441, 0.0953403, 
     &0.094317, 0.0932737, 0.0922101, 0.0911269, 0.0900243, 0.0889043, 
     &0.0877678, 0.0866176, 0.0854555, 0.0842843, 0.0831059, 0.0819233, 
     &0.0807391, 0.0795555, 0.0783748, 0.0771996, 0.0760317, 0.0748732, 
     &0.0737254, 0.0725897, 0.0714675, 0.0703598, 0.0692667, 0.0681891, 
     &0.0671271, 0.0660808, 0.0650502, 0.0601166, 0.0554934, 0.0511039, 
     &0.0469081, 0.0429128, 0.0391519, 0.0356579, 0.0324431, 0.0294951, 
     &0.026786, 0.0242869, 0.0219771, 0.0198474, 0.0178946, 0.0161153, 
     &0.0145015, 0.0130401, 0.0117155, 0.0105126, 0.00942002, 
     &0.00842961, 0.00753512, 0.00673031, 0.00600798, 0.00536004, 
     &0.00477832, 0.00425563, 0.00378616, 0.00336528, 0.00298896, 
     &0.00265334, 0.00235437, 0.00208804, 0.00185062, 0.0016389, 
     &0.00145026, 0.00128246, 0.00113351, 0.00100149, 0.000884528, 
     &0.000688981, 0.000535337, 0.00041517, 0.000321595, 0.000248695, 
     &0.000191937, 0.000147936, 0.000113911, 8.75761e-05, 6.72234e-05/
      save radhor
      PIFAC = ACOS(-ONE)
C     Spectrum
      RADHOR = (RHFACT*BHMASS)**(ONE/(TOTDIM-THREE))
      HWKTMP = (TOTDIM-THREE)/(4.0D0*PIFAC*RADHOR)      
      IF(GRYBDY) THEN 
         SPCMAX=MXARRY(TOTDIM-5,SPIN+1)
      ELSE
         IF (STAT.EQ.-1) THEN
            SPCMAX=HALF*HWKTMP**2
         ELSE
            SPCMAX=(TWO/THREE)*HWKTMP**2
         ENDIF
      ENDIF
      POINTS=NARRAY(TOTDIM-5)
C     MC for energy
 10   ETRAT=CHRUNI(0,ZERO,ETVALS(POINTS,TOTDIM-5))
      ENERGY=HWKTMP*ETRAT
      IF (GRYBDY) THEN 
          SPCVAL=CHUTAB(FLUX(1,TOTDIM-5,SPIN+1),ETVALS(1,TOTDIM-5),
     &        POINTS,ETRAT,4)
      ELSE
         SPCVAL=(ENERGY**2)/(EXP(ETRAT)-STAT)
      ENDIF
      IF (SPCVAL.LT.SPCMAX*CHRGEN(1)) GOTO 10
c      if ( energy*radhor.lt.1.0d0 ) goto 10
      END
CDECK  ID>, CHULB4.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Adapted by Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHULB4(PS,PI,PF)
C-----------------------------------------------------------------------
C     TRANSFORMS PI (GIVEN IN REST FRAME OF PS) INTO PF (IN LAB)
C     N.B. P(1,2,3,4) = (PX,PY,PZ,E); PS(5)=M
C     (taken from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION PF4,FN,PS(5),PI(4),PF(4)
      IF (PS(4).EQ.PS(5)) THEN
        PF(1)= PI(1)
        PF(2)= PI(2)
        PF(3)= PI(3)
        PF(4)= PI(4)
      ELSE
        PF4  = (PI(1)*PS(1)+PI(2)*PS(2)
     &         +PI(3)*PS(3)+PI(4)*PS(4))/PS(5)
        FN   = (PF4+PI(4)) / (PS(4)+PS(5))
        PF(1)= PI(1) + FN*PS(1)
        PF(2)= PI(2) + FN*PS(2)
        PF(3)= PI(3) + FN*PS(3)
        PF(4)= PF4
      END IF
      END
CDECK  ID>, CHULOB.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Adapted by Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHULOB(PS,PI,PF)
C-----------------------------------------------------------------------
C     TRANSFORMS PI (GIVEN IN REST FRAME OF PS) INTO PF (IN LAB)
C     N.B. P(1,2,3,4,5) = (PX,PY,PZ,E,M)
C     (taken from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION PS(5),PI(5),PF(5)
      CALL CHULB4(PS,PI,PF)
      PF(5)= PI(5)
      END
CDECK  ID>, CHUMAS.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHUMAS(P)
C-----------------------------------------------------------------------
C     PUTS INVARIANT MASS IN 5TH COMPONENT OF VECTOR
C     (NEGATIVE SIGN IF SPACELIKE) (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHUSQR,P(5)
      EXTERNAL CHUSQR
      P(5)=CHUSQR((P(4)+P(3))*(P(4)-P(3))-P(1)**2-P(2)**2)
      END
CDECK  ID>, CHUPCM.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      FUNCTION CHUPCM(EM0,EM1,EM2)
C-----------------------------------------------------------------------
C     C.M. MOMENTUM FOR DECAY MASSES EM0 -> EM1 + EM2
C     SET TO -1 BELOW THRESHOLD (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHUPCM,EM0,EM1,EM2,EMS,EMD
      EMS=ABS(EM1+EM2)
      EMD=ABS(EM1-EM2)
      IF (EM0.LT.EMS.OR.EM0.LT.EMD) THEN
        CHUPCM=-1.0D0
      ELSEIF (EM0.EQ.EMS.OR.EM0.EQ.EMD) THEN
        CHUPCM=0.0D0
      ELSE
        CHUPCM=SQRT((EM0+EMD)*(EM0-EMD)*
     &              (EM0+EMS)*(EM0-EMS))*0.5D0/EM0
      ENDIF
      END
CDECK  ID>, CHUROB.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHUROB(R,P,Q)
C-----------------------------------------------------------------------
C     ROTATES VECTORS BY INVERSE OF ROTATION MATRIX R 
C     (taken from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION S1,S2,S3,R(3,3),P(3),Q(3)
      S1=P(1)*R(1,1)+P(2)*R(2,1)+P(3)*R(3,1)
      S2=P(1)*R(1,2)+P(2)*R(2,2)+P(3)*R(3,2)
      S3=P(1)*R(1,3)+P(2)*R(2,3)+P(3)*R(3,3)
      Q(1)=S1
      Q(2)=S2
      Q(3)=S3
      END
CDECK  ID>, CHUROT.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHUROT(P,CP,SP,R)
C-----------------------------------------------------------------------
C     R IS ROTATION MATRIX TO GET FROM VECTOR P TO Z AXIS, FOLLOWED BY
C     A ROTATION BY PSI ABOUT Z AXIS, WHERE CP = COS-PSI, SP = SIN-PSI
C     (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION WN,CP,SP,PTCUT,PP,PT,CT,ST,CF,SF,P(3),R(3,3)
      DATA WN,PTCUT/1.D0,1.D-20/
      PT=P(1)**2+P(2)**2
      PP=P(3)**2+PT
      IF (PT.LE.PP*PTCUT) THEN
         CT=SIGN(WN,P(3))
         ST=0.0D0
         CF=1.0D0
         SF=0.0D0
      ELSE
         PP=SQRT(PP)
         PT=SQRT(PT)
         CT=P(3)/PP
         ST=PT/PP
         CF=P(1)/PT
         SF=P(2)/PT
      END IF
      R(1,1)= CP*CF*CT+SP*SF
      R(1,2)= CP*SF*CT-SP*CF
      R(1,3)=-(CP*ST)
      R(2,1)=-(CP*SF+SP*CF*CT)
      R(2,2)= CP*CF+SP*SF*CT
      R(2,3)=-(SP*ST)
      R(3,1)= CF*ST
      R(3,2)= SF*ST
      R(3,3)= CT
      END
CDECK  ID>, CHUSQR.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      FUNCTION CHUSQR(X)
C-----------------------------------------------------------------------
C     SQUARE ROOT WITH SIGN RETENTION (extracted from HERWIG)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION CHUSQR,X
      CHUSQR=SIGN(SQRT(ABS(X)),X)
      END
CDECK  ID>, CHUTAB.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Adapted by Bryan Webber
C-----------------------------------------------------------------------
      FUNCTION CHUTAB(F,A,NN,X,MM)
C-----------------------------------------------------------------------
C     MODIFIED CERN INTERPOLATION ROUTINE DIVDIF
C-----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER NN,MM,MMAX,N,M,MPLUS,IX,IY,MID,NPTS,IP,I,J,L,ISUB
      DOUBLE PRECISION CHUTAB,SUM,X,F(NN),A(NN),T(20),D(20)
      LOGICAL EXTRA
      DATA MMAX/10/
      N=NN
      M=MIN(MM,MMAX,N-1)
      MPLUS=M+1
      IX=0
      IY=N+1
      IF (A(1).GT.A(N)) GOTO 4
    1 MID=(IX+IY)/2
      IF (X.GE.A(MID)) GOTO 2
      IY=MID
      GOTO 3
    2 IX=MID
    3 IF (IY-IX.GT.1) GOTO 1
      GOTO 7
    4 MID=(IX+IY)/2
      IF (X.LE.A(MID)) GOTO 5
      IY=MID
      GOTO 6
    5 IX=MID
    6 IF (IY-IX.GT.1) GOTO 4
    7 NPTS=M+2-MOD(M,2)
      IP=0
      L=0
      GOTO 9
    8 L=-L
      IF (L.GE.0) L=L+1
    9 ISUB=IX+L
      IF ((1.LE.ISUB).AND.(ISUB.LE.N)) GOTO 10
      NPTS=MPLUS
      GOTO 11
   10 IP=IP+1
      T(IP)=A(ISUB)
      D(IP)=F(ISUB)
   11 IF (IP.LT.NPTS) GOTO 8
      EXTRA=NPTS.NE.MPLUS
      DO 14 L=1,M
      IF (.NOT.EXTRA) GOTO 12
      ISUB=MPLUS-L
      D(M+2)=(D(M+2)-D(M))/(T(M+2)-T(ISUB))
   12 I=MPLUS
      DO 13 J=L,M
      ISUB=I-L
      D(I)=(D(I)-D(I-1))/(T(I)-T(ISUB))
      I=I-1
   13 CONTINUE
   14 CONTINUE
      SUM=D(MPLUS)
      IF (EXTRA) SUM=0.5D0*(SUM+D(M+2))
      J=M
      DO 15 L=1,M
      SUM=D(J)+(X-T(J))*SUM
      J=J-1
   15 CONTINUE
      CHUTAB=SUM
      END
CDECK  ID>, CHVDIF.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHVDIF(N,P,Q,R)
C-----------------------------------------------------------------------
C     VECTOR DIFFERENCE (extracted from HERWIG)
C-----------------------------------------------------------------------
      DOUBLE PRECISION P(N),Q(N),R(N)
      INTEGER N,I
      DO 10 I=1,N
   10 R(I)=P(I)-Q(I)
      END
CDECK  ID>, CHVEQU.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHVEQU(N,P,Q)
C-----------------------------------------------------------------------
C     VECTOR EQUALITY (extracted from HERWIG)
C-----------------------------------------------------------------------
      DOUBLE PRECISION P(N),Q(N)
      INTEGER N,I
      DO 10 I=1,N
   10 Q(I)=P(I)
      END
CDECK  ID>, CHVSUM.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE CHVSUM(N,P,Q,R)
C-----------------------------------------------------------------------
C    VECTOR SUM (extracted from HERWIG)
C-----------------------------------------------------------------------
      DOUBLE PRECISION P(N),Q(N),R(N)
      DO 10 I=1,N
   10 R(I)=P(I)+Q(I)
      END
