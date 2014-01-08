!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE BOOLMAT( K1,K2, IV1,IV2, AAA,IMANAT,JFF,JZZ, IVC )
 USE M_MB_COMATI
 IMPLICIT NONE
 CHARACTER(8) :: AAA
 INTEGER      :: K1,K2, IV1,IV2, IMANAT,JFF,JZZ, IVC
 INTEGER :: Itry, Ndim_Volu, Ndim_Plan, Ndim_Edge, IP, Ioverf
!
   Itry = 0
   Ndim_Volu = 4
!
   Ndim_Plan = NPMAT1(IV1)-NPMAT0(IV1)+1 + NPMAT1(IV2)-NPMAT0(IV2)+1
   Ndim_Plan = 3 * Ndim_Plan
!
   Ndim_Edge = 0
   DO IP=NPMAT0(IV1),NPMAT1(IV1)
     Ndim_Edge = Ndim_Edge + NEMAT1(IP)-NEMAT0(IP)+1
   ENDDO
   DO IP=NPMAT0(IV2),NPMAT1(IV2)
     Ndim_Edge = Ndim_Edge + NEMAT1(IP)-NEMAT0(IP)+1
   ENDDO
   Ndim_Edge = 3 * Ndim_Edge
!
10 CALL BOOLMAT_DIM( Ndim_Volu,Ndim_Plan,Ndim_Edge, Ioverf,  &
                     K1,K2, IV1,IV2, AAA,IMANAT,JFF,JZZ, IVC )
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
