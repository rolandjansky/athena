!
!> \author M.Virchaux 
!
 SUBROUTINE DROITU(ALOCEL,STRICT,SSfromROA,BOGcase,ICC,IWW,LLI,SSI,TTI,MACADO, NBCADO,segcado)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS, ONLY : CP0001
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_SSBAN
 IMPLICIT NONE
 LOGICAL,       INTENT(IN)  :: ALOCEL, STRICT, SSfromROA, BOGcase
 INTEGER,       INTENT(IN)  :: MACADO, ICC(2), IWW(2), LLI(2)
 REAL(8),       INTENT(IN)  :: SSI(2), TTI(2)
 INTEGER,       INTENT(OUT) :: NBCADO
 TYPE(Segment), INTENT(OUT) :: segcado(MACADO)
 TYPE(DcaPoint) :: dcapoin(2)
 INTEGER, PARAMETER :: MAKA = 1000, MAKB = 50, NSSmax = 5
 INTEGER :: NBKA, NBKold, Nscan, IC1, IC2, I_single_ml, IW1, IW2, IGEOM1, IGEOM2
 INTEGER :: Itran1, Itran2, IMDT1, IMDT2, NSS, ISS, JSS, IP1, ITDEB1, ITFIN1, IT1, selecFlag, ICT
 INTEGER :: IOKSZ1, IP2, ITDEB2, ITFIN2, IT2, ISI1, ISI2, IOKSZ2, I, ICI, IWI, ISI1MA, ISI2MA
 INTEGER :: IGEOMI, ItranI, IMDTI, NOUSED, IP, ITDEB, ITFIN, IT, IK, NBKB, L, I1ML, KIK, i1, i2, i3
 INTEGER :: IN, IKPRE, INPRE, K, Iflop, Nflop, ISmin, IS, INOF, INOZ, I3MI, K1,K2, KTUB10,KTUB01
 INTEGER :: JCADO(2,100,N4M,4), JCAPRI(N4M,4)
 INTEGER :: IWWloc(2), JMDT(2), NREAR(MAKA), NML(2)
 INTEGER :: ITKA(N4M,4,2,MAKA), ISKA(N4M,4,2,MAKA)
 REAL(8) :: T01, T02, F01, Z01, Z02, Z02S, ADT0, SS1, SS2, SMA1P, SMA1N, SMA2P, SMA2N
 REAL(8) :: SiF01, CoF01, SiF02, CoF02, SiF0I, CoF0I, SBt,ZBt,TBt, SSB1,SSB2
 REAL(8) :: SS0, DSS, DSS21, DIREC, SSS, VSTF, T1, SF1, Z1, S1, VST, SSval(NSSmax)
 REAL(8) :: VV1, WZ1, WT1, R1A,R2A, T2, SF2, Z2, S2
 REAL(8) :: ALFA, VV, TETA, SITE, WZ, WT, Z1P, Z2P, cosdir, adcmax
 REAL(8) :: TTMOY,ZZMOY,SSMOY, DZTOL, VVtrue,WZtrue,WTtrue,VSTtrue
 REAL(8) :: R1, R2, T0true,Z0true,S0true, CTOT, RESscan
 REAL(8) :: S1true,Z1true,T1true,S2true,Z2true,T2true, T0I,F0I,Z0I, SMAIP,SMAIN
 REAL(8) :: Z0,T0,S0,A0,V0, DZZ, DAA, ZCUR,ACUR,SiCUR,CoCUR, ZCURold,ACURold, DZcut, DAcut
 REAL(8) :: Cmin, ZI,AI,SiI,CoI, CM1,C00,CP1, ESS, SiAKAIMP, CoAKAIMP, DT0I, SiAKA, CoAKA, Pena
 REAL(8) :: ZKA(MAKA), TKA(MAKA), SKA(MAKA), AKA(MAKA), VKA(MAKA), CKA(MAKA)
 REAL(8) :: ZKAIMP(MAKB), AKAIMP(MAKB)
 INTEGER :: IORD(MAKB)
 LOGICAL :: ToScan(MAKB), oneStrip
 INTEGER, PARAMETER :: nscama=9
 REAL(8) :: Cscan(-nscama:nscama)
 INTEGER :: IOKSZS, IOKALI
 LOGICAL :: rejectSegment, IOKSSS, rejectIt
 REAL(8) :: RFTfast
 REAL(8) :: Z1DCA, T1DCA, S1DCA 
 REAL(8) :: Z2DCA, T2DCA, S2DCA 
 REAL(8) :: XXXDCA, YYYDCA, ZZZDCA
 REAL(8) :: XXXDCB, YYYDCB, ZZZDCB
 REAL(8) :: XXXdir, YYYdir, ZZZdir
!
 REAL(8) :: RadTrackBidon
!
 REAL(8) :: InvSITE
 REAL(8) :: InvVV
 REAL(8) :: InvVVtrue
 REAL(8) :: InvVV1
 REAL(8) :: Z1DZ0I
 REAL(8) :: Z1DZ01
!
 INTEGER, PARAMETER, DIMENSION(2*nscama+1) :: Iscan = (/ 0, -1,1, -2,2, -3,3, -4,4, -5,5, -6,6, -7,7, -8,8, -9,9 /)
!
    NBKA   = 0
    NBCADO = 0
    IC1    = ICC(1)
    IC2    = ICC(2)
!
    DO 990 I_single_ml=1,2
      IF( MBSto >= 0 ) THEN
        IF( I_single_ml > 1 )         CYCLE
        IWWloc(1:2) = IWW(1:2)
      ELSE
        IWWloc(  I_single_ml) = IWW(I_single_ml)
        IWWloc(3-I_single_ml) = 0
        IF( IWWloc(I_single_ml) < 1 ) CYCLE
      ENDIF
!
!>> Fill TEMporary arrays of addresses for fast access to the hits
      CALL FILTEM(ICC,IWWloc)
!
      IF( IWWloc(1)*IWWloc(2) >= 1 ) THEN
!>> ---------------------------------------------------------------------
!>> Straight track segments search between multi-layer IC1,IW1
!>> and multi-layer IC2,IW2.
        IW1 = IWWloc(1)
        IW2 = IWWloc(2)
        IGEOM1 = IGEOMA(IC1,IFCHA(IC1,IW1),IZCHA(IC1,IW1))
        IGEOM2 = IGEOMA(IC2,IFCHA(IC2,IW2),IZCHA(IC2,IW2))
        Itran1 = ItransCHA(IGEOM1)
        Itran2 = ItransCHA(IGEOM2)
        T01   = TCECHAV(IGEOM1)
        T02   = TCECHAV(IGEOM2)
        F01   = FCECHAV(IGEOM1)
        SiF01 = SINCHAV(IGEOM1)
        CoF01 = COSCHAV(IGEOM1)
        SiF02 = SINCHAV(IGEOM2)
        CoF02 = COSCHAV(IGEOM2)
        Z01   = ZCECHAV(IGEOM1)
        Z02   = ZCECHAV(IGEOM2)
        Z02S  = Z01 * T02 / T01
        IMDT1 = ISTMDTV(IGEOM1)
        IMDT2 = ISTMDTV(IGEOM2)
        JMDT(1) = IMDT1
        JMDT(2) = IMDT2
        ADT0 = ABS(T01-T02)
