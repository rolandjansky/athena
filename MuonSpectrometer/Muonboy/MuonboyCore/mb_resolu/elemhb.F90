!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE ELEMHB(VIN,PAPAS,VOUT,CURVAB,SINFIM,COSFIM,DBDL,DBLDL)
 USE M_MB_General_CONSTANTS, ONLY : XLIGHT
 USE M_MB_PRECIS
 IMPLICIT NONE
 REAL(8) :: PAPAS, CURVAB, SINFIM, COSFIM, DBDL, DBLDL
 REAL(8) :: VIN(8), VOUT(8)
 INTEGER :: INCREA
 REAL(8) :: XX,YY,ZZ, UX,UY,UZ, PASTOT, PAS, FAC, EB2MAX,EB2MIN
 REAL(8) :: BXA,BYA,BZA,BBA, XXX,YYY,ZZZ, TX,TY,TZ, VX,VY,VZ,VV
 REAL(8) :: WX,WY,WZ, CT,ST, RR, AA,SA,CA, V,W,T, BXB,BYB,BZB
 REAL(8) :: BBAMB2, BX,BY,BZ,BB, VP,WP,TP, BFIM
!
!>> Suivi d'une particule chargee dans un champs magnetique
!>> par succession d'ELEMents d'Helices Circulaires.
!>> Champs mag. en  Tesla  et distances en  cm  !!!
!>>                *******                 ****
!>>          (MV   Revu et corrige 10/01/95)
!>> Charge de la particule = VIN(8)    (en general +1. ou -1.)
!
   XX = VIN(1)
   YY = VIN(2)
   ZZ = VIN(3)
   UX = VIN(4)
   UY = VIN(5)
   UZ = VIN(6)
   PASTOT = 0.D0
   DBDL   = 0.D0
   DBLDL  = 0.D0
   PAS = PAPAS
   INCREA = 0
!
   FAC = VIN(7) / FACREF
   FAC = MAX( FACMIN , MIN( FACMAX , FAC ) )
   EB2MAX = ( DBMAX * FAC )**2
   EB2MIN = ( DBMIN * FAC )**2
!
10 CALL BFIELD(XX,YY,ZZ,BXA,BYA,BZA)
!
   BBA = DSQRT( BXA**2 + BYA**2 + BZA**2 )
   IF(BBA < BBNUL) THEN
     XXX = XX + PAS*UX
     YYY = YY + PAS*UY
     ZZZ = ZZ + PAS*UZ
     GO TO 30
   ENDIF
   TX = BXA / BBA
   TY = BYA / BBA
   TZ = BZA / BBA
   VX = UY*BZA - UZ*BYA
   VY = UZ*BXA - UX*BZA
   VZ = UX*BYA - UY*BXA
   VV = DSQRT( VX**2 + VY**2 + VZ**2 )
   VX = VX / VV
   VY = VY / VV
   VZ = VZ / VV
   WX = TY*VZ - TZ*VY
   WY = TZ*VX - TX*VZ
   WZ = TX*VY - TY*VX
   CT = VV / BBA
   ST = ( UX*BXA + UY*BYA + UZ*BZA ) / BBA
   RR = VIN(7) * CT / XLIGHT / BBA
20 AA = PAS * CT / RR
   SA = SIN(AA)
   CA = COS(AA)
   V = RR * (1.D0 - CA) * VIN(8)
   W = RR * SA
   T = PAS * ST
   XXX = XX + V*VX + W*WX + T*TX
   YYY = YY + V*VY + W*WY + T*TY
   ZZZ = ZZ + V*VZ + W*WZ + T*TZ
