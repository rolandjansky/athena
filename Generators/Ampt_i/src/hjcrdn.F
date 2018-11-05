      FVOIDP FUNCTION HJCRDN_ADDRESS() 

        COMMON/hjcrdn/YP(3,300),YT(3,300)
      SAVE /hjcrdn/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      HJCRDN_ADDRESS = GETADDR(YP(1,1))
      
      RETURN
      END

