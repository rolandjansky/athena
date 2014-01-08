!
 SUBROUTINE addAlowMdt(RRR,TTT,FFF, IOK)
 USE M_MB_General_CONSTANTS, ONLY : Pi, TwoPi
 USE M_MB_AlowCell
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: RRR, TTT, FFF
 INTEGER, INTENT(OUT) :: IOK
 INTEGER :: J
 REAL(8) :: phidif, thedif
 REAL(8), PARAMETER :: DTcut=0.030D0, DFcut=Pi/4.d0
!
   IOK = 0
   DO J=1,NAlow
     phidif = FFF - AlowPhi(J)
     IF( phidif > Pi ) phidif = phidif - TwoPi
     IF( phidif <-Pi ) phidif = phidif + TwoPi
     IF( ABS(phidif) > DFcut )     CYCLE  ! temporary cut at pi/4
     thedif = TTT*sqrt(1.d0+tan(phidif)**2) - AlowThe(J)
     IF( ABS(thedif) > DTcut )     CYCLE
! we are compatible with an existing ROA => return
     RETURN
   ENDDO
!
!>> Here, one creates a new (theta,phi) cell
   NAlow = NAlow + 1
   IF( SIZE(AlowThe) < NAlow ) CALL Push_MB_AlowCell
   AlowR3d(NAlow) = RRR
   AlowThe(NAlow) = TTT
   AlowPhi(NAlow) = FFF
   AlowWei(NAlow) = 1.d0
   IOK = 1
!
 END SUBROUTINE addAlowMdt
!
