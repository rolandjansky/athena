!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEFPL_2DISC_RIRAZZAA( Jcolor, RI,RA,ZZ,AA, NR )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 INTEGER :: Jcolor,NR
 REAL(8) :: RI(*),RA(*),ZZ(*),AA(*)
 INTEGER :: IFULL,I,J,NP,J_Atlas_Ref_Syst,NI(2*NR+2)
 REAL(8) :: A,DA,A0,PXI(2*NR+2),PYI(2*NR+2),PZI(2*NR+2)
!
   IFULL = 0
   IF( RI(1) < Eps .AND. RI(2) < Eps ) IFULL = 1
   RI(1) = MAX( RI(1) , 2.D0*Eps )
   RI(2) = MAX( RI(2) , 2.D0*Eps )
   RA(1) = MAX( RA(1) , 4.D0*Eps )
   RA(2) = MAX( RA(2) , 4.D0*Eps )
!
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( ABS(AA(2)-AA(1)-360.D0) < Eps ) THEN
     DA = TwoPi / NR
     DO J=1,2
       DO I=1,NR
         A = (I-1)*DA
         PXI(I) = RA(J) * COS(A)
         PYI(I) = RA(J) * SIN(A)
         NI(I)  = 1
       ENDDO
       NP = NR
       IF( IFULL == 0 ) THEN
         NP = NP + 1
         PXI(NP) = PXI(1)
         PYI(NP) = PYI(1)
         NI(NP)  = 0
         DO I=1,NR
           NP = NP + 1
           PXI(NP) = RI(J)/RA(J) * PXI(I)
           PYI(NP) = RI(J)/RA(J) * PYI(I)
           NI(NP)  = 1
         ENDDO
         NP = NP + 1
         PXI(NP) = RI(J)/RA(J) * PXI(1)
         PYI(NP) = RI(J)/RA(J) * PYI(1)
         NI(NP)  = 0
       ENDIF
       PZI(1:NP) = ZZ(J)
       IF( J_Atlas_Ref_Syst >= 1 ) THEN
         PZI(1:NP) = - PZI(1:NP)
         CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PZI,PYI )
       ELSE
         CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PYI,PZI )
       ENDIF
     ENDDO
   ELSE
     A0 =     AA(1)    *PiS180
     DA = (AA(2)-AA(1))*PiS180 / NR
     DO J=1,2
       DO I=1,NR+1
         A = A0 + (I-1)*DA
         PXI(I) = RA(J) * COS(A)
         PYI(I) = RA(J) * SIN(A)
         NI(I)  = 1
       ENDDO
       IF( IFULL == 1 ) THEN
         NP = NR + 2
         PXI(NP) = 0.D0
         PYI(NP) = 0.D0
         NI(NP)  = 1
       ELSE
         NP = NR + 1
         DO I=NR+1,1,-1
           NP = NP + 1
           PXI(NP) = RI(J)/RA(J) * PXI(I)
           PYI(NP) = RI(J)/RA(J) * PYI(I)
           NI(NP)  = 1
         ENDDO
       ENDIF
       PZI(1:NP) = ZZ(J)
       IF( J_Atlas_Ref_Syst >= 1 ) THEN
         PZI(1:NP) = - PZI(1:NP)
         CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PZI,PYI )
       ELSE
         CALL DEFPLX( Zero, Jcolor, NP, NI,PXI,PYI,PZI )
       ENDIF
     ENDDO
   ENDIF
!
 END
