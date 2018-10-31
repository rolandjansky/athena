      FVOIDP FUNCTION HBT_ADDRESS() 

      PARAMETER  (MAXSTR=150001)
      COMMON/hbt/lblast(MAXSTR),xlast(4,MAXSTR),plast(4,MAXSTR),nlast
      SAVE /hbt/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      HBT_ADDRESS = GETADDR(lblast(1))
      
      RETURN
      END

