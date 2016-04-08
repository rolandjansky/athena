      SUBROUTINE INITPROTOS
      IMPLICIT NONE

#include "ProtosLHEF_i/Protos.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"

C...Initialize Information Using Protos

      lun=35                ! Select here some number not already used

C -----------------------------------------------------------------------------

!     Read and store protoslhef.dat information

      OPEN (lun,file='protoslhef.dat')

!     initialize
      ii=0
      in=0
      DO i=1,20
         string(i)=" "
      END DO

!     read
      DO i=1,20
         in=i-1
         READ (lun,"(A)",IOSTAT=ii) string(i)
         IF (ii .GT. 0)  THEN
            PRINT *, " ERROR :: ProtosLHEF_i :: FILE *** protoslhef.dat *** READ FAILED "
            EXIT
         ELSE IF (ii .LT. 0) THEN
            ! protoslhef.dat read OK
            EXIT
         END IF
      END DO

      CLOSE(lun)
C -----------------------------------------------------------------------------

!     Read the ASCII file with process information (named protoslhef.dat)

      OPEN (lun,file='protoslhef.dat')
      READ (lun,*) IHRD
      READ (lun,*) ET

      IDBMUP(1)    = 2212                      ! set proton as first beam
      IDBMUP(2)    = 2212                      ! set proton as second beam
      EBMUP(1)     = ET/2d0                    ! set energy of first beam
      EBMUP(2)     = ET/2d0                    ! set energy of second beam
      PDFGUP(1)    = -1                        ! author group for beam 1, according to Cernlib PDFlib 
      PDFGUP(2)    = -1                        ! author group for beam 2, according to Cernlib PDFlib 
      PDFSUP(1)    = -1                        ! PDFset ID for beam 1, according to Cernlib PDFlib
      PDFSUP(2)    = -1                        ! PDFset ID for beam 2, according to Cernlib PDFlib
      IDWTUP       =  3                        ! event weighting strategy: + input weights
      NPRUP        =  1                        ! number of processes to be considered 
      LPRUP(NPRUP) =  600 + IHRD               ! LHA code for each process

!     Process dependent information
      DO i=1,NPRUP
         XSECUP(i) = 1d0
         XERRUP(i) = 1d0
         XMAXUP(i) = 1d0
         LPRUP(i)  = LPRUP(i)
      END DO

C -----------------------------------------------------------------------------

!     Masses default values -> from Pythia8.183

      mtau=1.77682d0
      mb=4.80d0
      mt=171.0d0
      MW=80.385d0
      MZ=91.1876d0
      MH=125.0d0
      mL=400.0d0
      mQ=400.0d0
      MZp=500.0d0
      MWp=500.0d0

C -----------------------------------------------------------------------------

!     Specific Masses

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

!     Read matching parameters, if any

      IF ( (IHRD .EQ. 3) .OR. (IHRD .EQ. 61) .OR. (IHRD .EQ. 63)
     &  .OR. (IHRD .EQ. 65) ) THEN
        READ (lun,*) IMATCH,PTbmax
      ENDIF

      CLOSE (lun)

C -----------------------------------------------------------------------------

!     Open the ASCII event files (named protoslhef.events)

      OPEN (lun,file='protoslhef.events')

10001 FORMAT ('Unknown process IHRD = ',I2)

      END
