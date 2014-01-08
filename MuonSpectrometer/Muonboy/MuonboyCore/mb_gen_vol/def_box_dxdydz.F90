!
 SUBROUTINE DEF_BOX_DXDYDZ( Name, Jcolor, XDX )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
!>>               1   2   3
!>>   XDX(.)  =  dX  dY  dZ
!
 CHARACTER(*) :: Name
 INTEGER      :: Jcolor
 REAL(8)      :: XDX(*)
 INTEGER :: I,J_Atlas_Ref_Syst
 REAL(8) :: X1,X2,Y1,Y2,Z1,Z2
!
   DO I=1,3
     IF( ABS(XDX(I)) < Eps ) RETURN
   ENDDO
!
   X1 = -XDX(1) / 2.D0
   X2 =  XDX(1) / 2.D0
   Y1 = -XDX(2) / 2.D0
   Y2 =  XDX(2) / 2.D0
   Z1 = -XDX(3) / 2.D0
   Z2 =  XDX(3) / 2.D0
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, X1,X2,X2,X1, -Z1,-Z1,-Z1,-Z1, Y1,Y1,Y2,Y2 )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, X1,X2,X2,X1, -Z2,-Z2,-Z2,-Z2, Y1,Y1,Y2,Y2 )
   ELSE
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, X1,X2,X2,X1, Y1,Y1,Y2,Y2, Z1,Z1,Z1,Z1 )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, X1,X2,X2,X1, Y1,Y1,Y2,Y2, Z2,Z2,Z2,Z2 )
   ENDIF
   CALL CLOSE(Jcolor,1)
   CALL ENDVOL_SUV
!
 END SUBROUTINE DEF_BOX_DXDYDZ
!
