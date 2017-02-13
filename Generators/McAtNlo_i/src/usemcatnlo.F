C----------------------------------------------------------------------
      SUBROUTINE USEMCATNLO
C----------------------------------------------------------------------
C  Reads MC@NLO input files and fills Les Houches event common HEPEUP
C----------------------------------------------------------------------
#include "HERWIG65.INC"

C...=======================================================
C...Common function (05.16.2011, Soshi.Tsuno@cern.ch)
C...random number
      integer imode,itau1,itau2,itau3
      double precision polz0
      double precision pyr
      external pyr

      double precision plzapx
      external plzapx
C...=======================================================

C--Les Houches Common Blocks
#include "GeneratorFortranCommon/hepeup.inc"

CC COMMUNICATE HERWIG STATUS
      INTEGER ISTG
      COMMON /EVTCON/ ISTG

CC If FORMAT 4.0 with x1, x2, qscale is used 
      INTEGER READXXQ
      COMMON/IFXXQ/ READXXQ  

CC << OJ 
      INTEGER NENTRIES
      INTEGER NBACK
CC >> OJ 
C AOH anomalous coupling reweighting
c      DOUBLE PRECISION WGTACP(10)
      DOUBLE PRECISION WGTACP(28)
      INTEGER USERWGTS
      COMMON/CWGTACP/ WGTACP, USERWGTS

      DOUBLE PRECISION XMP2,XMA2,XMB2,BETA,VA,VB,SIGMA,DELTA,S2,
     & XKA,XKB,PTF,E,PL,XSCALE,
C gkh
     & X1, X2, QSCALE
      DOUBLE PRECISION PCM(5),PTR,XMTR,HWVDOT,HWULDO,PDB(5)
      INTEGER I,J,IC,JPR,MQQ,NQQ,IUNIT,ISCALE,I1HPRO,IBOS,NP,IG,IP,
     & ILEP,ID,IA,IB,ICOL4(4,4),ICOL5(5,18),ICOST(4,20),JJPROC,IVHVEC,
     & IVHLEP,MUP
      PARAMETER (IUNIT=61)
      LOGICAL BOPRO,NODEC,REMIT
      COMMON/NQQCOM/MQQ,NQQ
      COMMON/VHLIN/IVHVEC,IVHLEP
C---Colour flows for heavy quark pair production
      DATA ICOL4/
     & 10,02,10,02,01,20,20,01,12,23,10,03,12,31,30,02/
      DATA ICOL5/
     & 10,02,13,30,02, 10,02,32,10,03,
     & 10,21,30,20,03, 10,23,20,10,03,
     & 01,20,23,30,01, 01,20,31,20,03,
     & 01,23,03,20,01, 01,12,03,30,02,
     & 12,20,30,10,03, 12,30,10,30,02,
     & 12,03,02,10,03, 12,01,03,30,02,
     & 12,23,14,40,03, 12,34,32,10,04,
     & 12,23,43,10,04, 12,31,34,40,02,
     & 12,34,14,30,02, 12,31,42,30,04/
C---Colour flows for single top production
      DATA ICOST/
     & 20,12,00,10, 12,20,00,10, 10,02,02,10, 02,10,02,10,
     & 20,10,20,10, 20,02,01,10, 10,20,20,10, 02,20,01,10,
     & 20,02,01,10, 10,02,02,10, 02,20,01,10, 02,10,02,10,
     & 20,10,20,10, 10,20,20,10, 12,23,03,10, 23,12,03,10,
     & 30,12,32,10, 30,23,21,10, 12,30,32,10, 23,30,21,10/
      IF (IERROR.NE.0) RETURN
C---READ AN EVENT
C      IF(NQQ.GE.MQQ)CALL HWWARN('UPEVNT',201,*999)
      IF(NQQ.GE.MQQ) THEN
         ISTG=1
         RETURN
      ELSE
         ISTG=0
      ENDIF
      READ(IUNIT,901) I1HPRO,IC,NP
c      write(*,*)'i1hpro ',I1HPRO,IC,NP 
      READ(IUNIT,902) (IDUP(I),I=1,NP)
      READ(IUNIT,903) XWGTUP,XSCALE
C---Les Houches expects mean weight to be the cross section in pb
      XWGTUP= XWGTUP*MQQ
      READ(IUNIT,904) ((PUP(J,I),J=1,4),I=1,NP)
      NQQ=NQQ+1
C---Input format is now (px,py,pz,m)
      DO I=1,NP
         E=SQRT(HWVDOT(4,PUP(1,I),PUP(1,I)))
         PUP(5,I)=PUP(4,I)
         PUP(4,I)=E
      ENDDO
C-- gkh
      IF(READXXQ.GT.0) THEN
         READ(IUNIT,905) X1, X2, QSCALE
c         write(*,*) x1,x2,qscale
      ENDIF
c      write(*,*) 'readxxq',readxxq
      IF(READXXQ.EQ.2) THEN
         READ(IUNIT,906) (WGTACP(I),I=1,10)      
         USERWGTS=10
      ELSEIF (READXXQ.EQ.3) THEN
c         READ(IUNIT,906) (WGTACP(I),I=1,28)      
         READ(IUNIT,907) (WGTACP(I),I=1,28)     
         USERWGTS=28
      ELSE
         USERWGTS=0
      ENDIF 
c      if (userwgts .ne. 0 ) then 
c         do i=1,28
c            write(*,*) 'McAtNlo, wgtacp:', wgtacp(i)
c         enddo 
c      endif 
      CALL HWVSUM(4,PUP(1,1),PUP(1,2),PCM)
      CALL HWUMAS(PCM)
C---REMIT MEANS A REAL PARTON EMISSION OCCURRED
      REMIT=PUP(4,3).NE.ZERO
C---NODEC MEANS DECAYS NOT YET DONE
      NODEC=NP.EQ.5
      NUP=NP
C---CHECK PROCESS CODE
      JJPROC=MOD(ABS(IPROC),10000)
      JPR=JJPROC/100
      BOPRO=JPR.EQ.13.OR.JPR.EQ.14.OR.JPR.EQ.16.OR.JPR.EQ.36
      IF (BOPRO) THEN
C----------------------------------------------------------------------
C   SINGLE GAUGE OR HIGGS BOSON PRODUCTION
C   B = Z/gamma, W or H (SM or any MSSM neutral Higgs)
C-----------------------------------------------------------------------
C I1HPRO IDENTIFIES THE PARTONIC SUBPROCESS, WITH THE FOLLOWING CONVENTIONS:
C   I1HPRO         PROCESS
C    401        q qbar -> CLHEP::g B
C    402        q CLHEP::g    -> q B
C    403        qbar q -> CLHEP::g B
C    404        qbar CLHEP::g -> qbar B
C    405        CLHEP::g q    -> q B
C    406        CLHEP::g qbar -> qbar B
C    407        CLHEP::g CLHEP::g    -> CLHEP::g B
C-----------------------------------------------------------------------
C---NODEC=.TRUE. FOR HIGGS AND UNDECAYED EW BOSON
         NODEC=NP.EQ.4
         IHPRO=I1HPRO-400
         ISCALE=0
         IF(JPR.EQ.16)ISCALE=2
      ELSEIF (JPR.EQ.17.OR.JPR.EQ.20) THEN
