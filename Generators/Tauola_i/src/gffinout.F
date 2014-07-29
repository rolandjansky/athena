C     Tauola INOUT common block access
c     Borut Paul Kersevan (February 2003)
      FVOIDP FUNCTION FFINOUT_ADDRESS() 
      IMPLICIT NONE
#include "Tauola_i/ffinout.inc"
      FVOIDP GETADDR
      FFINOUT_ADDRESS = GETADDR(INUT)
      RETURN
      END
