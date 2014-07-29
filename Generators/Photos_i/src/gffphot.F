C     Tauola FFPHOT common block access
c     Borut Paul Kersevan (February 2003)
      FVOIDP FUNCTION FFPHOT_ADDRESS() 
      IMPLICIT NONE
#include "Photos_i/ffphot.inc"
      FVOIDP GETADDR
      FFPHOT_ADDRESS = GETADDR(IFPMODE)
      RETURN
      END