C----------------------------------------------------------------------
C   HEAVY Q and/or QBAR PRODUCTION
C   IPROC=-1705,-1706 for Q=b,t
C   IPROC=-2000,-2030/1/4 for single top, Wt+Wtb/Wtb/Wt
C-----------------------------------------------------------------------
C I1HPRO IDENTIFIES THE PARTONIC SUBPROCESS, WITH THE FOLLOWING CONVENTIONS:
C   I1HPRO         PROCESS
C    401        q qbar -> CLHEP::g Q Qbar
C    402        q CLHEP::g    -> q Q Qbar
C    403        qbar q -> CLHEP::g Q Qbar
C    404        qbar CLHEP::g -> qbar Q Qbar
C    405        CLHEP::g q    -> q Q Qbar
C    406        CLHEP::g qbar -> qbar Q Qbar
C    407        CLHEP::g CLHEP::g    -> CLHEP::g Q Qbar
C    408        q q    -> CLHEP::g t q
C    409        qbar qbar -> CLHEP::g tbar qbar
C-----------------------------------------------------------------------
C IC SPECIFIES THE COLOUR CONNECTION (NOW IN INPUT FILE)
C-----------------------------------------------------------------------
C---SET IHPRO AS FOR DIRECT PHOTON (IPROC=1800)
         IHPRO=I1HPRO-360
         ISCALE=0
         IF(ABS(IPROC).EQ.1705.OR.ABS(IPROC).EQ.11705)ISCALE=5
      ELSEIF (JPR.EQ.28) THEN
C----------------------------------------------------------------------
C   GAUGE BOSON PAIR PRODUCTION
C   VV=WW,ZZ,ZW+,ZW- FOR IPROC=-2850,-2860,-2870,-2880
C-----------------------------------------------------------------------
C I1HPRO IDENTIFIES THE PARTONIC SUBPROCESS, WITH THE FOLLOWING CONVENTIONS:
C   I1HPRO         PROCESS
C    401        q qbar -> CLHEP::g V V
C    402        q CLHEP::g    -> q V V
C    403        qbar q -> CLHEP::g V V
C    404        qbar CLHEP::g -> qbar V V
C    405        CLHEP::g q    -> q V V
C    406        CLHEP::g qbar -> qbar V V
C-----------------------------------------------------------------------
         IHPRO=I1HPRO-400
         ISCALE=0
      ELSEIF (JPR.EQ.26.OR.JPR.EQ.27) THEN
C----------------------------------------------------------------------
C   GAUGE BOSON PLUS HIGGS PRODUCTION
C   VH=WH,ZH FOR IPROC=-2600-ID,-2700-ID
C   WHERE ID CONTROLS HIGGS DECAY AS IN STANDARD HERWIG
C-----------------------------------------------------------------------
         IHPRO=I1HPRO-400
         ISCALE=0
      ELSEIF (JPR.EQ.1) THEN
C----------------------------------------------------------------------
C   E+E- ANNIHILATION
C----------------------------------------------------------------------
         ISCALE=1
      ELSE
         CALL HWWARN('UPEVNT',202)
         GOTO 999
      ENDIF
C---HARD SCALE
      SCALUP=PCM(5)
      IF (XSCALE.GT.0D0.AND.XSCALE.LT.PCM(5)) SCALUP=XSCALE
      IF (REMIT) THEN
         IF (ISCALE.EQ.0) THEN
            PTR=SQRT(PUP(1,3)**2+PUP(2,3)**2)
            SCALUP=PCM(5)-2.*PTR
         ELSEIF(ISCALE.EQ.1)THEN
            SCALUP=PCM(5)
         ELSEIF (ISCALE.EQ.2) THEN
            SCALUP=SQRT(PUP(1,3)**2+PUP(2,3)**2)
         ELSEIF (ISCALE.EQ.3.OR.ISCALE.EQ.4.OR.ISCALE.EQ.5) THEN
            PTR=SQRT(PUP(1,3)**2+PUP(2,3)**2)
            IA=4
            IB=5
            XMP2=PUP(5,3)**2
            XMA2=PUP(5,IA)**2
            XMB2=PUP(5,IB)**2
            S2=XMA2+XMB2+2*HWULDO(PUP(1,IA),PUP(1,IB))
            SIGMA=XMA2+XMB2
            DELTA=XMA2-XMB2
            BETA=SQRT(1-2*SIGMA/S2+(DELTA/S2)**2)
            VA=BETA/(1+DELTA/S2)
            VB=BETA/(1-DELTA/S2)
            XKA=HWULDO(PUP(1,3),PUP(1,IA))
            XKB=HWULDO(PUP(1,3),PUP(1,IB))
            E=(XKA+XKB)/SQRT(S2)
            PL=-(2.0/((VA+VB)*BETA*SQRT(S2))*(VA*XKA-VB*XKB))
            PTF=E**2-PL**2-XMP2
            IF (PTF.LE.ZERO) THEN
               CALL HWWARN('UPEVNT',103)
               GOTO 999
            ENDIF
            PTF=SQRT(PTF)
            IF(ISCALE.EQ.3)THEN
              SCALUP=PCM(5)-2.*MIN(PTR,PTF)
            ELSEIF(ISCALE.EQ.4)THEN
              SCALUP=MIN(PTR,PTF)
            ELSE
              SCALUP=(MIN(PTR,PTF))**2+(XMA2+XMB2)/2.D0
              SCALUP=SQRT(SCALUP)
            ENDIF
            IF (SCALUP.LE.ZERO) THEN
               CALL HWWARN('UPEVNT',100)
               GOTO 999
            ENDIF            
         ELSEIF (ISCALE.EQ.6) THEN
            XMTR=SQRT(PUP(5,4)**2+PUP(1,4)**2+PUP(2,4)**2)
            PTR=SQRT(PUP(1,3)**2+PUP(2,3)**2)
            SCALUP=PCM(5)-PTR-XMTR
            IF (SCALUP.LE.ZERO) THEN
               CALL HWWARN('UPEVNT',100)
               GOTO 999
            ENDIF
         ELSEIF (ISCALE.EQ.7) THEN
            SCALUP=SQRT(PUP(5,4)**2+PUP(1,4)**2+PUP(2,4)**2)
         ELSE
            CALL HWWARN('UPEVNT',501)
            GOTO 999
         ENDIF
      ELSE
         NUP=NUP-1
      ENDIF
C---INITIAL STATE
      DO I=1,2
         ISTUP(I)=-1
         MOTHUP(1,I)=0
         MOTHUP(2,I)=0
      ENDDO
