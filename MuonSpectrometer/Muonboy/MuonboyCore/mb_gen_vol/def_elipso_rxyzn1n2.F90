!
!> \author SaMuSoG
!
 SUBROUTINE DEF_ELIPSO_RXYZN1N2( Name, Jcolor, RadXYZ,N1,N2 )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 CHARACTER(len=*), INTENT(IN) :: Name
 INTEGER,          INTENT(IN) :: Jcolor,N1,N2
 REAL(8),          INTENT(IN) :: RadXYZ(3)
 INTEGER :: Nlati,Nlongi,Ilo,Ila,I,NLA1,NLA2,NLA1P
 REAL(8) :: FLA1,FLA2,Radius,RLA1,RLA2,ZLA1,ZLA2,RUNI,FLO1,FLO2,FLO3
 REAL(8) :: FacX,FacY,FacZ,X1,Y1,X2,Y2,X3,Y3,X4,Y4
!
   IF( ABS(RadXYZ(1)) < Eps .OR. ABS(RadXYZ(2)) < Eps .OR. ABS(RadXYZ(3)) < Eps ) RETURN
   Nlati  =  5
   Nlongi =  5
   IF( N1 >= 2 ) Nlati  = N1
   IF( N2 >= 2 ) Nlongi = N2
!
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   Radius = MAX( ABS(RadXYZ(1)) , ABS(RadXYZ(2)) , ABS(RadXYZ(3)) )
   FacX   = RadXYZ(1) / Radius
   FacY   = RadXYZ(2) / Radius
   FacZ   = RadXYZ(3) / Radius
!
   DO Ilo=1,Nlongi
    DO Ila=1,Nlati
     FLA1 = (Ila-1.D0) * Pi/2.D0/Nlati
     FLA2 =    Ila     * Pi/2.D0/Nlati
     RLA1 = Radius * SIN(FLA1)
     RLA2 = Radius * SIN(FLA2)
     ZLA1 = Radius * COS(FLA1) * FacZ
     ZLA2 = Radius * COS(FLA2) * FacZ
     IF( Ila == 1 ) THEN
        RUNI = RLA2
        NLA1 = 0
        NLA2 = 1
     ELSE
        NLA1  = NLA2
        NLA1P = NLA1 + 1
        IF( ABS(RLA2/NLA1-RUNI) > ABS(RLA2/NLA1P-RUNI) ) NLA2 = NLA1P
     ENDIF
     IF( NLA1 == NLA2 ) THEN
        DO I=1,NLA1
         FLO1 = (Ilo-1+REAL(I-1,8)/REAL(NLA1,8)) * TwoPi/Nlongi
         FLO2 = (Ilo-1+REAL(I  ,8)/REAL(NLA1,8)) * TwoPi/Nlongi
         X1 = RLA1 * COS(FLO1) * FacX
         Y1 = RLA1 * SIN(FLO1) * FacY
         X2 = RLA1 * COS(FLO2) * FacX
         Y2 = RLA1 * SIN(FLO2) * FacY
         X3 = RLA2 * COS(FLO2) * FacX
         Y3 = RLA2 * SIN(FLO2) * FacY
         X4 = RLA2 * COS(FLO1) * FacX
         Y4 = RLA2 * SIN(FLO1) * FacY
         CALL DEFPL4( Zero, Jcolor, 1,1,1,1, X1,X2,X3,X4, Y1,Y2,Y3,Y4,  ZLA1, ZLA1, ZLA2, ZLA2 )
         CALL DEFPL4( Zero, Jcolor, 1,1,1,1, X1,X2,X3,X4, Y1,Y2,Y3,Y4, -ZLA1,-ZLA1,-ZLA2,-ZLA2 )
        ENDDO
     ELSE
        DO I=1,NLA1
         FLO1 = (Ilo-1+REAL(I-1,8)/REAL(NLA1,8)) * TwoPi/Nlongi
         FLO2 = (Ilo-1+REAL(I  ,8)/REAL(NLA1,8)) * TwoPi/Nlongi
         FLO3 = (Ilo-1+REAL(I  ,8)/REAL(NLA2,8)) * TwoPi/Nlongi
         X1 = RLA1 * COS(FLO1) * FacX
         Y1 = RLA1 * SIN(FLO1) * FacY
         X2 = RLA1 * COS(FLO2) * FacX
         Y2 = RLA1 * SIN(FLO2) * FacY
         X3 = RLA2 * COS(FLO3) * FacX
         Y3 = RLA2 * SIN(FLO3) * FacY
         CALL DEFPL3( Zero, Jcolor, 1,1,1, X1,X2,X3, Y1,Y2,Y3,  ZLA1, ZLA1, ZLA2 )
         CALL DEFPL3( Zero, Jcolor, 1,1,1, X1,X2,X3, Y1,Y2,Y3, -ZLA1,-ZLA1,-ZLA2 )
        ENDDO
        DO I=1,NLA2
         FLO1 = (Ilo-1+REAL(I-1,8)/REAL(NLA2,8)) * TwoPi/Nlongi
         FLO2 = (Ilo-1+REAL(I  ,8)/REAL(NLA2,8)) * TwoPi/Nlongi
         FLO3 = 0.D0
         IF( NLA1 >= 1 ) FLO3 = (Ilo-1+REAL(I-1,8)/REAL(NLA1,8)) * TwoPi/Nlongi
         X1 = RLA2 * COS(FLO1) * FacX
         Y1 = RLA2 * SIN(FLO1) * FacY
         X2 = RLA2 * COS(FLO2) * FacX
         Y2 = RLA2 * SIN(FLO2) * FacY
         X3 = RLA1 * COS(FLO3) * FacX
         Y3 = RLA1 * SIN(FLO3) * FacY
         CALL DEFPL3( Zero, Jcolor, 1,1,1, X1,X2,X3, Y1,Y2,Y3,  ZLA2, ZLA2, ZLA1 )
         CALL DEFPL3( Zero, Jcolor, 1,1,1, X1,X2,X3, Y1,Y2,Y3, -ZLA2,-ZLA2,-ZLA1 )
        ENDDO
     ENDIF
    ENDDO
   ENDDO
!
   CALL ENDVOL_PUR
   CALL ENDSUV
!
 END SUBROUTINE DEF_ELIPSO_RXYZN1N2
