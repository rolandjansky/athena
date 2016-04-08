C******************************************************
C*          HV - Pythia interface.                    *
C*            Version 1.0, 02/06/2007                 *
C*                                                    *
C*  - Reads particle masses from event file           *
C*                                                    *
C*     Written by S.Giagu sefano.giagu@cern.ch        *
C*                                                    *
C******************************************************

C...UPINIT
C...Routine called by PYINIT to set up user-defined processes.

      SUBROUTINE INITHVGEN

      IMPLICIT NONE

C--GUP common block
#include "GeneratorFortranCommon/heprup.inc"

C...EXTERNAL statement links PYDATA on most machines
      EXTERNAL PYDATA

C--PYTHIA Particles Properties common Blocks
c     INTEGER KCHG,MDCY,MDME,KFDP
c     DOUBLE PRECISION PMAS,PARF,VCKM,BRAT
c     COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
c     COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)

      CHARACTER*120 DUMMY
      CHARACTER*120 BUFF
      CHARACTER*120 FILENAME
      INTEGER IGOOD, IALL, I, IDUM, IDP
      DOUBLE PRECISION XDUM
C     DOUBLE PRECISION XXPR

C...Set Particles properties and read input event file name
      FILENAME='HV.newlha.dat'
      WRITE(*,*) 'INITHVGEN Opening inputParHvGen.dat File'
      OPEN(UNIT=76,STATUS='OLD',FILE='inputParHvGen.dat')
      DO I=1,100
        READ(76,'(A)',ERR=300,END=301) BUFF
        IF(BUFF(2:6).EQ.'Input') THEN
          FILENAME = BUFF(8:80)
          WRITE(*,*) 'INITHVGEN: Input Event File is: ',FILENAME
        ELSE IF (BUFF(1:1).EQ.'!') THEN
C...Do nothing so far
          WRITE(*,*) BUFF
        ELSE
          CALL PYGIVE(BUFF)
        ENDIF
      ENDDO
 300  CONTINUE
      WRITE(*,*) 'INITHVGEN::ERROR: Reading inputParHvGen.dat File'
 301  CONTINUE
      CLOSE(76)

C...Set incoming beams: LHC.
      IDBMUP(1)=2212
      IDBMUP(2)=2212
      EBMUP(1)=7000D0
      EBMUP(2)=7000D0

C--   open the file
      WRITE(*,*) 'INITHVGEN Opening LesHouches Input File'
c     OPEN(UNIT=77,STATUS='OLD',FILE='HV.newlha.dat')
      OPEN(UNIT=77,STATUS='OLD',FILE=FILENAME)

C--   read header
      WRITE(*,*) 'INITHVGEN Read Header'
      DO I=1,5
        READ(77,'(A)') DUMMY !head + <init>
      ENDDO
      IF (DUMMY.NE.'<init>') GOTO 201   ! error
      READ(77,*) IDBMUP(1), IDBMUP(2),EBMUP(1),EBMUP(2),
     &           PDFGUP(1),PDFGUP(2),
     &           PDFSUP(1),PDFSUP(2),IDWTUP,NPRUP
      DO I=1,NPRUP
        READ(77,*) XSECUP(I),XERRUP(I),XMAXUP(I),LPRUP(I)
      ENDDO
      READ(77,'(A)') DUMMY !</init>
      IF (DUMMY.NE.'</init>') GOTO 204   ! error

C--  check number of events in file and file consistency

      IGOOD = 1
      IALL  = 0
      DO WHILE (IGOOD .EQ. 1)
        READ(77,'(A)',END=100) DUMMY ! '<event>
        IF (DUMMY.EQ.'</LesHouchesEvents>') GOTO 100
        IF (DUMMY.NE.'<event>') GOTO 101   ! error
         READ(77,*, END=102) IDP, IDUM, XDUM, XDUM, XDUM, XDUM
         DO I=1,IDP
            READ(77,'(A)',END=103) DUMMY
         ENDDO
        READ(77,'(A)',END=104) DUMMY ! '</event>'
        IF (DUMMY.NE.'</event>') GOTO 104   ! error
        IALL = IALL + 1
      ENDDO


 201  CONTINUE
      WRITE(*,*) 'INITHVGEN::ERROR: <init> not found as expected'
      GOTO 100
 204  CONTINUE
      WRITE(*,*) 'INITHVGEN::ERROR: </init> not found as expected'
      GOTO 100
 101  CONTINUE
      WRITE(*,*) 'INITHVGEN::ERROR: <event> not found as expected'
      GOTO 100
 102  CONTINUE
      WRITE(*,*) 'INITHVGEN::ERROR: line 1 event not found as expected'
      GOTO 100
 103  CONTINUE
      WRITE(*,*) 'INITHVGEN::ERROR: part block not found as expected'
      GOTO 100
 104  CONTINUE
      WRITE(*,*) 'INITHVGEN::ERROR: </event> not found as expected'
      GOTO 100
 100  CONTINUE
      WRITE(*,*) 'INITHVGEN Read Header and ',IALL,' events '
      REWIND(77)
      DO I=1,5
        READ(77,'(A)') DUMMY !head + <init>
      ENDDO
      READ(77,*) IDUM, IDUM, XDUM, XDUM, IDUM, IDUM, IDUM,
     &           IDUM, IDUM, IDUM
      DO I=1,IDUM
        READ(77,'(A)') DUMMY
      ENDDO
      READ(77,'(A)') DUMMY !</init>
      IF (DUMMY.NE.'</init>') THEN
        WRITE(*,*) 'INITHVGEN::ERROR: rewinding file'
      ENDIF

      RETURN
      END
