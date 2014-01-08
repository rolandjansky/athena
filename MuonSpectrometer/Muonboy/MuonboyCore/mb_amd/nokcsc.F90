!
!> Tells whether the point  ZZ,SS  is outside (NOKCSC=.true.)
!> or inside (NOKCSC=.false.) the area of CSC IC,INOF,INOZ
!> with  TOLZ  tolerance in Z and  TOLS  tolerance in S,
!> taking into account the possible cutouts
!
 LOGICAL FUNCTION NOKCSC(IGEOM,ZZ,SS,TOLZ,TOLS)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IGEOM
 REAL(8), INTENT(IN) :: ZZ, SS, TOLZ, TOLS
 LOGICAL :: INSIDE
 INTEGER :: JCUT, L
 REAL(8) :: ZZMZ0, EZ, Smini,Smaxi, ZB,SB
!
   NOKCSC = .TRUE.
!
   ZZMZ0 = ZZ - ZCECHAV(IGEOM)
   IF( ABS(ZZMZ0) > ZLOCHAV(IGEOM)/2.D0+TOLZ )          RETURN
   EZ = ( ZZMZ0 + ZLOCHAV(IGEOM)/2.d0 ) * ZLOCHAV_inv(IGEOM)
   Smini = SNICHAV(IGEOM) + EZ * (SNACHAV(IGEOM)-SNICHAV(IGEOM))
   Smaxi = SPICHAV(IGEOM) + EZ * (SPACHAV(IGEOM)-SPICHAV(IGEOM))
   IF( SS > Smaxi+TOLS )                                RETURN
   IF( SS < Smini-TOLS )                                RETURN
!
   JCUT = ISTCUTV(IGEOM)
   IF( JCUT > 0 ) THEN
     DO L=1,NBCUT(JCUT)
       CALL ApplyTol(ZZ,SS, ZTCUT(1:4,JCUT,L),SSCUT(1:4,JCUT,L), TOLZ,TOLS, ZB,SB)
       IF( INSIDE(ZB,SB,4,ZTCUT(1,JCUT,L),SSCUT(1,JCUT,L)) ) RETURN
     ENDDO
   ENDIF
!
   NOKCSC = .FALSE.
!
 END FUNCTION NOKCSC
!
