!
!> This routine is called for each tgc hit, and calls digiba.F90
!> to store it in Muonboy commons.
!
 SUBROUTINE MDIGITGC(Ident,STANAME,AreAmdbIds,                      &
                     StationEta,StationPhi,GasGap,IsStrip,Channel,  &
                     Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,          &
                     XXX,YYY,ZZZ,                                   &
                     ParticleCode,AthPosOff)
 IMPLICIT NONE
 INTEGER,      INTENT(IN)    :: Ident, AreAmdbIds
 CHARACTER(1), INTENT(IN)    :: STANAME(3)
 INTEGER,      INTENT(INOUT) :: StationEta, StationPhi
 INTEGER,      INTENT(INOUT) :: GasGap, IsStrip, Channel
 INTEGER,      INTENT(INOUT) :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8),      INTENT(IN)    :: XXX, YYY, ZZZ
 INTEGER,      INTENT(IN)    :: ParticleCode, AthPosOff
 INTEGER :: Jtyp
 REAL(8) :: TgcTime
!
    IF( AreAmdbIds == 0 ) THEN
      CALL GetAMDCindFromTGCwithJTYP(STANAME,             &
        StationEta,StationPhi,GasGap,IsStrip,Channel,     &
        Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,Jtyp)
    ELSE
      CALL GetTGCindFromAMDCwithJTYP(STANAME,             &
        Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,                  &
        StationEta,StationPhi,GasGap,IsStrip,Channel,Jtyp)
    ENDIF
!
    IF( Jtyp == 0 ) RETURN
!
    TgcTime = -999.d0
    CALL DIGIBA(Ident,                                 &
                Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,  &
                TgcTime, XXX,YYY,ZZZ,                  &
                ParticleCode, AthPosOff)
!
 END SUBROUTINE MDIGITGC
!
