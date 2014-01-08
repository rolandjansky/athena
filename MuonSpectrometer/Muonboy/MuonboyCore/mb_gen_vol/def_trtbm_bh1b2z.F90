!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_TRTBM_BH1B2Z( Name, Jcolor, XDX )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!>>                 1   2   3   4
!>>     XDX(.)  =  B1  h1  B2  dZ
!
 CHARACTER (len=*) :: Name
 INTEGER           :: Jcolor
 REAL(8)           :: XDX(*)
 INTEGER :: I,J_Atlas_Ref_Syst
 REAL(8) :: Alpha2,Delta2,X1,X2,X3,X4,Y1,Y2,Y3,Y4,Z1,Z2,DY
!
   DO I=1,4
     IF( ABS(XDX(I)) < Eps ) RETURN
   ENDDO
!
   Alpha2 = XDX(1)**2 + 4.D0 * XDX(2)**2
   Delta2 = Alpha2 - XDX(3)**2
   IF( Delta2 < Eps ) THEN
     PRINT 1000,XDX(1:4)
1000 FORMAT(' WARNING !  : Attempt to create a solid of type TRTMODULE with inconsistent parameters :',   &
            /'***********   ',4F12.4)
     RETURN
   ENDIF
!
   X1 = -XDX(1) / 2.D0
   X2 =  0.D0
   X3 =  XDX(1) / 2.D0
   Y1 = -XDX(2) / 2.D0
   Y2 =  XDX(2) / 2.D0
   Y3 = -XDX(2) / 2.D0
   Z1 = -XDX(4) / 2.D0
   Z2 =  XDX(4) / 2.D0
   DY = ( XDX(1)*XDX(3)*SQRT(Delta2) - 2.D0*XDX(2)*XDX(3)**2 ) / Alpha2
   X4 = - ( XDX(3)**2 + 2.D0*XDX(2)*DY ) / XDX(1)
   Y4 = Y2 + DY
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1+Eps,X2+Eps,X3, -Z1,-Z1,-Z1, Y1,Y2,Y3 )
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1+Eps,X2+Eps,X3, -Z2,-Z2,-Z2, Y1,Y2,Y3 )
     CALL CLOSE(Jcolor,1)
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1-Eps,X2-Eps,X4, -Z1,-Z1,-Z1, Y1,Y2,Y4 )
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1-Eps,X2-Eps,X4, -Z2,-Z2,-Z2, Y1,Y2,Y4 )
     CALL CLOSE(Jcolor,1)
   ELSE
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1+Eps,X2+Eps,X3, Y1,Y2,Y3, Z1,Z1,Z1 )
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1+Eps,X2+Eps,X3, Y1,Y2,Y3, Z2,Z2,Z2 )
     CALL CLOSE(Jcolor,1)
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1-Eps,X2-Eps,X4, Y1,Y2,Y4, Z1,Z1,Z1 )
     CALL DEFPL3( Zero, Jcolor, 1,1,1, X1-Eps,X2-Eps,X4, Y1,Y2,Y4, Z2,Z2,Z2 )
     CALL CLOSE(Jcolor,1)
   ENDIF
   CALL ENDVOL_SUV
!
 END
