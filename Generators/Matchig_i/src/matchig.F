***********************************
      FUNCTION MATCHIG(DCPARM, WGT)
***********************************
C...All real arithmetic in double precision.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C...Three Pythia functions return integers, so need declaring.
C...mf: Not needed anymore... commenting out
C      INTEGER PYK,PYCHGE,PYCOMP
C...EXTERNAL statement links PYDATA on most machines.
      EXTERNAL PYDATA
C...Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT3/XSFX(2,-40:40),ISIG(1000,3),SIGH(1000)
      COMMON/PYINT4/MWID(500),WIDS(500,5)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYINT9/VXPVMD(-6:6),VXPANL(-6:6),VXPANH(-6:6),VXPDGM(-6:6)
      COMMON/PYMSSM/IMSS(0:99),RMSS(0:99)
C...HEPEVT commonblock.
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &   JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
C...User process initialization commonblock.
#include "GeneratorFortranCommon/heprup.inc"
C...User process event common block.
#include "GeneratorFortranCommon/hepeup.inc"

      DOUBLE PRECISION MATCHIG
      DIMENSION DCPARM(*)

C   PARTON LISTS
      DIMENSION XPQ(-25:25,2),X(2)
      DIMENSION WDTP(0:400),WDTE(0:400,0:5)
      DIMENSION PHI(2), BETA(3)

C      WRITE(6,*) 'MATCHIG_I: In matchig.F'

C   ASSUME CENTER OF MASS-SYSTEM FOR COLLIDING PARTICLES
      S=(2*EBMUP(1))**2

C...Make BW shape for top with width from PMAS(6,2)
      PMTOP=PMAS(6,1)
      GTOP=PMAS(6,2)
      PMTOPMIN=0d0
      ALOW=ATAN((PMTOPMIN**2-PMTOP**2)/(PMTOP*GTOP))
      PMTOPMAX=SQRT(S)
      AHIGH=ATAN((PMTOPMAX**2-PMTOP**2)/(PMTOP*GTOP))
      SQM3=PMTOP**2+PMTOP*GTOP*TAN((AHIGH-ALOW)*DCPARM(9)+ALOW)
      PQM3=SQRT(SQM3)
C...Make BW shape for Higgs with width from PMAS(37,2)
      PMH=PMAS(37,1)
      GH=PMAS(37,2)
c   Cut BW wings at M_H +- Gamma_H+_MAX
      GHMX=GH*PARP(48)
      PMHMIN=MAX(0d0,PMH-GHMX)
      ALOW=ATAN((PMHMIN**2-PMH**2)/(PMH*GH))
      PMHMAX=MIN(PMH+GHMX,SQRT(S)-PQM3)
      IF(PMHMAX.LT.PMHMIN) THEN
c     TAUmin > 1
        MATCHIG=0
        RETURN
      ENDIF
      AHIGH=ATAN((PMHMAX**2-PMH**2)/(PMH*GH))
      SQM4=PMH**2+PMH*GH*TAN((AHIGH-ALOW)*DCPARM(10)+ALOW)
      PQM4=SQRT(SQM4)

      VINT(80)=1.

      XMSUM = PQM3+PQM4
      XMSUM2 = XMSUM**2
      TAUMIN = XMSUM2/S
      IF(TAUMIN.GT.1d0) THEN
c     This should never happen
        WRITE(*,*) 'WARNING! TAUMIN = ',TAUMIN
        MATCHIG=0
        RETURN
      ENDIF

C   TAU FROM 1/TAU**(NTAU+1)
      NTAU=1
      TAU=TAUMIN/(1+DCPARM(1)*(TAUMIN**NTAU-1))**(1./NTAU)
      SH = TAU*S

      COMFAC = PARU(1)*PARU(5)/SH
      COMFAC = COMFAC/NTAU*(1/TAUMIN**NTAU-1)*TAU**NTAU
      YSTMIN = 0.5D0*LOG(TAU)
      YSTMAX = -YSTMIN

C   YST FROM YST-YSTMIN OR YSTMAX-YST
      IF(DCPARM(2).LE.0.5) THEN
        YST=YSTMIN*(1-SQRT(2*DCPARM(2)))
        COMFAC = COMFAC*YSTMIN**2/(YST-YSTMIN)
      ELSE
        YST=YSTMAX*(1-SQRT(2*(1-DCPARM(2))))
        COMFAC = COMFAC*YSTMIN**2/(YSTMAX-YST)
      END IF

      X(1) = SQRT(TAU)*EXP(YST)
      X(2) = SQRT(TAU)*EXP(-YST)
      IF(X(1).LT.TAU.OR.X(2).LT.TAU.OR.X(1).GT.1D0.OR.X(2).GT.1D0)
     &   WRITE(*,*),'WARNING! ERROR IN X1,X2 GENERATION'

