C*************************************************************************
C
C USELHEF called by UPEVNT from upinit_py.F in Pythia_i or Herwig_i. See
C initlhef.F.
C
C UPEVNT is called by PYEVNT or PYEVNW to get user process event.
      
      SUBROUTINE USELHEF

      IMPLICIT NONE

C...Pythia parameters.
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)

C...User process initialization commonblock.
#include "GeneratorFortranCommon/heprup.inc"
C...User process event common block.
#include "GeneratorFortranCommon/hepeup.inc"
C...Pythia common blocks
      INTEGER PYCOMP,KCHG,MINT,NPART,NPARTD,IPART,MAXNUR
      DOUBLE PRECISION PMAS,PARF,VCKM,VINT,PTPART
C...Particle properties + some flavour parameters.
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYINT1/MINT(400),VINT(400)
      PARAMETER (MAXNUR=1000)
      COMMON/PYPART/NPART,NPARTD,IPART(MAXNUR),PTPART(MAXNUR)

C...Extra commonblock to transfer run info.
      INTEGER LNHIN
      COMMON/LHUPPRIV/LNHIN

C...Local variables
      INTEGER I,J,IBEG,MOTH,NUPREAD,IREM
      DOUBLE PRECISION PSUM,ESUM
c//

C   PDF info (will be passed via LHEFINFO common block)
      CHARACTER*4 PDFSTR
      INTEGER PARTID1,PARTID2
      DOUBLE PRECISION PARTX1,PARTX2,SCALEPDF,XPDF1,XPDF2
      COMMON/LHEFINFO/PARTID1,PARTID2,PARTX1,PARTX2,SCALEPDF,XPDF1,XPDF2

C...Lines to read in assumed never longer than 200 characters. 
      INTEGER MAXLEN
      PARAMETER (MAXLEN=200)
      CHARACTER*(MAXLEN) STRING

C...Format for reading lines.
      CHARACTER*6 STRFMT

C(BPK-JBD)->
cLet Herwig now if the last event has been read
      integer istg
      common /evtcon/ istg

      istg=0
C(BPK-JBD)-<

      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN

C...Loop until finds line beginning with "<event>" or "<event ". 
  100 READ(LNHIN,STRFMT,END=900,ERR=900) STRING
      IBEG=0
  110 IBEG=IBEG+1
C...Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-6) GOTO 110 
      IF(STRING(IBEG:IBEG+6).NE.'<event>'.AND.
     &STRING(IBEG:IBEG+6).NE.'<event ') GOTO 100

C...Read first line of event info.
      READ(LNHIN,*,END=900,ERR=900) NUPREAD,IDPRUP,XWGTUP,SCALUP,
     &AQEDUP,AQCDUP

C...Read NUP subsequent lines with information on each particle.
      ESUM=0d0
      PSUM=0d0
      NUP=1
      IREM=NUPREAD+1

C...Read all lines from input      
      DO 1120 I=1,NUPREAD
        READ(LNHIN,*,END=900,ERR=900) IDUP(I),ISTUP(I),
     &  MOTHUP(1,I),MOTHUP(2,I),ICOLUP(1,I),ICOLUP(2,I),
     &  (PUP(J,I),J=1,5),VTIMUP(I),SPINUP(I)
C...Reset resonance momentum to prepare for mass shifts
        IF(ISTUP(I).EQ.2.OR.ISTUP(I).EQ.3) PUP(3,I)=0
 1120 CONTINUE

      DO 120 I=1,NUPREAD
C...Remove Z/gamma resonance in e+e-
        IF(IABS(IDBMUP(1)).EQ.11.AND.IABS(IDBMUP(2)).EQ.11.AND.
     $       IDBMUP(1).EQ.-IDBMUP(2).AND.ISTUP(NUP).EQ.2.AND.
     $       MOTHUP(1,NUP).EQ.1) THEN
           IREM=I
           cycle
        ENDIF
        IF(ISTUP(NUP).EQ.1)THEN
C...Set massless particle masses to Pythia default. Adjust z-momentum. 
           IF(PUP(5,NUP).EQ.0D0.AND.IABS(IDUP(NUP)).GT.3
     $         .AND.IDUP(NUP).NE.21) THEN
              PUP(5,NUP)=PMAS(IABS(PYCOMP(IDUP(NUP))),1)
              PUP(3,NUP)=SIGN(SQRT(MAX(0d0,PUP(4,NUP)**2-PUP(5,NUP)**2-
     $           PUP(1,NUP)**2-PUP(2,NUP)**2)),PUP(3,NUP))
           ENDIF
C...Set gluon mass to zero (absorb gluon mass into z-momentum).
           IF(IDUP(NUP).EQ.21) THEN
              PUP(3,NUP)=SIGN(SQRT(PUP(3,NUP)**2+PUP(5,NUP)**2),PUP(3,NUP))
              PUP(5,NUP)=0D0
           ENDIF
           PSUM=PSUM+PUP(3,NUP)
C...Adjust mother information due to removed mother
           IF(MOTHUP(1,NUP).EQ.IREM)THEN
              MOTHUP(1,NUP)=1
              MOTHUP(2,NUP)=2
           ELSE IF(MOTHUP(1,NUP).GT.IREM)THEN
              MOTHUP(1,NUP)=MOTHUP(1,NUP)-1
              MOTHUP(2,NUP)=MOTHUP(2,NUP)-1
           ENDIF
C...Set mother resonance momenta
           MOTH=MOTHUP(1,NUP)
           DO WHILE (MOTH.GT.2)
             PUP(3,MOTH)=PUP(3,MOTH)+PUP(3,NUP)
             MOTH=MOTHUP(1,MOTH)
           ENDDO
        ENDIF
        NUP=NUP+1
  120 CONTINUE
      NUP=NUP-1

C   Read PDF info (if available)
  200 READ(LNHIN,STRFMT,END=900,ERR=900) STRING
      IBEG=0
  210 IBEG=IBEG+1

C   Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-7) GOTO 210
      IF(STRING(IBEG:IBEG+3).EQ.'#pdf') THEN
C        WRITE(*,*) "Found #pdf tag!"
C   Split line.
        READ(STRING,*,END=900,ERR=900) PDFSTR, PARTID1, PARTID2,
     &    PARTX1, PARTX2, SCALEPDF, XPDF1, XPDF2

C        write(*,*) "Lhef PDF info: ", PARTID1, PARTID2, PARTX1, PARTX2,
C     &    SCALEPDF, XPDF1, XPDF2
      ENDIF

C..Adjust mass of resonances
      DO I=1,NUP
         IF(ISTUP(I).EQ.2.OR.ISTUP(I).EQ.3)THEN
            PUP(5,I)=SQRT(PUP(4,I)**2-PUP(1,I)**2-PUP(2,I)**2-
     $             PUP(3,I)**2)
         ENDIF
      ENDDO

      ESUM=PUP(4,1)+PUP(4,2)

      RETURN

C...Error exit, typically when no more events.
  900 WRITE(*,*) ' Failed to read LHEF event information in USELHEF,'
      WRITE(*,*) ' assume end of file has been reached.'
      NUP=0
      MINT(51)=2
C(BPK-JBD)->
      istg = 1
C(BPK-JBD)-<

      RETURN
      END
