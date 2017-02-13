C     AcerMC acset common block access
c     Borut Paul Kersevan (February 2003)
      FVOIDP FUNCTION ACERMC_ACSET_ADDRESS() 
      IMPLICIT NONE
#include "AcerMC_i/AcerMC_acset.inc"
      FVOIDP GETADDR
      ACERMC_ACSET_ADDRESS = GETADDR(ACSET12)
      RETURN
      END