C...Set alpha_em and alpha_s as for 2->3 process
      Q2=SQM4
      IF(MSTP(39).EQ.8) Q2=PARP(194)
      AQEDUP=PYALEM(Q2)
      AQCDUP=PYALPS(PARP(34)*Q2)

      AEM = AQEDUP
      AS = AQCDUP
      XW = PARU(102)

      Q2RM=SQM4
      IF(MSTP(32).EQ.12) Q2RM=PARP(194)
      SQML=PYMRUN(5,Q2RM)**2
      SQMQ=PYMRUN(6,Q2RM)**2
      SQMW=PMAS(24,1)**2
      SQMHC=PMAS(37,1)**2

C   Set maximum parton shower scale as for 2->2 process
C   Only for Pythia versions up to (and including) 6.320
c      IF(MSTP(32).EQ.4) THEN
c        Q2PS=SH
c      ELSEIF(MSTP(32).EQ.11) THEN
c        Q2PS=0.25*(SQM3+SQM4+2*PQM3*PQM4)
c      ELSEIF(MSTP(32).EQ.12) THEN
c        Q2PS=PARP(193)
c      ELSE
c        WRITE(*,*) 'PS SCALE ',MSTP(32), ' NOT SUPPORTED'
c        WRITE(*,*) 'PLEASE USE MSTP(32)= 4, 11 or 12'
c        WRITE(*,*) 'EXECUTION STOPPED'
c        STOP
c      ENDIF
c      Q2PS=Q2PS*PARP(67)


      FHCQ=AS*AEM/XW/24D0
C...Mass-dependent H+ width given by PYWIDT
      GMMHC=PMAS(37,1)*PMAS(37,2)
      HBW4=GMMHC/((SQM4-SQMHC)**2+GMMHC**2)
      CALL PYWIDT(37,SQM4,WDTP,WDTE)
      GMMHCC=PQM4*WDTP(0)
      HBW4C=GMMHCC/((SQM4-SQMHC)**2+GMMHCC**2)
      FHCQ=FHCQ*HBW4C/HBW4
C...Similarly for top
      GMMT=PMAS(6,1)*PMAS(6,2)
      HBW3=GMMT/((SQM3-SQMT)**2+GMMT**2)
      CALL PYWIDT(6,SQM3,WDTP,WDTE)
      GMMTC=SQRT(SQM3)*WDTP(0)
      HBW3C=GMMTC/((SQM3-SQMT)**2+GMMTC**2)
      FHCQ=FHCQ*HBW3C/HBW3

      RM3=SQM3/SH
      RM4=SQM4/SH
      BE34=SQRT(MAX(0D0,(1D0-RM3-RM4)**2-4D0*RM3*RM4))

C   COS(THETA-HAT) FROM 1/(X0+COS(THETA-HAT))
      X0=1.1
      CTH=(X0-1)*((X0+1)/(X0-1))**DCPARM(3)-X0
      COMFAC=COMFAC*LOG((X0+1)/(X0-1))*(CTH+X0)*0.5D0*BE34

      UH=(-0.5D0)*SH*((1D0-RM3-RM4)+
     &   BE34*CTH)
c      UH=-0.5D0*SH*((1D0-RM3-RM4)+
c     &   BE34*CTH)

      KCHHC=ISIGN(1,IDUP(4))

C   THE 2->2 MATRIX ELEMENT ITSELF
      FACHCQ=FHCQ*(SQML*PARU(141)**2+SQMQ/PARU(141)**2)/SQMW*
     &   (SH/(SQMQ-UH)+2D0*SQMQ*(SQMHC-UH)/(SQMQ-UH)**2+(SQMQ-UH)/SH-
     &   2D0*SQMQ/(SQMQ-UH)+2D0*(SQMHC-UH)/(SQMQ-UH)*
     &   (SQMHC-SQMQ-SH)/SH)
      SIGHDC=COMFAC*FACHCQ
C   Suppression from closed decay channels
      IF(WIDS(37,(5-KCHHC)/2)*WIDS(6,(5+KCHHC)/2).GT.0d0)
     &   SIGHDC=SIGHDC*WIDS(37,(5-KCHHC)/2)*WIDS(6,(5+KCHHC)/2)
      XQSUM=0
      I=2
      IF(DCPARM(6).LE.0.5) I=1

      SMB=PMAS(5,1)**2

C   SQPTH is the maximum pt for the IS radiation of the 2->2 process
      SQPTH=0.25D0*SH*BE34**2*(1D0-CTH**2)
      if(MINT(35).LT.3)
     $   SQPTH=SQPTH*PARP(67)

