!
!> \author M.Virchaux
!
 SUBROUTINE CreateAlow(RRR,TTT,FFF)
 USE M_MB_General_CONSTANTS, ONLY : TwoPi
 USE M_MB_AlowCell
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: RRR, TTT, FFF
 INTEGER :: J, ISI
 REAL(8) :: R3DDIF, THEDIF, PHIDIF, FFFPMP, DIFPMP, FFFCOR, FFFUPD, PIMIPI
 REAL(8), PARAMETER :: DRALOY=80.D0, DTALOY=0.015D0, DFALOY=0.030D0
!----------------------------------------------------------
!
   DO J=1,NAlow
     R3DDIF = ABS( RRR - AlowR3d(J) )
     IF( R3DDIF > DRALOY )    CYCLE
     THEDIF = ABS(  TTT   - AlowThe(J) )
     IF( THEDIF > DTALOY )    CYCLE
     PHIDIF = 999999.D0
     DO ISI=-1,1
       FFFPMP = FFF + ISI*TwoPi
       DIFPMP = ABS( FFFPMP - AlowPhi(J) )
       IF( DIFPMP >= PHIDIF ) CYCLE
       FFFCOR = FFFPMP
       PHIDIF = DIFPMP
     ENDDO
     IF( PHIDIF > DFALOY )    CYCLE
!>> Here, one updates an already existing (theta,phi) cell
     AlowR3d(J) = (AlowR3d(J)*AlowWei(J)+RRR)   /(AlowWei(J)+1.D0)
     AlowThe(J) = (AlowThe(J)*AlowWei(J)+TTT)   /(AlowWei(J)+1.D0)
     FFFUPD     = (AlowPhi(J)*AlowWei(J)+FFFCOR)/(AlowWei(J)+1.D0)
     AlowPhi(J) = PIMIPI(FFFUPD)
     AlowWei(J) = AlowWei(J)+1.D0
     RETURN
   ENDDO
!
!>> Here, one creates a new (theta,phi) cell
   NAlow = NAlow + 1
   IF( SIZE(AlowThe) < NAlow ) CALL Push_MB_AlowCell
   AlowR3d(NAlow) = RRR
   AlowThe(NAlow) = TTT
   AlowPhi(NAlow) = PIMIPI(FFF)
   AlowWei(NAlow) = 1.D0
   NAlowGood = NAlow 
!
 END SUBROUTINE CreateAlow
!
