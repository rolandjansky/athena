!
 SUBROUTINE DEF_CONE_RDZN( Name, Jcolor, RDZ, NN )
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>                  1        2        3
!>>     RDZ(.)  =  Radius   dZ/2    Radius
!>>                outer            inner
!
 CHARACTER(*) :: Name
 INTEGER      :: Jcolor,NN
 REAL(8)      :: RDZ(*)
 INTEGER :: J,JP1,IADFIN,IAD3(3),NII(2*NN+2),IAD(2*NN+2)
 REAL(8) :: ANG,X,Y,Z
!
   IF(     RDZ(1)  < Eps )    RETURN
   IF(     RDZ(3) >= RDZ(1) ) RETURN
   IF( ABS(RDZ(2)) < Eps )    RETURN
   IF(          NN < 3      ) RETURN
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
!
   IF( RDZ(3) < Eps ) THEN
     IF( SIZE(XYZadge)/3 < Nadge+NN+1 ) CALL Push_MB_Gen_Vol_MADGE
     DO J=1,NN
       ANG = (J-1.D0) * TwoPi / NN
       X =   RDZ(1) * COS(ANG)
       Y =   RDZ(1) * SIN(ANG)
       Z = - RDZ(2)
       CALL ADD_ADGE_XYZ_Ref_Syst(X,Y,Z, IAD(J))
       NII(J) = 1
     ENDDO
     CALL DEFPLA( Jcolor, NN, NII, IAD )
     CALL ADD_ADGE_XYZ_Ref_Syst(Zero,Zero,RDZ(2), IADFIN)
     DO J=1,NN
       JP1 = MOD(J,NN) + 1
       IAD3(1) = IAD(J)
       IAD3(2) = IAD(JP1)
       IAD3(3) = IADFIN
       CALL DEFPLA( Jcolor, 3, NII, IAD3 )
     ENDDO
   ELSE
     IF( SIZE(XYZadge)/3 < Nadge+2*NN+1 ) CALL Push_MB_Gen_Vol_MADGE
     NII(1:2*NN+2) = 1
     DO J=1,NN
       ANG = (J-1.D0) * TwoPi / NN
       X =   RDZ(1) * COS(ANG)
       Y =   RDZ(1) * SIN(ANG)
       Z = - RDZ(2)
       CALL ADD_ADGE_XYZ_Ref_Syst(X,Y,Z, IAD(J))
     ENDDO
     IAD(NN+1) = IAD(1)
     NII(NN+1) = 0
     DO J=1,NN
       ANG = (NN+1.D0-J) * TwoPi / NN
       X =   RDZ(3) * COS(ANG)
       Y =   RDZ(3) * SIN(ANG)
       Z = - RDZ(2)
       CALL ADD_ADGE_XYZ_Ref_Syst(X,Y,Z, IAD(NN+1+J))
     ENDDO
     IAD(2*NN+2) = IAD(NN+2)
     NII(2*NN+2) = 0
     CALL DEFPLA( Jcolor, 2*NN+2, NII, IAD )
     CALL ADD_ADGE_XYZ_Ref_Syst(Zero,Zero,RDZ(2), IADFIN)
     DO J=1,2*NN+1
       IF( J == NN+1 ) CYCLE
       IAD3(1) = IAD(J)
       IAD3(2) = IAD(J+1)
       IAD3(3) = IADFIN
       CALL DEFPLA( Jcolor, 3, NII, IAD3 )
     ENDDO
   ENDIF
!
   CALL ENDVOL_SUV
!
 END SUBROUTINE DEF_CONE_RDZN
!
