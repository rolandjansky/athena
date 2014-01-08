!
!> \author M.Virchaux
!
 SUBROUTINE DOSTEPHC(Vin,NVin,BXYZ,PAS, Vout,IBNUL, BB1, VX1,VY1,VZ1, WX1,WY1,WZ1, TX1,TY1,TZ1, CT1,ST1,RR1 )
 USE M_MB_General_CONSTANTS, ONLY : InvLight
 USE M_MB_PRECIS
 IMPLICIT NONE
 INTEGER :: NVin, IBNUL
 REAL(8) :: PAS, BB1, VX1,VY1,VZ1, WX1,WY1,WZ1, TX1,TY1,TZ1, CT1,ST1,RR1
 REAL(8) :: Vin(8,NVin), Vout(8,NVin)
 REAL(8) :: BXYZ(12)
 INTEGER :: I
 REAL(8) :: VIN11,VIN21,VIN31, BX1,BY1,BZ1, VIN4,VIN5,VIN6, VV1, AA1,SA1,CA1
 REAL(8) :: V1,W1,T1, VP1,WP1,TP1, DX,DY,DZ, BXI,BYI,BZI, BBI
 REAL(8) :: TX,TY,TZ, VX,VY,VZ,VV, WX,WY,WZ, CT,ST,RR, AA,SA,CA, A2
 REAL(8) :: V,W,T, VP,WP,TP
!
 REAL(8) :: InvBB1
 REAL(8) :: InvVV1
 REAL(8) :: InvBBI
 REAL(8) :: InvVV
!
!>> WARNING !!!
!>> Input vector  Vin  may be overwriten by output vector  Vout
!>> (when they are identical at the calling step)
!
   VIN11 = Vin(1,1)
   VIN21 = Vin(2,1)
   VIN31 = Vin(3,1)
!
   BX1 = BXYZ(1)
   BY1 = BXYZ(2)
   BZ1 = BXYZ(3)
   BB1 = SQRT( BX1**2 + BY1**2 + BZ1**2 )
   IF( BB1 < BBNUL ) THEN
     Vout(1,1) = VIN11 + PAS*Vin(4,1)
     Vout(2,1) = VIN21 + PAS*Vin(5,1)
     Vout(3,1) = VIN31 + PAS*Vin(6,1)
     Vout(4,1) = Vin(4,1)
     Vout(5,1) = Vin(5,1)
     Vout(6,1) = Vin(6,1)
     Vout(7,1) = Vin(7,1)
     Vout(8,1) = Vin(8,1)
     IBNUL = 1
   ELSE
     VIN4 = Vin(4,1)
     VIN5 = Vin(5,1)
     VIN6 = Vin(6,1)
     InvBB1 = 1.d0 / BB1
     TX1 = BX1 * InvBB1
     TY1 = BY1 * InvBB1
     TZ1 = BZ1 * InvBB1
     VX1 = VIN5*BZ1 - VIN6*BY1
     VY1 = VIN6*BX1 - VIN4*BZ1
     VZ1 = VIN4*BY1 - VIN5*BX1
     VV1 = SQRT( VX1**2 + VY1**2 + VZ1**2 )
     InvVV1 = 1.d0 / VV1
     VX1 = VX1 * InvVV1
     VY1 = VY1 * InvVV1
     VZ1 = VZ1 * InvVV1
     WX1 =  TY1*VZ1 - TZ1*VY1
     WY1 =  TZ1*VX1 - TX1*VZ1
     WZ1 =  TX1*VY1 - TY1*VX1
     CT1 = VV1 * InvBB1
     ST1 = VIN4*TX1 + VIN5*TY1 + VIN6*TZ1
     RR1 = Vin(7,1) * CT1 * InvLight * InvBB1
     AA1 = PAS * CT1 / RR1
!>>--- aproximated sin and cos ----------------
!*** SA1 = AA1  - AA1**3/6.D0 + AA1**5/120.D0
!*** CA1 = 1.D0 - AA1**2/2.D0 + AA1**4/24.D0
     A2  = AA1**2
     SA1 = ( 1.d0 - A2/6.D0 ) * AA1
     CA1 =   1.d0 - A2*0.5d0
