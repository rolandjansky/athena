      FVOIDP FUNCTION USERGAMPRT_ADDRESS()
      IMPLICIT NONE
#include "Tauola_i/UserGamprt.inc"
   
     
      FVOIDP GETADDR
      
      USERGAMPRT_ADDRESS = GETADDR(UGUSED)
      RETURN
      END
