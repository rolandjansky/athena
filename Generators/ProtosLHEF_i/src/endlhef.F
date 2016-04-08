      SUBROUTINE ENDLHEF
      IMPLICIT NONE

#include "ProtosLHEF_i/Protos.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"


!    End Lhef file

      WRITE (24,1000) '</LesHouchesEvents>'
      CLOSE(24)

 1000  FORMAT (A)

      END
