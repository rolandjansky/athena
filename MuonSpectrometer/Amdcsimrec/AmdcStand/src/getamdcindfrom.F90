!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromCSC( TypChar1,StationEta,StationPhi,ChamberLayer,WireLayer,   &
                               MeasuresPhi,Strip,  Jff,Jzz,Job,Jsl,Jtube,SZflag )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: ChamberLayer,WireLayer
 INTEGER,      INTENT(IN)  :: MeasuresPhi,Strip
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jsl,Jtube
 REAL(8),      INTENT(OUT) :: SZflag
 INTEGER :: Jtyp 
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
!
    call GetAMDCindFromCSCwithJTYP( TypChar1,StationEta,StationPhi,ChamberLayer,WireLayer,   &
                                    MeasuresPhi,Strip,  Jff,Jzz,Job,Jsl,Jtube,SZflag, Jtyp )
!
 END SUBROUTINE GetAMDCindFromCSC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromCSCwithJTYP( TypChar1,StationEta,StationPhi,ChamberLayer,WireLayer,   &
                                       MeasuresPhi,Strip,  Jff,Jzz,Job,Jsl,Jtube,SZflag, Jtyp )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: ChamberLayer,WireLayer
 INTEGER,      INTENT(IN)  :: MeasuresPhi,Strip
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jsl,Jtube
 REAL(8),      INTENT(OUT) :: SZflag
 INTEGER,      INTENT(OUT) :: Jtyp
 CHARACTER(3) :: Car3
 INTEGER      :: JTROU3, Jgeo, Kfzr, J, Jtyp_geo_obj
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    Jff    = 0
    Jzz    = 0
    Job    = 0
    Jsl    = 0
    Jtube  = 0
    SZflag = 0.D0
!
    IF( StationPhi*ChamberLayer*WireLayer <= 0 ) RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                              RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,StationPhi,StationEta))
    IF( Jgeo <= 0 )                              RETURN
    Kfzr = 110 + ChamberLayer
    DO J=1,NOBJ(Jtyp,Jgeo)
      Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,J)
      IF( IndexCSC(Jtyp_geo_obj) == Kfzr ) THEN
        Job = J
        GO TO 100
      ENDIF
    ENDDO
    RETURN
!
100 Jff    = StationPhi
    Jzz    = StationEta
    Jsl    = WireLayer
    Jtube  = Strip
    SZflag = 1.D0 - MeasuresPhi*2.D0
!
 END SUBROUTINE GetAMDCindFromCSCwithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromMDT( TypChar1,StationEta,StationPhi,Multilayer,TubeLayer,Tube,   &
                               Jff,Jzz,Job,Jsl,Jtube )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi,Multilayer
 INTEGER,      INTENT(IN)  :: TubeLayer,Tube
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jsl,Jtube
 INTEGER      :: Jtyp 
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
!
    call GetAMDCindFromMDTwithJTYP( TypChar1,StationEta,StationPhi,Multilayer,TubeLayer,Tube,   &
                                    Jff,Jzz,Job,Jsl,Jtube, Jtyp)
!
 END SUBROUTINE GetAMDCindFromMDT
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromMDTwithJTYP( TypChar1,StationEta,StationPhi,Multilayer,TubeLayer,Tube,   &
                                       Jff,Jzz,Job,Jsl,Jtube, Jtyp)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi,Multilayer
 INTEGER,      INTENT(IN)  :: TubeLayer,Tube
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jsl,Jtube
 INTEGER,      INTENT(OUT) :: JTyp
 CHARACTER(3) :: Car3
 INTEGER      :: JTROU3, Jgeo, Kfzr, J, Jtyp_geo_obj
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    Jff   = 0
    Jzz   = 0
    Job   = 0
    Jsl   = 0
    Jtube = 0
!
    IF( StationPhi*Multilayer*TubeLayer <= 0 ) RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                            RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,StationPhi,StationEta))
    IF( Jgeo <= 0 )                            RETURN
    Kfzr = 110 + Multilayer
    DO J=1,NOBJ(Jtyp,Jgeo)
      Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,J)
      IF( IndexMDT(Jtyp_geo_obj) == Kfzr ) THEN
        Job = J
        GO TO 100
      ENDIF
    ENDDO
    RETURN
!
100 Jff   = StationPhi
    Jzz   = StationEta
    Jsl   = TubeLayer
    Jtube = Tube
