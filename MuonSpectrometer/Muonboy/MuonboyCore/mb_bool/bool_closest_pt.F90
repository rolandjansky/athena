!
!> Find which are the two points (one of polygon (XPA,YPA) and the
!> other of polygon (XPB,YPB) ) which are the closest of each other
!> \author SaMuSoG
!
 SUBROUTINE Bool_Closest_Pt(NPA,XPA,YPA,NPB,XPB,YPB, IPA,IPB)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: NPA,NPB
 INTEGER, INTENT(OUT) :: IPA,IPB
 REAL(8), INTENT(IN)  :: XPA(*),YPA(*),XPB(*),YPB(*)
 INTEGER :: JA,JB
 REAL(8) :: dd,dd0
!
   IPA = 1
   IPB = 1
   dd0 = (XPA( 1)-XPB( 1))**2 + (YPA( 1)-YPB( 1))**2
   DO JA=1,NPA
     DO JB=1,NPB
       dd  = (XPA(JA)-XPB(JB))**2 + (YPA(JA)-YPB(JB))**2
       IF( dd >= dd0 ) CYCLE
       IPA = JA
       IPB = JB
       dd0 = dd
     ENDDO
   ENDDO
!
 END SUBROUTINE Bool_Closest_Pt
!
