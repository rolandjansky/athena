      FVOIDP FUNCTION PARA8_ADDRESS() 

      common /para8/ idpert,npertd,idxsec
      save /para8/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      PARA8_ADDRESS = GETADDR(idpert)
      
      RETURN
      END

