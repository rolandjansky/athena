!
!> \author J.Meyer
!
 SUBROUTINE DEF_ELCYL_R1R2DZN( Name, Jcolor, RZ, NR )
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>                 1     2       3     4      5   
!>>     RZA(.)  = RXmin  RXmax  RYmin  RYmax  dZ
!
 CHARACTER(len=*), INTENT(IN) :: Name
 INTEGER,          INTENT(IN) :: Jcolor,NR
 REAL(8),          INTENT(IN) :: RZ(*)
 INTEGER :: I12,NR12,I,NP,Istep,IAD4(4)
 INTEGER :: NII(2*NR+26),IADI(2*NR+26),IADA(2*NR+26)
 REAL(8) :: RXI,RXA,RYI,RYA,RA,ZI,ZA,RAZAmax,A,DA
 REAL(8) :: PXI(NR+12),PYI(NR+12)
!
   IF( RZ(1) < Eps .AND. RZ(2) < Eps ) RETURN
   IF( RZ(3) < Eps .AND. RZ(4) < Eps ) RETURN
   IF( ABS(RZ(2)-RZ(1)) < Eps )        RETURN
   IF( ABS(RZ(4)-RZ(3)) < Eps )        RETURN
   IF( ABS(RZ(5)) < Eps )              RETURN
!
   RXI = MIN( RZ(1) , RZ(2) )
   RXA = MAX( RZ(1) , RZ(2) )
   RYI = MIN( RZ(3) , RZ(4) )
   RYA = MAX( RZ(3) , RZ(4) )
   ZI = MIN( -RZ(5)/2.D0 , RZ(5)/2.D0 )
   ZA = MAX( -RZ(5)/2.D0 , RZ(5)/2.D0 )
   RA = MAX( RYA , RXA )
   RAZAmax = MAX( RA , ZA )
!
!>> Principal volume !!!
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL DEF_D4Volu( RA , RA*5.D0 , RA*16.D0 , RAZAmax )
   NR12 = NR
   IF( MOD(NR,12) == 0 ) THEN
     I12 = NR / 12
   ELSE
     I12 = 0
   ENDIF
!
   DA = TwoPi / NR12
   DO I=1,NR12
     A = (I-1)*DA
     PXI(I) = RXA * COS(A)
     PYI(I) = RYA * SIN(A)
     NII(I) = 1
   ENDDO
!
   IF( RXI <= Eps .OR. RYI <= Eps) THEN
     NP = NR12
     NII(1:NP) = 1
     IF( SIZE(XYZadge)/3 < Nadge+2*NP ) CALL Push_MB_Gen_Vol_MADGE
     DO I=1,NP
       CALL ADD_ADGE_XYZ_Ref_Syst(PXI(I),PYI(I),ZI, IADI(I))
       CALL ADD_ADGE_XYZ_Ref_Syst(PXI(I),PYI(I),ZA, IADA(I))
     ENDDO
   ELSE
     NP = 2*NR12 + 2
     NII( 1:NP ) = 1
     NII(NR12+1) = 0
     NII(  NP  ) = 0
     IF( SIZE(XYZadge)/3 < Nadge+4*NR12 ) CALL Push_MB_Gen_Vol_MADGE
     DO I=1,NR12
       CALL ADD_ADGE_XYZ_Ref_Syst(PXI(I),PYI(I),ZI, IADI(I))
       CALL ADD_ADGE_XYZ_Ref_Syst(PXI(I),PYI(I),ZA, IADA(I))
     ENDDO
     IADI(NR12+1) = IADI(1)
     IADA(NR12+1) = IADA(1)
     DO I=1,NR12
       CALL ADD_ADGE_XYZ_Ref_Syst(RXI/RXA*PXI(I),RYI/RYA*PYI(I),ZI,IADI(NR12+1+I))
       CALL ADD_ADGE_XYZ_Ref_Syst(RXI/RXA*PXI(I),RYI/RYA*PYI(I),ZA,IADA(NR12+1+I))
     ENDDO
     IADI(2*NR12+2) = IADI(NR12+2)
     IADA(2*NR12+2) = IADA(NR12+2)
   ENDIF
   CALL DEFPLA( Jcolor, NP, NII, IADI )
   CALL DEFPLA( Jcolor, NP, NII, IADA )
   CALL CLOSE(Jcolor,1)
   CALL ENDVOL
