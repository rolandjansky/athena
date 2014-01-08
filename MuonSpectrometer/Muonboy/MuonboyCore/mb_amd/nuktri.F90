!
!> Tells whether the point ZZ,SS is outside (NUKTRI=.true.)
!> or inside (NUKTRI=.false.) the area of trigger chamber ICT
!> with TOLZ tolerance in Z and TOLS tolerance in S.
!> IGNORING the possible cutouts in this trigger chamber.
!
!> \author M.Virchaux
!
 LOGICAL FUNCTION NUKTRI(ICT,ZZ,SS,TOLZ,TOLS)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ICT                !< trigger station index
 REAL(8), INTENT(IN) :: ZZ, SS, TOLZ, TOLS
 REAL(8) :: ZZMZ0, ZLOS2, EZ, SWIZZP, SWIZZN
!
   NUKTRI = .TRUE.
!
   ZZMZ0 = ZZ - ZCETRI(ICT)
   ZLOS2 = ZLOTRI(ICT)  / 2.D0
   IF( ABS(ZZMZ0) > ZLOS2+TOLZ )                     RETURN
!
   EZ = (ZZMZ0+ZLOS2) / ZLOTRI(ICT)
   SWIZZP = SPITRI(ICT) + EZ * (SPATRI(ICT)-SPITRI(ICT))
   SWIZZN = SNITRI(ICT) + EZ * (SNATRI(ICT)-SNITRI(ICT))
   IF( SS > SWIZZP+TOLS .OR. SS < SWIZZN-TOLS )      RETURN
!
   NUKTRI = .FALSE.
!
 END FUNCTION NUKTRI
!
