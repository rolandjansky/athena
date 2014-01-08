!
 SUBROUTINE DEF_BOX_DX12DY12DZ( Name, Jcolor, XDX )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 CHARACTER(*) :: Name
 INTEGER      :: Jcolor
 REAL(8)      :: XDX(*)
 INTEGER :: I,J_Atlas_Ref_Syst
 REAL(8) :: XI1,XI2,XA1,XA2,YI1,YI2,YA1,YA2,ZI,ZA
!
!>>                 1    2    3    4    5
!>>     XDX(.)  =  dX1  dX2  dY1  dY2  dZ
!
   DO I=1,5
     IF( ABS(XDX(I)) < Eps ) RETURN
   ENDDO
   XI1 = -XDX(1) / 2.D0
   XI2 =  XDX(1) / 2.D0
   XA1 = -XDX(2) / 2.D0
   XA2 =  XDX(2) / 2.D0
   YI1 = -XDX(3) / 2.D0
   YI2 =  XDX(3) / 2.D0
   YA1 = -XDX(4) / 2.D0
   YA2 =  XDX(4) / 2.D0
   ZI  = -XDX(5) / 2.D0
   ZA  =  XDX(5) / 2.D0
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XI1,XI2,XI2,XI1, -ZI,-ZI,-ZI,-ZI, YI1,YI1,YI2,YI2 )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XA1,XA2,XA2,XA1, -ZA,-ZA,-ZA,-ZA, YA1,YA1,YA2,YA2 )
   ELSE
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XI1,XI2,XI2,XI1, YI1,YI1,YI2,YI2, ZI,ZI,ZI,ZI )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XA1,XA2,XA2,XA1, YA1,YA1,YA2,YA2, ZA,ZA,ZA,ZA )
   ENDIF
   CALL CLOSE(Jcolor,1)
   CALL ENDVOL_SUV
!
 END SUBROUTINE DEF_BOX_DX12DY12DZ
!