C---FINAL STATE
      DO I=3,NUP
         ISTUP(I)=1
         MOTHUP(1,I)=1
         MOTHUP(2,I)=2
      ENDDO
      IF (BOPRO.AND.NODEC) THEN
C---SINGLE BOSON (UNDECAYED)
         IF (REMIT) THEN
C---SET COLOUR CONNECTIONS
            DO I=1,3
               ICOLUP(1,I)=501
               ICOLUP(2,I)=502
            ENDDO
            IF (IHPRO.EQ.1) THEN
               ICOLUP(2,1)=0
               ICOLUP(1,2)=0
            ELSEIF (IHPRO.EQ.2) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=0
               ICOLUP(2,3)=0
            ELSEIF (IHPRO.EQ.3) THEN
               ICOLUP(1,1)=0
               ICOLUP(2,2)=0
            ELSEIF (IHPRO.EQ.4) THEN
               ICOLUP(1,1)=0
               ICOLUP(2,1)=501
               ICOLUP(1,3)=0
            ELSEIF (IHPRO.EQ.5) THEN
               ICOLUP(1,2)=502
               ICOLUP(2,2)=0
               ICOLUP(2,3)=0
            ELSEIF (IHPRO.EQ.6) THEN
               ICOLUP(1,2)=0
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
            ELSEIF (IHPRO.EQ.7) THEN
               ICOLUP(1,2)=502
               ICOLUP(2,2)=503
               ICOLUP(2,3)=503
            ELSE
               CALL HWWARN('UPEVNT',101)
               GOTO 999
            ENDIF
         ELSE
            CALL HWVEQU(5,PUP(1,4),PUP(1,3))
C---SET COLOUR CONNECTIONS
            DO I=1,2
               ICOLUP(1,I)=0
               ICOLUP(2,I)=0
            ENDDO
            IF (IDUP(1).GT.0) THEN
               ICOLUP(1,1)=501
               ICOLUP(2,2)=501
               IF (IDUP(1).GT.0) THEN
C---GG FUSION
                  ICOLUP(2,1)=502
                  ICOLUP(1,2)=502
               ENDIF
            ELSE
C---QBAR Q
               ICOLUP(2,1)=501
               ICOLUP(1,2)=501
            ENDIF
         ENDIF
         ICOLUP(1,NUP)=0
         ICOLUP(2,NUP)=0
C---LOAD BOSON ID
         IF (JPR.EQ.13) THEN
            IDUP(NUP)=23
         ELSEIF (JPR.EQ.16) THEN
            IDUP(NUP)=25
         ELSEIF (JPR.EQ.36) THEN
            IBOS=MOD(JJPROC,100)
            IF (IBOS.EQ.10) THEN
               IDUP(NUP)=26
            ELSEIF (IBOS.EQ.20) THEN
               IDUP(NUP)=35
            ELSEIF (IBOS.EQ.30) THEN
               IDUP(NUP)=36 
            ELSE
               CALL HWWARN('UPEVNT',502)
               GOTO 999
            ENDIF
         ELSEIF (JPR.EQ.14) THEN
            IBOS=0
            DO I=1,NUP-1
               ID=IDUP(I)
               IF (ID.EQ.21) THEN
                  IC=0
               ELSEIF (ID.GT.0) THEN
                  IC=ICHRG(ID)
               ELSE
                  IC=ICHRG(6-ID)
               ENDIF
               IBOS=IBOS+IC
            ENDDO
            IF (REMIT) IBOS=IBOS-2*IC
            IF (ABS(IBOS).NE.3) THEN
               CALL  HWWARN('UPEVNT',503)
               GOTO 999
            ENDIF
            IDUP(NUP)=8*IBOS
         ENDIF
      ELSEIF (JPR.EQ.17) THEN
C---HEAVY QUARKS
         IF (REMIT) THEN
C---3-BODY FINAL STATE
C---SET COLOUR CONNECTIONS
            IF (IC.LE.18) THEN
               DO I=1,5
                  CALL UPCODE(ICOL5(I,IC),ICOLUP(1,I))
               ENDDO
            ELSE
               CALL HWWARN('UPEVNT',105)
               GOTO 999
            ENDIF
         ELSE
C---2-BODY FINAL STATE
            DO IP=3,NUP
               IDUP(IP)=IDUP(IP+1)
               CALL HWVEQU(5,PUP(1,IP+1),PUP(1,IP))
            ENDDO
C---SET COLOUR CONNECTIONS
            IF (IC.LE.4) THEN
               DO I=1,4
                  CALL UPCODE(ICOL4(I,IC),ICOLUP(1,I))
               ENDDO
            ELSE
               CALL HWWARN('UPEVNT',104)
               GOTO 999
            ENDIF
         ENDIF
         IF (.NOT.NODEC) THEN
C---T-TBAR WITH DECAYS
C   LES HOUCHES COMMON IS FILLED AS FOLLOWS:
C
C  1 INCOMING PARTON
C  2 INCOMING PARTON
C  3 OUTGOING LIGHT PARTON (IF ANY, OTHERWISE 4-13 BECOME 3-12)
C  4 TOP
C  5 ANTITOP
C  6 QUARK FROM TOP
C  7 W+ FROM TOP
C  8 ANTIQUARK FROM ANTITOP
C  9 W- FROM ANTITOP
C 10 DECAY PRODUCT FROM W+ FROM TOP
C 11 DECAY PRODUCT FROM W+ FROM TOP
C 12 DECAY PRODUCT FROM W- FROM ANTITOP
C 13 DECAY PRODUCT FROM W- FROM ANTITOP
C
C---RECONSTRUCT TOP DECAYS
            IF (MOD(JJPROC,10).NE.6) THEN
               CALL HWWARN('UPEVNT',210)
               GOTO 999
            ENDIF
            NP=NUP
C--W DECAYS
            IDUP(NP+1)=IDUP(NP-5)
            IDUP(NP+2)=IDUP(NP-4)
            IDUP(NP+3)=IDUP(NP-2)
            IDUP(NP+4)=IDUP(NP-1)
            IDUP(NP-1)=IDUP(NP)
            CALL HWVEQU(5,PUP(1,NP-5),PUP(1,NP+1))
            CALL HWVEQU(5,PUP(1,NP-4),PUP(1,NP+2))
            CALL HWVEQU(5,PUP(1,NP-2),PUP(1,NP+3))
            CALL HWVEQU(5,PUP(1,NP-1),PUP(1,NP+4))
            CALL HWVEQU(5,PUP(1,NP  ),PUP(1,NP-1))
            CALL HWVSUM(4,PUP(1,NP+1),PUP(1,NP+2),PUP(1,NP-2))
            CALL HWVSUM(4,PUP(1,NP+3),PUP(1,NP+4),PUP(1,NP))
            CALL HWUMAS(PUP(1,NP-2))
            CALL HWUMAS(PUP(1,NP))
            IDUP(NP-2)=-(24*(MOD(IDUP(NP+1),2)+MOD(IDUP(NP+2),2)))
            IDUP(NP)=-IDUP(NP-2)
            DO IP=NP-3,NP+4
               ISTUP(IP)=1
            ENDDO
            ISTUP(NP-2)=2
            ISTUP(NP)=2