C   Z FROM Z
      ZMIN=X(I)
      Q2OPT=MIN(SQRT(SH*SMB),SQPTH)
c      Q2OPT=MIN(SQRT(SH*SMB),Q2PS)
      ZMAX=SH*Q2OPT/((Q2OPT+SMB)*(SH+Q2OPT))
      IF(ZMIN.GE.ZMAX) THEN
        MATCHIG=0
        RETURN
      END IF
      Z=SQRT(ZMIN**2+DCPARM(4)*(ZMAX**2-ZMIN**2))
      BTILDE=0.5*(ZMAX**2-ZMIN**2)/Z
      Y=X(I)/Z

      IF(MINT(35).lt.3)THEN
C     Old, Q2-ordered, showers
C   Q2 FROM 1/(Q2+SMB)
        Q2MIN=0.5*(SH*(1/Z-1)-SMB)-
     &     0.5*SQRT((SH*(1/Z-1)-SMB)**2-4*SH*SMB)
        Q2MAX=0.5*(SH*(1/Z-1)-SMB)+
     &     0.5*SQRT((SH*(1/Z-1)-SMB)**2-4*SH*SMB)
c      Q2MAX=MIN(Q2PS,Q2MAX)     ! For version < 6.321
        ZSMB=SMB
      ELSE
C     New, pT-ordered, showers, ordered in (1-z)*(Q2+SMB)
C   Q2 FROM 1/pT2=1/((1-z)(Q2+SMB))
        Q2MIN=(1-Z)*(0.5*(SH*(1/Z-1)-SMB)-
     &     0.5*SQRT((SH*(1/Z-1)-SMB)**2-4*SH*SMB))
        Q2MAX=(1-Z)*(0.5*(SH*(1/Z-1)-SMB)+
     &     0.5*SQRT((SH*(1/Z-1)-SMB)**2-4*SH*SMB))
        ZSMB=(1-Z)*SMB
      ENDIF
      Q2MAX=MIN(SQPTH,Q2MAX)
      Q2VAL=(Q2MIN+ZSMB)*((Q2MAX+ZSMB)/(Q2MIN+ZSMB))**DCPARM(5)-ZSMB
      BTILDE=BTILDE*LOG((Q2MAX+ZSMB)/(Q2MIN+ZSMB))*(Q2VAL+ZSMB)

      ASRAD=AS
      Q2RAD=Q2SF

      PGB=0.5D0*(Z**2+(1-Z)**2)

C...CALCULATE MOMENTA OF IN- AND OUTGOING PARTONS
C   PX,PY,PZ,E,M OF INGOING GLUONS
      TAUP=X(3-I)*Y
      SHP=TAUP*S
      TAUPR=SQRT(TAUP)

      PUP(1,1)=0D0
      PUP(2,1)=0D0
      PUP(3,1)=TAUPR*EBMUP(1)
      PUP(4,1)=TAUPR*EBMUP(1)
      PUP(5,1)=0D0
      PUP(1,2)=0D0
      PUP(2,2)=0D0
c      PUP(3,2)=-TAUPR*EBMUP(1)
      PUP(3,2)=(-TAUPR)*EBMUP(1)
      PUP(4,2)=TAUPR*EBMUP(1)
      PUP(5,2)=0D0

C   OUTGOING B QUARK
      P2VAL=SMB

      IF(MINT(35).lt.3)THEN
        Q2=Q2VAL
      ELSE
        Q2=Q2VAL/(1-Z)
      ENDIF
      SPTB=Q2-Z*(Q2+P2VAL)*(SH+Q2)/SH
      SMTB=SPTB+P2VAL
      PHI(1)=PARU(2)*DCPARM(7)
      PUP(1,4)=SQRT(SPTB)*COS(PHI(1))
      PUP(2,4)=SQRT(SPTB)*SIN(PHI(1))
      PUP(4,4)=(1-Z)*PUP(4,1)+P2VAL/(4*PUP(4,1))
      PUP(3,4)=(2d0*PUP(4,4)*PUP(4,I)-Q2-P2VAL)/(2d0*PUP(3,I))
      PUP(5,4)=PMAS(5,1)

C   TOP AND HIGGS: NEED BOOSTS AND ROTATIONS; USE P VECTOR
C   101 IS GLUON, 102 IS INCOMING B QUARK
      DO 100 J=1,5
        P(101,J)=PUP(J,3-I)
 100  CONTINUE
      P(102,1)=-PUP(1,4)
      P(102,2)=-PUP(2,4)
      P(102,3)=PUP(3,I)-PUP(3,4)
      P(102,4)=PUP(4,I)-PUP(4,4)
      P(102,5)=0
      DO 110 J=1,3
