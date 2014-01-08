!
!> same as nokcsc.F90, but taking the inactive region into account
!
 LOGICAL FUNCTION NAKCSC(IGEOM,ZZ,SS,TOLZ,TOLS)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IGEOM
 REAL(8), INTENT(IN) :: ZZ, SS, TOLZ, TOLS
 INTEGER :: I
 REAL(8) :: ZZMZ0, ZDEMI, EZ
 INTEGER :: intpos
!
    NAKCSC = .true.
!
    I = intpos(IGEOM,IGEOMCSC,NChaCSC)
    IF( I <= 0 )                    RETURN
    ZDEMI = ZoopCSC(I) / 2.d0
    ZZMZ0 = ZZ - ZCECHAV(IGEOM)
    IF( ABS(ZZMZ0) > ZDEMI + TOLZ ) RETURN
    EZ = ZZMZ0 + ZDEMI
    IF( ABS(ZoopCSC(I)-ZiipCSC(I)) < 0.010d0 ) THEN   ! CSS
      EZ = EZ / ZiipCSC(I)
      IF( ABS(SS) > S1PpCSC(I) + EZ*(S2PipCSC(I)-S1PpCSC(I)) + TOLS )    RETURN 
    ELSE IF( EZ <= ZiipCSC(I) ) THEN
      EZ = EZ / ZiipCSC(I)
      IF( ABS(SS) > S1PpCSC(I) + EZ*(S2PipCSC(I)-S1PpCSC(I)) + TOLS )    RETURN
    ELSE
      EZ = (EZ-ZiipCSC(I)) / (ZoopCSC(I)-ZiipCSC(I))
      IF( ABS(SS) > S2PipCSC(I) + EZ*(S2Po1pCSC(I)-S2PipCSC(I)) + TOLS ) RETURN
    ENDIF
!
    NAKCSC = .false.
!
 END FUNCTION NAKCSC 
!
