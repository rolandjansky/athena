C     Tauola PHOLUN common block access
c     Borut Paul Kersevan (February 2003)
      FVOIDP FUNCTION FFPHOLUN_ADDRESS() 
      IMPLICIT NONE
#include "Photos_i/ffpholun.inc"
      FVOIDP GETADDR
      FFPHOLUN_ADDRESS = GETADDR(PHLUN)
      RETURN
      END
