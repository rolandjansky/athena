!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Reset_MB_General_Volumes
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
   Nsuvo    = 0
   Nvolu    = 0
   Nplan    = 0
   Nedge    = 0
   Nadge    = 0
   NbNamVol = 1
!
   IF( SIZE(K4Volu) /= MVOLU ) THEN
     DEALLOCATE(NamVol)
     DEALLOCATE(NplanI)
     DEALLOCATE(NplanA)
     DEALLOCATE(K4Volu)
     DEALLOCATE(IclVolu)
     DEALLOCATE(CenVolu)
     DEALLOCATE(D4Volu)
     ALLOCATE( NamVol( 2,MVOLU) )
     ALLOCATE( NplanI(M4,MVOLU) )
     ALLOCATE( NplanA(M4,MVOLU) )
     ALLOCATE( K4Volu(   MVOLU) )
     ALLOCATE( IclVolu(  MVOLU) )
     ALLOCATE( CenVolu(3,MVOLU) )
     ALLOCATE( D4Volu(M4,MVOLU) )
   ENDIF
   IF( SIZE(NedgeI) /= MPLAN ) THEN
     DEALLOCATE(NedgeI)
     DEALLOCATE(NedgeA)
     DEALLOCATE(IcoPlan)
     ALLOCATE( NedgeI( MPLAN) )
     ALLOCATE( NedgeA( MPLAN) )
     ALLOCATE( IcoPlan(MPLAN) )
   ENDIF
   IF( SIZE(IvuEdge) /= MEDGE ) THEN
     DEALLOCATE(IvuEdge)
     DEALLOCATE(IadEdge)
     DEALLOCATE(XYZadge)
     ALLOCATE( IvuEdge(  MEDGE) )
     ALLOCATE( IadEdge(  MEDGE) )
     ALLOCATE( XYZadge(3,MADGE) )
   ENDIF
!
   NamVol(  1:2 ,1:MVOLU) = 0
   CenVolu( 1:3 ,1:MVOLU) = 0.D0
   D4Volu(1:M4-1,1:MVOLU) = 999999999.D0
   D4Volu(  M4  ,1:MVOLU) = 1.D0
   K4Volu(       1:MVOLU) = 1
!
 END