C--TOP DECAYS
            CALL HWVSUM(4,PUP(1,NP-3),PUP(1,NP-2),PUP(1,NP-5))
            CALL HWVSUM(4,PUP(1,NP-1),PUP(1,NP  ),PUP(1,NP-4))
            CALL HWUMAS(PUP(1,NP-5))
            CALL HWUMAS(PUP(1,NP-4))
            IDUP(NP-5)=IDUP(NP-2)/4
            IDUP(NP-4)=-IDUP(NP-5)
            ISTUP(NP-5)=2
            ISTUP(NP-4)=2
            MOTHUP(1,NP-5)=1
            MOTHUP(2,NP-5)=2
            MOTHUP(1,NP-4)=1
            MOTHUP(2,NP-4)=2
            MOTHUP(1,NP-3)=NP-5
            MOTHUP(1,NP-2)=NP-5
            MOTHUP(1,NP-1)=NP-4
            MOTHUP(1,NP  )=NP-4
            MOTHUP(1,NP+1)=NP-2
            MOTHUP(1,NP+2)=NP-2
            MOTHUP(1,NP+3)=NP
            MOTHUP(1,NP+4)=NP
            DO IP=NP-3,NP+4
               MOTHUP(2,IP)=MOTHUP(1,IP)
               ICOLUP(1,IP)=0
               ICOLUP(2,IP)=0
            ENDDO
            ICOLUP(1,NP-3)=ICOLUP(1,NP-5)
            ICOLUP(2,NP-3)=ICOLUP(2,NP-5)
            ICOLUP(1,NP-1)=ICOLUP(1,NP-4)
            ICOLUP(2,NP-1)=ICOLUP(2,NP-4)
            IF (IDUP(NP+1).GT.0) THEN
               ICOLUP(1,NP+1)=505
               ICOLUP(2,NP+2)=505
            ELSE
               ICOLUP(2,NP+1)=505
               ICOLUP(1,NP+2)=505
            ENDIF
            IF (IDUP(NP+3).GT.0) THEN
               ICOLUP(1,NP+3)=506
               ICOLUP(2,NP+4)=506
            ELSE
               ICOLUP(2,NP+3)=506
               ICOLUP(1,NP+4)=506
            ENDIF
            NUP=NP+4
         ENDIF
C-- gkh      ELSEIF (JPR.EQ.20) THEN
      ELSEIF (JPR.EQ.20 .AND. JJPROC.NE.2040.AND.JJPROC.NE.2041
     &        .AND.JJPROC.NE.2044) THEN
         IF (JJPROC.GE.2030) THEN
C---TOP+W FINAL STATE
C---SET COLOUR CONNECTIONS
            IF (IC.GT.20) THEN
               CALL HWWARN('UPEVNT',111)
               GOTO 999
            ENDIF
            DO I=1,4
               CALL UPCODE(ICOST(I,IC),ICOLUP(1,I))
            ENDDO
            ICOLUP(1,5)=0
            ICOLUP(2,5)=0
            IF (.NOT.REMIT) THEN
C---2-BODY FINAL STATE
               DO IP=3,NUP
                  IDUP(IP)=IDUP(IP+1)
                  ICOLUP(1,IP)=ICOLUP(1,IP+1)
                  ICOLUP(2,IP)=ICOLUP(2,IP+1)
                  CALL HWVEQU(5,PUP(1,IP+1),PUP(1,IP))
               ENDDO
            ENDIF
            IF (NODEC) THEN
               IP=NUP-1
            ELSE
               IP=NUP-2
C---T W (OR TBAR W) WITH DECAYS
C   LES HOUCHES COMMON IS FILLED AS FOLLOWS:
C
C  1 INCOMING PARTON
C  2 INCOMING PARTON
C  3 OUTGOING LIGHT PARTON (IF ANY, OTHERWISE 4-11 BECOME 3-10)
C  4 TOP
C  5 W FROM TOP
C  6 QUARK FROM TOP
C  7 DECAY PRODUCT FROM W FROM TOP
C  8 DECAY PRODUCT FROM W FROM TOP
C  9 W NOT FROM TOP
C 10 DECAY PRODUCT FROM W NOT FROM TOP
C 11 DECAY PRODUCT FROM W NOT FROM TOP
C
C---RECONSTRUCT W DECAY
               NP=NUP
               IDUP(NP+1)=-(24*(MOD(IDUP(NP-1),2)+MOD(IDUP(NP),2)))
               IDUP(NP+2)=IDUP(NP-1)
               IDUP(NP+3)=IDUP(NP)
               CALL HWVEQU(5,PUP(1,NP-1),PUP(1,NP+2))
               CALL HWVEQU(5,PUP(1,NP),PUP(1,NP+3))
               CALL HWVSUM(4,PUP(1,NP-1),PUP(1,NP),PUP(1,NP+1))
               CALL HWUMAS(PUP(1,NP+1))
               ISTUP(NP+1)=2
               ISTUP(NP+2)=1
               ISTUP(NP+3)=1
               MOTHUP(1,NP+1)=1
               MOTHUP(2,NP+1)=2
               MOTHUP(1,NP+2)=NP+1
               MOTHUP(2,NP+2)=NP+1
               MOTHUP(1,NP+3)=NP+1
               MOTHUP(2,NP+3)=NP+1
               ICOLUP(1,NP+1)=0
               ICOLUP(2,NP+1)=0
               ICOLUP(1,NP+2)=505
               ICOLUP(2,NP+2)=0
               ICOLUP(1,NP+3)=0
               ICOLUP(2,NP+3)=505
               NUP=NP+3
            ENDIF                  
            IF (IDUP(IP).LT.0) THEN
C---CHARGE CONJUGATE PROCESS
               DO I=1,NUP
                  IA=ICOLUP(1,I)
                  ICOLUP(1,I)=ICOLUP(2,I)
                  ICOLUP(2,I)=IA
               ENDDO
            ENDIF
         ELSE
