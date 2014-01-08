!
 SUBROUTINE DEFPLX( F0, ICOLOR, NB, NI, XI,YI,ZI )
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER  ::  ICOLOR,NB,NI(*)
 REAL(8)  ::  F0,XI(*),YI(*),ZI(*)
 INTEGER :: L
 REAL(8) :: SINF0,COSF0
!
   SINF0 = SIN(F0)
   COSF0 = COS(F0)
   IF( SIZE(NedgeI) < Nplan+1 ) CALL Push_MB_Gen_Vol_MPLAN
   Nplan = Nplan + 1
   NedgeI( Nplan) = Nedge + 1
   NedgeA( Nplan) = Nedge + NB
   IcoPlan(Nplan) = ICOLOR
   IF( SIZE(IvuEdge)   < Nedge+NB ) CALL Push_MB_Gen_Vol_MEDGE
   IF( SIZE(XYZadge)/3 < Nadge+NB ) CALL Push_MB_Gen_Vol_MADGE
   DO L=1,NB
     Nedge = Nedge + 1
     Nadge = Nadge + 1
     IvuEdge(Nedge) = NI(L)
     IadEdge(Nedge) = Nadge
     IF( I_Atlas_Ref_Syst >= 1 ) THEN
       XYZadge(1,Nadge) = XI(L)*COSF0 - ZI(L)*SINF0
       XYZadge(2,Nadge) = YI(L)
       XYZadge(3,Nadge) = XI(L)*SINF0 + ZI(L)*COSF0
     ELSE
       XYZadge(1,Nadge) = XI(L)*COSF0 - YI(L)*SINF0
       XYZadge(2,Nadge) = XI(L)*SINF0 + YI(L)*COSF0
       XYZadge(3,Nadge) = ZI(L)
     ENDIF
   ENDDO
!
 END SUBROUTINE DEFPLX
!
