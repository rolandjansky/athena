      FVOIDP FUNCTION PARA7_ADDRESS() 

      common /para7/ ioscar,nsmbbbar,nsmmeson
      SAVE /para7/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      PARA7_ADDRESS = GETADDR(ioscar)
      
      RETURN
      END

