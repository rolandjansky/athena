      FVOIDP FUNCTION ANIM_ADDRESS() 

      common/anim/nevent,isoft,isflag,izpc
      SAVE /anim/
      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      ANIM_ADDRESS = GETADDR(nevent)
      
      RETURN
      END

