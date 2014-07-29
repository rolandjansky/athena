C     Tauola FFTAU common block access
c     Borut Paul Kersevan (February 2003)
      FVOIDP FUNCTION FFTAU_ADDRESS() 
      IMPLICIT NONE
#include "Tauola_i/fftau.inc"
      FVOIDP GETADDR
      FFTAU_ADDRESS = GETADDR(IFPOL)
      RETURN
      END