!>>--------------------------------------------
     V1  = RR1 * (1.D0 - CA1) * Vin(8,1)
     W1  = RR1 * SA1
     T1  = PAS * ST1
     VP1 = CT1 * SA1 * Vin(8,1)
     WP1 = CT1 * CA1
     TP1 = ST1
     Vout(1,1) = VIN11 + V1*VX1 +  W1*WX1 +  T1*TX1
     Vout(2,1) = VIN21 + V1*VY1 +  W1*WY1 +  T1*TY1
     Vout(3,1) = VIN31 + V1*VZ1 +  W1*WZ1 +  T1*TZ1
     Vout(4,1) =        VP1*VX1 + WP1*WX1 + TP1*TX1
     Vout(5,1) =        VP1*VY1 + WP1*WY1 + TP1*TY1
     Vout(6,1) =        VP1*VZ1 + WP1*WZ1 + TP1*TZ1
     Vout(7,1) = Vin(7,1)
     Vout(8,1) = Vin(8,1)
     IBNUL = 0
   ENDIF
!
   DO I=2,NVin
     DX  = Vin(1,I) - VIN11
     DY  = Vin(2,I) - VIN21
     DZ  = Vin(3,I) - VIN31
     BXI = BX1 + BXYZ( 4)*DX + BXYZ( 5)*DY + BXYZ( 6)*DZ
     BYI = BY1 + BXYZ( 7)*DX + BXYZ( 8)*DY + BXYZ( 9)*DZ
     BZI = BZ1 + BXYZ(10)*DX + BXYZ(11)*DY + BXYZ(12)*DZ
     BBI = SQRT( BXI**2 + BYI**2 + BZI**2 )
     IF( BBI < BBNUL ) THEN
       Vout(1,I) = Vin(1,I) + PAS*Vin(4,I)
       Vout(2,I) = Vin(2,I) + PAS*Vin(5,I)
       Vout(3,I) = Vin(3,I) + PAS*Vin(6,I)
       Vout(4,I) = Vin(4,I)
       Vout(5,I) = Vin(5,I)
       Vout(6,I) = Vin(6,I)
       Vout(7,I) = Vin(7,I)
       Vout(8,I) = Vin(8,I)
     ELSE
       VIN4 = Vin(4,I)
       VIN5 = Vin(5,I)
       VIN6 = Vin(6,I)
       InvBBI = 1.d0/BBI
       TX = BXI * InvBBI
       TY = BYI * InvBBI
       TZ = BZI * InvBBI
       VX = VIN5*BZI - VIN6*BYI
       VY = VIN6*BXI - VIN4*BZI
       VZ = VIN4*BYI - VIN5*BXI
       VV = SQRT( VX**2 + VY**2 + VZ**2 )
       InvVV = 1.d0 / VV
       VX = VX * InvVV
       VY = VY * InvVV
       VZ = VZ * InvVV
       WX =  TY*VZ - TZ*VY
       WY =  TZ*VX - TX*VZ
       WZ =  TX*VY - TY*VX
       CT = VV * InvBBI
       ST = VIN4*TX + VIN5*TY + VIN6*TZ
       RR = Vin(7,I) * CT * InvLight * InvBBI
       AA = PAS * CT / RR
!>>--- aproximated sin and cos ----------------
!***** SA =  AA  - AA**3/6.D0 + AA**5/120.D0
!***** CA = 1.D0 - AA**2/2.D0 + AA**4/24.D0
       A2 = AA**2
       SA = ( 1.d0 - A2/6.D0 ) * AA
       CA =   1.d0 - A2*0.5d0
!>>--------------------------------------------
       V  = RR * (1.D0 - CA) * Vin(8,I)
       W  = RR * SA
       T  = PAS * ST
       VP = CT * SA * Vin(8,I)
       WP = CT * CA
       TP = ST
       Vout(1,I) = Vin(1,I) + V*VX +  W*WX +  T*TX
       Vout(2,I) = Vin(2,I) + V*VY +  W*WY +  T*TY
       Vout(3,I) = Vin(3,I) + V*VZ +  W*WZ +  T*TZ
       Vout(4,I) =           VP*VX + WP*WX + TP*TX
       Vout(5,I) =           VP*VY + WP*WY + TP*TY
       Vout(6,I) =           VP*VZ + WP*WZ + TP*TZ
       Vout(7,I) = Vin(7,I)
       Vout(8,I) = Vin(8,I)
     ENDIF
   ENDDO
!
 END SUBROUTINE DOSTEPHC
!
