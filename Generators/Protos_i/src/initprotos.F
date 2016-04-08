**********************************************************************************
**********************************************************************************
      SUBROUTINE INITPROTOS
      IMPLICIT NONE

#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"

      print *, "*************************************"
      print *, "* Protos_i   interface for ATHENA   *"
      print *, "* version 00-00-06     19/5/2011   *"
      print *, "*                                   *"
      print *, "* Nuno Castro (nuno.castro@cern.ch) *"
      print *, "* LIP and Universidade do Minho     *"
      print *, "*                                   *"
      print *, "* based on protint.f, provided in   *"
      print *, "* Protos 2.1A                       *"
      print *, "*                                   *"
      print *, "* Protos generator developed by     *"
      print *, "* Juan Antonio Aguilar Saavedra     *"
      print *, "* Universidad de Granada            *"
      print *, "*************************************"

C...Event Weighting Strategy: +/- input weights

      IDWTUP = -3

C...Initialize Information Using Protos

      CALL PROTOS_INIT

      END


**********************************************************************************


**********************************************************************************
**********************************************************************************
      SUBROUTINE PROTOS_INIT
      IMPLICIT NONE

#include "Protos_i/Protos.inc"
#include "GeneratorFortranCommon/heprup.inc"

!     External variables

      INTEGER LONGITUD

!     Local

      CHARACTER*100 string
      INTEGER i,idum1,idum2
      REAL*8 ET
      REAL*8 mt,Gt,mb,MH                    ! For Red and White
      REAL*8 mL,GE,GN,mX,GX,mD,GD           ! For Triada
      REAL*8 mQ,GQ,Vmix                     ! For Rose

      lun=35                ! Select here some number not already used

!     Read the ASCII file with process information (named protos.dat)

      OPEN (lun,file='protos.dat')
      READ (lun,*) IHRD
      READ (lun,*) ET

!     Set p pbar and CM energy

      Write(*,*) " LHC considered: pp collisions at sqrt(s) = ", ET, " GeV"
      IDBMUP(1)=2212
      IDBMUP(2)=2212
      EBMUP(1)=ET/2d0
      EBMUP(2)=ET/2d0
      Write(*,*) " Beam energy:",EBMUP(1)

C...PROCESS TAGS
      NPRUP  = 1
      LPRUP(NPRUP)=600+IHRD

