!
!> This routine is called for each csc strip, and calls digitu.F90
!> to store it in Muonboy commons. It is no more used are Muonboynow
!> uses the CSC clusters (see mdigicscclu.F90).
!
 SUBROUTINE MDIGICSC(Ident,STANAME,AreAmdbIds,                     &
                     StationEta,StationPhi,ChamberLayer,WireLayer, &
                     MeasuresPhi,Strip,                            &
                     Jtyp,Jff,Jzz,Job,Jsl,Jtube,SZflag,            &
                     XXX,YYY,ZZZ,                                  &
                     DepositedCharge,ParticleCode,AthPosOff)
 IMPLICIT NONE
 INTEGER     , INTENT(IN)    :: Ident
 CHARACTER(1), INTENT(IN)    :: STANAME(3)
 INTEGER     , INTENT(IN)    :: AreAmdbIds
 INTEGER     , INTENT(INOUT) :: StationEta,StationPhi,ChamberLayer,WireLayer
 INTEGER     , INTENT(INOUT) :: MeasuresPhi,Strip
 INTEGER     , INTENT(INOUT) :: Jtyp,Jff,Jzz,Job,Jsl,Jtube
 REAL(8)     , INTENT(INOUT) :: SZflag
 REAL(8)     , INTENT(IN)    :: XXX, YYY, ZZZ
 REAL(8)     , INTENT(IN)    :: DepositedCharge
 INTEGER     , INTENT(IN)    :: ParticleCode, AthPosOff
 INTEGER :: Status
!
    IF( AreAmdbIds == 0 ) THEN
      CALL GetAMDCindFromCSCwithJTYP(STANAME,         &
        StationEta,StationPhi,ChamberLayer,WireLayer, &
        MeasuresPhi,Strip,                            &
        Jff,Jzz,Job,Jsl,Jtube,SZflag,Jtyp)
    ELSE
      CALL GetCSCindFromAMDCwithJTYP(STANAME,         &
        Jff,Jzz,Job,Jsl,Jtube,SZflag,                 &
        StationEta,StationPhi,ChamberLayer,WireLayer, &
        MeasuresPhi,Strip,Jtyp)
    ENDIF
!
    IF( Jtyp == 0 ) RETURN
!
    Status = 0
    CALL DIGITU(Ident,                                     &
                Jtyp,Jff,Jzz,Job,Jsl,Jtube,                &
                XXX,YYY,ZZZ,                               &
                DepositedCharge,0.d0,SZflag,ParticleCode,  &
                Status,AthPosOff)
!
 END SUBROUTINE MDIGICSC
!
