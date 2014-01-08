!
 SUBROUTINE AMDC2DATA
 IMPLICIT NONE
!
!> is the routine for dumping important part of AMDB
!> Author     Sergei Baranov + SaMuSoG
!> Created    26 may 2004 + modifs 3 june 2004
!
!- AMDC string
    open (37,file='amdctemp.data',status='UNKNOWN',form='FORMATTED')
    call WRITE_AMDC(37)
    close(37)
!
!- AGDD string
    open (37,file='agddtemp.data',status='UNKNOWN',form='FORMATTED')
    call WRITE_AGDD(37)
    close(37)
!
 END SUBROUTINE AMDC2DATA
!