!
30 CALL BFIELD(XXX,YYY,ZZZ,BXB,BYB,BZB)
!
   BBAMB2 = (BXA-BXB)**2 + (BYA-BYB)**2 + (BZA-BZB)**2
   IF( BBAMB2 > EB2MAX .AND. PAS > PASMIN ) THEN
     PAS = PAS / 2.D0
     INCREA = 0
     IF(BBA < BBNUL) THEN
       XXX = XX + PAS*UX
       YYY = YY + PAS*UY
       ZZZ = ZZ + PAS*UZ
       GO TO 30
     ELSE
       GO TO 20
     ENDIF
   ELSEIF( BBAMB2 < EB2MIN .AND. INCREA >= 1 ) THEN
     PAS = 2.D0 * PAS
     IF( PASTOT+PAS >= PAPAS ) THEN
       PAS = PAPAS-PASTOT
       INCREA = 0
     ENDIF
     IF(BBA < BBNUL) THEN
       XXX = XX + PAS*UX
       YYY = YY + PAS*UY
       ZZZ = ZZ + PAS*UZ
       GO TO 30
     ELSE
       GO TO 20
     ENDIF
   ENDIF
   BX = (BXA + BXB) / 2.D0
   BY = (BYA + BYB) / 2.D0
   BZ = (BZA + BZB) / 2.D0
   BB = DSQRT( BX**2 + BY**2 + BZ**2 )
   IF(BB < BBNUL) THEN
     PASTOT = PASTOT + PAS
     IF(PASTOT < 0.9999D0*PAPAS) THEN
       XX = XX + PAS*UX
       YY = YY + PAS*UY
       ZZ = ZZ + PAS*UZ
       PAS = 2.D0*PAS
       IF( PASTOT+PAS >= PAPAS ) THEN
         PAS = PAPAS-PASTOT
         INCREA = 0
       ELSE
         INCREA = 1
       ENDIF
       GO TO 10
     ENDIF
     VOUT(1) = XX + PAS*UX
     VOUT(2) = YY + PAS*UY
     VOUT(3) = ZZ + PAS*UZ
     VOUT(4) = UX
     VOUT(5) = UY
     VOUT(6) = UZ
     VOUT(7) = VIN(7)
     VOUT(8) = VIN(8)
     RETURN
   ENDIF
   TX = BX / BB
   TY = BY / BB
   TZ = BZ / BB
   VX = UY*BZ - UZ*BY
   VY = UZ*BX - UX*BZ
   VZ = UX*BY - UY*BX
   VV = DSQRT( VX**2 + VY**2 + VZ**2 )
   VX = VX / VV
   VY = VY / VV
   VZ = VZ / VV
   WX =  TY*VZ - TZ*VY
   WY =  TZ*VX - TX*VZ
   WZ =  TX*VY - TY*VX
   CT = VV / BB
   ST = ( UX*BX + UY*BY + UZ*BZ ) / BB
   RR = VIN(7) * CT / XLIGHT / BB
   AA = PAS * CT / RR
   SA = SIN(AA)
   CA = COS(AA)
   V = RR * (1.D0 - CA) * VIN(8)
   W = RR * SA
   T = PAS * ST
   VP = CT * SA * VIN(8)
   WP = CT * CA
   TP = ST
   PASTOT = PASTOT + PAS
   IF(PASTOT < 0.9999D0*PAPAS) THEN
     XX = XX + V*VX +  W*WX +  T*TX
     YY = YY + V*VY +  W*WY +  T*TY
     ZZ = ZZ + V*VZ +  W*WZ +  T*TZ
     UX =     VP*VX + WP*WX + TP*TX
     UY =     VP*VY + WP*WY + TP*TY
     UZ =     VP*VZ + WP*WZ + TP*TZ
     BFIM  = BY*COSFIM - BX*SINFIM
     DBDL  = DBDL  + BFIM*PAS
     DBLDL = DBLDL + (CURVAB+PASTOT-PAS/2.D0)*BFIM*PAS
     IF( PASTOT+PAS >= PAPAS ) THEN
       PAS = PAPAS-PASTOT
       INCREA = 0
     ELSE
       INCREA = 1
     ENDIF
     GO TO 10
   ENDIF
   VOUT(1) = XX + V*VX +  W*WX +  T*TX
   VOUT(2) = YY + V*VY +  W*WY +  T*TY
   VOUT(3) = ZZ + V*VZ +  W*WZ +  T*TZ
   VOUT(4) =     VP*VX + WP*WX + TP*TX
   VOUT(5) =     VP*VY + WP*WY + TP*TY
   VOUT(6) =     VP*VZ + WP*WZ + TP*TZ
   VOUT(7) = VIN(7)
   VOUT(8) = VIN(8)
   BFIM  = BY*COSFIM - BX*SINFIM
   DBDL  = DBDL  + BFIM*PAS
   DBLDL = DBLDL + (CURVAB+PASTOT-PAS/2.D0)*BFIM*PAS
!
 END
