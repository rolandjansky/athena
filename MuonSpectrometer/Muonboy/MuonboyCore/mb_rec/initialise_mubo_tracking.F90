!
 SUBROUTINE Initialise_Mubo_Tracking
 USE M_MB_PRECIS
 IMPLICIT NONE
!
!>> Initialisation of Module /PRECIS/ <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!>> For tracking in mag. field  e.g. :
!>> maximum step of tracking (in cm), tracking precision (in Tesla and cm), ...
   BBNUL   = 0.00050D0
   BBMIN   = 0.01000D0
   STEP(1) =    20.D0
   STEP(2) =     6.D0
   STEP(3) =    25.D0
   STEP(4) =   100.D0
   PASMIN  =  1.600D0
!************************************************
   FACREF  = 30.000D0
   FACMIN  =  0.200D0
   FACMAX  =  3.000D0
!************************************************
   DBMAX   = 0.300D0
   DBMIN   = 0.120D0
!************************************************
 END SUBROUTINE Initialise_Mubo_Tracking
!
