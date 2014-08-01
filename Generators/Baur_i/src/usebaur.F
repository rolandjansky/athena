C...Routine called by PYEVNT or PYEVNW to get user process event
      
      SUBROUTINE USEBAUR

      IMPLICIT NONE

C...Three Pythia functions return integers, so need declaring.
      INTEGER KSUSY1,KSUSY2,KTECHN,KEXCIT,KDIMEN
     &   ,       N,NPAD,K, MSTU,MSTJ, KCHG, MDCY,MDME,KFDP 
     &   ,       MRPY, MSEL,MSELPD,MSUB,KFIN, MSTP,MSTI, MINT
     &   ,       ISET,KFPR,ICOL, ISIG, MWID, NGENPD,NGEN
     &   ,       IMSS, IDR,IDR2,KFR, IHIST,INDX       
      DOUBLE PRECISION P,V, PARU,PARJ, PMAS,PARF,VCKM, BRAT
     &   ,                RRPY, CKIN, PARP,PARI, VINT, COEF
     &   ,                XSFX,SIGH, WIDS, XSEC, SIGT
     &   ,                XPVMD,XPANL,XPANH,XPBEH,XPDIR
     &   ,                VXPVMD,VXPANL,VXPANH,VXPDGM, RMSS
     &   ,                ZMIX,UMIX,VMIX,SMZ,SMW,SFMIX,ZMIXI,UMIXI,VMIXI
     &   ,                RVLAM,RVLAMP,RVLAMB, AB,RMS,RES,DCMASS
     &   ,                RM,A,B,RESM,RESW, XXM, X1, BIN

C...Parameter statement to help give large particle numbers
C...(left- and righthanded SUSY, technicolor, excited fermions,
C...extra dimensions)
      PARAMETER (KSUSY1=1000000,KSUSY2=2000000,KTECHN=3000000,
     &   KEXCIT=4000000,KDIMEN=5000000)

C...EXTERNAL statement links PYDATA on most machines.
      EXTERNAL PYDATA

C...Commonblocks.
C...The event record.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
C...Parameters.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
C...Particle properties + some flavour parameters.
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...Decay information.
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
C...Particle names
      COMMON/PYDAT4/CHAF(500,2)
      CHARACTER CHAF*16
C...Random number generator information.
      COMMON/PYDATR/MRPY(6),RRPY(100)
C...Selection of hard scattering subprocesses.
      COMMON/PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
C...Parameters. 
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
C...Internal variables.
      COMMON/PYINT1/MINT(400),VINT(400)
C...Process information.
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
C...Parton distributions and cross sections.
      COMMON/PYINT3/XSFX(2,-40:40),ISIG(1000,3),SIGH(1000)
C...Resonance width and secondary decay treatment.
      COMMON/PYINT4/MWID(500),WIDS(500,5)
C...Generation and cross section statistics.
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
C...Process names.
      COMMON/PYINT6/PROC(0:500)
      CHARACTER PROC*28
C...Total cross sections.
      COMMON/PYINT7/SIGT(0:6,0:6,0:5)
C...Photon parton distributions: total and valence only.
      COMMON/PYINT8/XPVMD(-6:6),XPANL(-6:6),XPANH(-6:6),XPBEH(-6:6), 
     &   XPDIR(-6:6) 
      COMMON/PYINT9/VXPVMD(-6:6),VXPANL(-6:6),VXPANH(-6:6),VXPDGM(-6:6) 
C...Supersymmetry parameters.
      COMMON/PYMSSM/IMSS(0:99),RMSS(0:99)
C...Supersymmetry mixing matrices.
      COMMON/PYSSMT/ZMIX(4,4),UMIX(2,2),VMIX(2,2),SMZ(4),SMW(2),
     &   SFMIX(16,4),ZMIXI(4,4),UMIXI(2,2),VMIXI(2,2)
C...R-parity-violating couplings in supersymmetry.
      COMMON/PYMSRV/RVLAM(3,3,3), RVLAMP(3,3,3), RVLAMB(3,3,3)
