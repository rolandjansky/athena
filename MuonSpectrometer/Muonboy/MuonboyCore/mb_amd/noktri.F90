!
!> Tells whether the point ZZ,SS is outside (NOKTRI=.true.)
!> or inside (NOKTRI=.false.) the area of trigger chamber ICT
!> with TOLZ tolerance in Z and TOLS tolerance in S.
!> taking into account the possible cutouts in this trigger chamber.
!
!> \author M.Virchaux
!
 LOGICAL FUNCTION NOKTRI(ICT,ZZ,SS,TOLZ,TOLS)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ICT                !< trigger station index
 REAL(8), INTENT(IN) :: ZZ, SS, TOLZ, TOLS
 LOGICAL :: INSIDE
 INTEGER :: JCUT, L
 REAL(8) :: ZZMZ0, ZLOS2, EZ, SWIZZP, SWIZZN, ZB,SB
!
   NOKTRI = .TRUE.
!
   ZZMZ0 = ZZ - ZCETRI(ICT)
   ZLOS2 = ZLOTRI(ICT) * 0.5d0
   IF( ABS(ZZMZ0) > ZLOS2+TOLZ )                             RETURN
!
   EZ = (ZZMZ0+ZLOS2) / ZLOTRI(ICT)
   SWIZZP = SPITRI(ICT) + EZ * (SPATRI(ICT)-SPITRI(ICT))
   SWIZZN = SNITRI(ICT) + EZ * (SNATRI(ICT)-SNITRI(ICT))
   IF( SS > SWIZZP+TOLS )                                    RETURN
   IF( SS < SWIZZN-TOLS )                                    RETURN
!
   JCUT = ITRCUT(ICT)
   IF( JCUT >= 1 ) THEN
     DO L=1,NBCUT(JCUT)
       CALL ApplyTol(ZZ,SS, ZTCUT(1,JCUT,L),SSCUT(1,JCUT,L), TOLZ,TOLS, ZB,SB)
       IF( INSIDE(ZB,SB,4,ZTCUT(1,JCUT,L),SSCUT(1,JCUT,L)) ) RETURN
     ENDDO
   ENDIF
!
   NOKTRI = .FALSE.
!
 END FUNCTION NOKTRI
!
