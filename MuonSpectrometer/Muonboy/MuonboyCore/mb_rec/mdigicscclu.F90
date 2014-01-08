!
!> This routine is called for each csc cluster, and calls digiclu.F90
!> to store it in Muonboy commons.
!> \author  J.F.Laporte
!
 SUBROUTINE mdigicscclu(Ident,STANAME,AreAmdbIds,                      &
                        StationEta,StationPhi,ChamberLayer,WireLayer,  &
                        MeasuresPhi,                                   &
                        Jtyp,Jff,Jzz,Job,Jsl,SZflag,                   &
                        XXX,YYY,ZZZ,Reso,                              &
                        ParticleCode,                                  &
                        DepositedCharge,Time,Status,TimeStatus)
 IMPLICIT NONE
 INTEGER     , INTENT(IN)    :: Ident
 CHARACTER(1), INTENT(IN)    :: STANAME(3)
 INTEGER     , INTENT(IN)    :: AreAmdbIds
 INTEGER     , INTENT(INOUT) :: StationEta,StationPhi,ChamberLayer,WireLayer
 INTEGER     , INTENT(INOUT) :: MeasuresPhi
 INTEGER     , INTENT(INOUT) :: Jtyp,Jff,Jzz,Job,Jsl 
 REAL(8)     , INTENT(INOUT) :: SZflag
 REAL(8)     , INTENT(IN)    :: XXX, YYY, ZZZ, Reso
 INTEGER     , INTENT(IN)    :: ParticleCode
 REAL(8)     , INTENT(IN)    :: DepositedCharge
 REAL(8)     , INTENT(IN)    :: Time
 INTEGER     , INTENT(IN)    :: Status,TimeStatus
 INTEGER :: Strip
 INTEGER :: Jtube
!
    IF( AreAmdbIds == 0 ) THEN
      Strip = 1
      CALL GetAMDCindFromCSCwithJTYP(STANAME,          &
        StationEta,StationPhi,ChamberLayer,WireLayer,  &
        MeasuresPhi,Strip,                             &
        Jff,Jzz,Job,Jsl,Jtube,SZflag,Jtyp)
    ELSE
      Jtube = 1
      CALL GetCSCindFromAMDCwithJTYP(STANAME,          &
        Jff,Jzz,Job,Jsl,Jtube,SZflag,                  &
        StationEta,StationPhi,ChamberLayer,WireLayer,  &
        MeasuresPhi,Strip,Jtyp)
    ENDIF
!
    IF( Jtyp == 0 ) RETURN
!
    CALL DigiClu(Ident,Jtyp,Jff,Jzz,Job,Jsl,SZflag,         &
                 XXX,YYY,ZZZ,Reso,                          &
                 ParticleCode,DepositedCharge,Time,Status,  &
                 TimeStatus)
!
 END SUBROUTINE mdigicscclu
!
