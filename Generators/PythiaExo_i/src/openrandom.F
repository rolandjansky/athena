      INTEGER FUNCTION OPENRANDOM(OPTION,LULIS,FILLIS)
      IMPLICIT NONE
 
      integer LULIS,IERR,OPTION
      CHARACTER*60 FILLIS

C          Open for write the pythia random number File
C          ====================================
C
      OPENRANDOM = 0
      IF(LULIS.GT.0 .and. OPTION .gt.0)THEN
         IF(OPTION .eq. 1) THEN
            OPEN ( UNIT=LULIS, FORM='UNFORMATTED',
     &           FILE=FILLIS,STATUS='UNKNOWN',IOSTAT=IERR)
         ELSEIF (OPTION .eq. 2) THEN
            OPEN ( UNIT=LULIS, FORM='UNFORMATTED',
     &           FILE=FILLIS,STATUS='OLD',IOSTAT=IERR)
         ENDIF
         IF(IERR.NE.0)   THEN
            WRITE(6,*) 'Error ', IERR,
     &      ' failing to open PYTHIA Random Number file'
            RETURN
         ELSE
            WRITE(6,*) 'Open PYTHIA Random Number file'
         ENDIF
      ENDIF

C     When in reading mode count the number of records in the file
      IF (OPTION .eq. 2) THEN
         DO
           READ(LULIS,END=120,IOSTAT=IERR)
           OPENRANDOM = OPENRANDOM + 1
         ENDDO
 120     REWIND(LULIS)
      ENDIF
      
      RETURN

      END
