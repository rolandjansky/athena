!
!> \author M.Virchaux
!
 LOGICAL FUNCTION IsTubeDisabled(IGEOM,IP,Ichan)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IGEOM, IP, Ichan
 INTEGER :: IMDT, Nchan, Iaddress, I32a, I32b
!
   IsTubeDisabled = .FALSE.
   IF( IadDisChan(IGEOM) < 1 ) RETURN
   IMDT     = ISTMDTV(IGEOM)
   Nchan    = NINT( ZLOCHAV(IGEOM) * PASTUB_inv(IMDT) )
   Iaddress = (IP-1)*Nchan + Ichan - 1
   I32a = Iaddress / 32
   I32b = Iaddress - I32a*32
   IsTubeDisabled = BTEST(IbitDedChan(IadDisChan(IGEOM)+I32a),I32b)
!
 END FUNCTION IsTubeDisabled
!
