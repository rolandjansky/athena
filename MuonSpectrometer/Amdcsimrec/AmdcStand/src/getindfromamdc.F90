!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetCSCindFromAMDC( TypChar1,Jff,Jzz,Job,Jsl,Jtube,SZflag,                           &
                               StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube
 REAL(8)      :: SZflag
 INTEGER      :: StationEta,StationPhi,ChamberLayer,WireLayer
 INTEGER      :: MeasuresPhi,Strip
 INTEGER      :: Jtyp
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
     call GetCSCindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jsl,Jtube,SZflag,                                 &
                                     StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip, Jtyp )
!
 END SUBROUTINE GetCSCindFromAMDC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetCSCindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jsl,Jtube,SZflag,                                &
                                       StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip, Jtyp )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube
 REAL(8)      :: SZflag
 INTEGER      :: StationEta,StationPhi,ChamberLayer,WireLayer
 INTEGER      :: MeasuresPhi,Strip
 INTEGER      :: Jtyp, Jgeo, Jtyp_geo_obj, K
 INTEGER      :: JTROU3
 CHARACTER(3) :: Car3
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    StationEta   = 0
    StationPhi   = 0
    ChamberLayer = 0
    WireLayer    = 0
    MeasuresPhi  = 0
    Strip        = 0
!
    IF( Jff*Job*Jsl <= 0 )            RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                   RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    IF( Jgeo <= 0 )                   RETURN
    Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,Job)
    IF( IndexCSC(Jtyp_geo_obj) <= 0 ) RETURN
!
    StationEta   = Jzz
    StationPhi   = Jff
    K            = IndexCSC(Jtyp_geo_obj) / 10
    ChamberLayer = IndexCSC(Jtyp_geo_obj) - 10*K
    WireLayer    = Jsl
    IF( SZflag < 0.D0 ) MeasuresPhi = 1
    Strip        = Jtube
!
 END SUBROUTINE GetCSCindFromAMDCwithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetMDTindFromAMDC( TypChar1,Jff,Jzz,Job,Jsl,Jtube,                   &
                               StationEta,StationPhi,Multilayer,TubeLayer,Tube )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube
 INTEGER      :: StationEta,StationPhi,Multilayer,TubeLayer,Tube
 INTEGER      :: Jtyp
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
    call GetMDTindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jsl,Jtube,                         &
                                    StationEta,StationPhi,Multilayer,TubeLayer,Tube, Jtyp )
!
 END SUBROUTINE GetMDTindFromAMDC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetMDTindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jsl,Jtube,                         &
                                       StationEta,StationPhi,Multilayer,TubeLayer,Tube, Jtyp )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube
 INTEGER      :: StationEta,StationPhi,Multilayer,TubeLayer,Tube
 INTEGER      :: Jtyp, Jgeo, Jtyp_geo_obj, K
 INTEGER      :: JTROU3
 CHARACTER(3) :: Car3
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    StationEta = 0
    StationPhi = 0
    Multilayer = 0
    TubeLayer  = 0
    Tube       = 0
!
    IF( Jff*Job*Jsl <= 0 )            RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                   RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    IF( Jgeo <= 0 )                   RETURN
    Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,Job)
    IF( IndexMDT(Jtyp_geo_obj) <= 0 ) RETURN
!
    StationEta = Jzz
    StationPhi = Jff
    K          = IndexMDT(Jtyp_geo_obj) / 10
    Multilayer = IndexMDT(Jtyp_geo_obj) - 10*K
    TubeLayer  = Jsl
    Tube       = Jtube
!
 END SUBROUTINE GetMDTindFromAMDCwithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetRPCindFromAMDC( TypChar1,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,            &
                               StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,  &
                               GasGap,MeasuresPhi,Strip )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: DoubletR,DoubletZ,DoubletPhi
 INTEGER      :: GasGap,MeasuresPhi,Strip
 INTEGER      :: Jtyp
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
     call GetRPCindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,            &
                                     StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,  &
                                     GasGap,MeasuresPhi,Strip, Jtyp )
!
 END SUBROUTINE GetRPCindFromAMDC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetRPCindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,           &
                                       StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi, &
                                       GasGap,MeasuresPhi,Strip, Jtyp )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: DoubletR,DoubletZ,DoubletPhi
 INTEGER      :: GasGap,MeasuresPhi,Strip
 INTEGER      :: Jtyp, Jgeo, Jtyp_geo_obj, K, K1, K2, K3
 INTEGER      :: JTROU3
 CHARACTER(3) :: Car3
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    StationEta  = 0
    StationPhi  = 0
    DoubletR    = 0
    DoubletZ    = 0
    DoubletPhi  = 0
    GasGap      = 0
    MeasuresPhi = 0
    Strip       = 0
