!
!> \author SamuSog
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 SUBROUTINE SETNUL(T,N,TVERIF)
!
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: N
 REAL(8), INTENT(IN)    :: TVERIF
 REAL(8), INTENT(INOUT) :: T(N)
!
    IF( ABS(T(N)-TVERIF) > 0.1D0 ) THEN
      PRINT 7770,N,T(N),TVERIF
7770  FORMAT(' In SETNUL :  T(',I6,') =',F15.3/'     while :     TVERIF =',F15.3,'  =====>  STOP !')
      STOP
    ENDIF
!
    T(1:N) = 0.D0
!
 END SUBROUTINE SETNUL
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 SUBROUTINE SETNIL(I,N,IVERIF)
! 
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: N, IVERIF
 INTEGER, INTENT(INOUT) :: I(N)
!
    IF( I(N) /= IVERIF ) THEN
      PRINT 7770,N,I(N),IVERIF
7770  FORMAT(' In SETNIL :  I(',I6,') =',I9/'     while :     IVERIF =',I9,'  =====>  STOP !')
      STOP
    ENDIF
!
    I(1:N) = 0
!
 END SUBROUTINE SETNIL
!
