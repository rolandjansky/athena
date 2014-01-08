!
!> returns iok=0 if everything was ok,
!> iok=1 if the fit failed,
!> iok=2 if too much trials have been done for this search,
!> iok=3 if too much trials have been done for the event,
!> iok=9 is fatal problem.
!
 SUBROUTINE DoTheMainFit(iok)
 USE M_MB_Control
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: iok
 INTEGER :: fitStatus
 REAL(8) :: R2FIN, AZFIN
!
    iok = 1
!
! Here one candidate track may have been found, from it we perform a
! complementary search using all possible stations
100 CALL searchForCrossedStations
!
    IF( NBLIBR /= 0 ) THEN
! Principal Fit : fit the track adding the NBLIBR stations
      CALL mainFit(fitStatus,.false.)
      IF( fitStatus /= 0 ) THEN
        iok = fitStatus
        RETURN
      ENDIF
      GO TO 100
    ENDIF
!
! Final pattern recognition fit of type 1 with SIGMAK = 5.
! if not done and at least 3 stations  or  if fit was not of type 1
    IF( ItypFit == 2 ) THEN
! Store the tracks parameters for ItypFit = 2
      IF( AtCosm == 0 ) THEN
        R2FIN = SQRT( VFIN(1)**2 + VFIN(2)**2 )
        AZFIN = ABS( VFIN(3) )
        IF( ABS(R2FIN-RCHAMI) > 0.1D0 .AND. ABS(AZFIN-ZCHAMI) > 0.1D0 ) THEN
          IF(MBPri >= 1) PRINT 950,VFIN(8)*VFIN(7),R2FIN,AZFIN,RCHAMI,ZCHAMI
950       FORMAT(' For a candidate track with Pmu =',F9.2,' GeV,'                      &
                /' One has  R2FIN,AZFIN =',2F12.4,'  while RCHAMI,ZCHAMI =',2F12.4     &
                ,'  =====>  Reject it ! (but for test beam or cosmics)')
          RETURN
        ENDIF
      ENDIF
    ENDIF
!
    iok = 0
!
 END SUBROUTINE DoTheMainFit
