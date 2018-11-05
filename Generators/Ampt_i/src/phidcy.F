      FVOIDP FUNCTION PHIDCY_ADDRESS() 

      common /phidcy/ iphidcy,pttrig,ntrig,maxmiss,ipi0dcy
      save /phidcy/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      PHIDCY_ADDRESS = GETADDR(iphidcy)
      
      RETURN
      END