!
!>> Deals with S coordinates
!>> Beware : for EndCap, S scales roughly with Z, so what is measured is in fact S/Z
        NSS = 1
        SS1 = SSI(1)
        SS2 = SSI(2)
        oneStrip = .false.
        IF( SS1 > 100000.D0 .AND. SS2 > 100000.D0 ) THEN
          IF( SSfromROA ) THEN ! use the roa to find the S positions
            CALL getSSval( IC1, IGEOM1, IGEOM2, NSSmax, NSS, SSval )
            IF( NSS <= 0 ) GO TO 900
          ELSE
            SMA1P=(SPICHAV(IGEOM1)+SPACHAV(IGEOM1))*0.5d0
            SMA1N=(SNICHAV(IGEOM1)+SNACHAV(IGEOM1))*0.5d0
            SMA2P=(SPICHAV(IGEOM2)+SPACHAV(IGEOM2))*0.5d0
            SMA2N=(SNICHAV(IGEOM2)+SNACHAV(IGEOM2))*0.5d0
            SS0 = MIN(SMA1N,SMA2N) + 5.D0
            DSS = MAX(SMA1P,SMA2P) - MIN(SMA1N,SMA2N) - 10.D0
            NSS = DSS / 50.D0
            NSS = MAX( 5 , NSS )
          ENDIF
        ELSE IF( SS1 > 100000.D0 ) THEN
          IF( IC2 <= NCOUBA ) THEN
            SS1 = SS2 * T01 / T02
          ELSE
            SS1 = SS2 * Z01 / Z02
            SS2 = SS1 * T02 / T01
          ENDIF
          oneStrip = .true.
        ELSE IF( SS2 > 100000.D0 ) THEN
          SS2 = SS1 * T02 / T01
          oneStrip = .true.
        ELSE
          IF( IC2 > NCOUBA ) THEN
            DSS21 = SS2*Z01/Z02 - SS1
            SS1 = SS1         + DSS21*(T01-TTI(1))/(TTI(2)-TTI(1))
            SS2 = SS2*Z01/Z02 - DSS21*(T02-TTI(2))/(TTI(1)-TTI(2))
            SS2 = SS2 * T02 / T01
          ENDIF
        ENDIF
!
        IF( oneStrip ) THEN ! check if not already done by a segment with strips on two sides
bI1:      DO I1=NSBAN0(IC1,IW1),NSBAN1(IC1,IW1)-1
            ICT = KBANHI(ISBAN(1,I1))/1000
            DO I2=NSBAN0(IC2,IW2),NSBAN1(IC2,IW2)-1
              IF( KBANHI(ISBAN(1,I2))/1000 == ICT ) CYCLE bI1 ! special BOF-BOG case in findra
            ENDDO
            DO I2=NSBAN0(IC2,IW2),NSBAN1(IC2,IW2)-1
              SSB1 = SSBAN(I1)
              SSB2 = SSBAN(I2)
              IF( IC2 > NCOUBA ) THEN
                DSS21 = SSB2*Z01/Z02 - SSB1
                SSB1 = SSB1         + DSS21*(T01-TTI(1))/(TTI(2)-TTI(1))
                SSB2 = SSB2*Z01/Z02 - DSS21*(T02-TTI(2))/(TTI(1)-TTI(2))
                SSB2 = SSB2 * T02 / T01
              ENDIF
              IF( ABS(SS1-SSB1) < 2.0d0 .AND. ABS(SS2-SSB2) < 2.0d0 ) GO TO 26
            ENDDO
          ENDDO bI1
        ENDIF
!
!>> DIREC < 0 if one is in the "negative" end cap
        DIREC = 1.D0
        IF( T01 < 0.D0 ) DIREC = -1.D0
!
!>> First step, one looks for all possible candidates
!>> (on different S positions when there is no strip measurement)
        DO 25 ISS=1,NSS
          IF( SSfromROA ) THEN
            SSS = SSval(ISS)
            IF(IC2 <= NCOUBA) THEN
              SS1 = SSS * 2.D0*T01 / (T01+T02)
              SS2 = SSS * 2.D0*T02 / (T01+T02)
            ELSE
              SS1 = SSS
              SS2 = SSS * T02 / T01
            ENDIF
          ELSE IF(NSS >= 2) THEN
            IF( ISS == 1 ) THEN
              JSS = (NSS+1)/2 - 1
            ELSE IF( doRTmiddle ) THEN
              EXIT
            ELSE IF( ISS == (NSS+1)/2 ) THEN
              JSS = 0
            ELSE
              JSS = ISS - 1
            ENDIF
            SSS = SS0 + JSS * DSS / (NSS-1)
            IF(IC2 <= NCOUBA) THEN
              SS1 = SSS * 2.D0*T01 / (T01+T02)
              SS2 = SSS * 2.D0*T02 / (T01+T02)
            ELSE
              SS1 = SSS
              SS2 = SSS * T02 / T01
            ENDIF
          ENDIF
!>> Slope in the (S,T) plane
          VSTF = (SS2-SS1) / (T02-T01)
          NBKold = NBKA
          DO 21 IP1=1,NLTUB(IMDT1)
            T1  = T01 + DIREC * TLTUB(IMDT1,IP1,1)
            SF1 = SS1 + VSTF * (T1-T01)
            ITDEB1 = NTUBH0(IC1,IW1,IP1) + 1
            ITFIN1 = NTUBH0(IC1,IW1,IP1) + NTUBHI(IC1,IW1,IP1)
            DO 20 IT1=ITDEB1,ITFIN1
              IF( LTUBHI(IT1) /= 1  )               CYCLE
              IF( KTUBHI(IT1) >= 30 )               CYCLE ! already used on the 2 sides
              Z1 = ZTUBHI(IT1)
              IF(IC1 <= NCOUBA) THEN
                S1  = SF1
                IF( ALOCEL ) THEN
                  IF( IOKALI(F01,Z1,T1,S1,2) == 0 ) CYCLE
                ENDIF
                VST = VSTF
              ELSE
                Z1DZ01 = Z1 / Z01
                S1  = SF1  * Z1DZ01
                IF( ALOCEL ) THEN
                  IF( IOKALI(F01,T1,Z1,S1,2) == 0 ) CYCLE
                ENDIF
                VST = VSTF * Z1DZ01
              ENDIF
              VV1 = SQRT( Z1**2 + T1**2 )
              InvVV1 = 1.d0/VV1
              WZ1 = Z1 *InvVV1
              WT1 = T1 *InvVV1
