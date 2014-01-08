!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE EFKIMS(NBM,RRM,RRS,X0S, EFF)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: NBM
 REAL(8), INTENT(IN)  :: RRM(*), RRS, X0S
 REAL(8), INTENT(OUT) :: EFF
 INTEGER :: I
 REAL(8) :: THETA0, S1, SX, SY, SXY, SX2, SY2, XXX, YYY, DELTA, AAA, BBB
!
!>> Compute the effect EFF of a multiple scattering center
!>> of X0S rad. length at a location RRS  on the chi2 of a
!>> track measured at NBM locations RRM(i)
!
   THETA0 = 0.0136D0 * SQRT(X0S) * ( 1.D0 + 0.038D0*LOG(X0S) )
   S1  = 0.D0
   SX  = 0.D0
   SY  = 0.D0
   SXY = 0.D0
   SX2 = 0.D0
   SY2 = 0.D0
   DO I=1,NBM
     XXX = RRM(I)
     IF( XXX <= RRS ) THEN
       YYY = 0.D0
     ELSE
       YYY = THETA0 * (XXX-RRS)
     ENDIF
     S1  = S1  + 1.D0
     SX  = SX  + XXX
     SY  = SY  + YYY
     SXY = SXY + XXX*YYY
     SX2 = SX2 + XXX**2
     SY2 = SY2 + YYY**2
   ENDDO
!
   DELTA = SX2*S1 - SX**2
   AAA = ( SXY*S1 - SY*SX )  / DELTA
   BBB = ( SX2*SY - SXY*SX ) / DELTA
   EFF = SY2 - AAA*SXY - BBB*SY
!
 END