C---SINGLE TOP: IA,IB ARE THE QUARKS THAT ARE COLOUR CONNECTED
C   I.E. (FOR H EVENTS) THOSE THAT ARE NOT CONNECTED TO GLUON
            IA=IC/10
            IB=IC-10*IA
            IF (IA.LT.1.OR.IA.GT.5) THEN
               CALL HWWARN('UPEVNT',108)
            ELSEIF (IB.LT.1.OR.IB.GT.5) THEN
               CALL HWWARN('UPEVNT',109)
            ELSEIF (IA.EQ.IB) THEN
               CALL HWWARN('UPEVNT',110)
            ENDIF
            IF (IERROR.NE.0) GOTO 999
            IF (.NOT.NODEC) IDUP(5)=IDUP(5)+IDUP(6)
            DO I=1,5
               IF (I.EQ.IA.OR.I.EQ.IB) THEN
                  IF (IDUP(I).GT.0) THEN
                     ICOLUP(1,I)=501
                     ICOLUP(2,I)=0
                  ELSE
                     ICOLUP(1,I)=0
                     ICOLUP(2,I)=501
                  ENDIF
               ELSEIF (IDUP(I).EQ.21) THEN
                  IG=I
                  ICOLUP(1,I)=502
                  ICOLUP(2,I)=503
               ELSEIF (IDUP(I).GT.0) THEN
                  ICOLUP(1,I)=502
                  ICOLUP(2,I)=0
               ELSE
                  ICOLUP(1,I)=0
                  ICOLUP(2,I)=502
               ENDIF
            ENDDO
            IF (.NOT.NODEC) IDUP(5)=IDUP(5)-IDUP(6)
            IF (REMIT) THEN
C---3-BODY FINAL STATE
C---COMPLETE GLUON COLOUR CONNECTIONS
               IF (.NOT.NODEC) IDUP(5)=IDUP(5)+IDUP(6)
               DO I=1,5
                  IF (I.NE.IA.AND.I.NE.IB.AND.I.NE.IG) THEN
                     IF (IDUP(I).GT.0) THEN
                        IF((I.LT.3.AND.IG.LT.3)
     &                       .OR.(I.GT.2.AND.IG.GT.2)) ICOLUP(1,I)=503
                     ELSE
                        IF((I.LT.3.AND.IG.GT.2)
     &                       .OR.(I.GT.2.AND.IG.LT.3)) ICOLUP(2,I)=503
                     ENDIF
                  ENDIF
               ENDDO
               IF (.NOT.NODEC) IDUP(5)=IDUP(5)-IDUP(6)
            ELSE
C---2-BODY FINAL STATE
               DO IP=3,NUP
                  IDUP(IP)=IDUP(IP+1)
                  CALL HWVEQU(5,PUP(1,IP+1),PUP(1,IP))
               ENDDO
C---SET COLOUR CONNECTIONS
               ICOLUP(1,3)=ICOLUP(1,4)
               ICOLUP(2,3)=ICOLUP(2,4)
               ICOLUP(1,4)=ICOLUP(1,5)
               ICOLUP(2,4)=ICOLUP(2,5)
            ENDIF
         ENDIF
         IF (.NOT.NODEC) THEN
            IF (JJPROC.GE.2030) THEN
               NP=NUP-5
            ELSE
               NP=NUP
C---SINGLE TOP WITH DECAYS
C   LES HOUCHES COMMON IS FILLED AS FOLLOWS:
C
C  1 INCOMING PARTON
C  2 INCOMING PARTON
C  3 OUTGOING LIGHT PARTON (IF ANY, OTHERWISE 4-9 BECOME 3-8)
C  4 OUTGOING BBAR-TYPE QUARK
C  5 TOP
C  6 W FROM TOP
C  7 QUARK FROM TOP
C  8 DECAY PRODUCT FROM W FROM TOP
C  9 DECAY PRODUCT FROM W FROM TOP
C
C---RECONSTRUCT TOP DECAY
            ENDIF
            IDUP(NP+1)=IDUP(NP-2)
            IDUP(NP+2)=IDUP(NP-1)
            IDUP(NP-1)=-(24*(MOD(IDUP(NP+1),2)+MOD(IDUP(NP+2),2)))
            IDUP(NP-2)=IDUP(NP-1)/4
            CALL HWVEQU(5,PUP(1,NP-2),PUP(1,NP+1))
            CALL HWVEQU(5,PUP(1,NP-1),PUP(1,NP+2))
            CALL HWVSUM(4,PUP(1,NP+1),PUP(1,NP+2),PUP(1,NP-1))
            CALL HWVSUM(4,PUP(1,NP-1),PUP(1,NP  ),PUP(1,NP-2))
            CALL HWUMAS(PUP(1,NP-1))
            CALL HWUMAS(PUP(1,NP-2))
            DO IP=NP-3,NP+2
               ISTUP(IP)=1
            ENDDO
            ISTUP(2)=-1
            ISTUP(NP-1)=2
            ISTUP(NP-2)=2
            MOTHUP(1,NP-3)=1
            MOTHUP(2,NP-3)=2
            MOTHUP(1,NP-2)=1
            MOTHUP(2,NP-2)=2
            MOTHUP(1,NP-1)=NP-2
            MOTHUP(1,NP  )=NP-2
            MOTHUP(1,NP+1)=NP-1
            MOTHUP(1,NP+2)=NP-1
            DO IP=NP-1,NP+2
               MOTHUP(2,IP)=MOTHUP(1,IP)
               ICOLUP(1,IP)=0
               ICOLUP(2,IP)=0
            ENDDO
            ICOLUP(1,NP)=ICOLUP(1,NP-2)
            ICOLUP(2,NP)=ICOLUP(2,NP-2)
            IF (IDUP(NP+1).GT.0) THEN
               ICOLUP(1,NP+1)=504
               ICOLUP(2,NP+2)=504
            ELSE
               ICOLUP(2,NP+1)=504
               ICOLUP(1,NP+2)=504
            ENDIF
            IF (JJPROC.LT.2030) NUP=NP+2
         ENDIF
       ELSEIF (JPR.EQ.1) THEN
C---E+E- ANNIHILATION
         DO I=1,NUP
            ICOLUP(1,I)=0
            ICOLUP(2,I)=0
         ENDDO
C---RESCALE 3-MOMENTA TO PUT PARTONS ON-SHELL
         PUP(5,1)=RMASS(121)
         PUP(5,2)=PUP(5,1)
         CALL HWURSC(2,PUP)
         PUP(5,3)=RMASS(13)
         PUP(5,4)=RMASS(ABS(IDUP(4)))
         PUP(5,5)=PUP(5,4)
         IF (REMIT) THEN
            CALL HWURSC(3,PUP(1,3))
            ICOLUP(1,3)=501
            ICOLUP(2,3)=502
            IF (IDUP(4).GT.0) THEN
               ICOLUP(1,4)=502
               ICOLUP(2,5)=501
            ELSE
               ICOLUP(2,4)=501
               ICOLUP(1,5)=502
            ENDIF
         ELSE
            CALL HWURSC(2,PUP(1,4))
            DO I=3,4
               CALL HWVEQU(5,PUP(1,I+1),PUP(1,I))
               IDUP(I)=IDUP(I+1)
               ISTUP(I)=1
            ENDDO
            IF (IDUP(3).GT.0) THEN
               ICOLUP(1,3)=501
               ICOLUP(2,4)=501
            ELSE
               ICOLUP(2,3)=501
               ICOLUP(1,4)=501
            ENDIF
         ENDIF
      ELSE
