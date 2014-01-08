!
   MODULE M_MB_InfoSim
     IMPLICIT NONE
!
     INTEGER, PARAMETER :: NTrMax=50
!
     TYPE trkSim
       INTEGER :: codePDG, charge
       REAL(8) :: posx, posy, posz
       REAL(8) :: momx, momy, momz
     END TYPE trkSim
!
! tracks at spectrometer entrance
!
     INTEGER,      SAVE :: NTrSpSim=0
     TYPE(trkSim), SAVE :: TrSpSim(NTrMax)
!
!
   CONTAINS
     SUBROUTINE AddTrSim(code, pos, mom)
       INTEGER, INTENT(IN) :: code
       REAL(8), INTENT(IN) :: pos(3), mom(3)
       INTEGER :: Icol, Icha
       NTrSpSim = MIN(NTrSpSim+1, NTrMax)
       TrSpSim(NTrSpSim)%codePDG = code
       TrSpSim(NTrSpSim)%posx    = pos(1)
       TrSpSim(NTrSpSim)%posy    = pos(2)
       TrSpSim(NTrSpSim)%posz    = pos(3)
       TrSpSim(NTrSpSim)%momx    = mom(1)
       TrSpSim(NTrSpSim)%momy    = mom(2)
       TrSpSim(NTrSpSim)%momz    = mom(3)
       CALL PDGcode(code, Icol, Icha)
       TrSpSim(NTrSpSim)%charge  = Icha
     END SUBROUTINE AddTrSim
!------------------------------------------------------
     SUBROUTINE Reset_MB_InfoSim
       NTrSpSim = 0
     END SUBROUTINE Reset_MB_InfoSim
!------------------------------------------------------
     SUBROUTINE PDGcode(code, Icol, Icha)
     IMPLICIT NONE
     INTEGER, INTENT(IN)  :: code
     INTEGER, INTENT(OUT) :: Icol, Icha
        SELECT CASE( code )
          CASE( 13, -13) ; Icol = 3 ; Icha = -code/13    ! muons
          CASE( 11, -11) ; Icol = 8 ; Icha = -code/11    ! electrons
          CASE DEFAULT   ; Icol = 1 ; Icha = 1
        END SELECT
     END SUBROUTINE PDGcode
   END MODULE M_MB_InfoSim
!
