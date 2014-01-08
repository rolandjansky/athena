!
 PURE INTEGER FUNCTION intpos(ival, iarray, n)
! gives the position of ival in the array iarray, return 0 if not found
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ival, n
 INTEGER, INTENT(IN) :: iarray(n)
 INTEGER :: k
    intpos = 0
    DO k=1,n
      IF( iarray(k) /= ival ) CYCLE
      intpos = k
      EXIT
    ENDDO
!
 END FUNCTION intpos
!
