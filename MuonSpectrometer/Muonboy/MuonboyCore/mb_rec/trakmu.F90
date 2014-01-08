!
 SUBROUTINE TRAKMU(IFLAG)
 USE M_MB_Control
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IFLAG
!
! IFLAG = 0 full tracking
!         1 stop when all planes crossed (i.e. no propagation to spectro entrance)
!         9 special treatment to check the planes to use in TRAKDO
!
   SELECT CASE( ItypFit )
     CASE( 1, 4 )
       CALL GIVDEB1(1)
       CALL TRAKTU(1,IFLAG)
     CASE( 2, 5 )
       CALL GIVDEB1(1)
       CALL TRAKDO(1,IFLAG)
     CASE( 3 )
       CALL GIVDEB2(1)
       CALL TRAFAX(1,IFLAG)
     CASE( 6 )
       CALL GIVDEB1(1)
       CALL TRAFAX(1,IFLAG)       
   END SELECT
!
 END SUBROUTINE TRAKMU
!
