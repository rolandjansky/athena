!
!> compute the lateral dimensions of a station
!> \author M.Virchaux
!
 SUBROUTINE Get_Tube_Lateral_Dim(IGEOM,ZZ, Smini,Smaxi)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IGEOM
 REAL(8), INTENT(IN)  :: ZZ
 REAL(8), INTENT(OUT) :: Smini, Smaxi
 INTEGER :: ISTACA
 REAL(8) :: ZZMZ0, DSTACA, EZ
 REAL(8), PARAMETER :: DecaSta = 0.1000D0
!
   ZZMZ0  = ZZ - ZCECHAV(IGEOM) + 0.5d0*ZLOCHAV(IGEOM)
!>> Possible "staircasing" of the lateral edges of the multilayer
   DSTACA = STACHAV(IGEOM)
   IF( DSTACA > 1.D0 ) THEN
     ISTACA = (ZZMZ0-DecaSta) / DSTACA
     EZ = ISTACA * DSTACA * XSTCHAV(IGEOM) * ZLOCHAV_inv(IGEOM)
   ELSE
     EZ =        ZZMZ0                     * ZLOCHAV_inv(IGEOM)
   ENDIF
   Smini = SNICHAV(IGEOM) + EZ * (SNACHAV(IGEOM)-SNICHAV(IGEOM))
   Smaxi = SPICHAV(IGEOM) + EZ * (SPACHAV(IGEOM)-SPICHAV(IGEOM))
!
 END SUBROUTINE Get_Tube_Lateral_Dim
!
