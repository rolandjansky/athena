!
 LOGICAL FUNCTION isMisaligned(IC,INOF,INOZ)
 USE M_MB_MuGeom, ONLY : ResBadCha
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IC, INOF, INOZ
!
   isMisaligned = .false.
   SELECT CASE( IC )
     CASE(  2 )
       IF( INOZ == 1 .OR. INOZ == 16 ) isMisaligned = .true.    ! BIS8
     CASE( 16 )
       isMisaligned = .true.                                    ! BEE
     CASE( 19, 20 )
       IF( INOF == 1 .OR. INOF == 5 ) THEN
         IF( INOZ == 4 .OR. INOZ == 13 ) isMisaligned = .true.  ! EIL sector 1 and 5, eta=4
       ENDIF
     CASE( 23, 24 )
       IF( INOF == 3 ) isMisaligned = .true.                    ! EEL sector 3
   END SELECT
   IF( isMisaligned ) THEN
     IF( ResBadCha(IC) < 0.001d0 ) isMisaligned = .false.
   ENDIF
!
 END FUNCTION isMisaligned
!
!
!
 LOGICAL FUNCTION isBadlyBuilt(IC,INOF,INOZ)
 USE M_MB_Control, ONLY : ResBadTomo
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IC, INOF, INOZ
!
 isBadlyBuilt = .false.
 IF( ResBadTomo < 0.001d0 ) RETURN
 IF( IC == 3 ) THEN
   IF( INOF == 7 ) THEN
     IF( INOZ == 9 )         isBadlyBuilt = .true.     ! BIL1A13
   ENDIF
 ENDIF
!
 END FUNCTION isBadlyBuilt
!
