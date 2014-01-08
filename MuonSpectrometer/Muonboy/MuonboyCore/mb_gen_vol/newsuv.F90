!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE NEWSUV(Name)
 USE M_MB_Increm_Index
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
!
   CALL Increm_Index(Nsuvo,MSUVO,'MSUVO')
   CALL Set_CHASUV(Nsuvo,Name)
   NvoluI(Nsuvo) = Nvolu + 1
   NvoluA(Nsuvo) = Nvolu
!
 END
