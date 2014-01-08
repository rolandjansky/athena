!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GET_CENVOL_IV(IV,X0,Y0,Z0)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IV
 REAL(8), INTENT(OUT) :: X0,Y0,Z0
!
   IF( IV >= 1 .AND. IV <= Nvolu ) THEN
     X0 = CenVolu(1,IV)
     Y0 = CenVolu(2,IV)
     Z0 = CenVolu(3,IV)
   ELSE
     X0 = 0.D0
     Y0 = 0.D0
     Z0 = 0.D0
   ENDIF
!
 END