!
    IF( Jff*Job*Jspli*Jsl*Jsz <= 0 )  RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                   RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    IF( Jgeo <= 0 )                   RETURN
    Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,Job)
    IF( IndexRPC(Jtyp_geo_obj) <= 0 ) RETURN
!
    K  = IndexRPC(Jtyp_geo_obj)
    K1 = K / 100
    K  = K - 100*K1
    K2 = K / 10
    K3 = K - 10*K2
!
    StationEta  = Jzz
    StationPhi  = Jff
    DoubletR    = K3
    DoubletZ    = K2 + (Jspli-1)/2
    DoubletPhi  = K1 + 1 - MOD(Jspli,2)
    GasGap      = Jsl
    MeasuresPhi = 2 - Jsz
    Strip       = Jstri
!
 END SUBROUTINE GetRPCindFromAMDCwithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetTGCindFromAMDC( TypChar1,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,      &
                               StationEta,StationPhi,GasGap,IsStrip,Channel )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER      :: StationEta,StationPhi,GasGap,IsStrip,Channel
 INTEGER      :: Jtyp
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
     call GetTGCindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,           &
                                     StationEta,StationPhi,GasGap,IsStrip,Channel, Jtyp)
!
 END SUBROUTINE GetTGCindFromAMDC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetTGCindFromAMDCwithJTYP( TypChar1,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,           &
                                       StationEta,StationPhi,GasGap,IsStrip,Channel, Jtyp)
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER      :: StationEta,StationPhi,GasGap,IsStrip,Channel
 INTEGER      :: Jtyp
 INTEGER      :: JTROU3, Jtyp_ff_zz, Jgeo, Jtyp_geo_obj, K
 CHARACTER(3) :: Car3
!--------------------------------------------------------------------
! Get Athena Id from Amdc Id
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    StationEta = 0
    StationPhi = 0
    GasGap     = 0
    IsStrip    = 0
    Channel    = 0
!
    IF( Jff*Job*Jspli*Jsl*Jsz <= 0 )  RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                   RETURN
    Jtyp_ff_zz = JTYP_F_Z(Jtyp,Jff,Jzz)
    IF( IndexTGC(Jtyp_ff_zz) <= 0 )   RETURN
    Jgeo = IGEO_V(Jtyp_ff_zz)
    IF( Jgeo <= 0 )                   RETURN
    Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,Job)
    IF( IndexTGC(Jtyp_geo_obj) <= 0 ) RETURN
!
    K          =       IndexTGC(Jtyp_geo_obj) / 100
    StationEta = SIGN( IndexTGC(Jtyp_geo_obj) - 100*K , Jzz )
    StationPhi =       IndexTGC(Jtyp_ff_zz) / 100
    GasGap     = Jsl
    IsStrip    = 2 - Jsz
    Channel    = Jstri
!
 END SUBROUTINE GetTGCindFromAMDCwithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetNberOfMDTMultilayers( TypChar1,Jff,Jzz,      &
                               NberOfMDTMultilayers )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz 
 INTEGER      :: NberOfMDTMultilayers
 INTEGER      :: Jtyp
!--------------------------------------------------------------------
! Get nber of MDT multilayers
!--------------------------------------------------------------------
!
     call GetNberOfMDTMultilayerswithJTYP( TypChar1,Jff,Jzz,       &
                                     NberOfMDTMultilayers, Jtyp)
!
 END SUBROUTINE GetNberOfMDTMultilayers
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetNberOfMDTMultilayerswithJTYP( TypChar1,Jff,Jzz,           &
                                      NberOfMDTMultilayers, Jtyp)
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz 
 INTEGER      :: NberOfMDTMultilayers
 INTEGER      :: Jtyp
 INTEGER      :: JTROU3, Jgeo, job,jtec
 CHARACTER(3) :: Car3
!--------------------------------------------------------------------
! Get nber of MDT multilayers
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
    NberOfMDTMultilayers = 0
!
    IF( Jff <= 0 )        RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                   RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    IF( Jgeo <= 0 )                   RETURN
!
    DO JOB=1,NOBJ(JTYP,JGEO)
      JTEC = ITEC(JTYP,JGEO,JOB)
      IF (CARTEC(JTEC) == 'MDT') NberOfMDTMultilayers = NberOfMDTMultilayers + 1
    enddo
