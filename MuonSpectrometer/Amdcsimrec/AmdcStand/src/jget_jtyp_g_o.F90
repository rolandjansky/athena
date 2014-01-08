!
 INTEGER FUNCTION JGET_JTYP_G_O(JTYP,JGEO,JOB)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: JTYP, JGEO, JOB
!
#include "AmdcStand/amdcsim_com.inc"
!
      IF( JTYP_G_O(JTYP,JGEO,JOB) <= 0 ) THEN
         NB_JTYP_G_O = NB_JTYP_G_O + 1
         IF( NB_JTYP_G_O > MTYP_G_O ) THEN
            PRINT 7777,MTYP_G_O
7777        FORMAT(/' ===> While reading the AMDC_SIMREC data base',   &
                   /' ===> One has reach the limit MTYP_G_O =',I7,     &
                   /' ====================>  STOP !')
            STOP
         ENDIF
         JTYP_G_O(JTYP,JGEO,JOB) = NB_JTYP_G_O
      ENDIF
!
      JGET_JTYP_G_O = JTYP_G_O(JTYP,JGEO,JOB)
!
 END FUNCTION JGET_JTYP_G_O
!
