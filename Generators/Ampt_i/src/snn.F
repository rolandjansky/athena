      FVOIDP FUNCTION SNN_ADDRESS() 

      common/snn/efrm,npart1,npart2,epsiPz,epsiPt,PZPROJ,PZTARG
      SAVE /snn/

      EXTERNAL AMPTDATA ! All that is required to force loading of BLOCK DATA
      FVOIDP GETADDR

      SNN_ADDRESS = GETADDR(efrm)
      
      RETURN
      END

