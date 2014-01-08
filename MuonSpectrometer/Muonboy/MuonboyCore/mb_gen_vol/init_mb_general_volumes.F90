!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Init_MB_General_Volumes
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>> Initialisation of Module M_MB_General_Volumes
   CALL Free_MB_General_Volumes
!
   ALLOCATE( NamVol( 2,MVOLU) )
   ALLOCATE( NplanI(M4,MVOLU) )
   ALLOCATE( NplanA(M4,MVOLU) )
   ALLOCATE( K4Volu(   MVOLU) )
   ALLOCATE( IclVolu(  MVOLU) )
   ALLOCATE( CenVolu(3,MVOLU) )
   ALLOCATE( D4Volu(M4,MVOLU) )
   ALLOCATE( NedgeI(   MPLAN) )
   ALLOCATE( NedgeA(   MPLAN) )
   ALLOCATE( IcoPlan(  MPLAN) )
   ALLOCATE( IvuEdge(  MEDGE) )
   ALLOCATE( IadEdge(  MEDGE) )
   ALLOCATE( XYZadge(3,MADGE) )
   ALLOCATE( CharacNamVol(MNAMV) )
   NAMVsize = MNAMV
!
   Nsuvo    = 0
   Nvolu    = 0
   Nplan    = 0
   Nedge    = 0
   Nadge    = 0
   NbNamVol = 1
   CharacNamVol(1) = 'Undefined_Name'
   NamSuv(:,:) = 0
   I_Atlas_Ref_Syst = 1
!
   NamVol(:,:) = 0
   K4Volu(:)   = 1      
   CenVolu(:,:) = 0.D0
   D4Volu(1:M4-1,:) = 999999999.D0
   D4Volu(  M4  ,:) = 1.D0
!
 END
