!
!> \author M.Virchaux
!
 SUBROUTINE GetTubeEnds( IT, IMdtCsc,I1rst2nd,XYZ1,XYZ2,AssociatedReso )
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_General_CONSTANTS, ONLY : Zero, One
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IT
 INTEGER, INTENT(OUT) :: IMdtCsc, I1rst2nd
 REAL(8), INTENT(OUT) :: XYZ1(3),XYZ2(3), AssociatedReso
 INTEGER :: Icode,IC,IW,IP,INOF,INOZ,IGEOM,IMDT,Itran,IcutZ2
 REAL(8) :: A0,T0,Z0,G0,S1P,S1N,S2P,S2N,Direc,EZ
 REAL(8) :: Z1,Z2,SinA0,CosA0,SinF0,CosF0,AA,AM1,AP1,ASS
 REAL(8) :: SStube,ZTtube,TZtube
 REAL(8) :: SStru1,ZTtru1,TZtru1
 REAL(8) :: SStru2,ZTtru2,TZtru2
 REAL(8) :: Stube1,Ztube1,Ttube1
 REAL(8) :: Stube2,Ztube2,Ttube2
 LOGICAL, EXTERNAL :: NOKCHA
!>>
!>>  14/09/06: JFL Adapted from Original GetTubeEnds code
!>>
!

   IMdtCsc   = 0
   I1rst2nd  = 0
   XYZ1(1:3) = 0.D0
   XYZ2(1:3) = 0.D0
   AssociatedReso = 10.0D0
!
   Icode = KTUBHI(IT)
   IC    =  Icode / 10000
   IW    = (Icode - IC*10000) / 10
   IP    =  Icode - IC*10000 - IW*10
   INOF  = IFCHA(IC,IW)
   INOZ  = IZCHA(IC,IW)
   IGEOM = IGEOMA(IC,INOF,INOZ)
   IMDT  = ISTMDTV(IGEOM)
   Itran = ItransCHA(IGEOM)
!
   A0    = GCECHAV(IGEOM)
   T0    = TCECHAV(IGEOM)
   Z0    = ZCECHAV(IGEOM)
   G0    = ZLOCHAV(IGEOM)
   S1P   = SPICHAV(IGEOM)
   S1N   = SNICHAV(IGEOM)
   S2P   = SPACHAV(IGEOM)
   S2N   = SNACHAV(IGEOM)
   Direc = SIGN( One, T0 )
   SinF0 = SINCHAV(IGEOM)
   CosF0 = COSCHAV(IGEOM)
!
   SStube = STUBHI(IT)
   ZTtube = ZTUBHI(IT)
   TZtube = T0 + Direc*TLTUB(IMDT,IP,1)
   IF( IC <= NCOUCH ) THEN
!>> MDT's ------------------------------------
     IMdtCsc  = 1
     I1rst2nd = 1
     EZ = (ZTUBHI(IT)-Z0+G0/2.D0) / G0
     Stube1 = S1P + EZ*(S2P-S1P)
     Stube2 = S1N + EZ*(S2N-S1N)
     CALL Pos_True_from_Ideal(Itran,Stube1,ZTtube,TZtube, SStru1,ZTtru1,TZtru1)
     CALL Pos_True_from_Ideal(Itran,Stube2,ZTtube,TZtube, SStru2,ZTtru2,TZtru2)
     IF( IC <= NCOUBA ) THEN
!>> Barrel MDT tube hit
       Stube1 = SStru1
       Stube2 = SStru2
       Ztube1 = ZTtru1
       Ztube2 = ZTtru2
       Ttube1 = TZtru1
       Ttube2 = TZtru2
     ELSE
!>> End-cap MDT tube hit
       Stube1 = SStru1
       Stube2 = SStru2
       Ztube1 = TZtru1
       Ztube2 = TZtru2
       Ttube1 = ZTtru1
       Ttube2 = ZTtru2
     ENDIF
   ELSE
