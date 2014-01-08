!
 SUBROUTINE loadalinesfake
 IMPLICIT NONE
#include "AmdcStand/amdcsim_com.inc"
!
 INTEGER :: JTYP, JFF, JZZ, JOB
 REAL(8) :: DataAline(6), tran, rota
 INTEGER :: ier
 REAL    :: rand
 LOGICAL, SAVE :: FIRST=.TRUE.
 INTEGER(KIND=4) SEED
 
 IF( FIRST ) THEN
   FIRST=.FALSE.
   seed = 1234
   call srand(seed) 
 endif
   
 JTYP = 0 
 JFF  = 0 
 JZZ  = 0 
 JOB  = 0
 CALL setalines (JTYP, JFF, JZZ, JOB, DataAline, ier)

 tran = 0.050d0 * sqrt(12.d0)  !< unit is mm
 do jtyp=1,mtyp
   IF( CARTYP(JTYP)(1:1) == 'B' ) THEN
     rota = 0.00010d0 * sqrt(12.d0)
   ELSE IF( CARTYP(JTYP)(1:1) == 'E' ) THEN
     rota = 0.00015d0 * sqrt(12.d0)
   ELSE
     cycle
   ENDIF
   do jff=1,8
     do jzz=-mzz,mzz
       DataAline(1) = ( rand(0) - 0.5d0 ) * tran  
       DataAline(2) = ( rand(0) - 0.5d0 ) * tran   
       DataAline(3) = ( rand(0) - 0.5d0 ) * tran  
       DataAline(4) = ( rand(0) - 0.5d0 ) * rota
       DataAline(5) = ( rand(0) - 0.5d0 ) * rota 
       DataAline(6) = ( rand(0) - 0.5d0 ) * rota
       CALL setalines (JTYP, JFF, JZZ, JOB, DataAline, ier)
       if( ier /= 0 ) then
         write(*,*) ' loadalinesfake : increase Madjust '
         stop
       endif  
     enddo
   enddo
 enddo
 
 END SUBROUTINE loadalinesfake
!
