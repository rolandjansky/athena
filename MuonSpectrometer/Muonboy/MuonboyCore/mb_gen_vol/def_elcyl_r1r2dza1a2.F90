!
!> \author J.Meyer
!
 SUBROUTINE DEF_ELCYL_R1R2DZA1A2( Name, Jcolor, RZA, NR )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
!>>                 1     2       3     4      5     6       7
!>>     RZA(.)  = RXmin  RXmax  RYmin  RYmax  dZ   Phimin  Phimax
!
 CHARACTER(*), INTENT(IN) :: Name
 INTEGER,      INTENT(IN) :: Jcolor,NR
 REAL(8),      INTENT(IN) :: RZA(*)
 INTEGER :: I,NP,NI(2*NR+2),J_Atlas_Ref_Syst
 REAL(8) :: RIX,RAX,RIY,RAY,ZI,ZA,AI,AA,A0,DA,A
 REAL(8) :: PXI(2*NR+2),PYI(2*NR+2),PZI(2*NR+2)
!
   RIX = MIN( RZA(1) , RZA(2) )
   RAX = MAX( RZA(1) , RZA(2) )
   RIY = MIN( RZA(3) , RZA(4) )
   RAY = MAX( RZA(3) , RZA(4) )
   ZI = MIN( -RZA(5)/2.D0 , RZA(5)/2.D0 )
   ZA = MAX( -RZA(5)/2.D0 , RZA(5)/2.D0 )
   AI = RZA(6)
   AA = RZA(7)
   IF( AA < AI ) AA = AA + 360.D0
   IF( RIX < -Eps )            RETURN
   IF( RAX <  Eps )            RETURN
   IF( RIY < -Eps )            RETURN
   IF( RAY <  Eps )            RETURN
   IF( RAX-RIX < Eps )         RETURN
   IF( RAY-RIY < Eps )         RETURN
   IF( ZA-ZI < Eps )           RETURN
   IF( AA-AI < Eps )           RETURN
   IF( AA-AI > 360.D0+Eps )    RETURN
   IF( ABS(AA-AI-360.D0) < Eps ) THEN
     CALL DEF_ELCYL_R1R2DZN( Name, Jcolor, RZA, NR )
   ELSE
     A0 =    AI  *PiS180
     DA = (AA-AI)*PiS180 / NR
     DO I=1,NR+1
       A = A0 + (I-1)*DA
       PXI(I) = RAX * COS(A)
       PYI(I) = RAY * SIN(A)
       NI(I)  = 1
     ENDDO
     IF( RIX < Eps .AND. RIY < Eps) THEN
       NP = NR + 2
       PXI(NP) = 0.D0
       PYI(NP) = 0.D0
       NI(NP)  = 1
     ELSE
       NP = NR + 1
       DO I=NR+1,1,-1
         NP = NP + 1
         PXI(NP) = RIX/RAX * PXI(I)
         PYI(NP) = RIY/RAY * PYI(I)
         NI(NP)  = 1
       ENDDO
     ENDIF
     CALL NEWVOL(Name,1)
     CALL DEF_CENVOL(Zero,Zero,Zero)
     CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
     IF( J_Atlas_Ref_Syst >= 1 ) THEN
       PZI(1:NP) = -ZI
       CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PZI,PYI )
       PZI(1:NP) = -ZA
       CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PZI,PYI )
     ELSE
       PZI(1:NP) = ZI
       CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PYI,PZI )
       PZI(1:NP) = ZA
       CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PYI,PZI )
     ENDIF
     CALL CLOSE(Jcolor,1)
     CALL ENDVOL_SUV
   ENDIF
!
 END SUBROUTINE DEF_ELCYL_R1R2DZA1A2
!
