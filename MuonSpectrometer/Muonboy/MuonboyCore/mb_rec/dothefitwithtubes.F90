!
!> returns iok=0 if everything was ok,
!> iok=1 if the fit failed,
!> iok=2 if too much trials have been done for this search,
!> iok=3 if too much trials have been done for the event,
!> iok=9 is fatal problem.
!
 SUBROUTINE DoTheFitWithTubes(iok)
 USE M_MB_Control
 USE M_MB_Fit
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: iok
 INTEGER :: fitStatus
!
    ItypFit = 1
    NBLIBR  = 0
    CALL Set_SIGMAK_CHINA_Z_S_X( 5.D0, 6.D0, 3.D0, 1.5D0 )
!
! Force radius to be recomputed as fits done with fixed radius
    RTUBHI(1:NTUBHM) = -1.d0
!
! Principal Fit : fit the track adding the NBLIBR stations
    CALL mainFit(fitStatus,.true.)
    IF( fitStatus /= 0 ) THEN
      iok = fitStatus
      RETURN
    ENDIF
!
    call DoTheMainFit(iok)
!
 END SUBROUTINE DoTheFitWithTubes
