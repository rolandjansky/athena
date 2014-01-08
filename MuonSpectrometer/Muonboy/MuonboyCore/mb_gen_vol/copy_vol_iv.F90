!
 SUBROUTINE COPY_VOL_IV( IV0, Name,Jvolclo,Jcolor, IVC )
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
 INTEGER           :: IV0,Jvolclo,Jcolor,IVC
 INTEGER  ::  Nadge_DEB,IP0,IE0,IA0,IA,K,Npltot,Nedtot
!
   Nadge_DEB = Nadge + 1
!>> Create new volume
   CALL NEWVOL(Name,Jvolclo)
   IVC = Nvolu
   CenVolu(1:3,IVC) = CenVolu(1:3,IV0)
   D4Volu(1:M4,IVC) = D4Volu(1:M4,IV0)
   Npltot = NplanA(M4,IV0) - NplanI(1,IV0) + 1
   Nedtot = NedgeA(NplanA(M4,IV0)) - NedgeI(NplanI(1,IV0)) + 1
   IF( SIZE(NedgeI)    < Nplan+Npltot ) CALL Push_MB_Gen_Vol_MPLAN
   IF( SIZE(IvuEdge)   < Nedge+Nedtot ) CALL Push_MB_Gen_Vol_MEDGE
   IF( SIZE(XYZadge)/3 < Nadge+Nedtot ) CALL Push_MB_Gen_Vol_MADGE
   DO IP0=NplanI(1,IV0),NplanA(M4,IV0)
     Nplan = Nplan + 1
     IF( Jcolor <= 0 ) THEN
       IcoPlan(Nplan) = IcoPlan(IP0)
     ELSE
       IcoPlan(Nplan) = Jcolor
     ENDIF
     NedgeI(Nplan) = Nedge + 1
     DO IE0=NedgeI(IP0),NedgeA(IP0)
       IA0 = IadEdge(IE0)
       DO IA=Nadge_DEB,Nadge
         IF( ABS(XYZadge(1,IA)-XYZadge(1,IA0)) < Eps .AND.  &
             ABS(XYZadge(2,IA)-XYZadge(2,IA0)) < Eps .AND.  &
             ABS(XYZadge(3,IA)-XYZadge(3,IA0)) < Eps )  GO TO 100
       ENDDO
       Nadge = Nadge + 1
       IA = Nadge
       XYZadge(1:3,IA) = XYZadge(1:3,IA0)
100    Nedge = Nedge + 1
       IvuEdge(Nedge) = IvuEdge(IE0)
       IadEdge(Nedge) = IA
     ENDDO
     NedgeA(Nplan) = Nedge
   ENDDO
   CALL ENDVOL_SUV
!
   DO K=1,M4
     NplanI(K,IVC) = NplanI(K,IV0) + NplanI(1,IVC)-NplanI(1,IV0)
     NplanA(K,IVC) = NplanA(K,IV0) + NplanI(1,IVC)-NplanI(1,IV0)
   ENDDO
!
 END SUBROUTINE COPY_VOL_IV
!
