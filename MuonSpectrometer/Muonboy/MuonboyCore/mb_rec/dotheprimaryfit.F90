!
!> returns iok=0 if everything was ok,
!> iok=1 if the fit failed,
!> iok=2 if too much trials have been done for this search,
!> iok=3 if too much trials have been done for the event,
!> iok=9 is fatal problem.
!> iok=-1 no segment in target
!> track fit will stop when iok>0
!
 SUBROUTINE DoThePrimaryFit(ID,ISC0,ISC1,currentSearch, iok)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_Fit
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ID, ISC0, ISC1, currentSearch
 INTEGER, INTENT(OUT) :: iok
 INTEGER :: ISC, L, fitStatus
 REAL(8) :: TZ1, TZ2
 INTEGER :: IFLECH_in
!
    iok = 1
!
    ISCI = ISC0
    CFI  = COS(FFDRAT(ID,ISC0))
    SFI  = SIN(FFDRAT(ID,ISC0))
    CFA  = 1.D0
    SFA  = 0.D0
    Z1   = ZZDRAT(ID,1,ISC0)
    T1   = TTDRAT(ID,1,ISC0)
    S1   = SSDRAT(ID,1,ISC0)
    Z2   = ZZDRAT(ID,2,ISC0)
    T2   = TTDRAT(ID,2,ISC0)
    S2   = SSDRAT(ID,2,ISC0)
    IF( ISC0 <= NCOUBAS2 ) THEN
      TZ1 = T1
      TZ2 = T2
    ELSEIF( ISC0 <= NMATTR ) THEN
      TZ1 = Z1
      TZ2 = Z2
    ELSE
      CFA = COS(GGDRAT(ID,ISC0))
      SFA = SIN(GGDRAT(ID,ISC0))
      TZ1 = CFA * Z1 + SFA * T1
      TZ2 = CFA * Z2 + SFA * T2
    ENDIF
    X1   = CFI * T1 - SFI * S1
    Y1   = SFI * T1 + CFI * S1
    X2   = CFI * T2 - SFI * S2
    Y2   = SFI * T2 + CFI * S2
#if !defined(ATLAS_NOCHECKMU)
    IF( TZ2/TZ1 > 1.D0 ) THEN
      WRITE(*,'(//5X,"TZ2 =",F12.4," > TZ1 =",F12.4," ======> STOP !"/)') TZ2,TZ1
      iok = 9
      RETURN
    ENDIF
#endif
!
! estimate the momentum from the segment position and angle
    CALL getInitialSmu(ISC0, X1,Y1,Z1, X2,Y2,Z2, SMU, fitStatus)
    IF( fitStatus /= 0 ) RETURN
    CALL Reset_MB_Fit
!
!
!-- Initial track search using only 2 stations
    NBFIXE = 1
    ISCFIXE(1) = ISC0
    IDFIXE(1)  = ID
    IF( ISENFI == 1 ) THEN
      IF( NBDRAT(ISC1) <= 0 ) THEN
        iok = -1 
        RETURN
      ENDIF
      NBLIBR = 1
      ISCLIBR(1) = ISC1
    ELSE
      NBLIBR = 0
      DO L=1,LSEARC(currentSearch)
        ISC = ISCEMI(L,currentSearch)
        IF( ISC == ISC0 .OR. NBDRAT(ISC) <= 0 ) CYCLE
        NBLIBR = NBLIBR + 1
        ISCLIBR(NBLIBR) = ISC
      ENDDO
      IF( NBLIBR <= 0 ) THEN
        iok = -1
        RETURN
      ENDIF
    ENDIF
    IF(MBPri >= 3) THEN
      WRITE(*,'(2X,"ISCFIXE =",10I4)') ISCFIXE(1:NBFIXE)
      WRITE(*,'(2X,"ISCLIBR =",10I4)') ISCLIBR(1:NBLIBR)
    ENDIF
!
! primary momentum scan and fit (only one segment can be added to the seeding segment)
    IFLECH_in = 0
    if ( currentSearch /= 0 ) IFLECH_in = IFLECH(currentSearch)
    CALL primaryFit(IFLECH_in,fitStatus)
    IF( fitStatus /= 0 ) THEN
      iok = fitStatus
      RETURN
    ENDIF
!
    iok = 0
!
 END SUBROUTINE DoThePrimaryFit 
!
