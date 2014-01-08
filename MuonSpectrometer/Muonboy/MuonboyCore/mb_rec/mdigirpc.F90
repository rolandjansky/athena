!
!> This routine is called for each rpc hit, and calls digiba.F90
!> to store it in Muonboy commons.
!
 SUBROUTINE MDIGIRPC(Ident,STANAME,AreAmdbIds,                           &
                     StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi, &
                     GasGap,MeasuresPhi,Strip,                           &
                     Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,               &
                     XXX,YYY,ZZZ,                                        &
                     ParticleCode,RpcTime,AthPosOff)
 IMPLICIT NONE
 INTEGER,      INTENT(IN)    :: Ident, AreAmdbIds
 CHARACTER(1), INTENT(IN)    :: STANAME(3)      
 INTEGER,      INTENT(INOUT) :: StationEta, StationPhi
 INTEGER,      INTENT(INOUT) :: DoubletR, DoubletZ, DoubletPhi
 INTEGER,      INTENT(INOUT) :: GasGap, MeasuresPhi, Strip
 INTEGER,      INTENT(INOUT) :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8),      INTENT(IN)    :: XXX, YYY, ZZZ, RpcTime
 INTEGER,      INTENT(IN)    :: ParticleCode, AthPosOff
 INTEGER :: Jtyp
!
    IF( AreAmdbIds == 0 ) THEN
      CALL GetAMDCindFromRPCwithJTYP(STANAME,                &
        StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,  &
        GasGap,MeasuresPhi,Strip,                            &
        Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,Jtyp)
    ELSE
      CALL GetRPCindFromAMDCwithJTYP(STANAME,                &
        Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,                     &
        StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,  &
        GasGap,MeasuresPhi,Strip,Jtyp)
    ENDIF
!
    IF( Jtyp == 0 ) RETURN
!     
    CALL DIGIBA(Ident,                                &
                Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, &
                RpcTime,XXX,YYY,ZZZ,                  &
                ParticleCode,AthPosOff)
!
 END SUBROUTINE MDIGIRPC
!
