!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE VOLABOOLB_IV(IBOOL1,IBOOL2, IV1,IV2,Name,Jcolor, IVC)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
 INTEGER           :: IBOOL1,IBOOL2,IV1,IV2,Jcolor,IVC
 INTEGER :: Itry,IP,Ioverf,Ndim_Volu
 INTEGER :: Ndim_Plan1,Ndim_Plan2,Ndim_Plan
 INTEGER :: Ndim_Edge1,Ndim_Edge2,Ndim_Edge
!
   Itry = 0
   Ndim_Volu = 4
!
   Ndim_Plan1 = NplanA(1,IV1) - NplanI(1,IV1)+1
   Ndim_Plan2 = NplanA(1,IV2) - NplanI(1,IV2)+1
   Ndim_Plan = 3*Ndim_Plan1 + 3*Ndim_Plan2
!
   Ndim_Edge1 = 0
   DO IP=NplanI(1,IV1),NplanA(1,IV1)
     Ndim_Edge1 = Ndim_Edge1 + NedgeA(IP)-NedgeI(IP)+1
   ENDDO
   Ndim_Edge2 = 0
   DO IP=NplanI(1,IV2),NplanA(1,IV2)
     Ndim_Edge2 = Ndim_Edge2 + NedgeA(IP)-NedgeI(IP)+1
   ENDDO
   Ndim_Edge = 3*Ndim_Edge1 + 3*Ndim_Edge2
!
10 CONTINUE
   CALL VOLABOOLB_IV_DIM( Ndim_Volu,Ndim_Plan,Ndim_Edge, Ioverf,  &
                          IBOOL1,IBOOL2, IV1,IV2,Name,Jcolor, IVC)
   IF( Ioverf >= 1 .AND. Ioverf <= 3 .AND. Itry <= 10 ) THEN
     Itry = Itry + 1
     IF( Ioverf == 1 ) THEN
       Ndim_Volu = 2*Ndim_Volu
     ELSEIF( Ioverf == 2) THEN
       Ndim_Plan = 2*Ndim_Plan
     ELSE
       Ndim_Edge = 2*Ndim_Edge
     ENDIF
     GO TO 10
   ENDIF
!
 END
