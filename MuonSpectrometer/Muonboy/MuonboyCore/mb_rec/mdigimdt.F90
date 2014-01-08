!
!> This routine is called for each mdt hit, and calls digitu.F90
!> to store it in Muonboy commons.
!
 SUBROUTINE MDIGIMDT(Ident,STANAME,AreAmdbIds,                                  &
                     StationEta,StationPhi,Multilayer,TubeLayer,Tube,           &
                     Jtyp,Jff,Jzz,Job,Jsl,Jtube,                                &
                     XXX,YYY,ZZZ,                                               &
                     DriftTime,ParticleCode,TDCcount,ADCcount,Status,AthPosOff)
 IMPLICIT NONE
 INTEGER     , INTENT(IN)    :: Ident
 CHARACTER(1), INTENT(IN)    :: STANAME(3)
 INTEGER     , INTENT(IN)    :: AreAmdbIds
 INTEGER     , INTENT(INOUT) :: StationEta,StationPhi,Multilayer,TubeLayer
 INTEGER     , INTENT(INOUT) :: Tube
 INTEGER     , INTENT(INOUT) :: Jtyp,Jff,Jzz,Job,Jsl,Jtube
 REAL(8)     , INTENT(IN)    :: XXX, YYY, ZZZ
 REAL(8)     , INTENT(IN)    :: DriftTime
 INTEGER     , INTENT(IN)    :: ParticleCode
 INTEGER     , INTENT(IN)    :: TDCcount
 INTEGER     , INTENT(IN)    :: ADCcount
 INTEGER     , INTENT(IN)    :: Status, AthPosOff
 REAL(8) :: ADC, TDC
!
    IF( AreAmdbIds == 0 ) THEN
      CALL GetAMDCindFromMDTwithJTYP(STANAME,                 &
        StationEta,StationPhi,Multilayer,TubeLayer,Tube,      &
        Jff,Jzz,Job,Jsl,Jtube,Jtyp)
    ELSE
      CALL GetMDTindFromAMDCwithJTYP(STANAME,                 &
        Jff,Jzz,Job,Jsl,Jtube,                                &
        StationEta,StationPhi,Multilayer,TubeLayer,Tube,Jtyp)
    ENDIF
!
    IF( Jtyp == 0 ) RETURN
!      
    ADC = ADCcount
    TDC = TDCcount
    CALL DIGITU(Ident,Jtyp,Jff,Jzz,Job,Jsl,Jtube,  &
                XXX,YYY,ZZZ,                       &
                ADC,TDC,DriftTime,ParticleCode,    &
                Status,AthPosOff)
!
 END SUBROUTINE MDIGIMDT
!