!     Masses

      IF ((IHRD .GE. 3) .AND. (IHRD .LE. 18)) THEN
        READ (lun,*) mt,Gt,mb,MH
      ELSE IF ((IHRD .GE. 31) .AND. (IHRD .LE. 33)) THEN
        READ (lun,*) mL,GE,GN,MH
      ELSE IF ((IHRD .GE. 34) .AND. (IHRD .LE. 38)) THEN
        READ (lun,*) mX,GX,idum1,idum2
        READ (lun,*) mL,GE,GN,MH
      ELSE IF ((IHRD .GE. 41) .AND. (IHRD .LE. 43)) THEN
        READ (lun,*) mD,GD
      ELSE IF ((IHRD .GE. 51) .AND. (IHRD .LE. 69)) THEN
        READ (lun,*) mt,Gt,mb,MH
        READ (lun,*) mQ,GQ,Vmix
      ELSE
        PRINT 10001,IHRD
        STOP
      ENDIF

      IF ((IHRD .GE. 3) .AND. (IHRD .LE. 18)) THEN

      WRITE (string,'(F6.2)') mt
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(6,1)='//string(1:i))
      WRITE (string,'(F6.2)') Gt
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(6,2)='//string(1:i))
      WRITE (string,'(F6.2)') mb
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(5,1)='//string(1:i))
      WRITE (string,'(F6.2)') MH
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(25,1)='//string(1:i))

      ELSE IF ((IHRD .GE. 31) .AND. (IHRD .LE. 33)) THEN

      WRITE (string,'(F7.2)') mL
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(17,1)='//string(1:i))
      CALL PYGIVE('PMAS(18,1)='//string(1:i))
      WRITE (string,'(F10.6)') GE
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(17,2)='//string(1:i))
      WRITE (string,'(F10.6)') GN
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(18,2)='//string(1:i))
      WRITE (string,'(F6.2)') MH
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(25,1)='//string(1:i))

      ELSE IF ((IHRD .GE. 34) .AND. (IHRD .LE. 36)) THEN

      WRITE (string,'(F7.1)') mX
      i=LONGITUD(string)
      IF (IHRD .NE. 35) THEN
        CALL PYGIVE('PMAS(32,1)='//string(1:i))
      ELSE
        CALL PYGIVE('PMAS(34,1)='//string(1:i))
      ENDIF
      WRITE (string,'(F6.2)') GX
      i=LONGITUD(string)
      IF (IHRD .NE. 35) THEN
        CALL PYGIVE('PMAS(32,2)='//string(1:i))
      ELSE
        CALL PYGIVE('PMAS(34,2)='//string(1:i))
      ENDIF
      WRITE (string,'(F7.2)') mL
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(17,1)='//string(1:i))
      CALL PYGIVE('PMAS(18,1)='//string(1:i))
      WRITE (string,'(F10.6)') GE
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(17,2)='//string(1:i))
      WRITE (string,'(F10.6)') GN
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(18,2)='//string(1:i))
      WRITE (string,'(F6.2)') MH
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(25,1)='//string(1:i))

      ELSE IF (IHRD .EQ. 38) THEN

      WRITE (string,'(F7.1)') mX
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(34,1)='//string(1:i))
      WRITE (string,'(F6.2)') GX
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(34,2)='//string(1:i))
      WRITE (string,'(F7.2)') mL
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(17,1)='//string(1:i))
      CALL PYGIVE('PMAS(18,1)='//string(1:i))
      WRITE (string,'(F10.6)') GE
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(17,2)='//string(1:i))
      WRITE (string,'(F10.6)') GN
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(18,2)='//string(1:i))
      WRITE (string,'(F6.2)') MH
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(25,1)='//string(1:i))

      ELSE IF ((IHRD .GE. 41) .AND. (IHRD .LE. 43)) THEN

c      WRITE (string,'(F7.1)') mD
c      i=LONGITUD(string)
c      CALL PYGIVE('PMAS(37,1)='//string(1:i))
c      CALL PYGIVE('PMAS(9900041,1)='//string(1:i))
c      WRITE (string,'(F10.6)') GD
c      i=LONGITUD(string)
c      CALL PYGIVE('PMAS(37,2)='//string(1:i))
c      CALL PYGIVE('PMAS(9900041,2)='//string(1:i))

      ELSE IF ((IHRD .GE. 51) .AND. (IHRD .LE. 69)) THEN

      WRITE (string,'(F6.2)') mt
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(6,1)='//string(1:i))
      WRITE (string,'(F7.2)') Gt
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(6,2)='//string(1:i))
      WRITE (string,'(F6.2)') mb
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(5,1)='//string(1:i))
      WRITE (string,'(F6.2)') MH
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(25,1)='//string(1:i))

      WRITE (string,'(F7.2)') mQ
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(8,1)='//string(1:i))
      CALL PYGIVE('PMAS(7,1)='//string(1:i))
      WRITE (string,'(F6.2)') GQ
      i=LONGITUD(string)
      CALL PYGIVE('PMAS(8,2)='//string(1:i))
      CALL PYGIVE('PMAS(7,2)='//string(1:i))

      ENDIF

!     Read matching parameters, if any

      IF ( (IHRD .EQ. 3) .OR. (IHRD .EQ. 61) .OR. (IHRD .EQ. 63)
     &  .OR. (IHRD .EQ. 65) ) THEN
        READ (lun,*) IMATCH,PTbmax
        IF (IMATCH .EQ. 2) CALL PYGIVE('MSTP(143)=1')
      ELSE
        IMATCH=0
      ENDIF

      CLOSE (lun)

!     Open the ASCII event files (named protos.events)

      OPEN (lun,file='protos.events')

10001 FORMAT ('Unknown process IHRD = ',I2)

      END
**********************************************************************************

**********************************************************************************
**********************************************************************************
      INTEGER FUNCTION LONGITUD(name)
      CHARACTER*100 name
      INTEGER i
      i = LEN(name)
      DO WHILE (name(i:i) .EQ. ' ')
        i = i - 1
      ENDDO
      LONGITUD = i
      RETURN
      END
**********************************************************************************


