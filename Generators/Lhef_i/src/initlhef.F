C*************************************************************************
C Lhef_i -- Les Houches Event Files interface
C           for ATHENA
C
C Date: 30 Jul 2008 15:34:08 CEST
C Maintainer: Jan Schumacher <Jan.Schumacher@cern.ch>
C
C This version is adapted from MadGraph_i, which is itself adapted from
C pythia-pgs/src/ME2pythia.f (http://madgraph.hep.uiuc.edu/)
C*************************************************************************

C*************************************************************************
C INITLHEF is called by UPINIT from upinit_py.F in Pythia_i or Herwig_i
C respectively, depending on ATLASCHOICE. The latter is set from the C++
C interface routines Pythia/Herwig_cxx. This indirection allows for a
C joboption configurable process. See example jobOptions in share.
C
C Note that some properties of Lhef_i are also hardcoded in
C EvgenJobOptions and EvgenJobTransforms.

      SUBROUTINE INITLHEF

      IMPLICIT NONE

C   HEPRUP LHA common block
#include "GeneratorFortranCommon/heprup.inc"

C...Pythia parameters.
      INTEGER MSTP,MSTI,MRPY
      DOUBLE PRECISION PARP,PARI,RRPY
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYDATR/MRPY(6),RRPY(100)

C... Extra commonblock to transfer input file unit (Adapted from
C... MadGraph, where it is used to share additional information)
      INTEGER LNHIN
      COMMON/LHUPPRIV/LNHIN

C FLAG FOR RECORD TYPE
      INTEGER ILHNEW
      COMMON/ILH/ILHNEW
      DATA ILHNEW/0/
      SAVE/ILH/

C...Lines to read in assumed never longer than 200 characters.
      INTEGER MAXLEN,IBEG,IPR
      PARAMETER (MAXLEN=200)
      CHARACTER*(MAXLEN) STRING

C...Format for reading lines.
      CHARACTER*6 STRFMT

      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN

C... Open the LHEF. LNHIN is shared via common block for the other
C... subroutines which have to read events. I am setting LNHIN to an
C... arbitrary number here, since beforehand it was not initialized
C... properly. JS
      LNHIN = 11
      OPEN(UNIT=LNHIN,STATUS='OLD',FILE='events.lhe')

C->BPK
C Check for new LH interface
c... use now only the LH interface!!!
      READ(LNHIN,STRFMT) STRING
      IF (STRING(1:17).EQ.'<LesHouchesEvents') THEN
         ILHNEW=1
         Print *,'LesHouchesEvents format detected: OK  !!!'
      ELSE
         Print *,'Error: not Les Houches format !!!'
         stop
      ENDIF

C...Loop until finds line beginning with "<init>" or "<init ".
  100 READ(LNHIN,STRFMT,END=130,ERR=130) STRING
      IBEG=0
  110 IBEG=IBEG+1
C...Allow indentation.
         IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-5) GOTO 110
         IF(STRING(IBEG:IBEG+5).NE.'<init>'.AND.
     &        STRING(IBEG:IBEG+5).NE.'<init ') GOTO 100

C...Read first line of initialization info.
         READ(LNHIN,*,END=130,ERR=130) IDBMUP(1),IDBMUP(2),EBMUP(1),
     &   EBMUP(2),PDFGUP(1),PDFGUP(2),PDFSUP(1),PDFSUP(2),IDWTUP,NPRUP

C...Read NPRUP subsequent lines with information on each process.
         DO 120 IPR=1,NPRUP
            READ(LNHIN,*,END=130,ERR=130) XSECUP(IPR),XERRUP(IPR),
     &           XMAXUP(IPR),LPRUP(IPR)
 120     CONTINUE

C->JS
C...Check for IDWTUP. -4,-3,3,4 should be ok, as no unweighting needs to
C...occur. Fail for all other values. (Only IABS(IDWTUP) \in {1,2,3,4}
C...defined in hep-ph/0109068. -2, ..., 2 use unweighting in the SHG,
C...which is not fully implemented here.)
C...2011-04-13: Allow unweighting, if NPRUP.EQ.1. Returning multiple
C...process types on demand is not implemented.
      IF(IABS(IDWTUP).LE.2) THEN
        write(*,*) 'Warning! Got IDWTUP == ', IDWTUP
        write(*,*) '   Unweighting has been requested.'
        write(*,*) '   Not tested extensively.'
        write(*,*) '   cf. hep-ph/0109068 and comments in initlhef.F'
        IF(NPRUP.NE.1) THEN
          write(*,*) 'Got NPRUP != 1, '
          write(*,*) 'which I cannot handle! Exiting.'
          STOP
        END IF
      END IF

C...Set PDFLIB or LHAPDF pdf number for Pythia
C...Note that Pythia will not look at PDFGUP and PDFSUP.

C...If you want Pythia to use PDFLIB, you have to set it by hand.
C...(You also have to ensure that the dummy routines
C...PDFSET, STRUCTM and STRUCTP in Pythia are not linked.)
C      MSTP(52)=2
C      MSTP(51)=1000*PDFGUP(1)+PDFSUP(1)