!
 END SUBROUTINE GetNberOfMDTMultilayerswithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetNberOfMDTLayers( TypChar1,Jff,Jzz, jMuLay ,    &
                               NberOfMDTLayers )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz , jMuLay
 INTEGER      :: NberOfMDTLayers
 INTEGER      :: Jtyp
!--------------------------------------------------------------------
! Get nber of MDT multilayers
!--------------------------------------------------------------------
!
     call GetNberOfMDTLayerswithJTYP( TypChar1,Jff,Jzz, jMuLay ,       &
                                     NberOfMDTLayers, Jtyp)
!
 END SUBROUTINE GetNberOfMDTLayers
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetNberOfMDTLayerswithJTYP( TypChar1,Jff,Jzz, jMuLay ,           &
                                      NberOfMDTLayers, Jtyp)
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz  , jMuLay
 INTEGER      :: NberOfMDTLayers
 INTEGER      :: Jtyp
 INTEGER      :: JTROU3, Jgeo, job,jtec,jsta
 INTEGER      :: NberOfMDTMultilayers
 CHARACTER(3) :: Car3
!--------------------------------------------------------------------
! Get nber of MDT multilayers
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
    NberOfMDTLayers = 0
!
    IF( Jff <= 0 )  RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                   RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    IF( Jgeo <= 0 )                   RETURN
!
    NberOfMDTMultilayers = 0
    DO JOB=1,NOBJ(JTYP,JGEO)
      JTEC = ITEC(JTYP,JGEO,JOB)
      JSTA = ISTA(JTYP,JGEO,JOB)
      IF (CARTEC(JTEC) == 'MDT') then
        NberOfMDTMultilayers = NberOfMDTMultilayers + 1
        if ( NberOfMDTMultilayers == jMuLay ) then
          NberOfMDTLayers = NLAZ(Jtec,Jsta)
          return
        endif
      endif
    enddo
!
 END SUBROUTINE GetNberOfMDTLayerswithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetNberOfMDTTubes( TypChar1,Jff,Jzz, jMuLay , jLay ,   &
                               NberOfMDTTubes )
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz , jMuLay, jLay
 INTEGER      :: NberOfMDTTubes
 INTEGER      :: Jtyp
!--------------------------------------------------------------------
! Get nber of MDT multilayers
!--------------------------------------------------------------------
!
     call GetNberOfMDTTubeswithJTYP( TypChar1,Jff,Jzz, jMuLay , jLay ,       &
                                     NberOfMDTTubes, Jtyp)
!
 END SUBROUTINE GetNberOfMDTTubes
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetNberOfMDTTubeswithJTYP( TypChar1,Jff,Jzz, jMuLay , jLay ,           &
                                      NberOfMDTTubes, Jtyp)
 IMPLICIT NONE
 CHARACTER(1) :: TypChar1(*)
 INTEGER      :: Jff,Jzz  , jMuLay, jLay
 INTEGER      :: NberOfMDTTubes
 INTEGER      :: Jtyp
 INTEGER      :: JTROU3, Jgeo, job,jtec,jsta
 INTEGER      :: NberOfMDTMultilayers
 INTEGER      :: NberOfMDTLayers
 CHARACTER(3) :: Car3
 REAL(8), PARAMETER :: Eps = 0.100D0
 REAL(8) :: TotLe
!--------------------------------------------------------------------
! Get nber of MDT multilayers
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
    NberOfMDTTubes = 0
!
    IF( Jff <= 0 )  RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                   RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    IF( Jgeo <= 0 )                   RETURN
!
    NberOfMDTMultilayers = 0
    DO JOB=1,NOBJ(JTYP,JGEO)
      JTEC = ITEC(JTYP,JGEO,JOB)
      JSTA = ISTA(JTYP,JGEO,JOB)
      IF (CARTEC(JTEC) == 'MDT') then
        NberOfMDTMultilayers = NberOfMDTMultilayers + 1
        if ( NberOfMDTMultilayers == jMuLay ) then
          NberOfMDTLayers = NLAZ(Jtec,Jsta)
          IF( jLay < 1 .OR. jLay > NberOfMDTLayers ) RETURN
          TotLe = Geo_Le(Jtyp,Jgeo,Job) - Eps
          NberOfMDTTubes = NINT( (TotLe-STAOO(Jtec,Jsta,jLay))/STAPP(Jtec,Jsta) ) + 1
          return
        endif
      endif
    enddo
!
 END SUBROUTINE GetNberOfMDTTubeswithJTYP
!
