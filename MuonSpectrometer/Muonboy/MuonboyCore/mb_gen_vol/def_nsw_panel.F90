!
 SUBROUTINE DEF_NSW_PANEL( Name, Jcolor, XDX )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
!>>               1       2       3       4
!>>   XDX(.)  =  lWidth  sWidth  Length  Tck
!
 CHARACTER(*) :: Name
 INTEGER      :: Jcolor
 REAL(8)      :: XDX(*)
 INTEGER :: I,J_Atlas_Ref_Syst
 REAL(8) :: X1s,X2s,X1l,X2l,Y1,Y2,Z1,Z2
!
   DO I=1,4
     IF( ABS(XDX(I)) < Eps ) RETURN
   ENDDO
!
   X1l = -XDX(1) / 2.D0
   X2l =  XDX(1) / 2.D0
   X1s = -XDX(2) / 2.D0
   X2s =  XDX(2) / 2.D0
   Y1 = -XDX(3) / 2.D0
   Y2 =  XDX(3) / 2.D0
   Z1 = -XDX(4) / 2.D0
   Z2 =  XDX(4) / 2.D0
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, Z1,Z2,Z2,Z1, -Y1,-Y1,-Y1,-Y1, X1s,X1s,X2s,X2s )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, Z1,Z2,Z2,Z1, -Y2,-Y2,-Y2,-Y2, X1l,X1l,X2l,X2l )
   ELSE
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, Z1,Z2,Z2,Z1, X1s,X1s,X2s,X2s, Y1,Y1,Y1,Y1 )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, Z1,Z2,Z2,Z1, X1l,X1l,X2l,X2l, Y2,Y2,Y2,Y2 )
   ENDIF
   CALL CLOSE(Jcolor,1)
   CALL ENDVOL_SUV
!
 END SUBROUTINE DEF_NSW_PANEL
!