!
 END SUBROUTINE GetAMDCindFromMDTwithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromRPC( TypChar1,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,   &
                               GasGap,MeasuresPhi,Strip,  Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: DoubletR,DoubletZ,DoubletPhi
 INTEGER,      INTENT(IN)  :: GasGap,MeasuresPhi,Strip
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER :: Jtyp 
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
!
     call GetAMDCindFromRPCwithJTYP( TypChar1,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,       &
                                     GasGap,MeasuresPhi,Strip,  Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp )
!
 END SUBROUTINE GetAMDCindFromRPC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromRPCwithJTYP( TypChar1,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,       &
                                       GasGap,MeasuresPhi,Strip,  Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: DoubletR,DoubletZ,DoubletPhi
 INTEGER,      INTENT(IN)  :: GasGap,MeasuresPhi,Strip
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER,      INTENT(OUT) :: Jtyp
 CHARACTER(3) :: Car3
 INTEGER      :: JobTemp(10),KzfTemp(10),Norder(10)
 INTEGER      :: JTROU3, Jgeo, NbTemp, J, Jtyp_geo_obj
 INTEGER      :: KfzrCur, KfCur, KzrCur, KzCur, KrCur, L, Lo
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    Jff   = 0
    Jzz   = 0
    Job   = 0
    Jspli = 0
    Jsl   = 0
    Jsz   = 0
    Jstri = 0
!
    IF( StationPhi*DoubletR*DoubletZ*DoubletPhi*GasGap <= 0 ) RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                                           RETURN
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,StationPhi,StationEta))
    IF( Jgeo <= 0 )                                           RETURN
!
    NbTemp = 0
    DO J=1,NOBJ(Jtyp,Jgeo)
      Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,J)
      KfzrCur = IndexRPC(Jtyp_geo_obj)
      KfCur   = KfzrCur / 100
      KzrCur  = KfzrCur - 100*KfCur
      KzCur   = KzrCur / 10
      KrCur   = KfzrCur - 100*KfCur - 10*KzCur
      IF( KrCur == DoubletR ) THEN
        NbTemp = MIN( NbTemp+1 , 10 )
        JobTemp(NbTemp) = J
        KzfTemp(NbTemp) = KzCur*10 + KfCur 
      ENDIF
    ENDDO
!
    CALL KROISS(KzfTemp,NbTemp,Norder)
    DO L=NbTemp,1,-1
      Lo = Norder(L)
      KzCur = KzfTemp(Lo) / 10
      KfCur = KzfTemp(Lo) - 10*KzCur
      IF( DoubletZ >= KzCur .AND. DoubletPhi >= KfCur ) THEN
        Jff   = StationPhi
        Jzz   = StationEta
        Job   = JobTemp(Lo)
        Jspli = 1 + DoubletPhi-KfCur + 2*(DoubletZ-KzCur)
        Jsl   = GasGap
        Jsz   = 2 - MeasuresPhi
        Jstri = Strip
        EXIT
      ENDIF
    ENDDO
!
 END SUBROUTINE GetAMDCindFromRPCwithJTYP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromTGC( TypChar1,StationEta,StationPhi,GasGap,IsStrip,Channel,   &
                               Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi,GasGap
 INTEGER,      INTENT(IN)  :: IsStrip,Channel
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER      ::Jtyp
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
!
    call GetAMDCindFromTGCwithJTYP( TypChar1,StationEta,StationPhi,GasGap,IsStrip,Channel,   &
                                    Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp )
!
 END SUBROUTINE GetAMDCindFromTGC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetAMDCindFromTGCwithJTYP( TypChar1,StationEta,StationPhi,GasGap,IsStrip,Channel,   &
                                       Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi,GasGap
 INTEGER,      INTENT(IN)  :: IsStrip,Channel
 INTEGER,      INTENT(OUT) :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 INTEGER,      INTENT(OUT) :: Jtyp
 CHARACTER(3) :: Car3
 INTEGER      :: JTROU3, KzzDeb, KzzFin, Kff, Kzz
 INTEGER      :: Jtyp_ff_zz, Kr, Jgeo, J, Jtyp_geo_obj
 INTEGER      :: K, KrCur
!--------------------------------------------------------------------
! Get Amdc Id from Athena Id
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
    Jff   = 0
    Jzz   = 0
    Job   = 0
    Jspli = 0
    Jsl   = 0
    Jsz   = 0
    Jstri = 0
!
    IF( StationPhi*GasGap <= 0 )                 RETURN
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp <= 0 )                              RETURN
    IF( StationEta < 0 ) THEN
      KzzDeb = -MZZ
      KzzFin =   -1
    ELSE
      KzzDeb =    1
      KzzFin =  MZZ
    ENDIF
    DO Kff=1,8
      DO Kzz=KzzDeb,KzzFin
        Jtyp_ff_zz = JTYP_F_Z(Jtyp,Kff,Kzz)
        IF( IndexTGC(Jtyp_ff_zz)/100 == StationPhi ) THEN
          Jff = Kff
          Jzz = Kzz
          GO TO 100
        ENDIF
      ENDDO
    ENDDO
    RETURN
!
100 Kr = ABS(StationEta)
    Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
    DO J=1,NOBJ(Jtyp,Jgeo)
      Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,J)
      K     = IndexTGC(Jtyp_geo_obj) / 100
      KrCur = IndexTGC(Jtyp_geo_obj) - 100*K
      IF( KrCur == Kr ) THEN
        Job = J
        GO TO 200
      ENDIF
    ENDDO
    Jff = 0
    Jzz = 0
    RETURN
!
200 Jspli = 1
    Jsl   = GasGap
    Jsz   = 2 - IsStrip
    Jstri = Channel
!
 END SUBROUTINE GetAMDCindFromTGCwithJTYP
!
