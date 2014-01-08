!
 SUBROUTINE saveActiveCSC(IGEOM,Zoop,Ziip,PitchEta,S1Pp,S2Pip,S2Po1p,PitchPhi) 
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IGEOM
 REAL(8), INTENT(IN) :: Zoop, Ziip, PitchEta
 REAL(8), INTENT(IN) :: S1Pp, S2Pip, S2Po1p, PitchPhi
 REAL(8) :: scalz, scals
 INTEGER :: intpos
!
   IF( intpos(IGEOM,IGEOMCSC,NChaCSC) == 0 ) THEN
     IF( NChaCSC >= NChaCSCMax ) THEN
       WRITE(*,*) ' saveActiveCSC : NChaCSCMax too small : increase it !!!'
       RETURN
     ENDIF
     NChaCSC = NChaCSC + 1
     IGEOMCSC(NChaCSC) = IGEOM
! Problem of precision : total length may be different than N*pitch => keep the biggest
     scalz = MAX( 0.1d0 , 192*PitchEta/Zoop/10.d0 )
     scals = MAX( 0.1d0 , 24*PitchPhi/S2Pip/10.d0 )
     ZoopCSC(NChaCSC)   = scalz * Zoop
     ZiipCSC(NChaCSC)   = scalz * Ziip
     S1PpCSC(NChaCSC)   = scals * S1Pp
     S2PipCSC(NChaCSC)  = scals * S2Pip
     S2Po1pCSC(NChaCSC) = scals * S2Po1p
   ENDIF
!
 END SUBROUTINE saveActiveCSC
!