c        BETA(J)=-(P(101,J)+P(102,J))/(P(101,4)+P(102,4))
        BETA(J)=(-(P(101,J)+P(102,J)))/(P(101,4)+P(102,4))

 110  CONTINUE

      IF(ABS(P(102,4)**2-SPTB-P(102,3)**2+Q2)
     &   .GT.1d-6)
     &   WRITE(*,*)'WARNING! ',
     &   -(P(102,4)**2-SPTB-P(102,3)**2),' !=',Q2

      K(101,1)=1
      K(102,1)=1
C   BOOST TO REST SYSTEM OF GLUON-B QUARK
      CALL PYROBO(101,102,0D0,0D0,BETA(1),BETA(2),BETA(3))

      ECM=P(101,4)+P(102,4)
      P(103,5)=PQM3
      P(104,5)=PQM4
      IF(ECM.LT.P(103,5)+P(104,5)) THEN
        WRITE(*,*)'NOT ENOUGH CM ENERGY TO MAKE H+ AND T'
        MINT(51)=1
        MATCHIG=0
        RETURN
      END IF
      P(103,4)=0.5*(ECM+(P(103,5)**2-P(104,5)**2)/ECM)
      P(103,3)=P(101,3)/ABS(P(101,3))*SQRT(P(103,4)**2-P(103,5)**2)
      P(104,4)=ECM-P(103,4)
      P(104,3)=-P(103,3)
      P(103,1)=0
      P(103,2)=0
      P(104,1)=0
      P(104,2)=0

      PHI(2)=PARU(2)*DCPARM(8)

      K(103,1)=1
      K(104,1)=1

C   ROTATE T-H+ SYSTEM BY THETA,PHI
      CALL PYROBO(103,104,ACOS(-CTH),PHI(2),0D0,0D0,0D0)

C   BOOST BACK TO GLUONIC CM SYSTEM
      CALL PYROBO(101,104,0D0,0D0,-BETA(1),-BETA(2),-BETA(3))

      DO 120 J=1,5
        PUP(J,3)=P(103,J)
        PUP(J,5)=P(104,J)
 120  CONTINUE

      K(105,1)=1

      DO 130 J1=1,5
        DO 140 J2=1,5
          P(100+J1,J2)=PUP(J2,J1)
 140    CONTINUE
 130  CONTINUE

C...Set factorization scale as for 2->3 process
      IF(MSTP(39).EQ.2) THEN
        SPTT=P(103,1)**2+P(103,2)**2
        Q2SF=MAX(SQM3+SPTT,SMTB)
      ELSE IF(MSTP(39).EQ.3) THEN
        Q2SF=SQM4
      ELSE IF(MSTP(39).EQ.5) THEN
        Q2SF=PMAS(37,1)**2
      ELSE IF(MSTP(39).EQ.6) THEN
        Q2SF=0.25*(PQM3+PQM4)**2
      ELSE IF(MSTP(39).EQ.8) THEN
        Q2SF=PARP(193)
      ELSE
        WRITE(*,*) 'FACTORIZATION SCALE ',MSTP(39), ' NOT SUPPORTED'
        WRITE(*,*) 'PLEASE USE MSTP(39)=2, 3, 5, 6 or 8'
        WRITE(*,*) 'EXECUTION STOPPED'
        STOP
      END IF
      SCALUP=SQRT(Q2SF)
      Q2SF=PARP(34)*Q2SF

C   PDFS (CTEQ5L DEFAULT) 2212=PROTON
      CALL PYPDFU(IDBMUP(3-I),X(3-I),Q2SF,XPQ(-25,3-I))
      CALL PYPDFU(IDBMUP(I),Y,Q2RAD,XPQ(-25,I))
      BTILDE=BTILDE*ASRAD/PARU(2)/(Q2VAL+ZSMB)*
     &   PGB*XPQ(0,I)

C   SUM QUARK CONTRIBUTIONS
      XQSUM=XQSUM+XPQ(0,3-I)*BTILDE
      XQSUM=2*XQSUM

C...XSEC IN PB
      MATCHIG = 2*SIGHDC*XQSUM*1D9

C   BOOST TO PROTON-PROTON SYSTEM
      X(I)=Y
      BETA(3)=(X(1)-X(2))/(X(1)+X(2))
      CALL PYROBO(101,105,0D0,0D0,0D0,0D0,BETA(3))

      K(101,1)=0
      K(102,1)=0
      K(103,1)=0
      K(104,1)=0
      K(105,1)=0

      DO 150 J1=1,5
        DO 160 J2=1,5
          PUP(J2,J1)=P(100+J1,J2)
 160    CONTINUE
 150  CONTINUE

      RETURN

      END