!>> CSC's ------------------------------------
     IMdtCsc = -1
     SinA0 = SIN(A0)
     CosA0 = COS(A0)
     IF( RTUBHI(IT) < -10.D0 ) THEN
!>> Reconstructed (cluster) second coordinate CSC hit
       I1rst2nd = -1
       AssociatedReso = DTUBHI(IT)
       TZtube = T0 + Direc*TLTUB(IMDT,IP,2)
       IF( CSCpa < 1 ) THEN
         AA  = (S2P-S2N) / (S1P-S1N)
         AM1 = AA - 1.D0
         AP1 = AA + 1.D0
         ASS = AA * (S2P-S1P)
         Z1 = Z0 - G0/2.D0
         Stube1 = SStube + (Z1-Z0) * (ASS+AM1*(SStube-S2P)) / (AP1*G0/2.D0)
         Ztube1 = - SinA0*Z1 + CosA0*TZtube
         Ttube1 =   CosA0*Z1 + SinA0*TZtube
         Z2 = Z0 + G0/2.D0
         IcutZ2 = 0
70       IcutZ2 = IcutZ2 + 1
         Stube2 = SStube + (Z2-Z0) * (ASS+AM1*(SStube-S2P)) / (AP1*G0/2.D0)
         IF( NOKCHA(IC,INOZ,IGEOM, Z2,Stube2, Zero,Zero) ) THEN
           Z2 = Z2 - 1.D0
           IF( IcutZ2 < 60 ) GO TO 70
         ENDIF
         Z2 = Z2 + 0.5D0
         Stube2 = SStube + (Z2-Z0) * (ASS+AM1*(SStube-S2P)) / (AP1*G0/2.D0)
       ELSE
         IF( SStube < S1N ) THEN
           Z1 = Z0 - G0/2.D0 + G0*(SStube-S1N)/(S2N-S1N)
         ELSEIF( SStube > S1P ) THEN
           Z1 = Z0 - G0/2.D0 + G0*(SStube-S1P)/(S2P-S1P)
         ELSE
           Z1 = Z0 - G0/2.D0
         ENDIF
         Stube1 = SStube
         Z2 = Z0 + G0/2.D0
         IcutZ2 = 0
80       IcutZ2 = IcutZ2 + 1
         IF( NOKCHA(IC,INOZ,IGEOM, Z2,SStube, Zero,Zero) ) THEN
           Z2 = Z2 - 1.D0
           IF( IcutZ2 < 60 ) GO TO 80
         ENDIF
         Z2 = Z2 + 0.5D0
         Stube2 = SStube
       ENDIF
       Ztube1 = - SinA0*Z1 + CosA0*TZtube
       Ttube1 =   CosA0*Z1 + SinA0*TZtube
       Ztube2 = - SinA0*Z2 + CosA0*TZtube
       Ttube2 =   CosA0*Z2 + SinA0*TZtube
     ELSEIF( RTUBHI(IT) < 0.D0 ) THEN
!>> Reconstructed (cluster) first coordinate CSC hit
       I1rst2nd =  1
       AssociatedReso = DTUBHI(IT)
       EZ = (ZTUBHI(IT)-Z0+G0/2.D0) / G0
       Stube1 = S1P + EZ*(S2P-S1P)
       Ztube1 = - SinA0*ZTtube + CosA0*TZtube
       Ttube1 =   CosA0*ZTtube + SinA0*TZtube
       Stube2 = S1N + EZ*(S2N-S1N)
       Ztube2 = Ztube1
       Ttube2 = Ttube1
     ENDIF
   ENDIF
!
   XYZ1(1) = CosF0*Ttube1  - SinF0*Stube1
   XYZ1(2) = SinF0*Ttube1  + CosF0*Stube1
   XYZ1(3) = Ztube1
   XYZ2(1) = CosF0*Ttube2  - SinF0*Stube2
   XYZ2(2) = SinF0*Ttube2  + CosF0*Stube2
   XYZ2(3) = Ztube2
!
 END SUBROUTINE GetTubeEnds
