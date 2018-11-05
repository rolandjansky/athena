      FVOIDP FUNCTION PARA2_ADDRESS() 

      double precision xmp, xmu, alpha, rscut2, cutof2
      common /para2/ xmp, xmu, alpha, rscut2, cutof2
      save /para2/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      PARA2_ADDRESS = GETADDR(xmp)
      
      RETURN
      END

