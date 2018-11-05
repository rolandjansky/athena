      FVOIDP FUNCTION CMSFLAG_ADDRESS() 

      double precision dshadow
      common /cmsflag/ dshadow, ishadow
      save /cmsflag/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      CMSFLAG_ADDRESS = GETADDR(dshadow)
      
      RETURN
      END

