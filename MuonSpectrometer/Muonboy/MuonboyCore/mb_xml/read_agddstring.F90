!
 SUBROUTINE Read_AgddString(ipos,lend,car480)
 IMPLICIT NONE
 INTEGER,        INTENT(INOUT) :: ipos
 LOGICAL,        INTENT(OUT)   :: lend
 CHARACTER(480), INTENT(OUT)   :: car480
 INTEGER :: imax, icar, ncar, j
!
#include "AmdcStand/amdcsim_com.inc"
!
 imax = ipos + 480
 IF( imax > LenAGDD ) imax = LenAgdd
 icar = imax
 ncar = imax - ipos
 DO j=ipos+1,imax
   IF( ICHAR(CarAGDD(j)) == 10 ) THEN
     icar = j
     ncar = j - 1 - ipos
     EXIT
   ENDIF
 ENDDO
 car480 = ''
 FORALL (j=1:ncar) car480(j:j) = CarAGDD(ipos+j)
 ipos = icar
 lend = .false.
 if( icar == LenAGDD ) lend = .true.
!
 END SUBROUTINE Read_AgddString
!
