!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE BOOLMAT_DIM( Ndim_Volu,Ndim_Plan,Ndim_Edge, Ioverf, K1,K2, IV1,IV2, AAA,IMANAT,JFF,JZZ, IVC )
 USE M_MB_COMATI
 USE M_MB_General_CONSTANTS, ONLY : EPSBOO
 IMPLICIT NONE
 CHARACTER(8) :: AAA
 INTEGER :: Ndim_Volu, Ndim_Plan, Ndim_Edge, Ioverf, K1,K2, IV1,IV2, IMANAT,JFF,JZZ, IVC
 INTEGER :: Nplan(2,Ndim_Volu)
 INTEGER :: Icolo(  Ndim_Plan)
 INTEGER :: Nedge(2,Ndim_Plan)
 REAL(8) :: Ptxyz(3,Ndim_Edge)
 INTEGER :: Iseen(  Ndim_Edge)
 INTEGER :: J, Nplan_Max, Nedge_Max, Ip, Ie, IA, IV, NAMATI_DEB
!
   IVC = 0
!
!>> Transfer the two volumes input of the boolean operation, into the
!>> argument of boolean operation routine
   Nplan_Max = 0
   Nedge_Max = 0
   DO J=1,2
     IF( J == 1 ) THEN
       IV = IV1
     ELSE
       IV = IV2
     ENDIF
     Nplan(1,J)           = Nplan_Max + 1
     DO IP=NPMAT0(IV),NPMAT1(IV)
       Nplan_Max = Nplan_Max + 1
       Icolo(  Nplan_Max)  = 1
       Nedge(1,Nplan_Max)  = Nedge_Max + 1
       DO IE=NEMAT0(IP),NEMAT1(IP)
         Nedge_Max = Nedge_Max +1
         CALL GETXYZMAT( IV, IE, Ptxyz(1,Nedge_Max) )
         Iseen(Nedge_Max) = 1
       ENDDO
       Nedge(2,Nplan_Max)  = Nedge_Max
     ENDDO
     Nplan(2,J)           = Nplan_Max
   ENDDO
!
!>> Perform boolean operation
   CALL Bool_Oper_Vol( Ndim_Volu,Ndim_Plan,Ndim_Edge, Ioverf,  &
                       K1,K2, 0, Nplan,Icolo,Nedge,Ptxyz,Iseen )
   IF( Ioverf >= 1 ) RETURN
!
!>> Put volume result of boolean operation into the matter bank
   NAMATI_DEB = NAMATI + 1
   IF( Nplan(2,3)-Nplan(1,3)+1 >= 4 ) THEN
     CALL NEWMAT( AAA, IMANAT, JFF, JZZ )
     DO Ip=Nplan(1,3),Nplan(2,3)
       NPMATI = NPMATI + 1
       NEMAT0(NPMATI)  = NEMATI + 1
       DO Ie=Nedge(1,Ip),Nedge(2,Ip)
         NEMATI = NEMATI + 1
         DO IA=NAMATI_DEB,NAMATI
           IF(   ABS(XYZMATI(1,IA)-Ptxyz(1,Ie)) < EPSBOO .AND.  &
                 ABS(XYZMATI(2,IA)-Ptxyz(2,Ie)) < EPSBOO .AND.  &
                 ABS(XYZMATI(3,IA)-Ptxyz(3,Ie)) < EPSBOO ) GO TO 100
         ENDDO
         NAMATI = NAMATI + 1
         IA = NAMATI
         XYZMATI(1:3,IA) = Ptxyz(1:3,Ie)
100      IAMATI(NEMATI)     = IA
       ENDDO
       NEMAT1(NPMATI)  = NEMATI
     ENDDO
     CALL ENDMAT
     IF( IV2 > IV1 ) THEN
       CALL ELIMAT( IV2 )
       CALL ELIMAT( IV1 )
     ELSE
       CALL ELIMAT( IV1 )
       CALL ELIMAT( IV2 )
     ENDIF
     IVC = NVMATI
   ENDIF
!
 END