C---BOSON PAIR OR LEPTON PAIR
         IF (BOPRO.OR.NODEC) THEN
            NUP=6
            DO I=6,5,-1
               CALL HWVEQU(5,PUP(1,I-1),PUP(1,I))
               IDUP(I)=IDUP(I-1)
               ISTUP(I)=1
            ENDDO
         ELSE
C---BOSON PAIR: ONE OR BOTH DECAYED
C---ADD BOSON(S) TO EVENT RECORD
            IF (ABS(IDUP(6)).LT.20) THEN
               NUP=8
               I=2
               IF (ABS(IDUP(4)).LT.20) THEN
                  NUP=10
                  I=3
               ENDIF
               MUP=NUP-1
               CALL HWVEQU(5,PUP(1,MUP-I),PUP(1,MUP))
               CALL HWVEQU(5,PUP(1,NUP-I),PUP(1,NUP))
               CALL HWVSUM(4,PUP(1,MUP),PUP(1,NUP),PUP(1,6))
               CALL HWUMAS(PUP(1,6))
               IDUP(MUP)=IDUP(MUP-I)
               IDUP(NUP)=IDUP(NUP-I)
               ISTUP(MUP)=1
               ISTUP(NUP)=1
               MOTHUP(1,MUP)=6
               MOTHUP(2,MUP)=6
               MOTHUP(1,NUP)=6
               MOTHUP(2,NUP)=6
               ISTUP(6)=2
               ID=MOD(IDUP(MUP),2)+MOD(IDUP(NUP),2)
               IF (ID.EQ.0) THEN
                  IDUP(6)=23
               ELSEIF (ABS(ID).EQ.1) THEN
                  IDUP(6)=-(24*ID)
               ELSE
                  CALL HWWARN('UPEVNT',106)            
                  GOTO 999            
               ENDIF
            ENDIF
            IF (ABS(IDUP(4)).LT.20) THEN
               CALL HWVZRO(4,PDB)
               DO I=8,7,-1
                  CALL HWVEQU(5,PUP(1,I-3),PUP(1,I))
                  CALL HWVSUM(4,PUP(1,I),PDB,PDB)
                  IDUP(I)=IDUP(I-3)
                  ISTUP(I)=1
                  MOTHUP(1,I)=5
                  MOTHUP(2,I)=5
               ENDDO
               CALL HWUMAS(PDB)
               CALL HWVEQU(5,PDB,PUP(1,5))
               ISTUP(5)=2
               ID=MOD(IDUP(7),2)+MOD(IDUP(8),2)
               IF (ID.EQ.0) THEN
                  IDUP(5)=23
               ELSEIF (ABS(ID).EQ.1) THEN
                  IDUP(5)=-(24*ID)
               ELSE
                  CALL HWWARN('UPEVNT',107)            
                  GOTO 999
               ENDIF
            ELSE
               CALL HWVEQU(5,PUP(1,4),PUP(1,5))
               IDUP(5)=IDUP(4)
               ISTUP(5)=1
               MOTHUP(1,5)=4
               MOTHUP(2,5)=4
            ENDIF
         ENDIF
C---ADD DIBOSON OR DILEPTON TO EVENT RECORD (TO FIX ITS MASS)
         CALL HWVZRO(4,PDB)
CC << OJ
         NENTRIES = 4
         NBACK = 2
         IF (((JPR.EQ.13).OR.(JPR.EQ.14)).AND.(REMIT.EQV..FALSE.)) THEN
            NENTRIES = 3
            NBACK = 1 
         ENDIF

         DO I=6,5,-1
            CALL HWVSUM(4,PUP(1,I),PDB,PDB)
c            MOTHUP(1,I)=4
c            MOTHUP(2,I)=4
CC << OJ
            MOTHUP(1,I)=NENTRIES
            MOTHUP(2,I)=NENTRIES
CC >> OJ
         ENDDO
         CALL HWUMAS(PDB)
c         CALL HWVEQU(5,PDB,PUP(1,4))
c         ISTUP(4)=2
c         IDUP(4)=0
CC << OJ
         CALL HWVEQU(5,PDB,PUP(1,NENTRIES))
         ISTUP(NENTRIES)=2
         IDUP(NENTRIES)=0
CC >> OJ
         IF (REMIT) THEN
C---SET COLOUR CONNECTIONS
            DO I=1,3
               ICOLUP(1,I)=501
               ICOLUP(2,I)=502
            ENDDO
            IF (IHPRO.EQ.1) THEN
               ICOLUP(2,1)=0
               ICOLUP(1,2)=0
            ELSEIF (IHPRO.EQ.2) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=0
               ICOLUP(2,3)=0
            ELSEIF (IHPRO.EQ.3) THEN
               ICOLUP(1,1)=0
               ICOLUP(2,2)=0
            ELSEIF (IHPRO.EQ.4) THEN
               ICOLUP(1,1)=0
               ICOLUP(2,1)=501
               ICOLUP(1,3)=0
            ELSEIF (IHPRO.EQ.5) THEN
               ICOLUP(1,2)=502
               ICOLUP(2,2)=0
               ICOLUP(2,3)=0
            ELSEIF (IHPRO.EQ.6) THEN
               ICOLUP(1,2)=0
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
            ELSE
               CALL HWWARN('UPEVNT',102)
               GOTO 999
            ENDIF
            DO I=4,NUP
               ICOLUP(1,I)=0
               ICOLUP(2,I)=0
            ENDDO
         ELSE
            DO I=5,NUP
c               CALL HWVEQU(5,PUP(1,I),PUP(1,I-2))
c               IDUP(I-2)=IDUP(I)
c               ISTUP(I-2)=ISTUP(I)
c               MOTHUP(1,I-2)=MOTHUP(1,I)-2
c               MOTHUP(2,I-2)=MOTHUP(1,I)-2
CC << OJ
               CALL HWVEQU(5,PUP(1,I),PUP(1,I-NBACK))
               IDUP(I-NBACK)=IDUP(I)
               ISTUP(I-NBACK)=ISTUP(I)
               IF (NBACK.EQ.1) THEN
                  MOTHUP(1,I-1)=3
                  MOTHUP(2,I-1)=3
               ELSE
                  MOTHUP(1,I-2)=MOTHUP(1,I)-NBACK
                  MOTHUP(2,I-2)=MOTHUP(1,I)-NBACK
               ENDIF
CC >> OJ 
            ENDDO
            IF (NBACK.EQ.2) THEN
               MOTHUP(1,3)=1
               MOTHUP(1,4)=1
            ENDIF
c            NUP=NUP-2
CC << OJ
            NUP=NUP-NBACK

CC >> OJ
C---SET COLOUR CONNECTIONS
            DO I=1,NUP
               ICOLUP(1,I)=0
               ICOLUP(2,I)=0
            ENDDO
            IF (IDUP(1).GT.0) THEN
               ICOLUP(1,1)=501
               ICOLUP(2,2)=501
            ELSE
               ICOLUP(2,1)=501
               ICOLUP(1,2)=501
            ENDIF
         ENDIF

         IF (BOPRO) THEN
