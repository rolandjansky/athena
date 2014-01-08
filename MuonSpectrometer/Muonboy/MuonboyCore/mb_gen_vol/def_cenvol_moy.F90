!
 SUBROUTINE DEF_CENVOL_MOY
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER :: IVOL,IP,IE,IA
 REAL(8) :: OCEN,XYZCEN(3)
!
   IVOL = Nvolu
   OCEN        = 0.D0
   XYZCEN(1:3) = 0.D0
   DO IP=NplanI(1,IVOL),NplanA(1,IVOL)
     DO IE=NedgeI(IP),NedgeA(IP)
       IA = IadEdge(IE)
       OCEN       = OCEN      + 1.D0
       XYZCEN(1:3) = XYZCEN(1:3) + XYZadge(1:3,IA)
     ENDDO
   ENDDO
   OCEN = MAX( 1.D0 , OCEN )
   CenVolu(1:3,IVOL) = XYZCEN(1:3) / OCEN
!
 END SUBROUTINE DEF_CENVOL_MOY
!
