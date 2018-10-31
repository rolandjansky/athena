      FVOIDP FUNCTION XYEMBED_ADDRESS() 

      PARAMETER (nxymax=10001)

      common/xyembed/nxyjet,xyjet(nxymax,2)
      SAVE /xyembed/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      INPUT1_ADDRESS = GETADDR(MASSPR)
      
      RETURN
      END

