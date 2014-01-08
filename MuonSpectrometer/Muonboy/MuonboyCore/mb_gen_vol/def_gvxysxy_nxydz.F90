!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_GVXYSXY_NXYDZ( Name, Jcolor, NXY, XDX, YDY, DZ )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Jcolor,NXY
 REAL(8),           INTENT(IN) :: XDX(*),YDY(*),DZ
!
 INTEGER :: NB2,NB4,J_Atlas_Ref_Syst,NI(4*NXY)
 REAL(8) :: XI(4*NXY),YI(4*NXY),ZI(4*NXY)
!
   NB2 = 2*NXY
   NB4 = 4*NXY
   NI(    1:NB4) = 1
   XI(    1:NXY) =   XDX( 1 :NXY)
   YI(    1:NXY) =   YDY( 1 :NXY)
   XI(NXY+1:NB2) = - XDX(NXY: 1 :-1)
   YI(NXY+1:NB2) =   YDY(NXY: 1 :-1)
   XI(NB2+1:NB4) =   XI (NB2: 1 :-1)
   YI(NB2+1:NB4) = - YI (NB2: 1 :-1)
   CALL NEWVOL( Name, 1 )
   CALL DEF_CENVOL( Zero,Zero,Zero )
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
      ZI(1:NB4) =   DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB4, NI, XI,ZI,YI )
      ZI(1:NB4) = - DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB4, NI, XI,ZI,YI )
   ELSE
      ZI(1:NB4) = - DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB4, NI, XI,YI,ZI )
      ZI(1:NB4) =   DZ/2.D0
      CALL DEFPLX( Zero, Jcolor, NB4, NI, XI,YI,ZI )
   ENDIF
   CALL CLOSE( Jcolor, 1 )
   CALL ENDVOL_SUV
!
 END
