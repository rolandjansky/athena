 subroutine rfttubcs(Ident,    &
     XXXDCA, YYYDCA, ZZZDCA ,  &
     XTrdir, YTrdir, ZTrdir ,  &
     XWiCen, YWiCen, ZWiCen ,  &
     XWidir, YWidir, ZWidir ,  &
     Radius )
  USE M_MB_RTDump
  implicit none
  INTEGER, INTENT(IN) :: Ident
  REAL(8), INTENT(IN) :: XXXDCA, YYYDCA, ZZZDCA   !< coordinates at DCA
  REAL(8), INTENT(IN) :: XTrdir, YTrdir, ZTrdir   !< track direction at DCA
  REAL(8) :: XWiCen, YWiCen, ZWiCen 
  REAL(8) :: XWidir, YWidir, ZWidir
  REAL(8) :: Radius
  REAL(8) :: ALFA
  INTEGER :: i, irt, J
  INTEGER, PARAMETER :: NPRMAX=50
  INTEGER, SAVE :: NPRINT=0
  !
  IF( RTfromDump .OR. IRTpara > 0 ) THEN    ! RT read directly on dump file
    irt = -1
    DO i=1,NRTdump
      IF(IdeRTdump(i) == Ident) THEN
        irt = i
        EXIT
      ENDIF
    ENDDO
    IF(irt > 0) THEN
      IF(PasRTdump(irt) < 0.000001D0) THEN   ! H8 case       (RS2) or Cosmics
        Radius = RayRTdump(0,irt)
      ELSE                                   ! Standard case (RS3)
        ALFA = ( DirRTdump(1,irt)*(XXXDCA-PosRTdump(1,irt)) + DirRTdump(2,irt)*(YYYDCA-PosRTdump(2,irt))        &
             +   DirRTdump(3,irt)*(ZZZDCA-PosRTdump(3,irt)) ) / PasRTdump(irt) / SQRT(SUM(DirRTdump(1:3,irt)**2))
        IF(ALFA < 0.) THEN
          IF(NPRINT < NPRMAX) THEN
            NPRINT = NPRINT + 1
            WRITE(*,*) ' == RFTTUBCS WARNING : outside tube ',Ident,ALFA
          ENDIF
          J = 0
          Radius = RayRTdump(J,irt)
        ELSE IF(ALFA >= NpaRTdump(irt)) THEN
          IF(NPRINT < NPRMAX) THEN
            NPRINT = NPRINT + 1
            WRITE(*,*) ' == RFTTUBCS WARNING : outside tube ',Ident,ALFA,NpaRTdump(irt)
          ENDIF
          J = NpaRTdump(irt)
          Radius = RayRTdump(J,irt)
        ELSE
          J = INT(ALFA)
          Radius = (ALFA-J)*RayRTdump(J+1,irt) + (J+1.D0-ALFA)*RayRTdump(J,irt)
        ENDIF
        IF( SWfromDump ) THEN
          XWiCen = SXPdump(J,irt)
          YWiCen = SYPdump(J,irt)
          ZWiCen = SZPdump(J,irt)
          XWidir = SXDdump(J,irt)
          YWidir = SYDdump(J,irt)
          ZWidir = SZDdump(J,irt)
        ELSE
          XWiCen = 0.d0
          YWiCen = 0.d0
          ZWiCen = 0.d0
          XWidir = 0.d0
          YWidir = 0.d0
          ZWidir = 0.d0
        ENDIF
      ENDIF
    ELSE
      WRITE(*,*) ' == RFTTUBCS WARNING : tube not found ',Ident
      STOP
    ENDIF
  ELSE IF( IRTpara < 0 ) THEN ! arbitrary value just to have nice displays
    Radius = fixedRadius
  ELSE
    WRITE(*,*) ' == RFTTUBCS ERROR : no RT relation'
    STOP
  ENDIF
 end subroutine rfttubcs
 
