!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE tF1GetCAmdcVar( Name, I1,I2,I3, CharVar )
!
 IMPLICIT NONE
 CHARACTER(len=*),  INTENT(IN)  :: Name
 INTEGER,           INTENT(IN)  :: I1,I2,I3
 CHARACTER(len=32), INTENT(OUT) :: CharVar
!
 CHARACTER(Len=1) :: Name1(32), Cvar1(32) 
 INTEGER          :: Long, Lvar, I
!--------------------------------------------------------------------
! Interface to F1GetCAmdcVar
! Return the values of AmdcSimrec CHARACTER parameters and variables
!--------------------------------------------------------------------
!
    CharVar = ''
    Long = MIN( LEN_TRIM(Name) , 32 )
    DO I=1,Long
      Name1(I) = Name(I:I)
    ENDDO
    call F1GetCAmdcVar( Long, Name1, I1, I2, I3, Lvar,Cvar1 )
    DO I=1,Lvar
      CharVar(I:I) = Cvar1(I)
    ENDDO
!      
 END SUBROUTINE tF1GetCAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetCAmdcVar( Long, Name1, I1, I2, I3, Lvar,Cvar1 )
!
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 INTEGER,           INTENT(OUT) :: Lvar
 CHARACTER (Len=1), INTENT(OUT) :: Cvar1(32)
!
 CHARACTER (len=Long) :: VarName
 CHARACTER (len=32)   :: Cvar
 INTEGER              :: I
!--------------------------------------------------------------------
! Interface to CAmdcVar
!--------------------------------------------------------------------
!
    DO I=1,Long
     VarName(I:I) = Name1(I)
    ENDDO
    CALL CAmdcVar( VarName, I1,I2,I3, Cvar )
    Lvar = MIN( LEN_TRIM(Cvar) , 32 )
    DO I=1,Lvar
     Cvar1(I) = Cvar(I:I)
    ENDDO
!
 END SUBROUTINE F1GetCAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetChannelPositionFromTGCind( TypChar1,StationEta,StationPhi,GasGap,IsStrip,Channel, &
                                            XYZ1,XYZ2,XYZ3,XYZ4, SZT1,SZT2,SZT3,SZT4 )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: GasGap,IsStrip,Channel
 REAL(8),      INTENT(OUT) :: XYZ1(3),XYZ2(3),XYZ3(3),XYZ4(3)
 REAL(8),      INTENT(OUT) :: SZT1(3),SZT2(3),SZT3(3),SZT4(3)
