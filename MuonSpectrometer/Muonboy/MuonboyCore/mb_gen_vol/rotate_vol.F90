!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE ROTATE_VOL( Name, Irelatif, Ixyz, Angle )
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Irelatif,Ixyz
 REAL(8),           INTENT(IN) :: Angle
 INTEGER              :: IS0,IV0,Jvolclo,IP,IE,IA,K,NADMI,NADMA
 INTEGER              :: IxyzLoc,Npltot,Nedtot
 INTEGER, ALLOCATABLE :: JAA(:)
 REAL(8)              :: X,Y,Z,XC,YC,ZC,AngleLoc,SINA,COSA
 REAL(8)              :: V3(3)
!
   DO IS0=1,Nsuvo
     DO IV0=NvoluI(IS0),NvoluA(IS0)
       IF( CHAVOL(IV0) == Name ) GO TO 10
     ENDDO
   ENDDO
   PRINT 1000,TRIM(Name)
   RETURN
!
1000 FORMAT(' In ROTATE_VOL : Volume ',A,'  does not exist !')
!
10 IxyzLoc  = Ixyz
   AngleLoc = Angle
   IF( I_Atlas_Ref_Syst >= 1 ) THEN
     IF( Ixyz == 2 ) THEN
       IxyzLoc  = 3
     ELSEIF( Ixyz == 3 ) THEN
       IxyzLoc  = 2
       AngleLoc = - Angle
     ENDIF
   ENDIF
   Jvolclo = IclVolu(IV0)
   CALL NEWVOL(Name,Jvolclo)
!
   SINA = SIN( AngleLoc * PiS180 )
   COSA = COS( AngleLoc * PiS180 )
   IF( Irelatif == 1 ) THEN
     XC = CenVolu(1,IV0)
     YC = CenVolu(2,IV0)
     ZC = CenVolu(3,IV0)
     CenVolu(1:3,Nvolu) = CenVolu(1:3,IV0)
   ELSE
     XC = 0.D0
     YC = 0.D0
     ZC = 0.D0
     IF( IxyzLoc == 1 ) THEN
       CenVolu(1,Nvolu) =  CenVolu(1,IV0)
       CenVolu(2,Nvolu) =  CenVolu(2,IV0)*COSA -CenVolu(3,IV0)*SINA
       CenVolu(3,Nvolu) =  CenVolu(2,IV0)*SINA +CenVolu(3,IV0)*COSA
     ELSEIF( IxyzLoc == 2 ) THEN
       CenVolu(1,Nvolu) =  CenVolu(1,IV0)*COSA +CenVolu(3,IV0)*SINA
       CenVolu(2,Nvolu) =  CenVolu(2,IV0)
       CenVolu(3,Nvolu) = -CenVolu(1,IV0)*SINA +CenVolu(3,IV0)*COSA
     ELSE
       CenVolu(1,Nvolu) =  CenVolu(1,IV0)*COSA -CenVolu(2,IV0)*SINA
       CenVolu(2,Nvolu) =  CenVolu(1,IV0)*SINA +CenVolu(2,IV0)*COSA
       CenVolu(3,Nvolu) =  CenVolu(3,IV0)
     ENDIF
   ENDIF
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
     IcoPlan(Nplan) = IcoPlan(IP)
     NedgeI(Nplan)  = Nedge + 1
     DO IE=NedgeI(IP),NedgeA(IP)
       IA = IadEdge(IE)
       IF( JAA(IA) < 1 ) THEN
         X = XYZadge(1,IA)
         Y = XYZadge(2,IA)
         Z = XYZadge(3,IA)
         IF( IxyzLoc == 1 ) THEN
           V3(1) = X
           V3(2) = YC + (Y-YC)*COSA - (Z-ZC)*SINA
           V3(3) = ZC + (Y-YC)*SINA + (Z-ZC)*COSA
         ELSEIF( IxyzLoc == 2 ) THEN
           V3(1) = XC + (X-XC)*COSA + (Z-ZC)*SINA
           V3(2) = Y
           V3(3) = ZC - (X-XC)*SINA + (Z-ZC)*COSA
         ELSE
           V3(1) = XC + (X-XC)*COSA - (Y-YC)*SINA
           V3(2) = YC + (X-XC)*SINA + (Y-YC)*COSA
           V3(3) = Z
         ENDIF
         Nadge = Nadge + 1
         XYZadge(1:3,Nadge) = V3(1:3)
         JAA(IA) = Nadge
       ENDIF
       Nedge = Nedge + 1
       IvuEdge(Nedge) = IvuEdge(IE)
       IadEdge(Nedge) = JAA(IA)
     ENDDO
     NedgeA(Nplan) = Nedge
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
