      FVOIDP FUNCTION INPUT1_ADDRESS() 

      common/input1/ MASSPR,MASSTA,ISEED,IAVOID,DT
      SAVE /input1/
      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      INPUT1_ADDRESS = GETADDR(MASSPR)
      
      RETURN
      END

