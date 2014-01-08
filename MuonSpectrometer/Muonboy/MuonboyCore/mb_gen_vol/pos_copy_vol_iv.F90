!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE POS_COPY_VOL_IV(Iv0, Name,Jvolclo,Jcolor,TRA,ROT, IvC)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
 INTEGER           :: Iv0,Jvolclo,Jcolor,IvC
 REAL(8)           :: TRA(3),ROT(3,3)
 INTEGER :: J,IEMI,IEMA,IP,IE,IA,IPNEW,IENEW,IANEW,IE_EQU,K
 INTEGER :: Npltot,Nedtot
 REAL(8) :: VV(3),UU(3)
!
   J = Jvolclo
   IF( Jvolclo == 0 ) J = IclVolu(Iv0)
   CALL NEWVOL(Name,J)
!
   IvC = Nvolu
   SELECT CASE( I_Atlas_Ref_Syst )
     CASE(  1 )
!>> Axis : X  lateral (horizontal)    =  + X  of ATLAS Coordinates
!>>        Y  beam axis (horizontal)  =  - Z  of ATLAS Coordinates (!!!)
!>>        Z  vertical (pointing up)  =  + Y  of ATLAS Coordinates
       CenVolu( 1 ,IvC) =   CenVolu( 1 ,Iv0) + TRA( 1 )
       CenVolu( 2 ,IvC) = - CenVolu( 3 ,Iv0) - TRA( 3 )
       CenVolu( 3 ,IvC) =   CenVolu( 2 ,Iv0) + TRA( 2 )
     CASE DEFAULT
       CenVolu(1:3,IvC) =   CenVolu(1:3,Iv0) + TRA(1:3)
   END SELECT
   D4Volu(1:M4,IvC) = D4Volu(1:M4,Iv0)
!
   IEMI = Nedge
   IEMA = 1
   DO IP=NplanI(1,Iv0),NplanA(M4,Iv0)
     DO IE=NedgeI(IP),NedgeA(IP)
       IEMI = MIN( IEMI , IE )
       IEMA = MAX( IEMA , IE )
     ENDDO
   ENDDO
!
   Npltot = NplanA(M4,Iv0) - NplanI(1,Iv0) + 1
   IF( SIZE(NedgeI)    < Nplan+Npltot ) CALL Push_MB_Gen_Vol_MPLAN
   DO IP=NplanI(1,Iv0),NplanA(M4,Iv0)
     IPNEW = Nplan + IP - NplanI(1,Iv0) + 1
     IF( Jcolor <= 0 ) THEN
       IcoPlan(IPNEW) = IcoPlan(IP)
     ELSE
       IcoPlan(IPNEW) = Jcolor
     ENDIF
     NedgeI(IPNEW) = Nedge + NedgeI(IP) - IEMI + 1
     NedgeA(IPNEW) = Nedge + NedgeA(IP) - IEMI + 1
   ENDDO
   Nplan = IPNEW
!
   IENEW = Nedge
   IANEW = Nadge
   Nedtot = IEMA - IEMI + 1
   IF( SIZE(IvuEdge)   < Nedge+Nedtot ) CALL Push_MB_Gen_Vol_MEDGE
   IF( SIZE(XYZadge)/3 < Nadge+Nedtot ) CALL Push_MB_Gen_Vol_MADGE
   DO 20 IE=IEMI,IEMA
     IA = IadEdge(IE)
     IENEW = IENEW + 1
     IvuEdge(IENEW) = IvuEdge(IE)
     DO IE_EQU=IEMI,IE-1
       IF( IadEdge(IE_EQU) == IA ) GO TO 10
     ENDDO
     IANEW = IANEW + 1
     VV(1:3) = XYZadge(1:3,IA) - CenVolu(1:3,Iv0)
     CALL MULT3M3V(ROT,VV, UU)
     SELECT CASE( I_Atlas_Ref_Syst )
       CASE(  1 )
         XYZadge( 1 ,IANEW) =  UU( 1 ) + CenVolu( 1 ,IvC)
         XYZadge( 2 ,IANEW) = -UU( 3 ) + CenVolu( 2 ,IvC)
         XYZadge( 3 ,IANEW) =  UU( 2 ) + CenVolu( 3 ,IvC)
       CASE DEFAULT
         XYZadge(1:3,IANEW) =  UU(1:3) + CenVolu(1:3,IvC)
     END SELECT
     IadEdge(IENEW) = IANEW
     CYCLE
10   IadEdge(IENEW) = IadEdge(Nedge + IE_EQU - IEMI + 1)
20 ENDDO
   Nedge = IENEW
   Nadge = IANEW
!
   CALL ENDVOL_SUV
!
   DO K=1,M4
     NplanI(K,IvC) = NplanI(K,IV0) + NplanI(1,IvC)-NplanI(1,IV0)
     NplanA(K,IvC) = NplanA(K,IV0) + NplanI(1,IvC)-NplanI(1,IV0)
   ENDDO
!
 END
