      FVOIDP FUNCTION COAL_ADDRESS() 

      double precision dpcoal,drcoal,ecritl
      common /coal/dpcoal,drcoal,ecritl
      save /coal/
      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      COAL_ADDRESS = GETADDR(dpcoal)
      
      RETURN
      END