C---DILEPTON PRODUCTION
            IBOS=MOD(JJPROC,100)
            ILEP=MOD(JJPROC,10)
            IBOS=IBOS-ILEP
C---LOAD LEPTON AND BOSON ID
            I=NUP-1
            J=NUP
            IF ( IBOS.EQ.50 .OR.
     #          (IBOS.EQ.60.AND.JPR.EQ.13) .OR.
     #          (IBOS.EQ.70.AND.JPR.EQ.13) ) THEN
               IDUP(I)=-ILEP-10
               IDUP(J)=-IDUP(I)
c               IF (REMIT) IDUP(4)=23
CC << OJ
               IDUP(NENTRIES)=23
CC >> OJ
            ELSEIF (IBOS.EQ.60.AND.JPR.EQ.14) THEN
               IDUP(I)=-9-2*ILEP
               IDUP(J)=1-IDUP(I)
c               IF (REMIT) IDUP(4)=24
CC << OJ
               IDUP(NENTRIES)=24
CC >> OJ
            ELSEIF (IBOS.EQ.70.AND.JPR.EQ.14) THEN
               IDUP(I)=-10-2*ILEP
               IDUP(J)=-1-IDUP(I)
c               IF (REMIT) IDUP(4)=-24
CC << OJ
               IDUP(NENTRIES)=-24
CC >> OJ
            ELSE
               CALL HWWARN('UPEVNT',504)
               GOTO 999
            ENDIF
         ENDIF

      ENDIF

cc << OJJ
cc         write(*,*) ' AFTER I  IDUP ISTUP MOTH1 MOTH2'
cc         Do I=1,NUP
cc            write(*,*) I,' ',IDUP(I),' ',ISTUP(I),' ',MOTHUP(1,I),
cc     $           ' ',MOTHUP(2,I),' ',PUP(1,I),' ',PUP(2,I),' ',
cc     $           PUP(3,I),' ',PUP(4,I)
cc         ENDDO

C...=======================================================
C...Fill spin information for tauola.
C...Specify the decal mode.
C...imode: 1:W, 2:WW, 4:Z, 5:WZ, 8:ZZ, 16:H, 17:WH, 20:ZH.
      imode = 0
      itau1 = 0
      itau2 = 0
      itau3 = 0
      do i=1,nup
        if (iabs(idup(i)).eq.24) then
          imode = imode + 1
        else if (iabs(idup(i)).eq.23) then
          imode = imode + 4
        else if (iabs(idup(i)).eq.25) then
          imode = imode + 16
        endif
      enddo
      do i=1,nup
         if(mothup(1,i).gt.0) then
            if (imode.eq.1) then ! single W production
               if (iabs(idup(i)).eq.15.and.iabs(idup(mothup(1,i)))
     &              .eq.24) then
                  itau1 = i
               endif
            else if (imode.eq.2) then ! WW production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.24) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.-24) then
                  itau2 = i
               endif
            else if (imode.eq.4) then ! single Z production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.23) then
                  itau1 = i
                  itau3 = mothup(1,i)
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.23) then
                  itau2 = i
               endif
            else if (imode.eq.5) then ! WZ production (so far, only assign for W decay.)
               if (iabs(idup(i)).eq.15.and.iabs(idup(mothup(1,i)))
     &              .eq.24) then
                  itau1 = i
               endif
            else if (imode.eq.8) then ! ZZ production (so far do nothing.)
            else if (imode.eq.16) then ! single H production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.25) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.25) then
                  itau2 = i
               endif
            else if (imode.eq.17) then ! WH production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.25) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.25) then
                  itau2 = i
               endif
               if (iabs(idup(i)).eq.15.and.iabs(idup(mothup(1,i)))
     &              .eq.24) then
                  itau3 = i
               endif
            else if (imode.eq.20) then ! ZH production (so far nothing for Z side.)
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.25) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.25) then
                  itau2 = i
               endif
            endif
        endif
      enddo

      if (itau1.ne.0) then
        spinup(itau1) = 0
      endif
      if (itau2.ne.0) then
        spinup(itau2) = 0
      endif
      if (itau3.ne.0) then
        spinup(itau3) = 0
      endif

C...Assign spin information.
      if (imode.eq.1) then         ! single W production
        if (itau1.ne.0) then
          spinup(itau1) = -1.0d0
        endif
      else if (imode.eq.2) then    ! WW production
        if (itau1.ne.0) then
          spinup(itau1) = -1.0d0
        endif
        if (itau2.ne.0) then
          spinup(itau2) = -1.0d0
        endif
c JK
c      else if (imode.eq.4) then    ! single Z production
      else if (imode.eq.4
     &       .and. itau1.ne.0.and.itau2.ne.0.and.itau3.ne.0) then ! single Z production


C...Special case. Temporaly create the HEPEVT block. Later delete it.
        nhep = 9
        isthep(4) = 121
        isthep(5) = 122
        isthep(6) = 120
        isthep(7) = 124
        isthep(8) = 1
        isthep(9) = 1
        idhep(4) = idup(1)
        idhep(5) = idup(2)
        idhep(6) = 0
        idhep(7) = idup(itau3)
        idhep(8) = idup(itau1)
        idhep(9) = idup(itau2)
        jmohep(1,4) = 6
        jmohep(2,4) = 5
        jmohep(1,5) = 6
        jmohep(2,5) = 4
        jmohep(1,6) = 4
        jmohep(2,6) = 5
        jmohep(1,7) = 6
        jmohep(2,7) = 7
        jmohep(1,8) = 7
        jmohep(2,8) = 8
        jmohep(1,9) = 7
        jmohep(2,9) = 9
        jdahep(1,4) = 5
        jdahep(2,4) = 5
        jdahep(2,5) = 4
        jdahep(2,5) = 4
        jdahep(2,6) = 7
        jdahep(2,6) = 7
        jdahep(2,7) = 8
        jdahep(2,7) = 9
        jdahep(2,8) = 0
        jdahep(2,8) = 0
        jdahep(2,9) = 0
        jdahep(2,9) = 0
        do i=1,5
          phep(i,4) = pup(i,1)
          phep(i,5) = pup(i,2)
          phep(i,6) = pup(i,1)+pup(i,2)
          phep(i,7) = pup(i,itau3)
          phep(i,8) = pup(i,itau1)
          phep(i,9) = pup(i,itau2)
        enddo
        phep(5,6) = sqrt((pup(4,1)+pup(4,2))**2
     $                  -(pup(1,1)+pup(1,2))**2
     $                  -(pup(2,1)+pup(2,2))**2
     $                  -(pup(3,1)+pup(3,2))**2)
        do i=1,4
          vhep(i,4) = 0.0d0
          vhep(i,5) = 0.0d0
          vhep(i,6) = 0.0d0
          vhep(i,7) = 0.0d0
          vhep(i,8) = 0.0d0
          vhep(i,9) = 0.0d0
        enddo

        polz0 = plzapx(1,7,8,9)
        if (pyr(0).LT.polz0) THEN
          spinup(itau1) = 1.0d0
          spinup(itau2) = 1.0d0
        else
          spinup(itau1) = -1.0d0
          spinup(itau2) = -1.0d0
        endif
