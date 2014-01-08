!
 SUBROUTINE DEF_ARROW_R1R2Z1Z2N( Name, Jcolor, RZ, NR )
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>              1    2    3    4
!>>   RZ(.)  =  R1   R2   Z1   Z2
!>>
!>>
!>>                              |\       ^
!>>                              | \      |
!>>      ^   ---------------------  \     | R2
!>>   R1 |   |                       \    |
!>>      -   |         O              >   -
!>>          |                       /
!>>          ---------------------  /
!>>                              | /
!>>                              |/
!>>
!>>          <------------------><---->
!>>                    Z1           Z2
!
 CHARACTER(*) :: Name
 INTEGER      :: Jcolor,NR
 REAL(8)      :: RZ(*)
 INTEGER :: L,J,JP1,IADFIN,IAD4(4),NII(NR),IAD(3*NR)
 REAL(8) :: Rcur,Zcur,ANG,Xcur,Ycur
!
   DO L=1,4
     IF( ABS(RZ(L)) < Eps ) RETURN
   ENDDO
!
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
!
   IF( SIZE(XYZadge)/3 < Nadge+3*NR+1 ) CALL Push_MB_Gen_Vol_MADGE
   DO L=0,2
     Rcur = RZ(1)
     IF( L == 2 ) Rcur = RZ(2)
     Zcur = RZ(3)/2.D0
     IF( L == 0 ) Zcur = - RZ(3)/2.D0
     DO J=1,NR
       ANG = (J-1.D0) * TwoPi / NR
       Xcur = Rcur * COS(ANG)
       Ycur = Rcur * SIN(ANG)
       CALL ADD_ADGE_XYZ_Ref_Syst(Xcur,Ycur,Zcur, IAD(J+L*NR))
     ENDDO
   ENDDO
   Zcur = RZ(3)/2.D0 + RZ(4)
   CALL ADD_ADGE_XYZ_Ref_Syst(Zero,Zero,Zcur, IADFIN)
!
   NII(:) = 1
   CALL DEFPLA( Jcolor, NR, NII, IAD )
   DO J=1,NR
     JP1 = MOD(J,NR) + 1
     IAD4(1) = IAD(J       )
     IAD4(2) = IAD(JP1     )
     IAD4(3) = IAD(JP1+  NR)
     IAD4(4) = IAD(J  +  NR)
     CALL DEFPLA( Jcolor, 4, NII, IAD4 )
     IAD4(1) = IAD(J  +2*NR)
     IAD4(2) = IAD(JP1+2*NR)
     CALL DEFPLA( Jcolor, 4, NII, IAD4 )
     IAD4(3) = IADFIN
     CALL DEFPLA( Jcolor, 3, NII, IAD4 )
   ENDDO
!
   CALL ENDVOL_SUV
!
 END SUBROUTINE DEF_ARROW_R1R2Z1Z2N
!
