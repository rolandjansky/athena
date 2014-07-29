C     Tauola FFTAUDET common block access
c     Eric Torrence (August 2009)
c     from Borut Paul Kersevan (February 2003)
      FVOIDP FUNCTION FFTAUDET_ADDRESS() 
      IMPLICIT NONE
#include "Tauola_i/taudet.inc"
      FVOIDP GETADDR
      FFTAUDET_ADDRESS = GETADDR(ITNP)
      RETURN
      END
