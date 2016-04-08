      SUBROUTINE WRITELHEF(retval)

#include "ProtosLHEF_i/Protos.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"

      IF (XWGTUP .NE. 1) THEN
         IF (NUP .EQ. 0) THEN
            retval = 1
         ELSE
            PRINT *, " WARNING :: ProtosLHEF_i :: EVENT WEIGHT different from +1 "
         ENDIF
      ELSE
         WRITE (24,1000) '<event>'
         WRITE (24,1001) NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP
         DO i=1,NUP
            WRITE (24,1002,advance='no') IDUP(i),ISTUP(i),MOTHUP(1,i),MOTHUP(2,i),ICOLUP(1,i),ICOLUP(2,i),PUP(1,i),PUP(2,i),PUP(3,i)
            WRITE (24,1003) PUP(4,i),PUP(5,i),VTIMUP(i),SPINUP(i)
         ENDDO
         WRITE (24,1000) '</event>'
      ENDIF

C -------------------------------------------------------

 1000  FORMAT (A)
 1001  FORMAT ('  ',I5,'  ',I5,'  ',F8.3,'  ',F8.3,'  ',F8.3,'  ',F8.3)
 1002  FORMAT ('  ',I5,'  ',I5,'  ',I3,'  ',I3,'  ',I3,'  ',I3,'  ',F15.9,'  ',F15.9,'  ',F15.9)
 1003  FORMAT ('  ',F15.9,'  ',F15.9,'  ',F15.9,'  ',F15.9)
 
      END
