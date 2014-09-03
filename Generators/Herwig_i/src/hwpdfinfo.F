      SUBROUTINE HWPDFINFO
C     Store the arguments to the PDF call at the appropriate stage of
C     the event

#include "HERWIG65.INC"
#include "GeneratorFortranCommon/hepeup.inc"

      DOUBLE PRECISION X1,X2,Q
      INTEGER ID1,ID2,IDPDF1,IDPDF2
      COMMON /HRWPDF/ X1,X2,Q,ID1,ID2,IDPDF1,IDPDF2

      x1=(phep(4,4)+abs(phep(3,4)))/(phep(4,1)+abs(phep(3,1)))
      x2=(phep(4,5)+abs(phep(3,5)))/(phep(4,2)+abs(phep(3,2)))
      IF (iproc.GE.0) THEN
         q=emsca
      ELSE
         q=scalup
      ENDIF
      id1=idpdg(idhw(4))
      id2=idpdg(idhw(5))
      idpdf1=modpdf(1)
      idpdf2=modpdf(2)

      RETURN
      END
