      SUBROUTINE PAROUT(IMODE)

C______________________________________________________________________________
C AUTHOR LMijovic
C helper subroutine for redirecting part of Pythia output to a separate file
C______________________________________________________________________________

      LOGICAL ISFILE
      INTEGER IFOUT1, IFOUT2

C     if the values are changed also change the MSTU(11) setting below
C     file for pythia parameter values after initialization 
      IFOUT1=7
C     file for pythia parameter values after event generation 
      IFOUT2=8  
    
      IF (IMODE .EQ. 1) THEN
         CALL PYGIVE("MSTU(11)=7")
         INQUIRE(FILE='par_after_init.out',EXIST=ISFILE)
         IF (ISFILE .EQV. .TRUE.) THEN
            OPEN(IFOUT1,FILE='after_init.out',STATUS='OLD',ERR=3001)
         ELSE
            OPEN(IFOUT1,FILE='after_init.out',STATUS='NEW',ERR=3001)
         ENDIF
      ELSE IF (IMODE .EQ. 2) THEN
         CALL PYGIVE("MSTU(11)=8")
         OPEN(IFOUT2,FILE='after_gen.out',STATUS='NEW',ERR=3001)
      ELSE IF (IMODE .EQ. 3) THEN
         CALL PYGIVE("MSTU(11)=8")
         OPEN(IFOUT2,FILE='after_gen.out',STATUS='OLD',ERR=3001)
      ENDIF
      
      RETURN
      
 3001 WRITE (*,*) 'could not open file for writing'
      
      END
