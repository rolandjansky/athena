!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE ENDVOL_PUR
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
 INTEGER, ALLOCATABLE :: NedgeINEW(:),NedgeANEW(:),IvuEdgeNEW(:),IadEdgeNEW(:)
 REAL(8), ALLOCATABLE :: XYZadgeNEW(:,:)
 INTEGER              :: NedgeBEG,NedgeEND,NedgeNEW
 INTEGER              :: NadgeBEG,NadgeEND,NadgeNEW
 INTEGER              :: NdimPlan,NdimEdge,NdimAdge,IP,IE,IA,IPN,IEN,IAN
 REAL(8)              :: DIFF
!
   NplanA(1:M4,Nvolu) = Nplan
!
   NedgeBEG = 999999999
   NedgeEND = 0
   NadgeBEG = 999999999
   NadgeEND = 0
   DO IP=NplanI(1,Nvolu),NplanA(M4,Nvolu)
    NedgeBEG = MIN( NedgeBEG , NedgeI(IP) )
    NedgeEND = MAX( NedgeBEG , NedgeA(IP) )
    DO IE=NedgeI(IP),NedgeA(IP)
     NadgeBEG = MIN( NadgeBEG , IadEdge(IE) )
     NadgeEND = MAX( NadgeEND , IadEdge(IE) )
    ENDDO
   ENDDO
   IF( NedgeEND /= Nedge ) THEN
      PRINT 7771,Nedge,NedgeEND
7771  FORMAT(/' In ENDVOL_PUR : Nedge,NedgeEND =',2I7,'  =====>  STOP !')
      STOP
   ENDIF
   IF( NadgeEND /= Nadge ) THEN
      PRINT 7772,Nadge,NadgeEND
7772  FORMAT(/' In ENDVOL_PUR : Nadge,NadgeEND =',2I7,'  =====>  STOP !')
      STOP
   ENDIF
!
   NdimPlan = NplanA(M4,Nvolu) - NplanI(1,Nvolu) + 3
   NdimEdge = NedgeEND - NedgeBEG + 3
   NdimAdge = NadgeEND - NadgeBEG + 3
   ALLOCATE( NedgeINEW (  NdimPlan) )
   ALLOCATE( NedgeANEW (  NdimPlan) )
   ALLOCATE( IvuEdgeNEW(  NdimEdge) )
   ALLOCATE( IadEdgeNEW(  NdimEdge) )
   ALLOCATE( XYZadgeNEW(3,NdimAdge) )
   NadgeNEW = 0
   NedgeNEW = 0
!
   DO IP=NplanI(1,Nvolu),NplanA(M4,Nvolu)
    IPN = IP - NplanI(1,Nvolu) + 1
    DO IE=NedgeI(IP),NedgeA(IP)
     NedgeNEW = NedgeNEW + 1
     IF( IE == NedgeI(IP) ) NedgeINEW(IPN) = NedgeNEW
     IF( IE == NedgeA(IP) ) NedgeANEW(IPN) = NedgeNEW
     IA = IadEdge(IE)
     DO IAN=1,NadgeNEW
      DIFF = ( XYZadge(1,IA) - XYZadgeNEW(1,IAN) )**2  &
           + ( XYZadge(2,IA) - XYZadgeNEW(2,IAN) )**2  &
           + ( XYZadge(3,IA) - XYZadgeNEW(3,IAN) )**2
      IF( DIFF < Eps ) GO TO 20
     ENDDO
        NadgeNEW = NadgeNEW + 1
        XYZadgeNEW(1:3,NadgeNEW) = XYZadge(1:3,IA)
        IAN = NadgeNEW
20   IadEdgeNEW(NedgeNEW) = IAN
     IvuEdgeNEW(NedgeNEW) = IvuEdge(IE)
    ENDDO
   ENDDO
!
   DO IP=NplanI(1,Nvolu),NplanA(M4,Nvolu)
    IPN = IP - NplanI(1,Nvolu) + 1
    NedgeI(IP) = NedgeINEW(IPN) + NedgeBEG - 1
    NedgeA(IP) = NedgeANEW(IPN) + NedgeBEG - 1
   ENDDO
   DO IEN=1,NedgeNEW
    IE = IEN + NedgeBEG - 1
    IadEdge(IE) = IadEdgeNEW(IEN) + NadgeBEG - 1
    IvuEdge(IE) = IvuEdgeNEW(IEN)
   ENDDO
   DO IAN=1,NadgeNEW
    IA = IAN + NadgeBEG - 1
    XYZadge(1:3,IA) = XYZadgeNEW(1:3,IAN)
   ENDDO
   Nedge = NedgeNEW + NedgeBEG - 1
   Nadge = NadgeNEW + NadgeBEG - 1
!
   DEALLOCATE( NedgeINEW )
   DEALLOCATE( NedgeANEW )
   DEALLOCATE( IvuEdgeNEW )
   DEALLOCATE( IadEdgeNEW )
   DEALLOCATE( XYZadgeNEW )
!
 END
