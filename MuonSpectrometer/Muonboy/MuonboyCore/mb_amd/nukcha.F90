!
!> Tells whether the point  ZZ,SS  is outside (NUKCHA=.true.)
!> or inside (NUKCHA=.false.) the area of multilayer  IC,INOF,INOZ
!> with  TOLZ  tolerance in Z and  TOLS  tolerance in S,
!> IGNORING the possible cutouts as well as the possible staircasing
!> in this multilayer.
!
!> \author M.Virchaux
!
 LOGICAL FUNCTION NUKCHA(IC,INOZ,IGEOM,ZZ,SS,TOLZ,TOLS)
 USE M_MB_Control
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IC, INOZ, IGEOM
 REAL(8), INTENT(IN) :: ZZ, SS, TOLZ, TOLS
 REAL(8) :: ZZMZ0, ZLOS2, DECAL, EZ, SWIZZP, SWIZZN
!
   NUKCHA = .TRUE.
!
   ZZMZ0 = ZZ - ZCECHAV(IGEOM)
   ZLOS2 = ZLOCHAV(IGEOM) / 2.D0
   DECAL = 0.1000D0
   IF( IC <= NCOUBA .AND. INOZ < NZMS2 ) DECAL = MDTnu * DECAL
   IF( ABS(ZZMZ0-DECAL) > ZLOS2+TOLZ )          RETURN
!
   EZ = (ZZMZ0+ZLOS2) * ZLOCHAV_inv(IGEOM)
   SWIZZP = SPICHAV(IGEOM) + EZ * (SPACHAV(IGEOM)-SPICHAV(IGEOM))
   SWIZZN = SNICHAV(IGEOM) + EZ * (SNACHAV(IGEOM)-SNICHAV(IGEOM))
   IF( SS > SWIZZP+TOLS )                       RETURN
   IF( SS < SWIZZN-TOLS )                       RETURN
!
   NUKCHA = .FALSE.
!
 END FUNCTION NUKCHA
!
