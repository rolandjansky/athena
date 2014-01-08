!
 SUBROUTINE Bool_Incr(N,Ndim,CarIncr,Ioverf)
 USE M_MB_General_CONSTANTS, ONLY : IdebugBOO
 IMPLICIT NONE
 CHARACTER(*), INTENT(IN)    :: CarIncr
 INTEGER,      INTENT(IN)    :: Ndim
 INTEGER,      INTENT(INOUT) :: N
 INTEGER,      INTENT(OUT)   :: Ioverf
!
   Ioverf = 0
   N = N + 1
   IF( N > Ndim ) THEN
!######################################################################################
     IF( IdebugBOO /= 0 ) PRINT 1000,TRIM(CarIncr),Ndim
1000 FORMAT(' Warning !   ',A,'  =',I7,' is not large enough !!!')
!######################################################################################
     Ioverf = 1
     N = Ndim
   ENDIF
!
 END SUBROUTINE Bool_Incr
!