!
!>> Simplified sub-volumes !!!
!
   IF( I12 <= 0 ) THEN
!>> Simplification level 4 !
     Istep = (NR-1) / 2
     CALL NEWsubVOL(4)
     IAD4(1) = IADI(1)
     IAD4(2) = IADI(1+Istep)
     IAD4(3) = IADA(1+Istep)
     IAD4(4) = IADA(1)
     CALL DEFPLA( Jcolor, 4, NII, IAD4 )
     IAD4(1) = IADI(1+Istep)
     IAD4(2) = IADI(1+2*Istep)
     IAD4(3) = IADA(1+2*Istep)
     IAD4(4) = IADA(1+Istep)
     CALL DEFPLA( Jcolor, 4, NII, IAD4 )
     CALL ENDsubVOL(4)
   ELSE
!>> Simplification level 2 (possibly) !
     IF( I12 > 1 ) THEN
       CALL NEWsubVOL(2)
       IF( NP == NR12 ) THEN
         NP = 12
         NII(1:12) = 1
         DO I=1,12
           IADI(I) = IADI(I12*I-I12+1)
           IADA(I) = IADA(I12*I-I12+1)
         ENDDO
       ELSE
         NP = 26
         NII(1:26) = 1
         NII( 13 ) = 0
         NII( 26 ) = 0
         DO I=1,12
           IADI(I) = IADI(I12*I-I12+1)
           IADA(I) = IADA(I12*I-I12+1)
         ENDDO
         IADI(13) = IADI(1)
         IADA(13) = IADA(1)
         DO I=1,12
           IADI(13+I) = IADI(NR12+1+I12*I-I12+1)
           IADA(13+I) = IADA(NR12+1+I12*I-I12+1)
         ENDDO
         IADI(26) = IADI(14)
         IADA(26) = IADA(14)
       ENDIF
       CALL DEFPLA( Jcolor, NP, NII, IADI )
       CALL DEFPLA( Jcolor, NP, NII, IADA )
       CALL CLOSE(Jcolor,1)
       CALL ENDsubVOL(2)
     ENDIF
!>> Simplification level 3 !
     CALL NEWsubVOL(3)
     IF( NP == 12 ) THEN
       NP = 6
       NII(1:6) = 1
       DO I=1,6
         IADI(I) = IADI(2*I-1)
         IADA(I) = IADA(2*I-1)
       ENDDO
     ELSE
       NP = 14
       NII(1:14) = 1
       NII(  7 ) = 0
       NII( 14 ) = 0
       DO I=1,6
         IADI(I) = IADI(2*I-1)
         IADA(I) = IADA(2*I-1)
       ENDDO
       IADI(7) = IADI(1)
       IADA(7) = IADA(1)
       DO I=1,6
         IADI(7+I) = IADI(13+2*I-1)
         IADA(7+I) = IADA(13+2*I-1)
       ENDDO
       IADI(14) = IADI(8)
       IADA(14) = IADA(8)
      ENDIF
      CALL DEFPLA( Jcolor, NP, NII, IADI )
      CALL DEFPLA( Jcolor, NP, NII, IADA )
      CALL CLOSE(Jcolor,1)
      CALL ENDsubVOL(3)
!>> Simplification level 4 !
      CALL NEWsubVOL(4)
      IAD4(1) = IADI(1)
      IAD4(2) = IADI(3)
      IAD4(3) = IADA(3)
      IAD4(4) = IADA(1)
      CALL DEFPLA( Jcolor, 4, NII, IAD4 )
      IAD4(1) = IADI(3)
      IAD4(2) = IADI(5)
      IAD4(3) = IADA(5)
      IAD4(4) = IADA(3)
      CALL DEFPLA( Jcolor, 4, NII, IAD4 )
      CALL ENDsubVOL(4)
    ENDIF
!
   CALL ENDSUV
!
 END SUBROUTINE DEF_ELCYL_R1R2DZN
