!
!> Effective tube resolution taken during segment reconstruction
!
 REAL(8) FUNCTION FRESEG(IGEOM, RRR)
 USE M_MB_Control
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IGEOM
 REAL(8), INTENT(IN) :: RRR
 INTEGER :: IMDT
 REAL(8) :: FREDIG
!
    IMDT   = ISTMDTV(IGEOM)
    FRESEG = FREDIG(RRR) + RESOTU(IMDT)
    FRESEG = SQRT( FRESEG**2 + RESEGTUB(IMDT)**2 - RESOTU(IMDT)**2 )
!
    IF( UseT0fit ) THEN  ! increase tube resolution if T0 fit failed
      IF( dT0val(IGEOM) > 900.d0 )  FRESEG = SQRT( FRESEG**2 + ReTubNoT0**2 )
    ENDIF
!
 END FUNCTION FRESEG
!
