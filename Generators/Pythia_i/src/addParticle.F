      SUBROUTINE ADDPARTICLE(FCHAR)

      CHARACTER*(*)  FCHAR
   
      OPEN(20,
     & FILE=FCHAR)
      
      CALL PYUPDA(3,20)
      CLOSE(20)

      RETURN
      END
