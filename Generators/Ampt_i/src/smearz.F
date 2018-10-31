      FVOIDP FUNCTION SMEARZ_ADDRESS() 

      double precision smearp,smearh
      common /smearz/smearp,smearh
      save /smearz/
      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      SMEARZ_ADDRESS = GETADDR(smearp)
      
      RETURN
      END

