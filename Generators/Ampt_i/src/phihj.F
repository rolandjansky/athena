      FVOIDP FUNCTION PHIHJ_ADDRESS() 

      common /phihj/ iphirp,phiRP
      SAVE /phihj/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      PHIHJ_ADDRESS = GETADDR(iphirp)
      
      RETURN
      END