c... If PDFGUP(1:2) is negative: Choose default 10042/cteq6ll
c... cteq6ll no longer hard-coded as default (CG 01/11)
      IF(PDFGUP(1).LT.0.AND.PDFGUP(2).LT.0) THEN
        WRITE(*,*) ' NOT!!! Using default PDF: cteq6ll.'
c        MSTP(51)=10042
      ELSE
        IF(PDFSUP(1).NE.19070.AND.
     &   (PDFSUP(1).NE.0.OR.PDFSUP(2).NE.0))THEN
c       Not CTEQ5L, which is standard in Pythia
           CALL PYGIVE('MSTP(52)=2')  ! use of PDFLIB/LHAPDF
c       The following works for both PDFLIB and LHAPDF (where PDFGUP(1)=0)
c       But note that the MadEvent output uses the LHAPDF numbering scheme
          IF(PDFSUP(1).NE.0)THEN
             MSTP(51)=1000*PDFGUP(1)+PDFSUP(1)
          ELSE
             MSTP(51)=1000*PDFGUP(2)+PDFSUP(2)
          ENDIF
        ENDIF
      ENDIF

c(BPK-JBD)-> fix for Herwig  ( *** need to check)
          IF (PDFSUP(1) .GT. 100) THEN
             PDFGUP(1) = -1
             PDFGUP(2) = -1
          ENDIF
c(BPK-JBD)-<

C...Initialize widths and partial widths for resonances.
         CALL PYINRE

C...Calculate xsec reduction due to non-decayed resonances
C...based on first event only!
         CALL BRSUPP_LHEF

         REWIND(LNHIN)

C...For photon initial states from protons: Set proton not to break up
      CALL PYGIVE('MSTP(98)=1')

         RETURN

C...Error exit: give up if initalization does not work.
 130     WRITE(*,*) ' Failed to read LHEF initialization information.'
         WRITE(*,*) ' Event generation will be stopped.'
         STOP

      RETURN
      END

C*********************************************************************

      subroutine BRSUPP_LHEF

      IMPLICIT NONE

C...Three Pythia functions return integers, so need declaring.
      INTEGER PYCOMP,MWID
      DOUBLE PRECISION WIDS

C...Resonance width and secondary decay treatment.
      COMMON/PYINT4/MWID(500),WIDS(500,5)

C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
#include "GeneratorFortranCommon/hepeup.inc"

C...Extra commonblock to transfer run info.
      INTEGER LNHIN
      COMMON/LHUPPRIV/LNHIN

      INTEGER I,J,IBEG
      REAL SUPPCS

C...Lines to read in assumed never longer than 200 characters.
      INTEGER MAXLEN
      PARAMETER (MAXLEN=200)
      CHARACTER*(MAXLEN) STRING

C...Format for reading lines.
      CHARACTER*6 STRFMT
      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN

C...Loop until finds line beginning with "<event>" or "<event ".
  100 READ(LNHIN,STRFMT,END=130,ERR=130) STRING
      IBEG=0
  110 IBEG=IBEG+1
C...Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-6) GOTO 110
      IF(STRING(IBEG:IBEG+6).NE.'<event>'.AND.
     &STRING(IBEG:IBEG+6).NE.'<event ') GOTO 100

C...Read first line of event info.
      READ(LNHIN,*,END=130,ERR=130) NUP,IDPRUP,XWGTUP,SCALUP,
     &AQEDUP,AQCDUP

C...Read NUP subsequent lines with information on each particle.
      DO 120 I=1,NUP
        READ(LNHIN,*,END=130,ERR=130) IDUP(I),ISTUP(I),
     &  MOTHUP(1,I),MOTHUP(2,I),ICOLUP(1,I),ICOLUP(2,I),
     &  (PUP(J,I),J=1,5),VTIMUP(I),SPINUP(I)
  120 CONTINUE

      SUPPCS=1.
      do I=3,NUP
        if (ISTUP(I).EQ.1.AND.(IABS(IDUP(I)).GE.23.OR.
     $     (IABS(IDUP(I)).GE.6.AND.IABS(IDUP(I)).LE.8)))
     $     THEN
          WRITE(*,*) 'Resonance ',IDUP(I), ' has BRTOT ',
     $       wids(PYCOMP(IDUP(I)),2)
          if(wids(PYCOMP(IDUP(I)),2).lt.0.95) then
            write(*,*) 'Decreasing cross section!'
            SUPPCS=SUPPCS*wids(PYCOMP(IDUP(I)),2)
          endif
        endif
      enddo
      if(SUPPCS.gt.0)then
         write(*,*)'Multiplying cross section by ',SUPPCS
         XSECUP(1)=XSECUP(1)*SUPPCS
      else
         write(*,*) 'Warning! Got cross section suppression 0'
         write(*,*) 'No cross section reduction done'
      endif
      RETURN

C...Error exit, typically when no more events.
  130 WRITE(*,*) ' Failed to read LHEF event information in BRSUPP_LH.'
      STOP
      END
