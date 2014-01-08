!
!> \author SamuSog
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 SUBROUTINE WRITE_AMDC(LUN) 
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LUN
 INTEGER :: Jdeb,Jfin,J
!
#include "AmdcStand/amdcsim_com.inc"
!
      Jdeb = 1
100   Jfin = Jdeb + 9999
      IF( Jfin >= LenAMDC ) THEN
         WRITE (LUN,1000) (CarAMDC(J),J=Jdeb,LenAMDC),CHAR(10)
         RETURN
      ELSE
         DO J=Jfin,Jdeb,-1
          IF( ICHAR(CarAMDC(J)) /= 10 ) CYCLE
             Jfin = J-1
             EXIT
         ENDDO
         WRITE (LUN,1000) (CarAMDC(J),J=Jdeb,Jfin)
         Jdeb = Jfin + 2
      ENDIF
      GO TO 100
!
1000  FORMAT(10001A1)
!
 END SUBROUTINE WRITE_AMDC
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 SUBROUTINE WRITE_AGDD(LUN)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LUN 
 INTEGER :: Jdeb,Jfin,J
!
#include "AmdcStand/amdcsim_com.inc"
!
      Jdeb = 1
100   Jfin = Jdeb + 9999
      IF( Jfin >= LenAGDD ) THEN
         WRITE (LUN,1000) (CarAGDD(J),J=Jdeb,LenAGDD),CHAR(10)
         RETURN
      ELSE
         DO J=Jfin,Jdeb,-1
          IF( ICHAR(CarAGDD(J)) /= 10 ) CYCLE
             Jfin = J-1
             EXIT
         ENDDO
         WRITE (LUN,1000) (CarAGDD(J),J=Jdeb,Jfin)
         Jdeb = Jfin + 2
      ENDIF
      GO TO 100
!
1000  FORMAT(10001A1)
!
 END SUBROUTINE WRITE_AGDD
!
