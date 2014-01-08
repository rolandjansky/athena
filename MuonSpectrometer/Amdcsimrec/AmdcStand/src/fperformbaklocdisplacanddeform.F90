!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FPerformBackDisplacAndDeform( NameType,Jff,Jzz,Job, XYZin, XYZout )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 REAL(8),      INTENT(IN)  :: XYZin(3)
 REAL(8),      INTENT(OUT) :: XYZout(3)
 INTEGER :: JTROU3
 INTEGER :: Jtyp
!--------------------------------------------------------------------
! Undo GLOBAL displacement and deformation parameters for the element NameType,Jff,Jzz,Job,
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    Jtyp = JTROU3(NameType,MTYP,CARTYP)
    CALL FPerformBackDisplacAndDeformJtypBased( Jtyp,Jff,Jzz,Job, XYZin, XYZout )
!
 END SUBROUTINE FPerformBackDisplacAndDeform
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FPerformBackDisplacAndDeformJtypBased( Jtyp,Jff,Jzz,Job, XYZin, XYZout )
 IMPLICIT NONE
 INTEGER,      INTENT(IN)  :: Jtyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 REAL(8),      INTENT(IN)  :: XYZin(3)
 REAL(8),      INTENT(OUT) :: XYZout(3)
 REAL(8) :: SZTin(3),SZTout(3)
 REAL(8) :: F0, SiFi,CoFi
 INTEGER :: IOK
!--------------------------------------------------------------------
! Undo GLOBAL displacement and deformation parameters for the element NameType,Jff,Jzz,Job,
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    XYZout(1:3) = XYZin(1:3)
!
    F0   = Pos_Phi_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    SiFi = SIN( F0 )
    CoFi = COS( F0 )
    SZTin(1) = - SiFi*XYZin(1) + CoFi*XYZin(2)
    SZTin(2) =        XYZin(3)
    SZTin(3) =   CoFi*XYZin(1) + SiFi*XYZin(2)
    IF( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) < 1 ) CALL SWAP_ZT(SZTin(2),SZTin(3))
!
    CALL FPerformBakLocDisplacAndDeformJtypBasedBase( Jtyp,Jff,Jzz,Job,SZTin,SZTout, IOK )
    if ( IOK.ne.1 ) return
!
    IF( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) < 1 ) CALL SWAP_ZT(SZTout(2),SZTout(3))
    XYZout(1) = CoFi*SZTout(3) - SiFi*SZTout(1)
    XYZout(2) = SiFi*SZTout(3) + CoFi*SZTout(1)
    XYZout(3) =      SZTout(2)
!
 END SUBROUTINE FPerformBackDisplacAndDeformJtypBased
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FPerformBakLocDisplacAndDeform( NameType,Jff,Jzz,Job, SZTin, SZTout )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 REAL(8),      INTENT(IN)  :: SZTin(3)
 REAL(8),      INTENT(OUT) :: SZTout(3)
 INTEGER :: JTROU3
 INTEGER :: Jtyp
!--------------------------------------------------------------------
! Undo LOCAL displacement and deformation parameters for the element NameType,Jff,Jzz,Job,
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    Jtyp = JTROU3(NameType,MTYP,CARTYP)
    CALL FPerformBakLocDisplacAndDeformJtypBased( Jtyp,Jff,Jzz,Job,SZTin, SZTout )
!
 END SUBROUTINE FPerformBakLocDisplacAndDeform
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FPerformBakLocDisplacAndDeformJtypBased( Jtyp,Jff,Jzz,Job,SZTin,SZTout )
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jtyp
 INTEGER, INTENT(IN)  :: Jff,Jzz,Job
 REAL(8), INTENT(IN)  :: SZTin(3)
 REAL(8), INTENT(OUT) :: SZTout(3)
 INTEGER :: IOK
!--------------------------------------------------------------------
! Undo LOCAL displacement and deformation parameters for the element Jtyp,Jff,Jzz,Job
!--------------------------------------------------------------------
!
    CALL FPerformBakLocDisplacAndDeformJtypBasedBase( Jtyp,Jff,Jzz,Job,SZTin,SZTout, IOK )
!
 END SUBROUTINE FPerformBakLocDisplacAndDeformJtypBased
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FPerformBakLocDisplacAndDeformJtypBasedBase( Jtyp,Jff,Jzz,Job,SZTin,SZTout, IOK )
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jtyp
 INTEGER, INTENT(IN)  :: Jff,Jzz,Job
 REAL(8), INTENT(IN)  :: SZTin(3)
 REAL(8), INTENT(OUT) :: SZTout(3)
 INTEGER, INTENT(OUT) :: IOK
 REAL(8) :: DisplacDir(3,4),DisplacBak(3,4)
 REAL(8) :: DeformParam(11) 
 REAL(8) :: DeformSS(4),DeformZZ(4),DeformTT(4)
 INTEGER :: DeformBA
 INTEGER :: IokDeform, IokDisplac, Jgeo
 REAL(8) :: SZTloc(3)
!--------------------------------------------------------------------
! Undo LOCAL displacement and deformation parameters for the element Jtyp,Jff,Jzz,Job
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    IOK = 0
    SZTout(1:3) = SZTin(1:3)
!
    IF( Jtyp < 1 )                           RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    IF( Jgeo < 1 )                           RETURN
!
    CALL GET_STATION_SZT_DEFORMATION( Jtyp,Jff,Jzz,Job, IokDeform,DeformParam,DeformSS,DeformZZ,DeformTT,DeformBA)
    CALL GET_STATION_CSC_ABG_SZT_TRANSFORM( Jtyp,Jff,Jzz,Job, IokDisplac, DisplacDir,DisplacBak )
    IF( IokDeform < 1 .AND. IokDisplac < 1 ) RETURN
!
    IOK = 1
    SZTloc(1:3) = SZTin(1:3)
!
    IF( IokDisplac > 0 ) THEN
      CALL APPLY_SZT_TRANSFORMATION( DisplacBak, SZTloc, SZTout )
      SZTloc(1:3) = SZTout(1:3)
    ENDIF
    IF( IokDeform > 0 ) THEN
      DeformParam(1:11) = - DeformParam(1:11)
      CALL APPLY_SZT_DEFORMATION( DeformParam, DeformSS,DeformZZ,DeformTT,DeformBA, SZTloc, SZTout )
    ENDIF
!
 END SUBROUTINE FPerformBakLocDisplacAndDeformJtypBasedBase
!
