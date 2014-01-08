!
 SUBROUTINE getSSval( IC, IGEOM1, IGEOM2, NSSmax, NSS, SSval )
 USE M_MB_MuGeom
 USE M_MB_AlowCell
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IC, IGEOM1, IGEOM2, NSSmax
 INTEGER, INTENT(OUT) :: NSS
 REAL(8), INTENT(OUT) :: SSval(NSSmax)
 INTEGER :: I, J
 REAL(8) :: COSF, SINF, TCHA, ZMI1, ZMA1, ZMI2, ZMA2, ZMIN, ZMAX
 REAL(8) :: EZ, SMI2, SMA2, SMIN, SMAX, X, Y, Z, SS, ZZ, TT, SStmp
 REAL(8), PARAMETER :: tolZ=5.d0, tolS=5.d0
!
! compute positions given by the ROA
!
   NSS  = 0
   COSF = COSCHAV(IGEOM1)
   SINF = SINCHAV(IGEOM1)
   TCHA = TCECHAV(IGEOM1)
   ZMI1 = ZCECHAV(IGEOM1) - ZLOCHAV(IGEOM1)/2.d0
   ZMA1 = ZCECHAV(IGEOM1) + ZLOCHAV(IGEOM1)/2.d0
   IF( IGEOM2 <= 0 ) THEN
     ZMIN = ZMI1
     ZMAX = ZMA1
   ELSE
     TCHA = ( TCHA + TCECHAV(IGEOM2) ) / 2.d0
     ZMI2 = ZCECHAV(IGEOM2) - ZLOCHAV(IGEOM2)/2.d0
     ZMA2 = ZCECHAV(IGEOM2) + ZLOCHAV(IGEOM2)/2.d0
     ZMIN = MIN( ZMI1 , ZMI2 )
     ZMAX = MAX( ZMA1 , ZMA2 )
   ENDIF
!
   DO I=1,NAlowGood    ! do not use ROA made from MDT
     X = SIN(AlowThe(I)) * COS(AlowPhi(I))
     Y = SIN(AlowThe(I)) * SIN(AlowPhi(I))
     Z = COS(AlowThe(I))
     CALL MuboXYZtoSZT( (IC<=NCOUBA), SINF,COSF, X,Y,Z, SS,ZZ,TT )
     IF( TT * TCHA <= 0 )   CYCLE
     ZZ = ZZ * TCHA / TT
     IF( ZZ < ZMIN - tolZ ) CYCLE
     IF( ZZ > ZMAX + tolZ ) CYCLE
     EZ = ( ZZ - ZMI1 ) / ( ZMA1 - ZMI1 )
     SMIN = SNICHAV(IGEOM1) + EZ*(SNACHAV(IGEOM1)-SNICHAV(IGEOM1))
     SMAX = SPICHAV(IGEOM1) + EZ*(SPACHAV(IGEOM1)-SPICHAV(IGEOM1))
     IF( IGEOM2 > 0 ) THEN
       EZ = ( ZZ - ZMI2 ) / ( ZMA2 - ZMI2 )
       SMI2 = SNICHAV(IGEOM2) + EZ*(SNACHAV(IGEOM2)-SNICHAV(IGEOM2))
       SMA2 = SPICHAV(IGEOM2) + EZ*(SPACHAV(IGEOM2)-SPICHAV(IGEOM2))
       SMIN = MIN( SMIN , SMI2 )
       SMAX = MAX( SMAX , SMA2 )
     ENDIF
     SS = SS * TCHA / TT
     IF( SS < SMIN - tolS ) CYCLE
     IF( SS > SMAX + tolS ) CYCLE
     SStmp = SS
     IF( IC > NCOUBA ) SStmp = SS * ZCECHAV(IGEOM1) / ZZ
     DO J=1,NSS
       IF( ABS(SSval(J)-SStmp) < 10.d0 ) GO TO 20
     ENDDO
     NSS = NSS + 1
     SSval(NSS) = SStmp
     IF( NSS >= NSSmax ) EXIT
20   CONTINUE
   ENDDO
!
 END SUBROUTINE getSSval
!