C...Internal parameters for R-parity-violating processes.
      COMMON/PYRVNV/AB(2,16,2),RMS(0:3),RES(6,5),IDR,IDR2,DCMASS,KFR(3)
      COMMON/PYRVPM/RM(0:3),A(2),B(2),RESM(2),RESW(2),MFLAG
      LOGICAL MFLAG
C...Parameters for Gauss integration of supersymmetric widths.
      COMMON/PYINTS/XXM(20)
      COMMON/PYG2DX/X1
C...Histogram information.
      COMMON/PYBINS/IHIST(4),INDX(1000),BIN(20000)
C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)

C...Extra commonblock to transfer run info.
      INTEGER MODE,LNHIN,LNHOUT,NREAD,MSCAL
      REAL*8 SCALEF
      COMMON/BAUPPRIV/SCALEF,LNHIN,NREAD,LNHOUT,MODE,MSCAL

      REAL*8 WTMAX
      INTEGER I,J
c      INTEGER ID,IDUMB
c      CHARACTER*132 BUFF

      WTMAX=XMAXUP(1)
      IDPRUP=LPRUP(1)

 99   CONTINUE

C...Zero some arrays in common blocks to simplify filling.
      
      NUP=6
      DO 100 I=1,NUP
        MOTHUP(1,I)=0
        MOTHUP(2,I)=0
        ICOLUP(1,I)=0
        ICOLUP(2,I)=0
        SPINUP(I)=0D0
        PUP(1,I)=0D0    
        PUP(2,I)=0D0    
        PUP(5,I)=0D0
        VTIMUP(I)=0D0
 100  CONTINUE

C-- ignore comment lines
c      READ(LNHIN,'(A132)',END=888,ERR=888) BUFF
c      DO WHILE(INDEX(BUFF,"#") .NE. 0 .OR. BUFF.EQ.' ')
c        READ(LNHIN,'(A132)',END=888,ERR=888) BUFF
c      ENDDO
C-- read number of particles,id#,weight,scale,alpha_em,alpha_s

      READ(LNHIN,*,END=888)
     $     (IDUP(J),J=1,NUP),(ISTUP(J),J=3,NUP),
     $     ((MOTHUP(I,J),I=1,2),J=3,NUP),
     $     ((ICOLUP(I,J),I=1,2),J=1,NUP),
     $     (SPINUP(J),J=1,NUP),
     $     ((PUP(I,J),I=1,5),J=1,NUP),SCALUP,AQEDUP,AQCDUP
      ISTUP(1) = -1
      ISTUP(2) = -1
      MOTHUP(1,1)=0
      MOTHUP(1,2)=0
      MOTHUP(2,1)=0
      MOTHUP(2,2)=0
      MOTHUP(1,3)=1
      MOTHUP(2,3)=2
      MOTHUP(1,4)=3
      MOTHUP(2,4)=3
      MOTHUP(1,5)=3
      MOTHUP(2,5)=3
      MOTHUP(1,6)=1
      MOTHUP(2,6)=2
      IDPRUP = 1
      XWGTUP = 10.0d0
      XWGTUP=WTMAX
c      write(*,*) "IDUP", IDUP(1),IDUP(2),IDUP(3),IDUP(4),IDUP(5),IDUP(6)

c      write(*,*) PUP(1,1),PUP(2,1),PUP(3,1),PUP(4,1),PUP(5,1)
c      write(*,*) PUP(1,2),PUP(2,2),PUP(3,2),PUP(4,2),PUP(5,2)
c      write(*,*) PUP(1,3),PUP(2,3),PUP(3,3),PUP(4,3),PUP(5,3)
c      write(*,*) PUP(1,4),PUP(2,4),PUP(3,4),PUP(4,4),PUP(5,4)
c      write(*,*) PUP(1,5),PUP(2,5),PUP(3,5),PUP(4,5),PUP(5,5)
c      write(*,*) PUP(1,6),PUP(2,6),PUP(3,6),PUP(4,6),PUP(5,6)

c      XWGTUP=XWGTUP*NREAD

