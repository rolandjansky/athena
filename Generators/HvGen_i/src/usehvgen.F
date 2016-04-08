C******************************************************
C*          HV - Pythia interface.                    *
C*            Version 1.0, 02/06/2007                 *
C*                                                    *
C*  - Reads particle masses from event file           *
C*                                                    *
C*     Written by S.Giagu sefano.giagu@cern.ch        *
C*                                                    *
C******************************************************

C...Routine called by PYEVNT or PYEVNW to get user process event

      SUBROUTINE USEHVGEN

      IMPLICIT NONE

C--GUP common block
#include "GeneratorFortranCommon/hepeup.inc"

      CHARACTER*19 DUMMY
      INTEGER I

C--  read one event
      NUP = 0

      READ(77,'(A)',END=100) DUMMY ! '<event>
      IF (DUMMY.EQ.'</LesHouchesEvents>') GOTO 105
      IF (DUMMY.NE.'<event>') THEN
         GOTO 101   ! error
      ENDIF
      READ(77,*, END=102) NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP
      DO I=1,NUP
         READ(77,*,END=103) IDUP(I),ISTUP(I),MOTHUP(1,I),MOTHUP(2,I),
     &       ICOLUP(1,I),ICOLUP(2,I),
     &       PUP(1,I),PUP(2,I),PUP(3,I),PUP(4,I),
     &       PUP(5,I),VTIMUP(I),SPINUP(I)
      ENDDO
      READ(77,'(A)',END=104) DUMMY ! '</event>'
      IF (DUMMY.NE.'</event>') GOTO 104   ! error

      GOTO 100

 101  CONTINUE
      WRITE(*,*) 'USEHVGEN::ERROR: <event> not found where expected'
      GOTO 100
 102  CONTINUE
      WRITE(*,*) 'USEHVGEN::ERROR: line 1 of eve not found as expected'
      GOTO 100
 103  CONTINUE
      WRITE(*,*) 'USEHVGEN::ERROR: part block not found as expected'
      GOTO 100
 104  CONTINUE
      WRITE(*,*) 'USEHVGEN::ERROR: </event> not found where expected'
      GOTO 100
 105  CONTINUE
      WRITE(*,*) 'USEHVGEN end of events in list'
      RETURN
 100  CONTINUE
      WRITE(*,*) 'USEHVGEN new event read'

      RETURN
      END
