!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE VOLABOOLB_IV_DIM( Ndim_Volu,Ndim_Plan,Ndim_Edge, Ioverf, IBOOL1,IBOOL2, IV1,IV2,Name,Jcolor, IVC)
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
 INTEGER           :: Ndim_Volu,Ndim_Plan,Ndim_Edge
 INTEGER           :: Ioverf, IBOOL1,IBOOL2,IV1,IV2,Jcolor,IVC
 INTEGER :: Nplan_Bmax,Nedge_Bmax,J,IV,IP,IE,IA,Nadge_DEB
 INTEGER :: Nplan_B(2,Ndim_Volu)
 INTEGER :: Icolo_B(  Ndim_Plan)
 INTEGER :: Nedge_B(2,Ndim_Plan)
 INTEGER :: Iseen_B(  Ndim_Edge)
 REAL(8) :: Ptxyz_B(3,Ndim_Edge)
!
   IVC = 0
   Nplan_Bmax = 0
   Nedge_Bmax = 0
!
   DO J=1,2
     IF( J == 1 ) THEN
       IV = IV1
     ELSE
       IV = IV2
     ENDIF
     Nplan_B(1,J)             = Nplan_Bmax + 1
     DO IP=NplanI(1,IV),NplanA(1,IV)
       Nplan_Bmax = Nplan_Bmax + 1
       Icolo_B(  Nplan_Bmax)  = IcoPlan(IP)
       Nedge_B(1,Nplan_Bmax)  = Nedge_Bmax + 1
       DO IE=NedgeI(IP),NedgeA(IP)
         IA = IadEdge(IE)
         Nedge_Bmax = Nedge_Bmax +1
         Ptxyz_B(1,Nedge_Bmax) = XYZadge(1,IA)
         Ptxyz_B(2,Nedge_Bmax) = XYZadge(2,IA)
         Ptxyz_B(3,Nedge_Bmax) = XYZadge(3,IA)
         Iseen_B(  Nedge_Bmax) = IvuEdge(IE)
       ENDDO
       Nedge_B(2,Nplan_Bmax)  = Nedge_Bmax
     ENDDO
     Nplan_B(2,J)            = Nplan_Bmax
   ENDDO
!
   CALL Bool_Oper_Vol( Ndim_Volu,Ndim_Plan,Ndim_Edge, Ioverf,  &
                       IBOOL1,IBOOL2, Jcolor, Nplan_B,Icolo_B,Nedge_B,Ptxyz_B,Iseen_B )
   IF( Ioverf >= 1 ) RETURN
!
   IF( SIZE(NedgeI)    < Nplan+Ndim_Plan ) CALL Push_MB_Gen_Vol_MPLAN
   IF( SIZE(IvuEdge)   < Nedge+Ndim_Edge ) CALL Push_MB_Gen_Vol_MEDGE
   IF( SIZE(XYZadge)/3 < Nadge+Ndim_Edge ) CALL Push_MB_Gen_Vol_MADGE
   Nadge_DEB = Nadge + 1
   IF( Nplan_B(2,3)-Nplan_B(1,3) >= 3 ) THEN
     CALL NEWVOL(Name,1)
     CenVolu(1:3,Nvolu) = CenVolu(1:3,IV1)
     D4Volu(1:M4,Nvolu) = D4Volu(1:M4,IV1)
     DO IP=Nplan_B(1,3),Nplan_B(2,3)
       Nplan = Nplan + 1
       IcoPlan(Nplan)      = Icolo_B(IP)
       NedgeI( Nplan)      = Nedge + 1
       DO IE=Nedge_B(1,IP),Nedge_B(2,IP)
         Nedge = Nedge + 1
         DO IA=Nadge_DEB,Nadge
           IF( ABS(XYZadge(1,IA)-Ptxyz_B(1,IE)) < Eps .AND.  &
               ABS(XYZadge(2,IA)-Ptxyz_B(2,IE)) < Eps .AND.  &
               ABS(XYZadge(3,IA)-Ptxyz_B(3,IE)) < Eps ) GO TO 100
         ENDDO
         Nadge = Nadge + 1
         IA = Nadge
         XYZadge(1:3,IA) = Ptxyz_B(1:3,IE)
100      IadEdge(Nedge) = IA
         IvuEdge(Nedge) = Iseen_B(IE)
       ENDDO
       NedgeA(Nplan)       = Nedge
     ENDDO
     CALL ENDVOL_SUV
     IVC = Nvolu
   ENDIF
!
 END
