!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE NEWsubVOL(Ksub)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ksub
!
   NplanI(Ksub:M4,Nvolu) = Nplan + 1
   NplanA(Ksub:M4,Nvolu) = Nplan
!
 END
