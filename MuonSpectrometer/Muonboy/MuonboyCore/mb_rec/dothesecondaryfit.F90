!
!> returns iok=0 if everything was ok,
!> iok=1 if the fit failed,
!> iok=2 if too much trials have been done for this search,
!> iok=3 if too much trials have been done for the event,
!> iok=9 is fatal problem.
!> iok=-1 no libr segment but fixed > 1
!> track fit will stop when iok>0
!
 SUBROUTINE DoTheSecondaryFit(currentSearch, iok)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_Fit
 USE M_MB_FAX
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: currentSearch
 INTEGER, INTENT(OUT) :: iok
 INTEGER :: ISC, L, fitStatus
 INTEGER :: ISCUSE(NMATMA)
!
    iok = 1
!
!-- Principal track search using (2) 3 or 4 stations
    NBLIBR = 0
    ISCUSE(:) = 0
    ISCUSE( ISCFIXE(1:NBFIXE) ) = 1
    if ( currentSearch /= 0 ) THEN
      DO L=1,LSEARC(currentSearch)
        ISC = ISCEMI(L,currentSearch)
        IF( NBDRAT(ISC) <= 0 .OR. ISCUSE(ISC) /= 0 ) CYCLE
        NBLIBR = NBLIBR + 1
        ISCLIBR(NBLIBR) = ISC
      ENDDO
    ELSE
      DO ISC=1,NMACSC
        IF( NBDRAT(ISC) <= 0 .OR. ISCUSE(ISC) /= 0 ) CYCLE
        NBLIBR = NBLIBR + 1
        ISCLIBR(NBLIBR) = ISC
      ENDDO
    ENDIF
    IF( NBLIBR <= 0 ) THEN
      IF( NBFIXE <= 1 ) RETURN
      iok = -1
      RETURN
    ENDIF
    IF(MBPri >= 3) THEN
      WRITE(*,'(2X,"ISCFIXE =",10I4)') ISCFIXE(1:NBFIXE)
      WRITE(*,'(2X,"ISCLIBR =",10I4)') ISCLIBR(1:NBLIBR)
    ENDIF
!
! Momentum scan before fit, and from the best momentum (SMU) found, define in which stations
! one must look during the fit (these are the NBLIBR stations)
!
    CALL secondaryScan(fitStatus)
    IF( fitStatus /= 0 ) THEN
      iok = fitStatus
      RETURN
    ENDIF
!
! Principal Fit : fit the track adding the NBLIBR stations
    CALL mainFit(fitStatus,.false.)
    IF( fitStatus /= 0 ) THEN
      iok = fitStatus
      RETURN
    ENDIF
!
    iok = 0
!
 END SUBROUTINE DoTheSecondaryFit
!
