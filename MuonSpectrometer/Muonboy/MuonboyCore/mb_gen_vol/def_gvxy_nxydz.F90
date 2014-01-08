!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_GVXY_NXYDZ( Name, Jcolor, NXY, XDX, YDY, DZ )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Jcolor,NXY
 REAL(8),           INTENT(IN) :: XDX(*),YDY(*),DZ
!
 INTEGER :: J_Atlas_Ref_Syst,NI(NXY)
 REAL(8) :: ZI(NXY)
!
!
   NI(1:NXY) = 1 
   CALL NEWVOL( Name, 1 )
   CALL DEF_CENVOL( Zero,Zero,Zero )
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
      ZI(1:NXY) =   DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NXY, NI, XDX,ZI,YDY )
      ZI(1:NXY) = - DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NXY, NI, XDX,ZI,YDY )
   ELSE
      ZI(1:NXY) = - DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NXY, NI, XDX,YDY,ZI )
      ZI(1:NXY) =   DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NXY, NI, XDX,YDY,ZI )
   ENDIF  
   CALL CLOSE( Jcolor, 1 )
   CALL ENDVOL_SUV
!
 END
