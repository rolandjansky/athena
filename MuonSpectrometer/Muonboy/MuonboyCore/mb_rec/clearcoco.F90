!
!> Clear the content of muonboy modules before processing an event
!
 SUBROUTINE CLEARCOCO(IdEve)
 USE M_MB_Control, ONLY : RecovDead
 USE M_MB_EventInfo
 USE M_MB_Digis
 USE M_MB_InfoSim
 USE M_MB_MuGeom
 IMPLICIT  NONE
 INTEGER, INTENT(IN) :: IdEve
!
   ievt = IdEve
   CALL Reset_MB_Digis
   CALL Reset_MB_InfoSim
   IF( RecovDead ) CALL Reset_RecovDead
!
 END SUBROUTINE CLEARCOCO
!
