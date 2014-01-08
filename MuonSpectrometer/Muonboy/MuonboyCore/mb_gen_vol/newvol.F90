!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE NEWVOL(Name,Jvolclo)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
 INTEGER           :: Jvolclo
!
   IF( SIZE(K4Volu) < Nvolu+1 ) CALL Push_MB_Gen_Vol_MVOLU
   Nvolu = Nvolu + 1
   CALL Set_CHAVOL(Nvolu,Name)
   IclVolu(    Nvolu) = Jvolclo
   NplanI(1:M4,Nvolu) = Nplan + 1
   NplanA(1:M4,Nvolu) = Nplan
   D4Volu(1:M4,Nvolu) = 999999999.D0
   D4Volu( M4 ,Nvolu) =         1.D0
   K4Volu(     Nvolu) = 1
!
 END
