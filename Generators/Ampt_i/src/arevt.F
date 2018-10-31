      FVOIDP FUNCTION AREVT_ADDRESS() 

      COMMON /AREVT/ IAEVT, IARUN, MISS
      SAVE /AREVT/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      AREVT_ADDRESS = GETADDR(IAEVT)
      
      RETURN
      END

