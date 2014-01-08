!
!> Tells whether the point  ZZ,SS  is outside (NOKCHA=.true.)
!> or inside (NOKCHA=.false.) the area of multilayer  IC,INOF,INOZ
!> with  TOLZ  tolerance in Z and  TOLS  tolerance in S,
!> taking into account the possible cutouts ias well as the
!> possible staircasing in this multilayer.
!
!>              ZCECHA                   |
!! ------------------------------------->|
!!                                       |
!!       |                ZLOCHA         |                               |
!!       |<------------------------------------------------------------->|
!!       |                               |                               |
!!       V                               V                               V
!!       .  ___     ___     ___     ___     ___     ___     ___     ___  .
!!       . /   \   /   \   /   \   /   \   /   \   /   \   /   \   /   \ .
!!   o   .|  *  | |  *  | |  *  | |  *  | |  *  | |  *  | |  *  | |  *  |.   o
!!       . \   /___\   /___\   /___\   /___\   /___\   /___\   /___\   /___
!!       .  ---/   \---/   \---/   \---/   \---/   \---/   \---/   \---/ . \
!!       a    |  *  | |  *  | |  *  | |  *  | |  *  | |  *  | |  *  | |  b  |    o
!!       .  ___\   /___\   /___\   /___\   /___\   /___\   /___\   /___\ . /
!!       . /   \---/   \---/   \---/   \---/   \---/   \---/   \---/   \---
!!   o   .|  *  | |  *  | |  *  | |  *  | |  *  | |  *  | |  *  | |  *  |.   o
!!       . \   /   \   /   \   /   \   /   \   /   \   /   \   /   \   / .
!!       .  ---     ---     ---     ---     ---     ---     ---     ---  .
!!       .                                                               .
!!
!!  The purpose of DECAL is to exclude wire "a" while keeping wire "b"
!
!> \author M.Virchaux   11/08/95
!
 LOGICAL FUNCTION NOKCHA(IC,INOZ,IGEOM,ZZ,SS,TOLZ,TOLS)
 USE M_MB_Control
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IC, INOZ, IGEOM
 REAL(8), INTENT(IN) :: ZZ, SS, TOLZ, TOLS
 LOGICAL :: INSIDE
 INTEGER :: JCUT, L
 REAL(8) :: ZZMZ0, DECAL, SWIZZN,SWIZZP, ZB,SB
!
   NOKCHA = .TRUE.
!
   ZZMZ0 = ZZ - ZCECHAV(IGEOM)
   DECAL = 0.1000D0
   IF( IC <= NCOUBA .AND. INOZ < NZMS2 ) DECAL = MDTnu * DECAL
   IF( ABS(ZZMZ0-DECAL) > ZLOCHAV(IGEOM)/2.D0+TOLZ )         RETURN
   CALL Get_Tube_Lateral_Dim(IGEOM,ZZ, SWIZZN,SWIZZP)
   IF( SS > SWIZZP+TOLS .OR. SS < SWIZZN-TOLS )              RETURN
!
   JCUT = ISTCUTV(IGEOM)
   IF( JCUT > 0 ) THEN
     DO L=1,NBCUT(JCUT)
       CALL ApplyTol(ZZ,SS, ZTCUT(1:4,JCUT,L),SSCUT(1:4,JCUT,L), TOLZ,TOLS, ZB,SB)
       IF( INSIDE(ZB,SB,4,ZTCUT(1,JCUT,L),SSCUT(1,JCUT,L)) ) RETURN
     ENDDO
   ENDIF
!
   NOKCHA = .FALSE.
!
 END FUNCTION NOKCHA
!
