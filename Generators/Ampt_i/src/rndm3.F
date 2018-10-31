      FVOIDP FUNCTION RNDM3_ADDRESS() 

      common /rndm3/ iseedp
      save /rndm3/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      RNDM3_ADDRESS = GETADDR(iseedp)
      
      RETURN
      END

