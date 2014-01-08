!
 INTEGER FUNCTION JGET_JTYP_F_Z(JTYP,JFF,JZZ)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: JTYP, JFF, JZZ
!
#include "AmdcStand/amdcsim_com.inc"
!
      IF( JTYP_F_Z(JTYP,JFF,JZZ) <= 0 ) THEN
         NB_JTYP_F_Z = NB_JTYP_F_Z + 1
         IF( NB_JTYP_F_Z > MTYP_F_Z ) THEN
            PRINT 7777,MTYP_F_Z
7777        FORMAT(/' ===> While reading the AMDC_SIMREC data base',   &
                   /' ===> One has reach the limit MTYP_F_Z =',I7,     &
                   /' ====================>  STOP !')
            STOP
         ENDIF
         JTYP_F_Z(JTYP,JFF,JZZ) = NB_JTYP_F_Z
      ENDIF
!
      JGET_JTYP_F_Z = JTYP_F_Z(JTYP,JFF,JZZ)
!
 END FUNCTION JGET_JTYP_F_Z
!
