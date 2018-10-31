      FVOIDP FUNCTION RUN_ADDRESS() 

      COMMON  /RUN/     NUM
      SAVE  /run/
      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      RUN_ADDRESS = GETADDR(NUM)
      
      RETURN
      END

