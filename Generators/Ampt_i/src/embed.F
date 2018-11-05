      FVOIDP FUNCTION EMBED_ADDRESS() 

      common/embed/iembed,nsembd,pxqembd,pyqembd,xembd,yembd,
     1     psembd,tmaxembd,phidecomp
      SAVE /embed/
      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      EMBED_ADDRESS = GETADDR(iembed)
      
      RETURN
      END