c      IF(IDPRUP.EQ.1) XWGTUP=XWGTUP*1d3

c      IF(XWGTUP.GT.WTMAX) XWGTUP=WTMAX



C...If you want to use some other scale for parton showering then the 
C...factorisation scale given by MadEvent, please implement the function PYMASC
C...(example function included below) 

      IF(MSCAL.GT.0) CALL PYMASC(SCALUP)

C...Needed to get the same scale for multiple interactions in Pythia as for the pdfs
      MSTP(32)=12
      PARP(193)=SCALUP**2
      PARP(194)=PARP(193)
      
C...Fix for PYTHIA 6.2
c   SCALUP=SCALUP*SQRT(PARP(67))
      
C...Template to add mothers, e.g. top or W going to partons
C...This is important to keep invariant masses in parton showering
C...Note that this must be done case by case knowing which partons
C...comes from which mother

c      DO IMOT=1,2
cC   Positions in record of first and last daughter
c        IF(IMOT.EQ.1) THEN
c          ID1=3
c          ID2=0
c          ID3=5
cC       Mother (particle code)
c          IMO=23
c        ELSE
c          ID1=4+1
c          ID2=6+1
c          ID3=8+1
cC       Mother (particle code)
c          IMO=-6
c        ENDIF
cC   Make place for mother
c      NUP=NUP+1
c      IJ=ID1
c      DO IC=NUP,IJ+1,-1
c        IDUP(IC)=IDUP(IC-1)
c        ISTUP(IC)=ISTUP(IC-1)
c        MOTHUP(1,IC)=MOTHUP(1,IC-1)
c        MOTHUP(2,IC)=MOTHUP(2,IC-1)
c        ICOLUP(1,IC)=ICOLUP(1,IC-1)
c        ICOLUP(2,IC)=ICOLUP(2,IC-1)
c        DO J=1,5
c          PUP(J,IC)=PUP(J,IC-1)
c        ENDDO
c      ENDDO
c      ID1=ID1+1
c      ID2=ID2+1
c      ID3=ID3+1
c      
cC   Parameters for mother
c      IDUP(IJ)=IMO
c      ISTUP(IJ)=2
cC   Momenta
c      DO J=1,4
c        PUP(J,IJ)=0d0
c        DO I=ID1,ID3
c          IF(I.EQ.ID1.OR.I.EQ.ID2.OR.I.EQ.ID3)THEN
c            PUP(J,IJ)=PUP(J,IJ)+PUP(J,I)
c          ENDIF
c        ENDDO
c      ENDDO
c      TEST=PUP(1,IJ)**2+PUP(2,IJ)**2+PUP(3,IJ)**2-PUP(4,IJ)**2
c      IF(TEST.LE.0D0) THEN
c        PUP(5,IJ)=DSQRT(-TEST)         
c      ELSEIF(DABS(TEST).LT.1D-4) THEN
c        PUP(5,IJ)=0D0
c      ELSE
c        PUP(5,IJ)=-1D0
c        PRINT*,' NEGATIVE MASS '
c      ENDIF
cC   Color
c      DO JJ=1,2
c        ICOLUP(JJ,IJ)=0
c        DO I=ID1,ID3
c          IF(I.EQ.ID1.OR.I.EQ.ID2.OR.I.EQ.ID3)THEN
c            ICOLUP(JJ,IJ)=ICOLUP(JJ,IJ)+ICOLUP(JJ,I)
c          ENDIF
c          DO J=ID1,ID3
c            IF(I.EQ.ID1.OR.I.EQ.ID2.OR.I.EQ.ID3)THEN
c              IF(I.NE.J.AND.ICOLUP(JJ,I).EQ.ICOLUP(3-JJ,J))
c     $           ICOLUP(JJ,IJ)=ICOLUP(JJ,IJ)-ICOLUP(JJ,I)
c            ENDIF
c          ENDDO
c        ENDDO
c      ENDDO
cc      PRINT *,'Color of t,b'=ICOLUP(JJ,IJ),ICOLUP(JJ,IJ)
c          
cC   Set mother of daughters
c      DO I=ID1,ID3
c        IF(I.EQ.ID1.OR.I.EQ.ID2.OR.I.EQ.ID3)THEN
c          MOTHUP(1,I)=IJ
c          MOTHUP(2,I)=IJ
cc          PRINT *,'Setting mother of ',I,' to ',IJ
c        ENDIF
c      ENDDO
c      ENDDO

      RETURN
     
 888  CONTINUE
      
      NUP=0
      WRITE(LNHOUT,*) 'last event !' 
      
      RETURN
      END
      
