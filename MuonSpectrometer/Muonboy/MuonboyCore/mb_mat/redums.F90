!
!> Reduce the number of radii of multiple scattering to  2
!> without touching to the radii of energy loss
!
 SUBROUTINE REDUMS(NMAMAX)
 USE M_MB_Control
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NMAMAX
 INTEGER :: I, L, NORDRE(KMASC)
 REAL(8) :: RANGE, X0TOT, T2TOT, B2TOT, TBTOT
 REAL(8) :: XI, YI, TI, TI2, S, Y1, Y2, X02, T2
!
   IF( NBMASC <= 1 ) RETURN
!
!-- Re-order in increasing order radii of multiple scattering and energy losses
   CALL CROISS(RRMASC,NBMASC,NORDRE)
   CALL REORDO(RRMASC,NORDRE,NBMASC)
   CALL REORDO(X0MASC,NORDRE,NBMASC)
   CALL REORDO(ALMASC,NORDRE,NBMASC)
   CALL REORDO(BLMASC,NORDRE,NBMASC)
!
   RANGE = RRMASC(NBMASC) - RRMASC(1)
!
!-- Find the resulting <Theta**2>, <Beta**2> and <Theta*Beta>
!-- (and also the total amount of X0's)
!-- and switches only e.loss (LLMASC = 1) for these NBMASC radii
   X0TOT = 0.D0
   T2TOT = 0.D0  ! A
   B2TOT = 0.D0  ! B
   TBTOT = 0.D0  ! C
   DO L=1,NBMASC
     TI2 = X0MASC(L) * ( 1.D0 + 0.038D0*LOG(X0MASC(L)) )**2
     YI  = (RRMASC(NBMASC)-RRMASC(L)) / RANGE
     X0TOT = X0TOT + X0MASC(L)
     T2TOT = T2TOT +         TI2
     B2TOT = B2TOT + YI**2 * TI2
     TBTOT = TBTOT +   YI  * TI2
     LLMASC(L) = 1
     S1MASC(L) = 0.D0
     S2MASC(L) = 0.D0
     THMASC(L) = 0.D0
   ENDDO
!
!>> Find the locations (Y1 and Y2) of  2  m.s. radii simulating the NBMASC ones above
   S = SQRT( T2TOT*B2TOT - TBTOT**2 )
   Y1 = ( TBTOT + S ) / T2TOT
   Y2 = ( TBTOT - S ) / T2TOT
   IF(MBPri >= 3) PRINT 70,RANGE,Y1,Y2
70 FORMAT(' In REDUMS : Range,Y1,Y2 =',F12.4,2F12.6)
!
!-- find the amount of X0's coresponding to a mean scat. angle = <Theta**2> / 2
   X02 = X0TOT / 2.D0
   T2  = T2TOT / 2.D0
   I = 0
   XI = X02
10 I = I + 1
   TI = XI * ( 1.D0 + 0.038D0*LOG(XI) )**2
   IF(MBPri >= 3) PRINT 71,I,X02,XI,T2,TI
71 FORMAT(' In REDUMS : i,X02,Xi,T2,Ti =',I5,4F15.9)
   IF( ABS(TI-T2) > 0.001D0 .AND. I <= 50 ) THEN
     XI = T2 - TI + XI
     GO TO 10
   ENDIF
!
!-- Add these two radii of m.s. in the 2 directions but without e.loss (LLMASC = 6)
   NBMASC = MIN( NBMASC, NMAMAX-2 )
   DO L=1,2
     LLMASC(NBMASC+L) = 6
     JVMASC(NBMASC+L) = 0
     X0MASC(NBMASC+L) = XI
     ALMASC(NBMASC+L) = 1.D0
     BLMASC(NBMASC+L) = 0.D0
     S1MASC(NBMASC+L) = 0.D0
     S2MASC(NBMASC+L) = 0.D0
     THMASC(NBMASC+L) = 0.D0
   ENDDO
   RRMASC(NBMASC+1) = RRMASC(NBMASC) - RANGE * Y1
   RRMASC(NBMASC+2) = RRMASC(NBMASC) - RANGE * Y2
   NBMASC = NBMASC + 2
!
 END SUBROUTINE REDUMS
!
