      FVOIDP FUNCTION RESDCY_ADDRESS() 

      common /resdcy/ NSAV,iksdcy
      save /resdcy/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      RESDCY_ADDRESS = GETADDR(NSAV)
      
      RETURN
      END