C*********************************************************************
C   PYMASC
C   Implementation of scale used in Pythia parton showers

      SUBROUTINE PYMASC(scale)
      IMPLICIT NONE

C...Arguments
      REAL*8 scale

C...Functions
      REAL*8 SMDOT5

C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)

C...Extra commonblock to transfer run info.
      INTEGER MODE,LNHIN,LNHOUT,NREAD,MSCAL
      REAL*8 SCALEF
      COMMON/BAUPPRIV/SCALEF,LNHIN,NREAD,LNHOUT,MODE,MSCAL
      SAVE/BAUPPRIV/      

C...Local variables
      INTEGER ICC1,ICC2,IJ,IDC1,IDC2,IC,IC1,IC2
      REAL*8 QMIN,QTMP

C   Just use the scale read off the event record
      scale=SCALUP

C   Alternatively:

C...  Guesses for the correct scale
C     Assumptions:
C     (1) if the initial state is a color singlet, then
C     use s-hat for the scale
C     
C     (2) if color flow to the final state, use the minimum
C     of the dot products of color connected pairs
C     (times two for consistency with above)

        QMIN=SMDOT5(PUP(1,1),PUP(1,2))
        ICC1=1
        ICC2=2
C     
C     For now, there is no generic way to guarantee the "right"
C     scale choice.  Here, we take the HERWIG pt. of view and
C     choose the dot product of the colored connected "primary"
C     pairs.
C     

        DO 101 IJ=1,NUP
          IF(MOTHUP(2,IJ).GT.2) GOTO 101
          IDC1=ICOLUP(1,IJ)
          IDC2=ICOLUP(2,IJ)
          IF(IDC1.EQ.0) IDC1=-1
          IF(IDC2.EQ.0) IDC2=-2
          
          DO 201 IC=IJ+1,NUP
            IF(MOTHUP(2,IC).GT.2) GOTO 201
            IC1=ICOLUP(1,IC)
            IC2=ICOLUP(2,IC)
            IF(ISTUP(IC)*ISTUP(IJ).GE.1) THEN
              IF(IDC1.EQ.IC2.OR.IDC2.EQ.IC1) THEN
                QTMP=SMDOT5(PUP(1,IJ),PUP(1,IC))
                IF(QTMP.LT.QMIN) THEN
                  QMIN=QTMP
                  ICC1=IJ
                  ICC2=IC
                ENDIF
              ENDIF
            ELSEIF(ISTUP(IC)*ISTUP(IJ).LE.-1) THEN
              IF(IDC1.EQ.IC1.OR.IDC2.EQ.IC2) THEN
                QTMP=SMDOT5(PUP(1,IJ),PUP(1,IC))          
                IF(QTMP.LT.QMIN) THEN
                  QMIN=QTMP
                  ICC1=IJ
                  ICC2=IC
                ENDIF
              ENDIF
            ENDIF
 201      CONTINUE
 101    CONTINUE

        scale=QMIN

      RETURN
      END

C...SMDOT5
C   Helper function

      FUNCTION SMDOT5(V1,V2)
      IMPLICIT NONE
      REAL*8 SMDOT5,TEMP
      REAL*8 V1(5),V2(5)
      INTEGER I

      SMDOT5=0D0
      TEMP=V1(4)*V2(4)
      DO I=1,3
        TEMP=TEMP-V1(I)*V2(I)
      ENDDO

      SMDOT5=SQRT(ABS(TEMP))

      RETURN
      END

