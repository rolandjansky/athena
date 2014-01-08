!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_GVXYSX_NXYDZ( Name, Jcolor, NXY, XDX, YDY, DZ )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Jcolor,NXY
 REAL(8),           INTENT(IN) :: XDX(*),YDY(*),DZ
!
 INTEGER :: NB,J_Atlas_Ref_Syst,NI(2*NXY)
 REAL(8) :: XI(2*NXY),YI(2*NXY),ZI(2*NXY)
!
   NB = 2*NXY
   NI(    1: NB) = 1
   XI(    1:NXY) =   XDX( 1 :NXY)
   YI(    1:NXY) =   YDY( 1 :NXY)
   XI(NXY+1: NB) = - XDX(NXY: 1 :-1)
   YI(NXY+1: NB) =   YDY(NXY: 1 :-1)
   CALL NEWVOL( Name, 1 )
   CALL DEF_CENVOL( Zero,Zero,Zero )
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
      ZI(1:NB) =   DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB, NI, XI,ZI,YI )
      ZI(1:NB) = - DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB, NI, XI,ZI,YI )
   ELSE
      ZI(1:NB) = - DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB, NI, XI,YI,ZI )
      ZI(1:NB) =   DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB, NI, XI,YI,ZI )
   ENDIF
   CALL CLOSE( Jcolor, 1 )
   CALL ENDVOL_SUV
!
 END
