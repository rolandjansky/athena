!
! \author SaMuSoG
!***********************************************************************
 SUBROUTINE DecoIdentAMDCMDT( Ident, STANAME, JTYP,JFF,JZZ,JOB, JSL,JTUBE,      &
                              StationEta,StationPhi,Multilayer,TubeLayer,Tube )
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ident
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: JTYP,JFF,JZZ,JOB
 INTEGER      :: JSL,JTUBE
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: Multilayer,TubeLayer,Tube
 INTEGER      :: JSZ
 CHARACTER(3) :: NameTYP

    CALL Deco_Ident_AMDC(Ident, JTYP,JFF,JZZ,JOB,JSL,JSZ,JTUBE)

    CALL FGetStationType( JTYP, NameTYP )
    STANAME(1)=NameTYP(1:1)
    STANAME(2)=NameTYP(2:2)
    STANAME(3)=NameTYP(3:3)

    call GetMDTindFromAMDCwithJTYP( STANAME, Jff,Jzz,Job, Jsl,Jtube,                       &
                                    StationEta,StationPhi,Multilayer,TubeLayer,Tube, Jtyp)

 END SUBROUTINE DecoIdentAMDCMDT
!
 SUBROUTINE DecoIdentAMDCRPC( Ident, STANAME, JTYP,JFF,JZZ,JOB, JSPLI, JSL, JSZ, JSTRI,                           &
                              StationEta,StationPhi, DoubletR, DoubletZ, DoubletPhi, GasGap, MeasuresPhi, Strip )
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ident
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: JTYP,JFF,JZZ,JOB
 INTEGER      :: JSPLI, JSL, JSZ, JSTRI
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: DoubletR, DoubletZ, DoubletPhi 
 INTEGER      :: GasGap, MeasuresPhi, Strip
 CHARACTER(3) :: NameTYP

    CALL Deco_Ident_AMDC(Ident, JTYP,JFF,JZZ,JOB,JSL,JSZ,JSTRI)

!RPCHOLEWARNING      
    JSPLI = 1 
            
    CALL FGetStationType( JTYP, NameTYP )
    STANAME(1)=NameTYP(1:1)
    STANAME(2)=NameTYP(2:2)
    STANAME(3)=NameTYP(3:3)

    call GetRPCindFromAMDCwithJTYP( STANAME,Jff,Jzz,Job,JSPLI,JSL,JSZ,JSTRI,                                            &
                                    StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,GasGap,MeasuresPhi,Strip, Jtyp )

 END SUBROUTINE DecoIdentAMDCRPC
!
 SUBROUTINE DecoIdentAMDCTGC( Ident,  STANAME, JTYP,JFF,JZZ,JOB, JSPLI, JSL, JSZ, JSTRI,  &
                              StationEta,StationPhi, GasGap,IsStrip,Channel )
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ident
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: JTYP, Jff,Jzz,Job 
 INTEGER      :: JSPLI, JSL, JSZ, JSTRI
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: GasGap,IsStrip,Channel
 CHARACTER(3) :: NameTYP

    CALL Deco_Ident_AMDC(Ident, JTYP,JFF,JZZ,JOB,JSL,JSZ,JSTRI)

    CALL FGetStationType( JTYP, NameTYP )
    STANAME(1)=NameTYP(1:1)
    STANAME(2)=NameTYP(2:2)
    STANAME(3)=NameTYP(3:3)

    JSPLI = 1 

    call GetTGCindFromAMDCwithJTYP( STANAME,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,            &
                                    StationEta,StationPhi,GasGap,IsStrip,Channel, Jtyp)

 END SUBROUTINE DecoIdentAMDCTGC
!
 SUBROUTINE DecoIdentAMDCCSC( Ident, STANAME, JTYP,JFF,JZZ,JOB, Jsl,Jtube,SZflag,               & 
                              StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip )
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ident
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: JTYP, Jff,Jzz,Job 
 INTEGER      :: Jsl,Jtube
 REAL(8)      :: SZflag
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: ChamberLayer,WireLayer,MeasuresPhi,Strip
 INTEGER      :: JSZ
 CHARACTER(3) :: NameTYP

    CALL Deco_Ident_AMDC(Ident, JTYP,JFF,JZZ,JOB,JSL,JSZ,jtube)
 
    SZflag = 3.d0 - 2.d0*JSZ ! >0 means Z, <0 means S

    CALL FGetStationType( JTYP, NameTYP )
    STANAME(1)=NameTYP(1:1)
    STANAME(2)=NameTYP(2:2)
    STANAME(3)=NameTYP(3:3)

    call GetCSCindFromAMDCwithJTYP( STANAME,Jff,Jzz,Job, Jsl,Jtube,SZflag,                                 &
                                    StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip, Jtyp )

 END SUBROUTINE DecoIdentAMDCCSC
!
!***********************************************************************
 SUBROUTINE translatetoamdcMDT(STANAME, Jtyp,Jff,Jzz,Job,Jsl,Jtube,      &
                              StationEta,StationPhi,Multilayer,TubeLayer,Tube )
 IMPLICIT NONE
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: Multilayer,TubeLayer,Tube
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube, Jtyp

 CALL GetAMDCindFromMDTwithJTYP( STANAME,StationEta,StationPhi,Multilayer,TubeLayer,Tube,   &
                                    Jff,Jzz,Job,Jsl,Jtube, Jtyp)

 END SUBROUTINE translatetoamdcMDT
!
 SUBROUTINE translatetoamdcRPC(STANAME, Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,                           &
                              StationEta,StationPhi, DoubletR, DoubletZ, DoubletPhi, GasGap, MeasuresPhi, Strip )
 IMPLICIT NONE
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: DoubletR, DoubletZ, DoubletPhi 
 INTEGER      :: GasGap, MeasuresPhi, Strip
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp 

 CALL GetAMDCindFromRPCwithJTYP( STANAME,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,       &
                                       GasGap,MeasuresPhi,Strip,  Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp )

 END SUBROUTINE translatetoamdcRPC
!
 SUBROUTINE translatetoamdcTGC( STANAME, Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,  &
                              StationEta,StationPhi, GasGap,IsStrip,Channel )
 IMPLICIT NONE
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: GasGap,IsStrip,Channel
 INTEGER      :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp 

 CALL GetAMDCindFromTGCwithJTYP( STANAME,StationEta,StationPhi,GasGap,IsStrip,Channel,   &
                                       Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, Jtyp )

 END SUBROUTINE translatetoamdcTGC
!
 SUBROUTINE translatetoamdcCSC(STANAME, Jtyp, Jff,Jzz,Job,Jsl,Jtube,SZflag,               & 
                              StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip )
 IMPLICIT NONE
 CHARACTER(1) :: STANAME(3)
 INTEGER      :: StationEta,StationPhi
 INTEGER      :: ChamberLayer,WireLayer,MeasuresPhi,Strip
 INTEGER      :: Jff,Jzz,Job,Jsl,Jtube, Jtyp
 REAL(8)      :: SZflag

 CALL GetAMDCindFromCSCwithJTYP( STANAME,StationEta,StationPhi,ChamberLayer,WireLayer,   &
                                       MeasuresPhi,Strip,  Jff,Jzz,Job,Jsl,Jtube,SZflag, Jtyp )

 END SUBROUTINE translatetoamdcCSC
!
