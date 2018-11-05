      FVOIDP FUNCTION RNDF77_ADDRESS() 

      COMMON/RNDF77/NSEED
      SAVE /RNDF77/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      RNDF77_ADDRESS = GETADDR(NSEED)
      
      RETURN
      END