!--------------------------------
              Z1DCA = Z1
              T1DCA = T1
              S1DCA = S1
              IF( doRTmiddle ) S1DCA = 0.d0
              Z2DCA = Z1DCA + WZ1
              T2DCA = T1DCA + WT1
              S2DCA = S1DCA + WT1 * VST
              CALL Pos_True_from_Ideal( Itran1, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
              Z1DCA = ZBt
              T1DCA = TBt
              S1DCA = SBt
              CALL Pos_True_from_Ideal( Itran1, S2DCA,Z2DCA,T2DCA, SBt,ZBt,TBt )
              Z2DCA = ZBt
              T2DCA = TBt
              S2DCA = SBt
              CALL MuboSZTtoXYZ( (IC1<=NCOUBA), SiF01,CoF01, S1DCA,Z1DCA,T1DCA, XXXDCA,YYYDCA,ZZZDCA )
              CALL MuboSZTtoXYZ( (IC1<=NCOUBA), SiF01,CoF01, S2DCA,Z2DCA,T2DCA, XXXDCB,YYYDCB,ZZZDCB )
!--------------------------------
              RadTrackBidon = 0.
              XXXdir = XXXDCB - XXXDCA
              YYYdir = YYYDCB - YYYDCA
              ZZZdir = ZZZDCB - ZZZDCA
              R1A = RFTfast(IT1,S1, RadTrackBidon, XXXDCA,YYYDCA,ZZZDCA, XXXdir,YYYdir,ZZZdir)
              IF( LTUBHI(IT1) /= 1 )               CYCLE ! already done, but needed because RFTfast could modify LTUBHI
              IF( R1A <= 0.d0 ) THEN
                ISI1MA = 1
              ELSE
                ISI1MA = 2
              ENDIF
              DO 16 IP2=1,NLTUB(IMDT2)
                T2  = T02 + DIREC * TLTUB(IMDT2,IP2,1)
                SF2 = SS1 + VSTF * (T2-T01)
                ITDEB2 = NTUBH0(IC2,IW2,IP2) + 1
                ITFIN2 = NTUBH0(IC2,IW2,IP2) + NTUBHI(IC2,IW2,IP2)
                DO 15 IT2=ITDEB2,ITFIN2
                  IF( LTUBHI(IT2) /= 1  )                                 CYCLE
                  IF( KTUBHI(IT2) >= 30 )                                 CYCLE ! already used on the two sides
                  Z2 = ZTUBHI(IT2)
!>> Restrict search to very roughly vertex pointing segments
                  IF( AtCosm == 0 ) THEN
                    DZTOL = 0.6D0 * (PASTUB(IMDT1)+PASTUB(IMDT2)+ABS(T2-T1))
                    IF( ABS(Z2-Z1*T2/T1) > DZTOL )                        CYCLE
                  ELSE
                    IF( ABS(Z2-Z1) > 5.d0 * ABS(T2-T1) )                  CYCLE
                  ENDIF
                  IF(IC2 <= NCOUBA) THEN
                    S2  = SF2
                    VST = VSTF
                    TTMOY = (T1+T2)*0.5d0
                    ZZMOY = (Z1+Z2)*0.5d0
                    SSMOY = (S1+S2)*0.5d0
                  ELSE
                    S2  = SF2 * Z2 / Z02S
                    VST = (S2-S1) / (T2-T1)
                    TTMOY = (Z1+Z2)*0.5d0
                    ZZMOY = (T1+T2)*0.5d0
                    SSMOY = (S1+S2)*0.5d0
                  ENDIF
!>> Is the candidate segment in an "allowed" (Theta,Phi) cell ?
                  selecFlag = 0
                  IF( ALOCEL ) THEN
                    K = IOKALI(F01,ZZMOY,TTMOY,SSMOY,2)
                    IF( K == 0 ) CYCLE
                    IF( K == 2 ) selecFlag = 2  ! tighter for roa defined from MDT only
                  ENDIF
                  VV   = SQRT( (Z2-Z1)**2 + (T2-T1)**2 )
                  InvVV = 1.d0 / VV
                  WZ   = (Z2-Z1) *InvVV
                  WT   = (T2-T1) *InvVV
!--------------------------------
                  Z1DCA = Z2
                  T1DCA = T2
                  S1DCA = S2
                  IF( doRTmiddle ) S1DCA = 0.d0
                  Z2DCA = Z1DCA + WZ
                  T2DCA = T1DCA + WT
                  S2DCA = S1DCA + WT * VST
                  CALL Pos_True_from_Ideal( Itran2, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
                  Z1DCA = ZBt
                  T1DCA = TBt
                  S1DCA = SBt
                  CALL Pos_True_from_Ideal( Itran2, S2DCA,Z2DCA,T2DCA, SBt,ZBt,TBt )
                  Z2DCA = ZBt
                  T2DCA = TBt
                  S2DCA = SBt
                  CALL MuboSZTtoXYZ( (IC2<=NCOUBA), SiF02,CoF02, S1DCA,Z1DCA,T1DCA, XXXDCA,YYYDCA,ZZZDCA )
                  CALL MuboSZTtoXYZ( (IC2<=NCOUBA), SiF02,CoF02, S2DCA,Z2DCA,T2DCA, XXXDCB,YYYDCB,ZZZDCB )
!--------------------------------
                  RadTrackBidon = 0.
                  XXXdir = XXXDCB - XXXDCA
                  YYYdir = YYYDCB - YYYDCA
                  ZZZdir = ZZZDCB - ZZZDCA
                  R2A = RFTfast(IT2,S2, RadTrackBidon, XXXDCA,YYYDCA,ZZZDCA, XXXdir,YYYdir,ZZZdir)
                  IF( LTUBHI(IT2) /= 1 )           CYCLE ! already done, but needed because RFTfast could modify LTUBHI
                  IF( R2A <= 0.d0 ) THEN
                    ISI2MA = 1
                  ELSE
                    ISI2MA = 2
                  ENDIF
                  ALFA = ATAN2( WT , WZ )
                  DO ISI1=1,ISI1MA
                    R1 = (2.D0*ISI1-3.D0) * R1A
b12:                DO 12 ISI2=1,ISI2MA
                      R2 = (2.D0*ISI2-3.D0) * R2A
                      IF( ABS( (R1+R2)*InvVV ) >= 1.D0 )        CYCLE
                      TETA = ALFA + ASIN( (R1+R2)*InvVV )
                      SITE = SIN( TETA )
                      IF( ABS(SITE) < 0.333D0 )                 CYCLE ! no more than 4 tubes per layer
                      InvSITE = 1.d0 / SITE
                      Z1P  = Z1 + R1*InvSITE
                      Z2P  = Z2 - R2*InvSITE
                      IF( Z1P > Z1 ) THEN
                        IF( KTUBHI(IT1)/10 == 1 )               CYCLE
                      ELSE
                        IF( KTUBHI(IT1)/10 == 2 )               CYCLE
                      ENDIF
                      IF( Z2P > Z2 ) THEN
                        IF( KTUBHI(IT2)/10 == 1 )               CYCLE
                      ELSE
                        IF( KTUBHI(IT2)/10 == 2 )               CYCLE
                      ENDIF
                      CALL Pos_True_from_Ideal(Itran1,S1,Z1P,T1, S1true,Z1true,T1true)
                      CALL Pos_True_from_Ideal(Itran2,S2,Z2P,T2, S2true,Z2true,T2true)
                      T0true   = (T1true+T2true) *0.5d0
                      Z0true   = (Z1true+Z2true) *0.5d0
                      S0true   = (S1true+S2true) *0.5d0
                      VVtrue   = SQRT( (Z2true-Z1true)**2 + (T2true-T1true)**2 )
                      InvVVtrue = 1.d0/VVtrue
                      WZtrue   = (Z2true-Z1true) *InvVVtrue
                      WTtrue   = (T2true-T1true) *InvVVtrue
                      VSTtrue  = (S2true-S1true) / (T2true-T1true)
                      IF( AtCosm == 0 ) THEN
                        cosdir   = ABS( T0true*WTtrue + Z0true*WZtrue ) / SQRT( T0true**2 + Z0true**2 )
                        IF( cosdir < 0.50d0 )                                     CYCLE
                        IF( CARISC((IC1+1)/2)(2:2) == 'I' .and. cosdir < 0.77d0 ) CYCLE
                      ELSE
                        cosdir = 1.0d0
                      ENDIF
                      IF( LLI(1) > 0 .OR. LLI(2) > 0 ) THEN !>> if S measurement available, try to refine S position
                        CALL fitSPosition(IGEOM1,IC1,LLI,Z0true,T0true,S0true,WZtrue,WTtrue,VSTtrue,rejectIt)
                        IF( rejectIt ) CYCLE
                      ENDIF
!!$                      IF( STRICT .AND. .NOT.BOGcase ) THEN !>> In "strict" conditions, ask for at least one "Z" strip hit
!!$                        IOKSZ1 = IOKSZS(IC1,IW1,Z0true,T0true,S0true,WZtrue,WTtrue,VSTtrue)
!!$                        IF( IOKSZ1 /= 1 ) THEN
!!$                          IOKSZ2 = IOKSZS(IC2,IW2,Z0true,T0true,S0true,WZtrue,WTtrue,VSTtrue)
!!$                          IF( IOKSZ2 /= 1 )  CYCLE
!!$                        ENDIF
!!$                      ENDIF
                      CALL DROIKI(Z0true,T0true,S0true,WZtrue,WTtrue,VSTtrue,CTOT,dcapoin,I1ML)
                      IF( CTOT > CHIMA1 )                                                     CYCLE
                      IF( rejectSegment( dcapoin(1)%III, dcapoin(2)%III, IWWloc, selecFlag) ) CYCLE
b11:                  DO 11 IK=NBKold+1,NBKA
                        DO i3=1,2
                          DO i2=1,4
                            DO i1=1,N4M
                              IF( ITKA(i1,i2,i3,IK) /= dcapoin(i3)%III(i1,i2) ) CYCLE b11
                              IF( ISKA(i1,i2,i3,IK) /= dcapoin(i3)%JJJ(i1,i2) ) CYCLE b11
                            ENDDO
                          ENDDO
                        ENDDO
                        IF( CTOT < CKA(IK) ) THEN
                          ZKA(IK) = Z0true
                          TKA(IK) = T0true
                          SKA(IK) = S0true
                          AKA(IK) = ATAN2( WTtrue , WZtrue )
                          VKA(IK) = VSTtrue
                          CKA(IK) = CTOT
                        ENDIF
                        CYCLE b12
11                    ENDDO b11
                      adcmax = 0.d0
                      DO i3=1,2
                        DO i2=1,4
                          DO i1=1,N4M
                            IF( dcapoin(i3)%III(i1,i2) > 0 ) adcmax = max( adcmax, dtubhi(dcapoin(i3)%III(i1,i2)) )
                          ENDDO
                        ENDDO
                      ENDDO
                      IF( adcmax < SegAdc ) CYCLE
                      NBKA = NBKA + 1
                      ZKA(NBKA) = Z0true
                      TKA(NBKA) = T0true
                      SKA(NBKA) = S0true
                      AKA(NBKA) = ATAN2( WTtrue , WZtrue )
                      VKA(NBKA) = VSTtrue
                      CKA(NBKA) = CTOT
                      DO i3=1,2
                        ITKA(:,:,i3,NBKA) = dcapoin(i3)%III(:,:)
                        ISKA(:,:,i3,NBKA) = dcapoin(i3)%JJJ(:,:)
                      ENDDO
                      IF( NBKA >= MAKA ) GO TO 70
12                  ENDDO b12
                  ENDDO
15              ENDDO
16            ENDDO
20          ENDDO
21        ENDDO
25      ENDDO
26      CONTINUE
!
        IF(NBKA <= 0) GO TO 900
!
      ELSE
!>> ---------------------------------------------------------------------
!>> Straight track segments search in multi-layer IC1,IW1
!>> (or IC2,IW2) alone.
        IF( IWWloc(1) >= 1 ) THEN
          I = 1
        ELSEIF( IWWloc(2) >= 1 ) THEN
          I = 2
        ELSE
          GO TO 900
        ENDIF
        ICI = ICC(I)
        IWI = IWWloc(I)
        IGEOMI = IGEOMA(ICI,IFCHA(ICI,IWI),IZCHA(ICI,IWI))
        ItranI = ItransCHA(IGEOMI)
        T0I    = TCECHAV(IGEOMI)
        F0I    = FCECHAV(IGEOMI)
        SiF0I  = SINCHAV(IGEOMI)
        CoF0I  = COSCHAV(IGEOMI)
        Z0I    = ZCECHAV(IGEOMI)
        IMDTI  = ISTMDTV(IGEOMI)
        JMDT(1) = IMDTI
        JMDT(2) = IMDTI
        ADT0 = 30.D0
!
!>> DIREC < 0 if one is in the "negative" end cap
        DIREC = 1.D0
        IF(T0I < 0.D0) DIREC = -1.D0
!
!>> Deals with S coordinates
        NSS = 1
        SSS = SSI(I)
        IF( SSS > 100000.D0 ) THEN
          SMAIP=(SPICHAV(IGEOMI)+SPACHAV(IGEOMI))*0.5d0
          SMAIN=(SNICHAV(IGEOMI)+SNACHAV(IGEOMI))*0.5d0
          SS0 = SMAIN + 5.D0
          DSS = SMAIP - SMAIN - 10.D0
          NSS = DSS / 50.D0
          NSS = MAX( 5 , NSS )
        ENDIF
!
!>> First step, one looks for all possible candidates
!>> using hits that have not been used (by double m.l. sgments) up to now
!>> (on diferent S positions when there is no strip measurement)
        NOUSED = 0
        DO IP=1,NLTUB(IMDTI)
          ITDEB = NTUBH0(ICI,IWI,IP) + 1
          ITFIN = NTUBH0(ICI,IWI,IP) + NTUBHI(ICI,IWI,IP)
          DO IT=ITDEB,ITFIN
            IF( KTUBHI(IT) <= 2 ) NOUSED = NOUSED + 1
            IF( NOUSED >= 2 ) GO TO 30
          ENDDO
        ENDDO
        GO TO 60
30      DO 55 ISS=1,NSS
          IF(NSS >= 2) THEN
            IF( ISS == 1 ) THEN
              JSS = (NSS+1)/2 - 1
            ELSE IF( doRTmiddle ) THEN
              EXIT
            ELSE IF( ISS == (NSS+1)/2 ) THEN
              JSS = 0
            ELSE
              JSS = ISS - 1
            ENDIF
            SSS = SS0 + JSS * DSS / (NSS-1)
          ENDIF
          VSTF = SSS / T0I
          NBKold = NBKA
          DO 51 IP1=1,NLTUB(IMDTI)-1
            T1  = T0I + DIREC * TLTUB(IMDTI,IP1,1)
            SF1 = SSS + VSTF * (T1-T0I)
            ITDEB1 = NTUBH0(ICI,IWI,IP1) + 1
            ITFIN1 = NTUBH0(ICI,IWI,IP1) + NTUBHI(ICI,IWI,IP1)
            DO 50 IT1=ITDEB1,ITFIN1
              IF( LTUBHI(IT1) /= 1 )                 CYCLE
              IF( KTUBHI(IT1) >= 3 )                 CYCLE
              Z1 = ZTUBHI(IT1)
              IF(ICI.LE.NCOUBA) THEN
                S1  = SF1
                IF( ALOCEL ) THEN
                  IF( IOKALI(F0I,Z1,T1,S1,1) == 0 )  CYCLE
                ENDIF
                VST = VSTF
              ELSE
                Z1DZ0I = Z1 / Z0I
                S1  = SF1  * Z1DZ0I
                IF( ALOCEL ) THEN
                  IF( IOKALI(F0I,T1,Z1,S1,1) == 0 )  CYCLE
                ENDIF
                VST = VSTF * Z1DZ0I
              ENDIF
              VV1 = SQRT( Z1**2 + T1**2 )
              InvVV1 = 1.d0/VV1
              WZ1 = Z1 *InvVV1
              WT1 = T1 *InvVV1
!--------------------------------
              Z1DCA = Z1
              T1DCA = T1
              S1DCA = S1
              IF( doRTmiddle ) S1DCA = 0.d0
              Z2DCA = Z1DCA + WZ1
              T2DCA = T1DCA + WT1
              S2DCA = S1DCA + WT1 * VST
              CALL Pos_True_from_Ideal( ItranI, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
              Z1DCA = ZBt
              T1DCA = TBt
              S1DCA = SBt
              CALL Pos_True_from_Ideal( ItranI, S2DCA,Z2DCA,T2DCA, SBt,ZBt,TBt )
              Z2DCA = ZBt
              T2DCA = TBt
              S2DCA = SBt
              CALL MuboSZTtoXYZ( (ICI<=NCOUBA), SiF0I,CoF0I, S1DCA,Z1DCA,T1DCA, XXXDCA,YYYDCA,ZZZDCA )
              CALL MuboSZTtoXYZ( (ICI<=NCOUBA), SiF0I,CoF0I, S2DCA,Z2DCA,T2DCA, XXXDCB,YYYDCB,ZZZDCB )
!--------------------------------
              RadTrackBidon = 0.
              XXXdir = XXXDCB - XXXDCA
              YYYdir = YYYDCB - YYYDCA
              ZZZdir = ZZZDCB - ZZZDCA
              R1A = RFTfast(IT1,S1, RadTrackBidon, XXXDCA,YYYDCA,ZZZDCA, XXXdir,YYYdir,ZZZdir)
              IF( LTUBHI(IT1) /= 1 )              CYCLE  ! already done, but needed because RFTfast could modify LTUBHI
              IF( R1A <= 0.d0 ) THEN
                ISI1MA = 1
              ELSE
                ISI1MA = 2
              ENDIF
              DO 46 IP2=IP1+1,NLTUB(IMDTI)
                T2  = T0I + DIREC * TLTUB(IMDTI,IP2,1)
                SF2 = SSS + VSTF * (T2-T0I)
                ITDEB2 = NTUBH0(ICI,IWI,IP2) + 1
                ITFIN2 = NTUBH0(ICI,IWI,IP2) + NTUBHI(ICI,IWI,IP2)
                DO 45 IT2=ITDEB2,ITFIN2
                  IF( LTUBHI(IT2) /= 1 )                                 CYCLE
                  IF( KTUBHI(IT2) >= 3 )                                 CYCLE
                  Z2 = ZTUBHI(IT2)
!>> Restrict search to very roughly vertex pointing segments
                  IF( AtCosm == 0 ) THEN
                    DZTOL = 0.6D0 * (2.D0*PASTUB(IMDTI)+ABS(T2-T1))
                    IF( ABS(Z2-Z1*T2/T1) > DZTOL )                       CYCLE
                  ELSE
                    IF( ABS(Z2-Z1) > 5.d0 * ABS(T2-T1) )                 CYCLE
                  ENDIF
                  IF( ICI <= NCOUBA ) THEN
                    S2  = SF2
                    VST = VSTF
                    TTMOY = (T1+T2)*0.5d0
                    ZZMOY = (Z1+Z2)*0.5d0
                    SSMOY = (S1+S2)*0.5d0
                  ELSE
                    S2  = SF2 * Z2 / Z0I
                    VST = (S2-S1) / (T2-T1)
                    TTMOY = (Z1+Z2)*0.5d0
                    ZZMOY = (T1+T2)*0.5d0
                    SSMOY = (S1+S2)*0.5d0
                  ENDIF
!>> Is the candidate segment in an "allowed" (Theta,Phi) cell ? only good ones from single ML
                  IF( ALOCEL .AND. IOKALI(F0I,ZZMOY,TTMOY,SSMOY,1) /= 1) CYCLE
                  VV   = SQRT( (Z2-Z1)**2 + (T2-T1)**2 )
                  InvVV = 1.d0 / VV
                  WZ   = (Z2-Z1) *InvVV
                  WT   = (T2-T1) *InvVV
!--------------------------------
                  Z1DCA= Z2
                  T1DCA= T2
                  S1DCA= S2
                  IF( doRTmiddle ) S1DCA = 0.d0
                  Z2DCA= Z1DCA + WZ
                  T2DCA= T1DCA + WT
                  S2DCA= S1DCA + WT * VST
                  CALL Pos_True_from_Ideal( ItranI, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
                  Z1DCA= ZBt
                  T1DCA= TBt
                  S1DCA= SBt
                  CALL Pos_True_from_Ideal( ItranI, S2DCA,Z2DCA,T2DCA, SBt,ZBt,TBt )
                  Z2DCA= ZBt
                  T2DCA= TBt
                  S2DCA= SBt
                  CALL MuboSZTtoXYZ( (ICI<=NCOUBA), SiF0I,CoF0I, S1DCA,Z1DCA,T1DCA, XXXDCA,YYYDCA,ZZZDCA )
                  CALL MuboSZTtoXYZ( (ICI<=NCOUBA), SiF0I,CoF0I, S2DCA,Z2DCA,T2DCA, XXXDCB,YYYDCB,ZZZDCB )
!--------------------------------
                  RadTrackBidon = 0.
                  XXXdir = XXXDCB - XXXDCA
                  YYYdir = YYYDCB - YYYDCA
                  ZZZdir = ZZZDCB - ZZZDCA
                  R2A = RFTfast(IT2,S2, RadTrackBidon, XXXDCA,YYYDCA,ZZZDCA, XXXdir,YYYdir,ZZZdir)
                  IF( LTUBHI(IT2) /= 1 )          CYCLE  ! already done, but needed because RFTfast could modify LTUBHI
                  IF( R2A <= 0.d0 ) THEN
                    ISI2MA = 1
                  ELSE
                    ISI2MA = 2
                  ENDIF
                  ALFA = ATAN2( WT , WZ )
                  DO ISI1=1,ISI1MA
                    R1 = (2.D0*ISI1-3.D0) * R1A
b42:                DO 42 ISI2=1,ISI2MA
                      R2 = (2.D0*ISI2-3.D0) * R2A
                      IF( ABS( (R1+R2)*InvVV ) >= 1.D0 )   CYCLE
                      TETA = ALFA + ASIN( (R1+R2)*InvVV )
                      SITE = SIN( TETA )
                      IF( ABS(SITE) < 0.333D0 )            CYCLE ! no more than 4 tubes per layer
                      InvSITE = 1.d0 / SITE
                      Z1P  = Z1 + R1*InvSITE
                      Z2P  = Z2 - R2*InvSITE
                      IF( Z1P > Z1 ) THEN
                        IF( KTUBHI(IT1) == 1 )             CYCLE
                      ELSE
                        IF( KTUBHI(IT1) == 2 )             CYCLE
                      ENDIF
                      IF( Z2P > Z2 ) THEN
                        IF( KTUBHI(IT2) == 1 )             CYCLE
                      ELSE
                        IF( KTUBHI(IT2) == 2 )             CYCLE
                      ENDIF
                      CALL Pos_True_from_Ideal(ItranI,S1,Z1P,T1, S1true,Z1true,T1true)
                      CALL Pos_True_from_Ideal(ItranI,S2,Z2P,T2, S2true,Z2true,T2true)
                      T0true   = (T1true+T2true) *0.5d0
                      Z0true   = (Z1true+Z2true) *0.5d0
                      S0true   = (S1true+S2true) *0.5d0
                      VVtrue   = SQRT( (Z2true-Z1true)**2 + (T2true-T1true)**2 )
                      InvVVtrue = 1.d0/VVtrue
                      WZtrue   = (Z2true-Z1true) *InvVVtrue
                      WTtrue   = (T2true-T1true) *InvVVtrue
                      VSTtrue  = (S2true-S1true) / (T2true-T1true)
                      IF( AtCosm == 0 ) THEN
                        cosdir   = ABS( T0true*WTtrue + Z0true*WZtrue ) / SQRT( T0true**2 + Z0true**2 )
                        IF( cosdir < 0.50d0 )                                     CYCLE
                        IF( CARISC((ICI+1)/2)(2:2) == 'I' .and. cosdir < 0.77d0 ) CYCLE
                      ELSE
                        cosdir = 1.0d0
                      ENDIF
                      IF( LLI(I) > 0 ) THEN !>> if S measurement available, try to refine S position
                        CALL fitSPosition(IGEOMI,ICI,LLI,Z0true,T0true,S0true,WZtrue,WTtrue,VSTtrue,rejectIt)
                        IF( rejectIt ) CYCLE
                      ENDIF
!!$                      IF( STRICT .AND. .NOT.BOGcase ) THEN !>> In "strict" conditions, ask for at least one "Z" strip hit
!!$                        IOKSZ1 = IOKSZS(ICI,IWI,Z0true,T0true,S0true,WZtrue,WTtrue,VSTtrue)
!!$                        IF( IOKSZ1 /= 1 )                CYCLE
!!$                      ENDIF
                      CALL DROIKI(Z0true,T0true,S0true,WZtrue,WTtrue,VSTtrue,CTOT,dcapoin,I1ML)
                      IF( CTOT > CHIMA1 )                                             CYCLE
                      IF( rejectSegment( dcapoin(1)%III, dcapoin(2)%III, IWWloc, 0) ) CYCLE
b41:                  DO 41 IK=NBKold+1,NBKA
                        DO i2=1,4
                          DO i1=1,N4M
                            IF( ITKA(i1,i2,I,IK) /= dcapoin(I)%III(i1,i2) )    CYCLE b41
                            IF( ISKA(i1,i2,I,IK) /= dcapoin(I)%JJJ(i1,i2) )    CYCLE b41
                          ENDDO
                        ENDDO
                        IF( CTOT < CKA(IK) ) THEN
                          ZKA(IK) = Z0true
                          TKA(IK) = T0true
                          SKA(IK) = S0true
                          AKA(IK) = ATAN2( WTtrue , WZtrue )
                          VKA(IK) = VSTtrue
                          CKA(IK) = CTOT
                        ENDIF
                        CYCLE b42
41                    ENDDO b41
                      adcmax = 0.d0
                      DO i2=1,4
                        DO i1=1,N4M
                          IF( dcapoin(I)%III(i1,i2) > 0 ) adcmax = max( adcmax, dtubhi(dcapoin(I)%III(i1,i2)) )
                        ENDDO
                      ENDDO
                      IF( adcmax < SegAdc ) CYCLE
                      NBKA = NBKA + 1
                      ZKA(NBKA) = Z0true
                      TKA(NBKA) = T0true
                      SKA(NBKA) = S0true
                      AKA(NBKA) = ATAN2( WTtrue , WZtrue )
                      VKA(NBKA) = VSTtrue
                      CKA(NBKA) = CTOT
                      DO i3=1,2
                        ITKA(:,:,i3,NBKA) = dcapoin(i3)%III(:,:)
                        ISKA(:,:,i3,NBKA) = dcapoin(i3)%JJJ(:,:)
                      ENDDO
                      IF( NBKA >= MAKA ) GO TO 70
42                  ENDDO b42
                  ENDDO
45              ENDDO
46            ENDDO
50          ENDDO
51        ENDDO
55      ENDDO
60      CONTINUE
!
        IF(NBKA <= 0) GO TO 900
!
      ENDIF
!>> ---------------------------------------------------------------------
!
!
!>> One limits the number of first candidates if it is too high
70    IF( NBKA > MAKB ) THEN
        CALL CROISS(CKA,NBKA,NREAR)
        NBKA = MAKB
        CALL KROISS(NREAR,NBKA,IORD)
        DO IK=1,NBKA
          KIK = NREAR(IORD(IK))
          IF( KIK == IK ) CYCLE
          ZKA(IK) = ZKA(KIK)
          TKA(IK) = TKA(KIK)
          SKA(IK) = SKA(KIK)
          AKA(IK) = AKA(KIK)
          VKA(IK) = VKA(KIK)
          CKA(IK) = CKA(KIK)
          ITKA(:,:,:,IK) = ITKA(:,:,:,KIK)
          ISKA(:,:,:,IK) = ISKA(:,:,:,KIK)
        ENDDO
      ENDIF
!
      IF( MBPri >= 5 ) THEN
        PRINT 1000,(L,ZKA(L),TKA(L),AKA(L)*57.2957795D0,CKA(L),L=1,NBKA)
1000    FORMAT(' First candidate No',I3,' :',2F12.4,F12.2,F14.2)
      ENDIF
!
! The scan of segments will be done only on those not changed (through changes in KTUBHI)
! since the previous scan
      ZKAIMP(1:NBKA) = ZKA(1:NBKA)
      AKAIMP(1:NBKA) = AKA(1:NBKA)
      ToScan(1:NBKA) = .true.
!
      RESscan = 0.5d0 * ( RESTUB(JMDT(1)) + RESTUB(JMDT(2)) )
      Nscan  = MIN( MAX( 4 , 3 + INT(RESscan/0.01D0) ) , nscama )
      RESscan = SIGMOK * RESscan / (Nscan-1)
!
!>> Second step, one trims the coordinates of the candidates
80    CALL CROISS(CKA,NBKA,NREAR)
!
b200: DO 200 IK=1,NBKA
        IN = NREAR(IK)
!
b100:   DO 100 IKPRE=1,IK-1
          INPRE = NREAR(IKPRE)
          DO I=1,2
            DO IP=1,NLTUB(JMDT(I))
              DO K=1,4
                IF( ITKA(IP,K,I,IN) /= ITKA(IP,K,I,INPRE) ) CYCLE b100
                IF( ISKA(IP,K,I,IN) /= ISKA(IP,K,I,INPRE) ) CYCLE b100
              ENDDO
            ENDDO
          ENDDO
          CKA(IN) = CKA(IN) + 10000.D0
          CYCLE b200
100     ENDDO b100
!
        Z0 = ZKA(IN)
        T0 = TKA(IN)
        S0 = SKA(IN)
        A0 = AKA(IN)
        V0 = VKA(IN)
        IF( .NOT.ToScan(IN) ) THEN
          ZCUR = ZKAIMP(IN)
          ACUR = AKAIMP(IN)
          SiCUR = SIN( ACUR )
          CoCUR = COS( ACUR )
          GO TO 160
        ENDIF
        DZZ   = RESscan
        DAA   = ABS( 2.D0 * RESscan * SIN(A0) / ADT0 )
        DZcut = DZZ / 20.d0
        DAcut = DAA / 20.d0
        Iflop = 1
        Nflop = 0
        ZCUR  = Z0
        ACUR  = A0
        SiCUR = SIN( ACUR )
        CoCUR = COS( ACUR )
        ZCURold = 9999999.D0
        ACURold = 9999999.D0
110     CONTINUE
        IF( Iflop == 1 ) THEN
          ISmin = 0
          Cmin  = 9999999.D0
          DO I=1,2*Nscan+1
            IS = Iscan(I)
            ZI = ZCUR + IS*DZZ
            CALL DROIKI(ZI,T0,S0,CoCUR,SiCUR,V0,Cscan(IS),dcapoin,I1ML)
            IF( Cscan(IS) < Cmin )  THEN
              ISmin = IS
              Cmin  = Cscan(IS)
            ELSEIF( Cmin < 999.D0 ) THEN
              IF( IS-ABS(ISmin) >= 2 ) EXIT
            ENDIF
          ENDDO
          ZCURold = ZCUR
          IF( ABS(ISmin) < Nscan ) THEN
            CM1 = Cscan(ISmin-1)
            C00 = Cscan(ISmin)
            CP1 = Cscan(ISmin+1)
            ESS = (CM1-CP1) / MAX( CM1+CP1-2.D0*C00 , CP0001 ) *0.5d0
            ZCUR = ZCUR + (ISmin+ESS)*DZZ
          ELSE
            ZCUR = ZCUR + ISmin*DZZ
          ENDIF
          Iflop = 2
        ELSE
          ISmin = 0
          Cmin  = 9999999.D0
          DO I=1,2*Nscan+1
            IS = Iscan(I)
            AI = ACUR + IS*DAA
            SiI = SIN( AI )
            CoI = COS( AI )
            CALL DROIKI(ZCUR,T0,S0,CoI,SiI,V0,Cscan(IS),dcapoin,I1ML)
            IF( Cscan(IS) < Cmin )  THEN
              ISmin = IS
              Cmin  = Cscan(IS)
            ELSEIF( Cmin < 999.D0 ) THEN
              IF( IS-ABS(ISmin) >= 2 ) EXIT
            ENDIF
          ENDDO
          ACURold = ACUR
          IF( ABS(ISmin) < Nscan ) THEN
            CM1 = Cscan(ISmin-1)
            C00 = Cscan(ISmin)
            CP1 = Cscan(ISmin+1)
            ESS = (CM1-CP1) / MAX( CM1+CP1-2.D0*C00 , CP0001 ) *0.5d0
            ACUR = ACUR + (ISmin+ESS)*DAA
          ELSE
            ACUR = ACUR + ISmin*DAA
          ENDIF
          SiCUR = SIN( ACUR )
          CoCUR = COS( ACUR )
          Iflop = 1
          Nflop = Nflop + 1
        ENDIF
        IF( Nflop < 3 ) THEN
          IF( ABS(ZCUR-ZCURold) > DZcut ) GO TO 110
          IF( ABS(ACUR-ACURold) > DAcut ) GO TO 110
        ENDIF
        ZKAIMP(IN) = ZCUR
        AKAIMP(IN) = ACUR
160     CALL DROIKI(ZCUR,T0,S0,CoCUR,SiCUR,V0,CKA(IN),dcapoin,I1ML)
        CALL DroiPena(ZCUR,T0,S0,CoCUR,SiCUR,V0,Pena)
        CKA(IN) = CKA(IN) + Pena
        DO I=1,2
          ITKA(:,:,I,MAKB+IN) = dcapoin(I)%III(:,:)
        ENDDO
        ToScan(IN) = .false.
!
200   ENDDO b200
!
      CALL CROISS(CKA,NBKA,NREAR)
!
#if !defined(ATLAS_NOPRINTMU)
      IF( MBPri >= 5 ) THEN
        PRINT 2000,( L,NREAR(L), ZKAIMP(NREAR(L)),TKA(NREAR(L)),   &
                     AKAIMP(NREAR(L))*57.2957795D0,CKA(NREAR(L)),L=1,NBKA)
2000    FORMAT(1X,I3,' Second candidate No',I3,' :',2F12.4,F12.2,F14.2)
      ENDIF
#endif
!
!
      IN = NREAR(1)
      IF( CKA(IN) >= CHIMA2 ) GO TO 900
!>> Here, one has found one more track : one stores it ...
      SiAKAIMP = SIN( AKAIMP(IN) )
      CoAKAIMP = COS( AKAIMP(IN) )
      saveDcaSeg = .true.
      CALL DROIKI(ZKAIMP(IN),TKA(IN),SKA(IN),CoAKAIMP,SiAKAIMP,VKA(IN),CTOT,dcapoin,I1ML)
      saveDcaSeg = .false.
!>> first, one removes it when it is a ghost from another candidate
      IF( NBCADO > 0 ) THEN
b206: DO 206 IK=1,NBCADO
          IF( CTOT-segcado(IK)%chi2 < CHIMA2/3.D0 )     CYCLE
          DO 204 I=1,2
            DO 203 IP=1,NLTUB(JMDT(I))
b202:         DO 202 K1=1,4
                IT1 = IABS( dcapoin(I)%III(IP,K1) )
                IF( IT1 == 0 .OR. IT1 > NTUBHT ) CYCLE
                DO K2=1,4
                  IT2 = IABS( JCADO(I,IK,IP,K2) )
                  IF( IT2 == IT1 )               CYCLE b202
                ENDDO
                CYCLE b206
202           ENDDO b202
203         ENDDO
204       ENDDO
          CKA(IN) = CKA(IN) + CHIMA1 ! reject this candidate 
          GO TO 260
206     ENDDO b206
      ENDIF
!
!>> then special case for segments with only one hit in each multi-layer
      NML(:) = 0
      DO I=1,2
        DO IP=1,NLTUB(JMDT(I))
          DO K=1,4
            IF( dcapoin(I)%III(IP,K) >= 1 )  NML(I) = NML(I) + 1
          ENDDO
        ENDDO
      ENDDO
      IF( NML(1) <= 1 .AND. NML(2) <= 1 ) THEN
        CKA(IN) = CKA(IN) + CHIMA1 ! reject this candidate 
        GO TO 260
      ENDIF
!
!>> then reject segment according to the pattern of tubes hit/missed/masked/inefficient
      IF( rejectSegment( dcapoin(1)%III, dcapoin(2)%III, IWWloc, 1) ) THEN
        CKA(IN) = CKA(IN) + CHIMA1
        GO TO 260
      ENDIF
!
!>> and at last, in "strict" conditions, ask that the segment is in the active region of the S strips used
!>> should no more be necessary because of the fitSPosition call 
      IF( STRICT ) THEN
        DO I=1,2
          IF( IWWloc(I) > 0 .AND. LLI(I) > 0 ) THEN
            INOF   = IFCHA(ICC(I),IWWloc(I))
            INOZ   = IZCHA(ICC(I),IWWloc(I))
            IGEOMI = IGEOMA(ICC(I),INOF,INOZ)
            IF( .NOT.IOKSSS( ICC(I),LLI(I),IGEOMI, ZKAIMP(IN),TKA(IN),SKA(IN),CoAKAIMP,SiAKAIMP,VKA(IN)) ) THEN
              CKA(IN) = CKA(IN) + CHIMA1
              GO TO 260
            ENDIF
          ENDIF
        ENDDO
      ENDIF
!
      NBCADO = NBCADO + 1
      segcado(NBCADO)%chi2 = CTOT
      segcado(NBCADO)%i1ml = I1ML
      segcado(NBCADO)%nu   = 0
      segcado(NBCADO)%no   = 0
      segcado(NBCADO)%nh   = 0
      IF( MBPri >= 3 ) THEN
        write(*,*) '====== new segment =========='
        write(*,*) ' CTOT I1ML IWW ',CTOT,I1ML,IWWloc(1:2)
        write(*,*) ' cosdir ',(TKA(IN)*SiAKAIMP+ZKAIMP(IN)*CoAKAIMP)/SQRT(TKA(IN)**2+ZKAIMP(IN)**2)
      ENDIF
      DO 240 I=1,2
        I3MI = 3 - I
        IF( IWWloc(I) >= 1 ) THEN
          INOF = IFCHA(ICC(I),IWWloc(I))
          INOZ = IZCHA(ICC(I),IWWloc(I))
          segcado(NBCADO)%tpos(I) = TCECHAV( IGEOMA(ICC(I),INOF,INOZ) )
          IF( IWWloc(I3MI) == 0 ) segcado(NBCADO)%tpos(I) = TCECHAV( IGEOMA(ICC(I),INOF,INOZ) ) - 8.D0*DIREC*(-1.D0)**I
        ELSE
          INOF = IFCHA(ICC(I3MI),IWWloc(I3MI))
          INOZ = IZCHA(ICC(I3MI),IWWloc(I3MI))
          segcado(NBCADO)%tpos(I) = TCECHAV( IGEOMA(ICC(I3MI),INOF,INOZ) ) - 8.D0*DIREC*(-1.D0)**I
        ENDIF
        DT0I = segcado(NBCADO)%tpos(I) - TKA(IN)
        segcado(NBCADO)%zpos(I) = ZKAIMP(IN) + DT0I/TAN( AKAIMP(IN) )
        segcado(NBCADO)%spos(I) =  SKA(IN)   + DT0I*VKA(IN)
        DO 230 IP=1,NLTUB(JMDT(I))
          IF( MBPri >= 3 .AND. IWWloc(I) > 0 ) WRITE(*,'(" I IP ",2I4,"  IT(1:4) ",4I10,5X,"--- IS(1:4) ",4I4)') &
            I,IP,dcapoin(I)%III(IP,1:4),dcapoin(I)%JJJ(IP,1:4)
          DO K=1,4
            JCADO(I,NBCADO,IP,K) = 0
            IF( IWWloc(I) <= 0 )  CYCLE
            IT = dcapoin(I)%III(IP,K)
            IS = dcapoin(I)%JJJ(IP,K)
            JCADO(I,NBCADO,IP,K) = IT
            IF( IT >= 1 ) THEN
!>> One flags the used hits : 0 = not used,
!>> 10 = used on the left   by a segment extending on two multi-layers,
!>> 20 = used on the right  "  "    "        "      "  "       "      ,
!>> 30 = used on both sides "  "    "        "      "  "       "
!>>  1 = used on the left   by a segment in a single multi-layer,
!>>  2 = used on the right  "  "    "    "  "   "         "      ,
!>>  3 = used on both sides "  "    "    "  "   "         "
              KTUB10 = KTUBHI(IT) / 10
              KTUB01 = KTUBHI(IT) - 10*KTUB10
              IF( IWWloc(1)*IWWloc(2) >= 1 .AND. NML(1)+NML(2) > 2 ) THEN
                IF( KTUB10 == 0 ) THEN
                  KTUBHI(IT) = 10*IS + KTUB01
                ELSEIF( KTUB10 /= IS ) THEN
                  KTUBHI(IT) =  30   + KTUB01
                ENDIF
              ELSE
                IF( KTUB01 == 0 ) THEN
                  KTUBHI(IT) = 10*KTUB10 + IS
                ELSEIF( KTUB01 /= IS ) THEN
                  KTUBHI(IT) = 10*KTUB10 +  3
                ENDIF
              ENDIF
              CALL AddHitToSeg( IP, K, dcapoin(I), segcado(NBCADO) )
! Now look at which other segments could be modified due to this used hit
              DO IK=1,NBKA
                IF( CKA(IK) > CHIMA1 )  CYCLE
                IF( IK == IN )          CYCLE
                IF( ToScan(IK) )        CYCLE
                DO KIK=1,4
                  IF( ABS(ITKA(IP,KIK,I,MAKB+IK)) == IT ) THEN
                    ToScan(IK) = .true.
                    EXIT
                  ENDIF
                ENDDO
              ENDDO
            ELSE IF( IT < 0 .AND. IT > -10000000 ) THEN
              CALL AddOutlierToSeg( IP, K, dcapoin(I), segcado(NBCADO) )
            ELSE IF( IT <= -20000000 ) THEN
              CALL AddHoleToSeg( IP, K, dcapoin(I), segcado(NBCADO) )
            ENDIF
          ENDDO
230     ENDDO
240   ENDDO
      IF( MBPri >= 3 ) THEN
        write(*,*) ' number of hits outliers holes ',segcado(NBCADO)%nu,segcado(NBCADO)%no,segcado(NBCADO)%nh
        write(*,*) ' =========== end ==============='
      ENDIF
!
!>>
260   IF( NBCADO >= MACADO ) GO TO 900
!>> and one goes back to the second step with the remaining candidates
      NBKB = 0
      DO IK=1,NBKA
        IF( CKA(IK) > CHIMA2 )  CYCLE ! chi2 is here after scan, so we can cut on CHIMA2
        IF( IK == IN )          CYCLE
        SiAKA = SIN( AKA(IK) )
        CoAKA = COS( AKA(IK) )
        CALL DROIKI(ZKA(IK),TKA(IK),SKA(IK),CoAKA,SiAKA,VKA(IK), CTOT, dcapoin, I1ML)
        IF( CTOT > CHIMA1 )     CYCLE
        CALL DroiPena(ZKA(IK),TKA(IK),SKA(IK),CoAKA,SiAKA,VKA(IK), Pena)
        CTOT = CTOT + Pena
        NBKB = NBKB + 1
        ZKA(NBKB) = ZKA(IK)
        TKA(NBKB) = TKA(IK)
        SKA(NBKB) = SKA(IK)
        AKA(NBKB) = AKA(IK)
        VKA(NBKB) = VKA(IK)
        CKA(NBKB) = CTOT
        DO I=1,2
          ITKA(:,:,I,NBKB) = dcapoin(I)%III(:,:)
          ISKA(:,:,I,NBKB) = dcapoin(I)%JJJ(:,:)
        ENDDO
        ZKAIMP(NBKB) = ZKAIMP(IK)
        AKAIMP(NBKB) = AKAIMP(IK)
        ToScan(NBKB) = ToScan(IK)
      ENDDO
      NBKA = NBKB
      IF( NBKA >= 1 ) GO TO 80
!
!
!>> End
900   CONTINUE
      IF( MBPri >= 3 ) THEN
        PRINT 6000,ICC(1),ICC(2),IWWloc(1),IWWloc(2),SSI(1),SSI(2),TTI(1),TTI(2),MACADO,NBCADO
6000    FORMAT('   Icc =',2I3,'   Iww =',2I3,'   SSi =',2F10.2,'   TTi =',2F10.2,   &
             '    Number of candidates (max=',I3,') :',I4)
        DO 911 IK=1,NBCADO
          PRINT 6100,IK,segcado(IK)%zpos(1:2),segcado(IK)%spos(1:2),segcado(IK)%chi2
6100      FORMAT('     Candidate No',I3,' : zt1,zt2,s1,s2,chi2 =',4F12.4,F12.2)
          DO 910 I=1,2
            IF( IWWloc(I) <= 0 ) CYCLE
            DO IP=1,NLTUB(JMDT(I))
              DO K=1,4
                IT = JCADO(I,IK,IP,K)
                IF( IT >= 1 ) THEN
                  JCAPRI(IP,K) = JTUBHI(IT)
                ELSEIF( IT <= -1 .AND. IT >= -NTUBHT ) THEN
                  JCAPRI(IP,K) = - JTUBHI(-IT)
                ELSE
                  JCAPRI(IP,K) = IT
                ENDIF
              ENDDO
            ENDDO
            WRITE(*,'(4I10,2(I15,3I10))') (JCAPRI(IP,1:4),IP=1,NLTUB(JMDT(I)))
910       ENDDO
911     ENDDO
      ENDIF
!
990 ENDDO
!
 END SUBROUTINE DROITU
!
