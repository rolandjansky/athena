!
!> \author SaMuSoG
!
      SUBROUTINE GET_AMDC_VERSION(I)
!
!- The present set of AMDC routines can read any AMDC_SIMREC data base
!- (i.e. is backward compatible) but it always puts its information
!- in commons and/or structures of the latest format which is
!- numbered as below :
!-      @@@@@
      I =  7
!-      @@@@@
!
      END SUBROUTINE GET_AMDC_VERSION
!
