!
!> Check loose compatibility with ROA :
!> return 0 if not,
!> return 1 if compatible with good ROA (from RPC/TGC or CSC),
!> return 2 if compatible with a MDT ROA (no phi info)
!
 INTEGER FUNCTION IOKALA(F0,ZZ,TT,SS)
 USE M_MB_General_CONSTANTS, ONLY : One, TwoPi
 USE M_MB_AlowCell
 IMPLICIT NONE
 REAL(8) :: F0, ZZ, TT, SS
 INTEGER :: J
 REAL(8) :: R2D2, R3D2, R2D, R3D, FF, PHI, THE
 REAL(8) :: PIMIPI, EE, DTDIF, DFDIF, THEDIF, PHIDIF
!
   R2D2 = TT**2 + SS**2
   R3D2 = R2D2  + ZZ**2
   R2D  = SQRT( R2D2 )
   R3D  = SQRT( R3D2 )
   FF   = F0 + ATAN2(SS,TT)
   PHI  = PIMIPI(FF)
   THE  = ATAN2(R2D,ZZ)
!
   IOKALA  = 0
   NAlowOK = 0
   DO J=1,NAlow
     EE     =  ( ( R3D - AlowR3d(J) ) / AlowDRM )**2
     DTDIF  = DTAlowA + MIN(One,EE)*RTAlowA
     THEDIF = ABS( THE - AlowThe(J) )
     IF( THEDIF >= DTDIF ) CYCLE
     DFDIF  = DFAlowA + MIN(One,EE)*RFAlowA
     PHIDIF = ABS( PHI - AlowPhi(J) )
     PHIDIF = MIN( PHIDIF , ABS(PHIDIF-TwoPi) )
     IF( PHIDIF < DFDIF ) THEN
       NAlowOK = NAlowOK + 1   ! fill a list (from findra) of cells in which to look for
       ListAlo(NAlowOK) = J    ! in droitu
       IF( NAlowOK == 1 ) THEN
         IF( J <= NAlowGood ) THEN
           IOKALA = 1
         ELSE
           IOKALA = 2
         ENDIF
       ENDIF
     ENDIF
   ENDDO
!
 END FUNCTION IOKALA
!
!
!
!
!> Check strict compatibility with ROA :
!> return 0 if not,
!> return 1 if compatible with good ROA (from RPC/TGC or CSC),
!> return 2 if compatible with a MDT ROA (no phi info)
!> Warning IOKALA should be called first to create the list of ROA to be used
!
 INTEGER FUNCTION IOKALI(F0,ZZ,TT,SS,NML)
 USE M_MB_General_CONSTANTS, ONLY : One, TwoPi
 USE M_MB_AlowCell
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: F0, ZZ, TT, SS
 INTEGER, INTENT(IN) :: NML
 INTEGER :: I, J
 REAL(8) :: R2D2, R3D2, R2D, R3D, FF, PHI, THE, RTAcur, RFAcur
 REAL(8) :: PIMIPI, EE, DTDIF, DFDIF, THEDIF, PHIDIF
!
   R2D2 = TT**2 + SS**2
   R3D2 = R2D2  + ZZ**2
   R2D  = SQRT( R2D2 )
   R3D  = SQRT( R3D2 )
   FF   = F0 + ATAN2(SS,TT)
   PHI  = PIMIPI(FF)
   THE  = ATAN2(R2D,ZZ)
   RTAcur = RTAlowI
   IF( NML == 1 )  RTAcur = RTAlowI / 2.d0  ! narrower theta road for segments with only 1 ML
   RFAcur = RFAlowI
!
   IOKALI = 1
   DO I=1,NAlowOK
     J = ListAlo(I)
     EE     =  ( ( R3D - AlowR3d(J) ) / AlowDRM )**2
     DTDIF  = DTAlowI + MIN(One,EE)*RTAcur
     THEDIF = ABS( THE - AlowThe(J) )
     IF( THEDIF >= DTDIF ) CYCLE
     DFDIF  = DFAlowI + MIN(One,EE)*RFAcur
     PHIDIF = ABS( PHI - AlowPhi(J) )
     PHIDIF = MIN( PHIDIF , ABS(PHIDIF-TwoPi) )
     IF( PHIDIF < DFDIF ) THEN
       IF( J > NAlowGood ) IOKALI = 2
       RETURN
     ENDIF
   ENDDO
   IOKALI = 0
!
 END FUNCTION IOKALI
!
