!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE TRANSLAT_VOL( Name, VV )
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 CHARACTER (len=*), INTENT(IN) :: Name
 REAL(8),           INTENT(IN) :: VV(*)
 INTEGER              :: IS0,IV0,Jvolclo,IP,IE,IA,K,NADMI,NADMA
 INTEGER              :: Npltot,Nedtot
 INTEGER, ALLOCATABLE :: JAA(:)
 REAL(8)              :: VLOC(3),V3(3)
!
   DO IS0=1,Nsuvo
     DO IV0=NvoluI(IS0),NvoluA(IS0)
       IF( CHAVOL(IV0) == Name ) GO TO 10
     ENDDO
   ENDDO
   WRITE(*,'(" In TRANSLAT_VOL : Volume ",A,"  does not exist !")') TRIM(Name)
   RETURN
!
10 VLOC(1:3) = VV(1:3)
   IF( I_Atlas_Ref_Syst >= 1 ) THEN
     VLOC(2) = - VV(3)
     VLOC(3) =   VV(2)
   ENDIF
   Jvolclo = IclVolu(IV0)
   CALL NEWVOL(Name,Jvolclo)
!
   CenVolu(1:3,Nvolu) = CenVolu(1:3,IV0) + VLOC(1:3)
   D4Volu(1:M4,Nvolu) = D4Volu(1:M4,IV0)
!
   NADMI = Nadge
   NADMA =     1
   DO IP=NplanI(1,IV0),NplanA(M4,IV0)
     DO IE=NedgeI(IP),NedgeA(IP)
       IA = IadEdge(IE)
       NADMI = MIN( NADMI , IA )
       NADMA = MAX( NADMA , IA )
     ENDDO
   ENDDO
   ALLOCATE( JAA(NADMI:NADMA) )
   JAA(NADMI:NADMA) = 0
!
   Npltot = NplanA(M4,IV0) - NplanI(1,IV0) + 1
   Nedtot = NedgeA(NplanA(M4,IV0)) - NedgeI(NplanI(1,IV0)) + 1
   IF( SIZE(NedgeI)    < Nplan+Npltot ) CALL Push_MB_Gen_Vol_MPLAN
   IF( SIZE(IvuEdge)   < Nedge+Nedtot ) CALL Push_MB_Gen_Vol_MEDGE
   IF( SIZE(XYZadge)/3 < Nadge+Nedtot ) CALL Push_MB_Gen_Vol_MADGE
   DO IP=NplanI(1,IV0),NplanA(M4,IV0)
     Nplan = Nplan + 1
     IcoPlan(Nplan)  = IcoPlan(IP)
     NedgeI(Nplan)   = Nedge + 1
     DO IE=NedgeI(IP),NedgeA(IP)
       IA = IadEdge(IE)
       IF( JAA(IA) < 1 ) THEN
         V3(1:3) = XYZadge(1:3,IA) + VLOC(1:3)
         Nadge = Nadge + 1
         XYZadge(1:3,Nadge) = V3(1:3)
         JAA(IA) = Nadge
       ENDIF
       Nedge = Nedge + 1
       IvuEdge(Nedge) = IvuEdge(IE)
       IadEdge(Nedge) = JAA(IA)
     ENDDO
     NedgeA(Nplan)   = Nedge
   ENDDO
!
   DEALLOCATE( JAA )
!
   CALL ENDVOL_SUV
!
   DO K=1,M4
     NplanI(K,Nvolu) = NplanI(K,IV0) + NplanI(1,Nvolu)-NplanI(1,IV0)
     NplanA(K,Nvolu) = NplanA(K,IV0) + NplanI(1,Nvolu)-NplanI(1,IV0)
   ENDDO
!
   CALL ELIM_VOL_IV(IV0)
!
 END
