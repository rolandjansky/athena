 SUBROUTINE InputGenTrack(PDGCodeIn, PosX,PosY,PosZ, PX,PY,PZ )
 USE M_MB_InfoSim
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: PDGCodeIn
 REAL(8), INTENT(IN) :: PosX,PosY,PosZ
 REAL(8), INTENT(IN) :: PX,PY,PZ
 REAL(8) :: Pos(3), Mom(3)
!
    Pos = (/ PosX/10.D0, PosY/10.D0, PosZ/10.D0 /)
    Mom = (/ PX/1000.D0, PY/1000.D0, PZ/1000.D0 /)
    CALL AddTrSim(PDGCodeIn, Pos, Mom)
!
 END
