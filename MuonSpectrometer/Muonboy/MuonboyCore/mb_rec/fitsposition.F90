!
!> improve the position of segments along the wire (S coordinate)
!
 SUBROUTINE fitSPosition(IGEOM,IC,LLI,ZZtrue,TTtrue,SStrue,VZtrue,VTtrue,VSTtrue,rejectIt)
 USE M_MB_Digis
 USE M_MB_SSBAN
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: IGEOM, IC, LLI(2)
 REAL(8), INTENT(IN)    :: ZZtrue,TTtrue,VZtrue,VTtrue
 REAL(8), INTENT(INOUT) :: SStrue, VSTtrue
 LOGICAL, INTENT(OUT)   :: rejectIt
 INTEGER :: J, K, KA, ICODE, ICT, IP, Itrans, IRPC, ISA(NSTRBA), N, ISTMAX, ISTMIN
 REAL(8) :: VStrue, COSF, SINF, AXX, AYY, AZZ, AVX, AVY, AVZ
 REAL(8) :: CASF, SANF, SIGT, BSS, BZZ, BTT, BVS, BVZ, BVT
 REAL(8) :: SS, ZZ, TT, WZ, WT, VZT, VST
 REAL(8) :: Z,S,T, X,Y
 REAL(8) :: Z0,DZ, S1P,S1N,S2P,S2N, AA,AM1,AP1,ASS
 REAL(8) :: SUMT2, SUMT, SUMTS, SUMS, SUM1, det, Tmax, Tmin
 REAL(8) :: SOMTS, SOMT2, W
 REAL(8), PARAMETER :: TolZ=40.d0
!
   rejectIt = .true.
   SOMTS = 0.d0
   SOMT2 = 0.d0
   SUMT2 = 0.d0
   SUMT  = 0.d0
   SUMTS = 0.d0
   SUMS  = 0.d0
   SUM1  = 0.d0
   VStrue = VSTtrue * VTtrue
   COSF = COSCHAV(IGEOM)
   SINF = SINCHAV(IGEOM)
   CALL MuboSZTtoXYZ( (IC<=NCOUBA), SINF,COSF, SStrue,ZZtrue,TTtrue, AXX,AYY,AZZ)
   CALL MuboSZTtoXYZ( (IC<=NCOUBA), SINF,COSF, VStrue,VZtrue,VTtrue, AVX,AVY,AVZ)
   TMIN =  999999.d0
   TMAX = -999999.d0
   DO J=1,2
     IF( LLI(J) <= 0 )  CYCLE
     ISA(1:NSTRBA) = ISBAN(1:NSTRBA,LLI(J))
     DO K=1,NSTRBA
       IF( ISA(K) == 0 ) EXIT
       IF( ISA(K) < 0 )  CYCLE
       ICODE = KBANHI(ISA(K)) / 10
       ICT   = ICODE / 100
       IP    = ICODE - ICT*100
       IRPC  = IRPTGC(ICT)
       SIGT = SIGN( 1.d0, TCETRI(ICT) )
       CASF  = COSTRI(ICT)
       SANF  = SINTRI(ICT)
       CALL MuboXYZtoSZT( (ICT<=NTRIBA), SANF,CASF, AXX,AYY,AZZ, BSS,BZZ,BTT)
       CALL MuboXYZtoSZT( (ICT<=NTRIBA), SANF,CASF, AVX,AVY,AVZ, BVS,BVZ,BVT)
       Itrans = ItransTRI(ICT)
       IF( ABS(BVT) < 1.0d-4 ) CYCLE     ! may happen when barrel-endcap station-trigger
       CALL SZTWU_Ideal_from_True(Itrans, BSS,BZZ,BTT,BVS,BVZ,BVT, SS,ZZ,TT,WZ,WT,VZT,VST)
       T = TCETRI(ICT) + SIGT*TBAND(IRPC,IP)
       Z = ZZ + (T-TT)*VZT
       S = SBANHI(ISA(K))
       N = 1
       W = 1.d0
       ISTMAX = ABS(LBANHI(ISA(K))) / 10
       ISTMIN = ABS(LBANHI(ISA(K))) / 10
       DO KA=K+1,NSTRBA  ! look for other strips on same station and same layer
         IF( ISA(KA) == 0 ) EXIT
         IF( ISA(KA) < 0 )  CYCLE
         ICODE = KBANHI(ISA(KA)) / 10
         IF( ICODE / 100 == ICT .AND. ICODE - ICT*100 == IP ) THEN
           S = S + SBANHI(ISA(KA))
           N = N + 1
           ISTMAX = MAX( ISTMAX, ABS(LBANHI(ISA(KA))) / 10 )  
           ISTMIN = MIN( ISTMIN, ABS(LBANHI(ISA(KA))) / 10 )
           ISA(KA) = - ISA(KA)
         ENDIF
       ENDDO
       IF( N > 1 ) THEN
         S = S / N
         IF( ISTMAX - ISTMIN > 1 )  W = W / ( ISTMAX - ISTMIN ) ! reduced weight if more than 2 strips or if holes
       ENDIF
