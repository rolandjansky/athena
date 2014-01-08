!
!
!
!
! Author : M.Virchaux & L.Chevalier
!***********************************************************************
 SUBROUTINE RESOMU(LUNB,LUNC,ETA,PHI,PTMU,NMSRES,  RESOLU,RESELO)
 IMPLICIT NONE
 INTEGER :: LUNB, LUNC, NMSRES
 REAL(8) :: ETA, PHI, PTMU, RESOLU, RESELO
 REAL(8) :: THETA, SINTHE, EMU, VECMU(8)
 REAL(8), PARAMETER :: RSTART = 0.2D0
!
!>> Get the starting coordinates of the muon,
   THETA = 2.D0 * ATAN(EXP(-ABS(ETA)))
   SINTHE = SIN(THETA)
   EMU = PTMU / SINTHE
   VECMU(4) = SINTHE*COS(PHI)
   VECMU(5) = SINTHE*SIN(PHI)
   VECMU(6) = COS(THETA)
   VECMU(1) = RSTART * VECMU(4)
   VECMU(2) = RSTART * VECMU(5)
   VECMU(3) = RSTART * VECMU(6)
   VECMU(7) = ABS(EMU)
   VECMU(8) = SIGN( 1.D0, EMU )
!
!>> Compute expected resolution
   CALL RESOMUB(LUNB,LUNC,VECMU,NMSRES,  RESOLU,RESELO)
!
 END
