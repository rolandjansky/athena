!
!> \author M.Virchaux
!
 SUBROUTINE GetStripEnds( &
  IS, IRpcTgc,I1rst2nd, &
  XYZ1,XYZ2,AssociatedReso , &
  VecNorm &
 )
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_General_CONSTANTS, ONLY : One
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IS
 INTEGER, INTENT(OUT) :: IRpcTgc,I1rst2nd
 REAL(8), INTENT(OUT) :: XYZ1(3),XYZ2(3), AssociatedReso
 REAL(8), INTENT(OUT) :: VecNorm(3)
 INTEGER :: Icode,ICT,IP,IRPC
 REAL(8) :: T0,Z0,G0,S1P,S1N,S2P,S2N,AA,AM1,AP1,ASS,Direc,S0,EZ
 REAL(8) :: SinF0,CosF0
 REAL(8) :: TZstrip,ZTstrip1,ZTstrip2,SSstrip1,SSstrip2
 REAL(8) :: S0A,EZ3,ZTstrip3,SSstrip3
 REAL(8) :: XYZ3(3)
 REAL(8) :: V12(3),V13(3),VNORM
!>>
!>>  14/09/06: JFL Adapted from original GetStripEnds code
!>>
!
   IRpcTgc  = 0
   I1rst2nd = 0
   XYZ1(:) = 0.D0
   XYZ2(:) = 0.D0
   AssociatedReso = 0.D0
!
   Icode = KBANHI(IS) / 10
   ICT   = Icode / 100
   IP    = Icode - ICT*100
   IRPC  = IRPTGC(ICT)
   IF( N2BAND(IRPC) <= 0 )                 RETURN
!
   T0    = TCETRI(ICT)
   Z0    = ZCETRI(ICT)
   G0    = ZLOTRI(ICT)
   S1P   = SPITRI(ICT)
   S1N   = SNITRI(ICT)
   S2P   = SPATRI(ICT)
   S2N   = SNATRI(ICT)
   AA    = (S2P-S2N) / (S1P-S1N)
   AM1   = AA - 1.D0
   AP1   = AA + 1.D0
   ASS   = AA * (S2P-S1P)
   Direc = SIGN( One, T0 )
   S0    = SBANHI(IS)
   S0A   = SBANHI(IS) + 1.d0
   SinF0 = SINTRI(ICT)
   CosF0 = COSTRI(ICT)
!
   TZstrip = T0 + Direc*TBAND(IRPC,IP)
   IF( IP <= N1BAND(IRPC) ) THEN
!>> S strip hit
     I1rst2nd = -1
     ZTstrip1 = Z0 - G0/2.D0
     ZTstrip2 = Z0 + G0/2.D0
     SSstrip1 = S0 + (ZTstrip1-Z0) * (ASS+AM1*(S0-S2P)) / (AP1*G0/2.D0)
     SSstrip2 = S0 + (ZTstrip2-Z0) * (ASS+AM1*(S0-S2P)) / (AP1*G0/2.D0)
     AssociatedReso = DSBAND(IRPC) / SQRT(12.D0)
     ZTstrip3 = Z0 - G0/2.D0
     SSstrip3 = S0A+ (ZTstrip3-Z0) * (ASS+AM1*(S0A-S2P))/ (AP1*G0/2.D0)
   ELSE
!>> Z/T strip hit
     I1rst2nd =  1
     EZ = (S0-Z0+G0/2.D0) / G0
     ZTstrip1 = S0
     ZTstrip2 = S0
     SSstrip1 = S1N + EZ*(S2N-S1N)
     SSstrip2 = S1P + EZ*(S2P-S1P)
     AssociatedReso = DZBAND(IRPC) / SQRT(12.D0)
     EZ3 = (S0A-Z0+G0/2.D0) / G0
     ZTstrip3 = S0A
     SSstrip3 = S1N + EZ3*(S2N-S1N)
   ENDIF
!
   IF( ICT <= NTRIBA ) THEN
!>> RPC
     IRpcTgc =  1
     XYZ1(1) = CosF0*TZstrip  - SinF0*SSstrip1
     XYZ1(2) = SinF0*TZstrip  + CosF0*SSstrip1
     XYZ1(3) = ZTstrip1
     XYZ2(1) = CosF0*TZstrip  - SinF0*SSstrip2
     XYZ2(2) = SinF0*TZstrip  + CosF0*SSstrip2
     XYZ2(3) = ZTstrip2
     XYZ3(1) = CosF0*TZstrip  - SinF0*SSstrip3
     XYZ3(2) = SinF0*TZstrip  + CosF0*SSstrip3
     XYZ3(3) = ZTstrip3
   ELSE
!>> TGC
     IRpcTgc = -1
     XYZ1(1) = CosF0*ZTstrip1 - SinF0*SSstrip1
     XYZ1(2) = SinF0*ZTstrip1 + CosF0*SSstrip1
     XYZ1(3) = TZstrip
     XYZ2(1) = CosF0*ZTstrip2 - SinF0*SSstrip2
     XYZ2(2) = SinF0*ZTstrip2 + CosF0*SSstrip2
     XYZ2(3) = TZstrip
     XYZ3(1) = CosF0*ZTstrip3 - SinF0*SSstrip3
     XYZ3(2) = SinF0*ZTstrip3 + CosF0*SSstrip3
     XYZ3(3) = TZstrip
   ENDIF
   V12(1) = XYZ2(1) - XYZ1(1)
   V12(2) = XYZ2(2) - XYZ1(2)
   V12(3) = XYZ2(3) - XYZ1(3)
   VNORM  = SQRT ( V12(1)**2 +V12(2)**2 + V12(3)**2 )
   V12(1) = V12(1)/VNORM  
   V12(2) = V12(2)/VNORM  
   V12(3) = V12(3)/VNORM  
   V13(1) = XYZ3(1) - XYZ1(1)
   V13(2) = XYZ3(2) - XYZ1(2)
   V13(3) = XYZ3(3) - XYZ1(3)
   VNORM  = SQRT ( V13(1)**2 +V13(2)**2 + V13(3)**2 )
   V13(1) = V13(1)/VNORM  
   V13(2) = V13(2)/VNORM  
   V13(3) = V13(3)/VNORM  
   VecNorm(1) =     V12(2) * V13(3) - V12(3) * V13(2)
   VecNorm(2) = - ( V12(1) * V13(3) - V12(3) * V13(1) )
   VecNorm(3) =     V12(1) * V13(2) - V12(2) * V13(1)
   VNORM  = SQRT ( VecNorm(1)**2 +VecNorm(2)**2 + VecNorm(3)**2 )
   VecNorm(1) = VecNorm(1)/VNORM  
   VecNorm(2) = VecNorm(2)/VNORM  
   VecNorm(3) = VecNorm(3)/VNORM  
!
 END SUBROUTINE GetStripEnds

