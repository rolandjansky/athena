      SUBROUTINE VETOPROTOS(IVETO)
      IMPLICIT NONE

      INTEGER IVETO

#include "ProtosLHEF_i/Protos.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"


!     Event record

      INTEGER N,NPAD,K
      REAL*8 P,V
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)

!     Local

      INTEGER IDB,ib
      REAL*8 Pb(0:3),PTb

!     ----------------------------------------------------

      IVETO=0
      IF ((IHRD .NE. 3) .OR. (IMATCH .NE. 2)) RETURN

      IDB=IDUP(2)
      ib=1

      DO WHILE ((K(ib,2) .NE. -IDB) .OR. (K(ib,1) .NE. 3))
        ib=ib+1
        IF (ib .GT. N) RETURN
      ENDDO

      Pb(0) = P(ib,4)
      Pb(1) = P(ib,1)
      Pb(2) = P(ib,2)
      Pb(3) = P(ib,3)

      PTb=SQRT(Pb(1)**2+Pb(2)**2)
      IF (PTb .GT. PTbmax) IVETO = 1

      RETURN

      END