C...Delete temporal HEPEVT.
        nhep = 3
        do i=4,9
          isthep(i) = 0
          idhep(i)  = 0
          jmohep(1,i) = 0
          jmohep(2,i) = 0
          jdahep(1,i) = 0
          jdahep(2,i) = 0
          phep(1,i) = 0.0d0
          phep(2,i) = 0.0d0
          phep(3,i) = 0.0d0
          phep(4,i) = 0.0d0
          phep(5,i) = 0.0d0
          vhep(1,i) = 0.0d0
          vhep(2,i) = 0.0d0
          vhep(3,i) = 0.0d0
          vhep(4,i) = 0.0d0
        enddo
      else if (imode.eq.5) then    ! WZ production (so far, only assign for W decay.)
        if (itau1.ne.0) then
          spinup(itau1) = -1.0d0
        endif
      else if (imode.eq.8) then    ! ZZ production (so far do nothing.)
      else if (imode.eq.16) then   ! single H production
        if (itau1.ne.0.and.itau2.ne.0) then
          if (pyr(0).lt.0.5d0) then
            spinup(itau1) =  1.0d0
            spinup(itau2) = -1.0d0
          else
            spinup(itau1) = -1.0d0
            spinup(itau2) =  1.0d0
          endif
        endif
      else if (imode.eq.17) then   ! WH production
        if (itau1.ne.0.and.itau2.ne.0) then
          if (pyr(0).lt.0.5d0) then
            spinup(itau1) =  1.0d0
            spinup(itau2) = -1.0d0
          else
            spinup(itau1) = -1.0d0
            spinup(itau2) =  1.0d0
          endif
        endif
        if (itau3.ne.0) then
          spinup(itau3) = -1.0d0
        endif
      else if (imode.eq.20) then   ! ZH production (so far nothing for Z side.)
        if (itau1.ne.0.and.itau2.ne.0) then
          if (pyr(0).lt.0.5d0) then
            spinup(itau1) =  1.0d0
            spinup(itau2) = -1.0d0
          else
            spinup(itau1) = -1.0d0
            spinup(itau2) =  1.0d0
          endif
        endif
      endif

C...Debug.
C      write(*,*) "ST...McAtNlo",imode,itau1,itau2,itau3
C      if (itau1.ne.0) then
C        write(*,*) "ST...McAtNlo after fill 1",idup(itau1),spinup(itau1)
C      endif
C      if (itau2.ne.0) then
C        write(*,*) "ST...McAtNlo after fill 2",idup(itau2),spinup(itau2)
C      endif
C      if (itau3.ne.0) then
C        write(*,*) "ST...McAtNlo after fill 3",idup(itau3),spinup(itau3)
C      endif
C...=======================================================



 999  CONTINUE
      IF(IERROR.LT.100) RETURN 
c     PRINT *
c      DO I=1,NUP
c         PRINT '(4I4,3F8.2)',IDUP(I),ISTUP(I),(MOTHUP(J,I),J=1,2),
c     &        (PUP(J,I),J=1,3)
c      ENDDO
      
c       IPR, IC, NP
 901  FORMAT(1X,I3,2(1X,I2))
c      (ID(I),I=1,NP)
 902  FORMAT(9(1X,I3))
c       XEVWGT,EMSCA
 903  FORMAT(2(1X,D14.8))
c      ((P(J,I),J=1,4),I=1,NP)
 904  FORMAT(36(1X,D14.8))
C gkh
 905  FORMAT(3(1X,D14.8))
 906  FORMAT(10(1x,d14.8))
 907  FORMAT(28(1x,d14.8))

c 901  FORMAT(1X,I3,4(1X,I2))
c 902  FORMAT(1X,D14.8)
c 903  FORMAT(16(1X,D14.8))
      END
C----------------------------------------------------------------------
      SUBROUTINE UPCODE(ICODE,ICOL)
C--DECODES COLOUR CONNECTIONS
C----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER ICODE,ICOL(2)
      ICOL(1)=ICODE/10
      IF (ICOL(1).NE.0) ICOL(1)=ICOL(1)+500
      ICOL(2)=MOD(ICODE,10)
      IF (ICOL(2).NE.0) ICOL(2)=ICOL(2)+500
      END

      SUBROUTINE HWURSC(NP,PP)
C  RESCALES A SET OF NP (<21) 3-MOMENTA PP(1-3,*) IN
C  THEIR CMF TO PUT PP ON MASS-SHELL AT MASSES PP(5,*) 
C----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER NP,IP,IT,NT
      DOUBLE PRECISION PP(5,*),P(5,20),P2(20),M2(20),SP(5),
     & TINY,FAC,ECM,DCM,EP,STEP,FRT,HWUSQR
      DATA TINY,NT/1D-9,20/
      IF (NP.GT.20) CALL HWWARN('HWURSC',300+NP)
C--COMPUTE CM MOMENTUM
      CALL HWVZRO(4,SP)
      DO IP=1,NP
         CALL HWVSUM(4,PP(1,IP),SP,SP)
      ENDDO
      CALL HWUMAS(SP)
C--BOOST TO CMF
      DO IP=1,NP
         CALL HWULOF(SP,PP(1,IP),P(1,IP))
         P2(IP)=P(1,IP)**2+P(2,IP)**2+P(3,IP)**2
         M2(IP)=P(5,IP)**2
      ENDDO
C--ITERATE RESCALING OF 3-MOMENTA
      FAC=1D0
      DO IT=1,NT
         ECM=0D0
         DCM=0D0
         DO IP=1,NP
            EP=HWUSQR(M2(IP)+FAC*P2(IP))
            IF (EP.GT.0D0) THEN
               ECM=ECM+EP
               DCM=DCM+P2(IP)/EP
            ENDIF
         ENDDO
         IF (DCM.EQ.0D0) CALL HWWARN('HWURSC',390)
         STEP=2D0*(ECM-SP(5))/DCM
         FAC=FAC-STEP
         IF (ABS(STEP).LT.TINY) GOTO 100
      ENDDO
C--FAILED TO CONVERGE
      CALL HWWARN('HWURSC',1)
C--CONVERGED: RESCALE 3-MOMENTA AND BOOST BACK 
 100  IF (FAC.LT.0D0) CALL HWWARN('HWURSC',391)
      FRT=SQRT(FAC)
      DO IP=1,NP
         CALL HWVSCA(3,FRT,P(1,IP),P(1,IP))
         P(4,IP)=SQRT(M2(IP)+FAC*P2(IP))
         CALL HWULOB(SP,P(1,IP),PP(1,IP))
      ENDDO
      END
