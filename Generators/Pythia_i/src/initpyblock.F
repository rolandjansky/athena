      SUBROUTINE INITPYBLOCK(LULIS,FILLIS)
      IMPLICIT NONE
 
      EXTERNAL PYDATA ! All that is required to force loading of BLOCK DATA
 
      COMMON/PYDAT4/CHAF(500,2)
      CHARACTER CHAF*16
      integer LULIS,IERR
c      CHARACTER* FILLIS
      character(len=*) FILLIS
c       This is the listing filefile

c      write(6,*) '<<<<<<<<< Hello from initpyblock >>>>>>>  ' 
c      write(6,*) CHAF(1,1), CHAF(1,2)
c      write(6,*) CHAF(100,1), CHAF(100,2)

C          Open for write the pythia listing file File
C          ====================================
C    
      IF(LULIS.GT.0)THEN
         OPEN ( UNIT=LULIS, FORM='FORMATTED',
     &         FILE=FILLIS,STATUS='NEW',IOSTAT=IERR)
         IF(IERR.NE.0)   THEN
            WRITE(6,*)
     &           'Error ', IERR,' failing to open PYTHIA Listing file'
            RETURN
         ELSE
            WRITE(6,*)
     &       'Open PYTHIA Listing  file'
         ENDIF
      ENDIF
      RETURN
      END
