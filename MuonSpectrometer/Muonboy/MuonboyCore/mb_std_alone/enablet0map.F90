!
 SUBROUTINE setenabletomap
 USE M_MB_T0fit
   modifyT0 = .true.
 END SUBROUTINE setenabletomap
!
 SUBROUTINE setdisabletomap
 USE M_MB_T0fit
   modifyT0 = .false.
 END SUBROUTINE setdisabletomap
!
 SUBROUTINE ResetSetTShift(t0)
 USE M_MB_T0fit
 REAL(8), INTENT(IN) :: t0
   drfromt0 = t0 * 1.5d0 / 700.d0  ! linear approximation for tests
 END SUBROUTINE ResetSetTShift
!
