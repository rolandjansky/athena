!
!> Give the magnetic field BXYZR8(1:3) (and its derivatives BXYZR8(4:12) if N312=12) at the point XYZR8
!> \author M.Virchaux
!
 SUBROUTINE DFIELD(XYZR8,N312, BXYZR8)
 USE M_MB_Control
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: N312
 REAL(8), INTENT(IN)  :: XYZR8(3)
 REAL(8), INTENT(OUT) :: BXYZR8(12)
 REAL(8) :: XYZM8(3)
 REAL(4) :: XYZR4(3), BXYZR4(12)
 INTEGER :: IER
!
   IF( .NOT.NoField ) THEN
     XYZR4(1:3) = XYZR8(1:3)

#if defined(ATLAS_STAND) && !defined(ATLAS_PERSINT)
     IF( FieldVers == 'B-3D' ) THEN        ! obsolete maps
       IF( N312 <= 3 ) THEN
         CALL GBMAGA( XYZR4 , BXYZR4 )
       ELSE
         CALL DBMAGA( XYZR4 , BXYZR4 )
       ENDIF
       BXYZR8(1:N312) = BXYZR4(1:N312)
     ELSE IF( FieldVers == 'FULL' ) THEN   ! full maps (in m)
       XYZM8(1:3) = XYZR8(1:3) * 0.01d0 
       IF( N312 <= 3 ) THEN
         CALL GBMAGZ( XYZM8 , BXYZR8, IER )
       ELSE
         CALL GBMAGZFD( XYZM8 , BXYZR8, IER )
         BXYZR8(4:N312) = 0.01d0 * BXYZR8(4:N312)
       ENDIF
     ELSE
       IF( N312 <= 3 ) THEN
         CALL GBMAGAXX( XYZR4 , BXYZR4 )
       ELSE
         CALL DBMAGAXX( XYZR4 , BXYZR4 )
       ENDIF
       BXYZR8(1:N312) = BXYZR4(1:N312)
     ENDIF
#else
     IF( N312 <= 3 ) THEN
       Call MUONBOYBFIELD( XYZR4 , BXYZR4 )
     ELSE
       Call MUONBOYBFIELDD( XYZR4 , BXYZR4 )
     ENDIF
     BXYZR8(1:N312) = BXYZR4(1:N312)
#endif
   ELSE
     BXYZR8(1:N312) = 0.D0
   ENDIF
!
#if defined(ATLAS_PERSINT)
   CALL PrintField( XYZR8, N312, BXYZR8 )
#endif
!
 END SUBROUTINE DFIELD
!
!
!
 SUBROUTINE BFIELD(X,Y,Z,BX,BY,BZ)
 IMPLICIT NONE
 REAL(8), INTENT(IN)  ::  X,  Y,  Z
 REAL(8), INTENT(OUT) :: BX, BY, BZ
 REAL(8) :: XYZ(3), BXYZ(12)
!
   XYZ(1) = X
   XYZ(2) = Y
   XYZ(3) = Z
   CALL DFIELD(XYZ, 3, BXYZ)
   BX = BXYZ(1)
   BY = BXYZ(2)
   BZ = BXYZ(3)
!
 END SUBROUTINE BFIELD
!
