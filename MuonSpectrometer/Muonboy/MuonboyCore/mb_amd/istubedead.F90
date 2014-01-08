!
!> \author M.Virchaux
!
 LOGICAL FUNCTION IsTubeDead(IGEOM,IP,Ichan)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IGEOM, IP, Ichan
 INTEGER :: IMDT, Nchan, Iaddress, I32a, I32b
!
   IsTubeDead = .FALSE.
   IF( IadDedChan(IGEOM) < 1 ) RETURN
   IMDT     = ISTMDTV(IGEOM)
   Nchan    = NINT( ZLOCHAV(IGEOM) * PASTUB_inv(IMDT) )
   Iaddress = (IP-1)*Nchan + Ichan - 1
   I32a = Iaddress / 32
   I32b = Iaddress - I32a*32
   IsTubeDead = BTEST( IbitDedChan(IadDedChan(IGEOM)+I32a) , I32b )
!
 END FUNCTION IsTubeDead
!
