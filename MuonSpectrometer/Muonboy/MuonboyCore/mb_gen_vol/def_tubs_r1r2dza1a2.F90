!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_TUBS_R1R2DZA1A2( Name, Jcolor, RZA, NR )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
!>>                 1     2      3     4       5
!>>     RZA(.)  =  Rmin  Rmax   dZ   Phimin  Phimax
!
 CHARACTER (len=*) :: Name
 INTEGER           :: Jcolor,NR
 REAL(8)           :: RZA(*)
 INTEGER :: I,NP,NI(2*NR+2),J_Atlas_Ref_Syst
 REAL(8) :: RI,RA,ZI,ZA,AI,AA,A0,DA,A
 REAL(8) :: PXI(2*NR+2),PYI(2*NR+2),PZI(2*NR+2)
!
   RI = MIN( RZA(1) , RZA(2) )
   RA = MAX( RZA(1) , RZA(2) )
   ZI = MIN( -RZA(3)/2.D0 , RZA(3)/2.D0 )
   ZA = MAX( -RZA(3)/2.D0 , RZA(3)/2.D0 )
   AI = RZA(4)
   AA = RZA(5)
   IF( AA < AI ) AA = AA + 360.D0
   IF( RI < -Eps )             RETURN
   IF( RA <  Eps )             RETURN
   IF( RA-RI < Eps )           RETURN
   IF( ZA-ZI < Eps )           RETURN
   IF( AA-AI < Eps )           RETURN
   IF( AA-AI > 360.D0+Eps )    RETURN
!
   IF( ABS(AA-AI-360.D0) < Eps ) THEN
     CALL DEF_TUBE_R1R2DZN( Name, Jcolor, RZA, NR )
   ELSE
     A0 =    AI  *PiS180
     DA = (AA-AI)*PiS180 / NR
     DO I=1,NR+1
       A = A0 + (I-1)*DA
       PXI(I) = RA * COS(A)
       PYI(I) = RA * SIN(A)
       NI(I)  = 1
     ENDDO
     IF( RI < Eps ) THEN
       NP = NR + 2
       PXI(NP) = 0.D0
       PYI(NP) = 0.D0
       NI(NP)  = 1
     ELSE
       NP = NR + 1
       DO I=NR+1,1,-1
         NP = NP + 1
         PXI(NP) = RI/RA * PXI(I)
         PYI(NP) = RI/RA * PYI(I)
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
 END