!
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8)      :: XYZabs(3,4),SZTloc(3,4)
!--------------------------------------------------------------------
! Interface to FGetChannelPositionFromTGCind
!--------------------------------------------------------------------
!
    CALL GetAMDCindFromTGC( TypChar1,StationEta,StationPhi,GasGap,IsStrip,Channel,  &
                            Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
!
    CALL FGetChannelPositionFromTGCind( Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, XYZabs, SZTloc )
    XYZ1(1:3) = XYZabs(1:3,1)
    XYZ2(1:3) = XYZabs(1:3,2)
    XYZ3(1:3) = XYZabs(1:3,3)
    XYZ4(1:3) = XYZabs(1:3,4)
    SZT1(1:3) = SZTloc(1:3,1)
    SZT2(1:3) = SZTloc(1:3,2)
    SZT3(1:3) = SZTloc(1:3,3)
    SZT4(1:3) = SZTloc(1:3,4)
!
 END SUBROUTINE F1GetChannelPositionFromTGCind
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetCSCindexFromStripPosition( TypChar1,StationEta,StationPhi,ChamberLayer,WireLayer, &
                                            MeasuresPhi, XYZ, Strip )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: ChamberLayer,WireLayer,MeasuresPhi
 REAL(8),      INTENT(IN)  :: XYZ(3)
!
 INTEGER,      INTENT(OUT) :: Strip
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube
 REAL(8)      :: SZflag
!--------------------------------------------------------------------
! Interface to FGetCSCindexFromStripPosition
! Get The strip number of CSC Strip
! from the other Ids and Global coordinates
!
!--------------------------------------------------------------------
!
    call GetAMDCindFromCSC( TypChar1,StationEta,StationPhi,ChamberLayer,WireLayer,  &
                            MeasuresPhi,Strip,  Jff,Jzz,Job,Jsl,Jtube,SZflag )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
    call FGetCSCindexFromStripPosition( Ctyp,Jff,Jzz,Job,Jsl,SZflag, XYZ, Strip )
!
 END SUBROUTINE F1GetCSCindexFromStripPosition 
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 INTEGER FUNCTION tF1GetIAmdcVar( Name, I1,I2,I3 )
!
 IMPLICIT NONE
 CHARACTER(len=*), INTENT(IN)  :: Name
 INTEGER,          INTENT(IN)  :: I1,I2,I3
!
 CHARACTER(Len=1) :: Name1(32)
 INTEGER          :: Long, Ivar, I
!--------------------------------------------------------------------
! Interface to F1GetIAmdcVar
! Return the values of Amdcsimrec INTEGER parameters and variables
!--------------------------------------------------------------------
!
    Long = MIN( LEN_TRIM(Name) , 32 )
    DO I=1,Long
      Name1(I) = Name(I:I)
    ENDDO
    call F1GetIAmdcVar( Long, Name1, I1, I2, I3, Ivar )
    tF1GetIAmdcVar = Ivar
!
 END FUNCTION tF1GetIAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetIAmdcVar( Long, Name1, I1, I2, I3, Ivar )
!
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 INTEGER,           INTENT(OUT) :: Ivar
!
 INTEGER, EXTERNAL    :: IAmdcVar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!--------------------------------------------------------------------
! Interface to IAmdcVar
!--------------------------------------------------------------------
!
    DO I=1,Long
     VarName(I:I) = Name1(I)
    ENDDO
    Ivar = IAmdcVar( VarName, I1,I2,I3 )
!
 END SUBROUTINE F1GetIAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetMDTindexFromTubePosition( TypChar1,StationEta,StationPhi,Multilayer,TubeLayer,  &
                                           XYZ, Tube )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: Multilayer,TubeLayer
 REAL(8),      INTENT(IN)  :: XYZ(3)
 INTEGER,      INTENT(OUT) :: Tube
!
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube, MDTnu
!--------------------------------------------------------------------
! Interface to FGetMDTindexFromTubePosition
!--------------------------------------------------------------------
!
    CALL GetAMDCindFromMDT( TypChar1,StationEta,StationPhi,Multilayer,TubeLayer,1,  &
                            Jff,Jzz,Job,Jsl,Jtube )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
    MDTnu = -1
    CALL FGetMDTindexFromTubePosition( Ctyp,Jff,Jzz,Job,Jsl, MDTnu, XYZ, Tube )
!
 END SUBROUTINE F1GetMDTindexFromTubePosition
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetNumberOfCutOutObjects(NameType1,Jgeo,Jcut, NbObj)
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jgeo,Jcut
 INTEGER,      INTENT(OUT) :: NbObj
!
 CHARACTER(3) :: Ctyp
!--------------------------------------------------------------------
! Interface to FGetNumberOfCutOutObjects
!--------------------------------------------------------------------
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    CALL FGetNumberOfCutOutObjects( Ctyp,Jgeo,Jcut, NbObj )
!
 END SUBROUTINE F1GetNumberOfCutOutObjects
!
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetNumberOfStationObjects(NameType1,Jgeo, NbObj)
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jgeo
 INTEGER,      INTENT(OUT) :: NbObj
!
 CHARACTER(3) :: Ctyp
!--------------------------------------------------------------------
! Interface to FGetNumberOfStationObjects
!--------------------------------------------------------------------
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    CALL FGetNumberOfStationObjects( Ctyp,Jgeo, NbObj )
!
 END SUBROUTINE F1GetNumberOfStationObjects
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 REAL(8) FUNCTION tF1GetRAmdcVar( Name, I1,I2,I3 )
!
 IMPLICIT NONE
 CHARACTER(len=*), INTENT(IN)  :: Name
 INTEGER,          INTENT(IN)  :: I1,I2,I3
 CHARACTER(Len=1) :: Name1(32)
!
 INTEGER          :: Long, I
 REAL(8)          :: Rvar
!--------------------------------------------------------------------
! Interface to F1GetRAmdcVar
! Return the values of Amdcsimrec REAL(8) parameters and variables
!--------------------------------------------------------------------
!
    Long = MIN( LEN_TRIM(Name) , 32 )
    DO I=1,Long
      Name1(I) = Name(I:I)
    ENDDO
    call F1GetRAmdcVar( Long, Name1, I1, I2, I3, Rvar )
    tF1GetRAmdcVar = Rvar
!
 END FUNCTION tF1GetRAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetRAmdcVar( Long, Name1, I1, I2, I3, Rvar )
!
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 REAL(8),           INTENT(OUT) :: Rvar
!
 REAL(8), EXTERNAL    :: RAmdcVar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!--------------------------------------------------------------------
! Interface to RAmdcVar
!--------------------------------------------------------------------
!
    DO I=1,Long
     VarName(I:I) = Name1(I)
    ENDDO
    Rvar = RAmdcVar( VarName, I1,I2,I3 )
!
 END SUBROUTINE F1GetRAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetRPCindexFromStripPosition( TypChar1,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi, &
                                            GasGap,MeasuresPhi, XYZ, Strip )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: DoubletR,DoubletZ,DoubletPhi
 INTEGER,      INTENT(IN)  :: GasGap,MeasuresPhi
 REAL(8),      INTENT(IN)  :: XYZ(3)
 INTEGER,      INTENT(OUT) :: Strip
!
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
!--------------------------------------------------------------------
! Interface to FGetRPCindexFromStripPosition
!--------------------------------------------------------------------
!
    CALL GetAMDCindFromRPC( TypChar1,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,  &
                            GasGap,MeasuresPhi,1,  Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
    CALL FGetRPCindexFromStripPosition( Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz, XYZ, Strip )
!
 END SUBROUTINE F1GetRPCindexFromStripPosition
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetStationCutOutObjParam(NameType1,Jgeo,Jcut,JcutObj, CutJob,CutDx,CutDy,CutWs,CutWl,CutLe,CutEx,CutAn )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jgeo,Jcut,JcutObj
 INTEGER,      INTENT(OUT) :: CutJob
 REAL(8),      INTENT(OUT) :: CutDx,CutDy
 REAL(8),      INTENT(OUT) :: CutWs,CutWl,CutLe,CutEx,CutAn
!
 CHARACTER(3) :: Ctyp
!--------------------------------------------------------------------
! Interface to FGetStationCutOutObjParam
!--------------------------------------------------------------------
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    CALL FGetStationCutOutObjParam( Ctyp,Jgeo,Jcut,JcutObj, CutJob,CutDx,CutDy,CutWs,CutWl,CutLe,CutEx,CutAn )
!
 END SUBROUTINE F1GetStationCutOutObjParam
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetStationDimensions( NameType1,Jgeo,Jcut, IsValid, DimLocX,DimLocY,DimLocZ,   &
                                                                  CenterLocX,CenterLocY,CenterLocZ )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jgeo,Jcut
 INTEGER,      INTENT(OUT) :: IsValid
 REAL(8),      INTENT(OUT) :: DimLocX,DimLocY,DimLocZ
 REAL(8),      INTENT(OUT) :: CenterLocX,CenterLocY,CenterLocZ
!
 CHARACTER(3) :: Ctyp
!--------------------------------------------------------------------
! Interface to FGetStationDimensions
!--------------------------------------------------------------------
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    CALL FGetStationDimensions( Ctyp,Jgeo,Jcut, IsValid, DimLocX,DimLocY,DimLocZ, &
                                                         CenterLocX,CenterLocY,CenterLocZ )
!
 END SUBROUTINE F1GetStationDimensions
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetStationObjectParam( NameType1,Jgeo,Job, GeomIsValid,GeomTechnoName1,GeomTechnoIndex,  &
                                                         GeomSplitX,GeomSplitY,GeomShapeIndex,         &
                                                         GeomDx,GeomDy,GeomDz,GeomWs,GeomWl,           &
                                                         GeomLe,GeomEx,GeomD1,GeomD2,GeomD3 )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jgeo,Job
 CHARACTER(1), INTENT(OUT) :: GeomTechnoName1(*)
 INTEGER,      INTENT(OUT) :: GeomIsValid,GeomTechnoIndex
 INTEGER,      INTENT(OUT) :: GeomSplitX,GeomSplitY,GeomShapeIndex
 REAL(8),      INTENT(OUT) :: GeomDx,GeomDy,GeomDz,GeomWs,GeomWl
 REAL(8),      INTENT(OUT) :: GeomLe,GeomEx,GeomD1,GeomD2,GeomD3
!
 CHARACTER(3) :: Ctyp,Ctec
!--------------------------------------------------------------------
! Interface to FGetStationObjectParam
!--------------------------------------------------------------------
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    CALL FGetStationObjectParam( Ctyp,Jgeo,Job, GeomIsValid,                                &
                                 Ctec,GeomTechnoIndex,GeomSplitX,GeomSplitY,GeomShapeIndex, &
                                 GeomDx,GeomDy,GeomDz,GeomWs,GeomWl,                        &
                                 GeomLe,GeomEx,GeomD1,GeomD2,GeomD3 )
    GeomTechnoName1(1) = Ctec(1:1)
    GeomTechnoName1(2) = Ctec(2:2)
    GeomTechnoName1(3) = Ctec(3:3)
!
 END SUBROUTINE F1GetStationObjectParam
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetStationPositionParam( NameType1,Jff,Jzz,                                     &
                                       PosiIsValid, PosiJgeo,PosiJcut,PosiIsBarrel,           &
                                       PosiPhi,PosiZ,PosiR,PosiS,PosiAlfa,PosiBeta,PosiGamma )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz
 INTEGER,      INTENT(OUT) :: PosiIsValid
 INTEGER,      INTENT(OUT) :: PosiJgeo,PosiJcut,PosiIsBarrel
 REAL(8),      INTENT(OUT) :: PosiPhi,PosiZ,PosiR,PosiS
 REAL(8),      INTENT(OUT) :: PosiAlfa,PosiBeta,PosiGamma
!
 CHARACTER(3) :: Ctyp
!--------------------------------------------------------------------
! Interface to FGetStationPositionParam
!--------------------------------------------------------------------
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    CALL FGetStationPositionParam( Ctyp,Jff,Jzz,                                          &
                                   PosiIsValid, PosiJgeo,PosiJcut,PosiIsBarrel,           &
                                   PosiPhi,PosiZ,PosiR,PosiS,PosiAlfa,PosiBeta,PosiGamma )
!
 END SUBROUTINE F1GetStationPositionParam
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetStationType( Jtyp, NameType1 )
!
 IMPLICIT NONE
 INTEGER,      INTENT(IN)  :: Jtyp
 CHARACTER(1), INTENT(OUT) :: NameType1(*)
!
 CHARACTER(3) :: Ctyp
!--------------------------------------------------------------------
! Interface to FGetStationType
!--------------------------------------------------------------------
!
    CALL FGetStationType( Jtyp, Ctyp )
    NameType1(1) = Ctyp(1:1)
    NameType1(2) = Ctyp(2:2)
    NameType1(3) = Ctyp(3:3)
!
 END SUBROUTINE F1GetStationType
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetStripPositionFromCSCindex( TypChar1,StationEta,StationPhi,ChamberLayer,WireLayer,  &
                                            MeasuresPhi,Strip,                                      &
                                            XYZ1,XYZ2,XYZ3,XYZ4,XYZCenter,                          &
                                            SZT1,SZT2,SZT3,SZT4,SZTCenter )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi,ChamberLayer
 INTEGER,      INTENT(IN)  :: WireLayer,MeasuresPhi,Strip
 REAL(8),      INTENT(OUT) :: XYZ1(3),XYZ2(3),XYZ3(3),XYZ4(3)
 REAL(8),      INTENT(OUT) :: XYZCenter(3)
 REAL(8),      INTENT(OUT) :: SZT1(3),SZT2(3),SZT3(3),SZT4(3)
 REAL(8),      INTENT(OUT) :: SZTCenter(3)
!
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube
 REAL(8)      :: SZflag
 REAL(8)      :: XYZabs(3,5),SZTloc(3,5)
!--------------------------------------------------------------------
! Interface to FGetStripPositionFromCSCindex
!--------------------------------------------------------------------
!
    call GetAMDCindFromCSC( TypChar1,StationEta,StationPhi,ChamberLayer,WireLayer,  &
                            MeasuresPhi,Strip,  Jff,Jzz,Job,Jsl,Jtube,SZflag )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
    CALL FGetStripPositionFromCSCindex( Ctyp,Jff,Jzz,Job,Jsl,Jtube,SZflag, XYZabs, SZTloc )
    XYZ1(1:3)      = XYZabs(1:3,1)
    XYZ2(1:3)      = XYZabs(1:3,2)
    XYZ3(1:3)      = XYZabs(1:3,3)
    XYZ4(1:3)      = XYZabs(1:3,4)
    XYZCenter(1:3) = XYZabs(1:3,5)
    SZT1(1:3)      = SZTloc(1:3,1)
    SZT2(1:3)      = SZTloc(1:3,2)
    SZT3(1:3)      = SZTloc(1:3,3)
    SZT4(1:3)      = SZTloc(1:3,4)
    SZTCenter(1:3) = SZTloc(1:3,5)
!
 END SUBROUTINE F1GetStripPositionFromCSCindex
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetStripPositionFromRPCindex( TypChar1,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,  &
                                            GasGap,MeasuresPhi,Strip,                                     &
                                            XYZ1,XYZ2,XYZ3,XYZ4, SZT1,SZT2,SZT3,SZT4 )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: DoubletR,DoubletZ,DoubletPhi
 INTEGER,      INTENT(IN)  :: GasGap,MeasuresPhi,Strip
 REAL(8),      INTENT(OUT) :: XYZ1(3),XYZ2(3),XYZ3(3),XYZ4(3)
 REAL(8),      INTENT(OUT) :: SZT1(3),SZT2(3),SZT3(3),SZT4(3)
!
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8)      :: XYZabs(3,4),SZTloc(3,4)
!--------------------------------------------------------------------
! Interface to FGetStripPositionFromRPCindex
!--------------------------------------------------------------------
!
    CALL GetAMDCindFromRPC( TypChar1,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,  &
                            GasGap,MeasuresPhi,Strip,  Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
    CALL FGetStripPositionFromRPCindex( Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, XYZabs, SZTloc )
    XYZ1(1:3) = XYZabs(1:3,1)
    XYZ2(1:3) = XYZabs(1:3,2)
    XYZ3(1:3) = XYZabs(1:3,3)
    XYZ4(1:3) = XYZabs(1:3,4)
    SZT1(1:3) = SZTloc(1:3,1)
    SZT2(1:3) = SZTloc(1:3,2)
    SZT3(1:3) = SZTloc(1:3,3)
    SZT4(1:3) = SZTloc(1:3,4)
!
 END SUBROUTINE F1GetStripPositionFromRPCindex
! 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetTubePositionFromMDTindex( TypChar1,StationEta,StationPhi,Multilayer,TubeLayer,Tube,  &
                                           XYZ1,XYZ2, SZT1,SZT2, Radius )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi,Multilayer
 INTEGER,      INTENT(IN)  :: TubeLayer,Tube
 REAL(8),      INTENT(OUT) :: XYZ1(3),XYZ2(3),SZT1(3),SZT2(3),Radius
!
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube,MDTnu
 REAL(8)      :: XYZabs(3,2),SZTloc(3,2)
!--------------------------------------------------------------------
! Interface to FGetTubePositionFromMDTindex
!--------------------------------------------------------------------
!
    CALL GetAMDCindFromMDT( TypChar1,StationEta,StationPhi,Multilayer,TubeLayer,Tube,  &
                            Jff,Jzz,Job,Jsl,Jtube )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
    MDTnu = -1
    CALL FGetTubePositionFromMDTindex( Ctyp,Jff,Jzz,Job,Jsl,Jtube, MDTnu, XYZabs, SZTloc, Radius )
    XYZ1(1:3) = XYZabs(1:3,1)
    XYZ2(1:3) = XYZabs(1:3,2)
    SZT1(1:3) = SZTloc(1:3,1)
    SZT2(1:3) = SZTloc(1:3,2)
!
 END SUBROUTINE F1GetTubePositionFromMDTindex
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetTGCindexFromChannelPos( TypChar1,StationEta,StationPhi,GasGap,IsStrip, XYZ, Channel )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: StationEta,StationPhi
 INTEGER,      INTENT(IN)  :: GasGap,IsStrip
 REAL(8),      INTENT(IN)  :: XYZ(3)
 INTEGER,      INTENT(OUT) :: Channel
!
 CHARACTER(3) :: Ctyp
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
!--------------------------------------------------------------------
! Interface to FGetTGCindexFromChannelPos
!--------------------------------------------------------------------
!
    CALL GetAMDCindFromTGC( TypChar1,StationEta,StationPhi,GasGap,IsStrip,1,  &
                            Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri )
    Ctyp(1:1)=TypChar1(1)
    Ctyp(2:2)=TypChar1(2)
    Ctyp(3:3)=TypChar1(3)
    CALL FGetTGCindexFromChannelPos( Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz, XYZ, Channel )
!
 END SUBROUTINE F1GetTGCindexFromChannelPos
!
