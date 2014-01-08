!
 SUBROUTINE DEFPLA( ICOLOR, NB, NI, IAD )
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ICOLOR,NB,NI(*),IAD(*)
 INTEGER :: I
!
   IF( SIZE(NedgeI) < Nplan+1 ) CALL Push_MB_Gen_Vol_MPLAN
   Nplan = Nplan + 1
   NedgeI( Nplan) = Nedge + 1
   NedgeA( Nplan) = Nedge + NB
   IcoPlan(Nplan) = ICOLOR
   IF( SIZE(IvuEdge) < Nedge+NB ) CALL Push_MB_Gen_Vol_MEDGE
   DO I=1,NB
     IvuEdge(Nedge+I) = NI(I)
     IadEdge(Nedge+I) = IAD(I)
   ENDDO
   Nedge = Nedge + NB
!
 END SUBROUTINE DEFPLA
!