! take into account the fact that SBANHI gives the S position at the center of the strip
       Z0  = ZCETRI(ICT)
       DZ  = ZLOTRI(ICT) / 2.D0
       IF( Z > Z0+DZ+tolZ .OR. Z < Z0-DZ-tolZ ) THEN
         CYCLE
       ELSE IF( Z > Z0+DZ ) THEN
         Z = Z0 + DZ
       ELSE IF( Z < Z0-DZ ) THEN
         Z = Z0 - DZ
       ENDIF
       S1P = SPITRI(ICT)
       S1N = SNITRI(ICT)
       S2P = SPATRI(ICT)
       S2N = SNATRI(ICT)
       AA  = (S2P-S2N) / (S1P-S1N)
       AM1 = AA - 1.D0
       AP1 = AA + 1.D0
       ASS = AA * (S2P-S1P)
       S = S + (Z-Z0)*(ASS+AM1*(S-S2P))/AP1/DZ
! end of S position correction
       CALL Pos_True_from_Ideal(Itrans, S,Z,T, SS,ZZ,TT)
       CALL MuboSZTtoXYZ( (ICT<=NTRIBA), SANF,CASF, SS,ZZ,TT, X,Y,Z)
       CALL MuboXYZtoSZT( (IC<=NCOUBA),  SINF,COSF, X,Y,Z, SS,ZZ,TT)
       rejectIt = .false.
       SOMTS = SOMTS + W*TT*SS
       SOMT2 = SOMT2 + W*TT*TT
       TT = TT - TTtrue
       Tmax = MAX( Tmax, TT )
       Tmin = MIN( Tmin, TT )
       SUMT2 = SUMT2 + W*TT*TT
       SUMT  = SUMT  + W*TT
       SUMTS = SUMTS + W*TT*SS
       SUMS  = SUMS  + W*SS
       SUM1  = SUM1  + W*1.d0 
     ENDDO
   ENDDO
!
   IF( .NOT.rejectIt ) THEN
     det = SUMT**2 - SUM1*SUMT2
     IF( det /= 0.d0 .AND. SUM1 > 1.6d0 .AND. ( Tmin*Tmax < 0.d0 .OR. Tmax-Tmin > 40.d0 ) ) THEN
       SStrue  = ( SUMTS*SUMT - SUMS*SUMT2 ) / det
       VSTtrue = (  SUMS*SUMT - SUM1*SUMTS ) / det
     ELSE ! not enough info, keep the pointing hypothesis
       VSTtrue = SOMTS / SOMT2
       SStrue  = VSTtrue * TTtrue
     ENDIF
   ENDIF
!
 END SUBROUTINE fitSPosition
!
