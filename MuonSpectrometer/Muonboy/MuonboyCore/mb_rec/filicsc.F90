!
!> Reconstruct straight track segments in CSC stations of type ISC 
!> \author A.Ouraou
!
 SUBROUTINE FILICSC(ISC, MACADO,NDIM1, JFLAGI,JFLAGO, NBDRIT, segm )
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS, ONLY : CP0000, C1P000, Pi
 USE M_MB_MuGeom
 USE M_MB_Types
 USE M_MB_Fit, ONLY : ICC_CSC, IWW_CSC, TTT_CSC
 IMPLICIT NONE
!
!-- Find straight track segments in CSC stations of type ISC
!-- Part dealing with two chambers is obsolete !! transformations may be differents,
!-- so they must be done when fitting the segment ... not done waiting for the installation
!-- of the second CSC (will never happen !!)
!--  When fitting in only one chamber, the transformation is done at the end
!
 INTEGER      , INTENT(IN)    :: ISC, MACADO, NDIM1
 INTEGER      , INTENT(OUT)   :: NBDRIT
 INTEGER      , INTENT(INOUT) :: JFLAGI(*), JFLAGO(*)
 TYPE(Segment), INTENT(OUT)   :: segm(NDIM1)
 TYPE(DCApoint) :: dcapoin
 INTEGER :: IC1, IC2, J, IC, IW, IMDT, IP, ITDEB, ITFIN, IT, Jcsc, intpos
 INTEGER :: IW1, INOF1, INOZ1, IGEOM1, IMDT1, ITDEB1, ITFIN1, IP1, ITZ1, ITS1
 INTEGER :: IW2, INOF2, INOZ2, IGEOM2, IMDT2, ITDEB2, ITFIN2, IP2, ITZ2, ITS2
 INTEGER :: ITZ1_m, ITZ2_m, ITS1_m, ITS2_m, NTOT1, NTOT2, NTOT
 INTEGER :: ITZ1_s, ITZ2_s, ITS1_s, ITS2_s
 INTEGER :: I, ICI, IWI, INOFI, INOZI, IGEOMI, IMDTI
 INTEGER :: JFLAG_POS, JFLAG_NEG, INOF, INOZ, K, ITT, Itrans, ngoodz
 REAL(8) :: F01, G01, T01, DIREC, F02, G02, T02, CA0, SA0
 REAL(8) :: CTOT_m, Z1_m, Z2_m, S1_m, S2_m, TH1, ZH1, SH1, TH2, ZH2, SH2
 REAL(8) :: CTOT_s, Z1_s, Z2_s, S1_s, S2_s
 REAL(8) :: SL1, ZL1, TL1, SL2, ZL2, TL2
 REAL(8) :: ZH01, SH01, ZH02, SH02, CTOT, CTOT1, CTOT2, PENA, PENA_m, PENA_s
 REAL(8) :: F0I, G0I, T0I
 REAL(8) :: VZ,VT,VS, CSCMIN, ZP,ZM, SP,SM
 REAL(8) :: ZH0I, SH0I, ZFITM, SFITM, cosi, cosj, angi, angj
 REAL(8) :: V3(3), ZFIT(2), SFIT(2), ZCOR(2), SCOR(2), TCOR(2)
 REAL(8) :: VZ_s,VT_s,VS_s, ZFIT_s(2), SFIT_s(2), CSCMIN_s
 REAL(8) :: ss, zz, tt, ds, dz, dt
 LOGICAL :: reject(NDIM1)
 LOGICAL :: NOKCHA
!-----------------> @@@@@@@@@@@@@@@@@@@@@@@@@
 REAL(8), PARAMETER :: DZTOL = 3.D0, DSTOL = 4.D0
 REAL(8), PARAMETER :: CHIMA_CSC = 100.D0
 REAL(8), PARAMETER :: CTOT_ok = 0.10D0
 REAL(8), PARAMETER :: DTT_CSC = 10.D0
 REAL(8), PARAMETER :: tolz = 0.d0, tols = 0.d0
 REAL(8), PARAMETER :: angcut = 12.d0, dangcut = 8.d0
!-----------------> @@@@@@@@@@@@@@@@@@@@@@@@@
!
    NBDRIT = 0
    IF( ISC <= NMATTR ) RETURN
!
    IC1 = 2*ISC
    IC2 = 2*ISC - 1
    ICC_CSC(1) = IC1
    ICC_CSC(2) = IC2
! Fill the temporary arrays :
!  JFLAGI=1 for Z, 2 for S
!  JFLAGO= nber of allowed segments using this hit
    DO J=1,2
      IC = ICC_CSC(J)
      DO IW=1,NCHAHI(IC)
        IMDT = ISTMDTV( IGEOMA(IC,IFCHA(IC,IW),IZCHA(IC,IW)) )
        DO IP=1,NLTUB(IMDT)
          ITDEB = NTUBH0(IC,IW,IP) + 1
          ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
          DO IT=ITDEB,ITFIN
            IF( RTUBHI(IT) < -10.D0 ) THEN   !>> S reconstructed position !!!
              JFLAGI(IT) = 2
              JFLAGO(IT) = 2
            ELSEIF( RTUBHI(IT) < 0.D0 ) THEN !>> Z reconstructed position !!!
              JFLAGI(IT) = 1
              JFLAGO(IT) = 2
              IF( LTUBHI(IT) == 0 .OR. LTUBHI(IT) == 10 ) JFLAGO(IT) = 1
            ELSE                             !>> Others !!!
              JFLAGI(IT) = 0
              JFLAGO(IT) = 0
            ENDIF
          ENDDO
        ENDDO
      ENDDO
    ENDDO
!
!    the code with 2 ML is obsolete
!    
    GO TO 250
!
!>> --------------------------------------------------------------------------------
!>> Straight track segments search between multilayer IC1,IW1 and multilayer IC2,IW2
    DO 201 IW1=1,NCHAHI(IC1)
      INOF1  = IFCHA(IC1,IW1)
      INOZ1  = IZCHA(IC1,IW1)
      IGEOM1 = IGEOMA( IC1, INOF1, INOZ1 )
      IMDT1  = ISTMDTV(IGEOM1)
      F01    = FCECHAV(IGEOM1)
      G01    = GCECHAV(IGEOM1)
      T01    = TCECHAV(IGEOM1)
      DIREC  = SIGN( C1P000, T01 )
      IWW_CSC(1) = IW1
      TTT_CSC(1) = T01
      DO 200 IW2=1,NCHAHI(IC2)
        INOF2  = IFCHA(IC2,IW2)
        INOZ2  = IZCHA(IC2,IW2)
        IGEOM2 = IGEOMA( IC2, INOF2, INOZ2 )
        IMDT2  = ISTMDTV(IGEOM2)
        F02    = FCECHAV(IGEOM2)
        G02    = GCECHAV(IGEOM2)
        T02    = TCECHAV(IGEOM2)
        IWW_CSC(2) = IW2
        TTT_CSC(2) = T02
!-- The two multi-layers must be in the same phi position
        IF( ABS(F01-F02) > 0.01D0 ) CYCLE
!-- With the same tilt
        IF( ABS(G01-G02) > 0.01D0 ) CYCLE
!-- The two multi-layers must be in the same end cap
        IF( T01*T02 < 0.D0 )        CYCLE
        CA0 = COS(G01)
        SA0 = SIN(G01)
#if !defined(ATLAS_NOPRINTMU)
        IF( MBPri >= 3 ) THEN
          PRINT 7771,CARISC(ISC),IC1,INOF1,INOZ1,IC2,INOF2,INOZ2
7771      FORMAT(' In FILICSC : CARISC,Ic,Inof,Inoz-1-2 = ',3X,A3,3X,6I5)
        ENDIF
#endif
!
100     CTOT_m = 999999999.D0
        ITZ1_m = 0
        ITZ2_m = 0
        ITS1_m = 0
        ITS2_m = 0
        PENA_m = 0.D0
        Z1_m   = 0.D0
        Z2_m   = 0.D0
        S1_m   = 0.D0
        S2_m   = 0.D0
!-- Loop on all the layers of multilayer 1
        DO 151 IP1=1,NLTUB(IMDT1)
          TH1 = T01 + DIREC * (TLTUB(IMDT1,IP1,1)+TLTUB(IMDT1,IP1,2))/2.d0
          ITDEB1 = NTUBH0(IC1,IW1,IP1) + 1
          ITFIN1 = NTUBH0(IC1,IW1,IP1) + NTUBHI(IC1,IW1,IP1)
!-- Loop on all the Z hits of this layer
          DO 150 ITZ1=ITDEB1,ITFIN1
            IF( JFLAGI(ITZ1) /= 1 ) CYCLE
            ZH1 = ZTUBHI(ITZ1)
!-- Loop on all the layers of multilayer 2
            DO 141 IP2=1,NLTUB(IMDT2)
              TH2 = T02 + DIREC * (TLTUB(IMDT2,IP2,1)+TLTUB(IMDT2,IP2,2))/2.d0
              ITDEB2 = NTUBH0(IC2,IW2,IP2) + 1
              ITFIN2 = NTUBH0(IC2,IW2,IP2) + NTUBHI(IC2,IW2,IP2)
!-- Loop on all the Z hits of this layer
              DO 140 ITZ2=ITDEB2,ITFIN2
                IF( JFLAGI(ITZ2) /= 1 )             CYCLE
                ZH2 = ZTUBHI(ITZ2)
                IF( ABS(ZH2-ZH1*TH2/TH1) > DZTOL )  CYCLE
                IF( ABS(ZH2-ZH1) > ABS(TH2-TH1) )   CYCLE
!
!-- Loop on all the coresponding S hits of the first layer
                DO 130 ITS1=ITDEB1,ITFIN1
                  IF( JFLAGI(ITS1) /= 2 )             CYCLE
                  SH1 = STUBHI(ITS1)
                  IF( NOKCHA(IC1,INOZ1,IGEOM1,ZH1,SH1,tolz,tols) ) CYCLE
!-- Loop on all the coresponding S hits of the second layer
                  DO 120 ITS2=ITDEB2,ITFIN2
                    IF( JFLAGI(ITS2) /= 2 )             CYCLE
                    SH2 = STUBHI(ITS2)
                    IF( ABS(SH2-SH1*TH2/TH1) > DSTOL )  CYCLE
                    IF( ABS(SH2-SH1) > ABS(TH2-TH1) )   CYCLE
                    IF( NOKCHA(IC2,INOZ2,IGEOM2,ZH2,SH2,tolz,tols) ) CYCLE
                    V3(1:3) = (/ ZH2-ZH1, TH2-TH1, SH2-SH1 /)
                    CALL NORM3V( V3 )
                    CALL U3V( V3,    VZ,      VT,      VS   )
                    ZH01 = ZH1 + (T01-TH1) * VZ/VT
                    SH01 = SH1 + (T01-TH1) * VS/VT
                    CALL CHICSC(.true., IC1,INOF1,INOZ1, ZH01,SH01, VZ,VT,VS, NTOT1,CTOT1, dcapoin)
                    ZH02 = ZH2 + (T02-TH2) * VZ/VT
                    SH02 = SH2 + (T02-TH2) * VS/VT
                    CALL CHICSC(.true., IC2,INOF2,INOZ2, ZH02,SH02, VZ,VT,VS, NTOT2,CTOT2, dcapoin)
                    CTOT = CTOT1 + CTOT2
! add small penalty for non pointing segments
                    PENA = MAX( 0.D0 , 1.D0-(VT*T01+VZ*ZH01+VS*SH01)**2/(T01**2+ZH01**2+SH01**2) )
                    PENA = 10.D0 * SQRT( PENA )
                    IF( CTOT + PENA < CTOT_m + PENA_m ) THEN
                      ITZ1_m = ITZ1
                      ITZ2_m = ITZ2
                      ITS1_m = ITS1
                      ITS2_m = ITS2
                      PENA_m = PENA
                      Z1_m   = ZH01
                      Z2_m   = ZH02
                      S1_m   = SH01
                      S2_m   = SH02
                      CTOT_m = CTOT
                      IF( CTOT_m <= CTOT_ok ) GO TO 160
                    ENDIF
120               ENDDO
130             ENDDO
!
140           ENDDO
141         ENDDO
150       ENDDO
151     ENDDO
!
160     CONTINUE
        IF( MBPri >= 3 ) THEN
           PRINT 7772,CTOT_m,ITZ1_m,ITZ2_m,ITS1_m,ITS2_m,Z1_m,Z2_m,S1_m,S2_m
7772       FORMAT('      Chi2,IT_z1_z2_s1_s2,Z1,Z2,S1,S2 =',F14.3,4I5,4F12.4)
        ENDIF
!
        IF( CTOT_m < 9.1D0*CHIMA_CSC ) THEN
          ngoodz = 2
          CALL fitSegmentCSC( Z1_m,Z2_m,S1_m,S2_m, CSCMIN, ZP,ZM,SP, ngoodz )
          SM = 0.d0
          ZFIT(1) = (ZP+ZM) * 0.5d0
          ZFIT(2) = (ZP-ZM) * 0.5d0
          SFIT(1) = (SP+SM) * 0.5d0  ! a revoir  pas a jour
          SFIT(2) = (SP-SM) * 0.5d0
          IF( MBPri >= 3 ) THEN
            PRINT 7773,CSCMIN,ZFIT(1),ZFIT(2),SFIT(1),SFIT(2)
7773        FORMAT('  After fit :        Chi2,Z1,Z2,S1,S2 =',F14.3,20X,4F12.4)
          ENDIF
          IF( CSCMIN < 6.1D0*CHIMA_CSC ) THEN
            V3(1:3) = (/ ZFIT(2)-ZFIT(1), TTT_CSC(2)-TTT_CSC(1), SFIT(2)-SFIT(1) /)
            CALL NORM3V( V3 )
            CALL U3V( V3,        VZ,          VT,          VS       )
            NBDRIT = NBDRIT + 1
            segm(NBDRIT)%chi2 = CSCMIN
            segm(NBDRIT)%phi0 = F01
            segm(NBDRIT)%alfa = G01
            segm(NBDRIT)%jff  = INOF1
            segm(NBDRIT)%nu   = 0
            segm(NBDRIT)%no   = 0
            segm(NBDRIT)%nh   = 0
            JFLAG_POS = 0
            JFLAG_NEG = 0
            ngoodz = 0
            DO 172 J=1,2
              segm(NBDRIT)%iw(J) = IWW_CSC(J)
              segm(NBDRIT)%is(J) = 0
              segm(NBDRIT)%spos(J) = SFIT(J)
              segm(NBDRIT)%zpos(J) = - SA0*ZFIT(J) + CA0*TTT_CSC(J)
              segm(NBDRIT)%tpos(J) =   CA0*ZFIT(J) + SA0*TTT_CSC(J)
              IC   = ICC_CSC(J)
              INOF = IFCHA(IC,IWW_CSC(J))
              INOZ = IZCHA(IC,IWW_CSC(J))
              IMDT = ISTMDTV( IGEOMA(IC,INOF,INOZ) )
              CALL CHICSC(.true., IC,INOF,INOZ, ZFIT(J),SFIT(J), VZ,VT,VS, NTOT,CTOT, dcapoin)
              DO IP=1,NLTUB(IMDT)
                DO K=1,2
                  ITT = dcapoin%III(IP,K)
                  IF( ITT >= 1 ) THEN
                    IF( LTUBHI(ITT) == 0 .OR. LTUBHI(ITT) == 10 ) ngoodz = ngoodz + 1
                    CALL AddHitToSeg( IP, K, dcapoin, segm(NBDRIT) )
                    IF( JFLAGI(ITT) >= 1 ) THEN
                      JFLAG_POS = JFLAG_POS + 1
                      JFLAGI(ITT) = - JFLAGI(ITT)
                    ELSE
                      JFLAG_NEG = JFLAG_NEG + 1
                    ENDIF
                  ELSE IF( dcapoin%JJJ(IP,K) <= -20000000 ) THEN
                    CALL AddHoleToSeg( IP, K, dcapoin, segm(NBDRIT) )
                  ENDIF
                ENDDO
              ENDDO
172         ENDDO
            segm(NBDRIT)%errz = 2.d0 * RESTUB(IMDT1)
            IF( JFLAG_NEG > JFLAG_POS ) NBDRIT = NBDRIT - 1
            IF( NBDRIT >= MACADO .OR. NBDRIT >= NDIM1 ) GO TO 900
          ENDIF
          IF( JFLAGI(ITZ1_m) >= 1 ) JFLAGI(ITZ1_m) = - JFLAGI(ITZ1_m)
          IF( JFLAGI(ITZ2_m) >= 1 ) JFLAGI(ITZ2_m) = - JFLAGI(ITZ2_m)
          IF( JFLAGI(ITS1_m) >= 1 ) JFLAGI(ITS1_m) = - JFLAGI(ITS1_m)
          IF( JFLAGI(ITS2_m) >= 1 ) JFLAGI(ITS2_m) = - JFLAGI(ITS2_m)
          GO TO 100
        ENDIF
!
200   ENDDO
201 ENDDO
!
!----------------------------------------------------------------------------------
250 CONTINUE
!
!-- One goes on (with the remaining hits) with straight track segments search in a
!-- single multi-layer ICI,IWI
    DO 500 I=1,2
      IWW_CSC(1:2) = 0
      ICI = ICC_CSC(I)
      DO 400 IWI=1,NCHAHI(ICI)
        IWW_CSC(I) = IWI
        INOFI  = IFCHA(ICI,IWI)
        INOZI  = IZCHA(ICI,IWI)
        IGEOMI = IGEOMA( ICI, INOFI, INOZI )
        IMDTI  = ISTMDTV(IGEOMI)
        Itrans = ItransCHA(IGEOMI)
        Jcsc   = intpos(IGEOMI,IGEOMCSC,NChaCSC)
        F0I    = FCECHAV(IGEOMI)
        G0I    = GCECHAV(IGEOMI)
        T0I    = TCECHAV(IGEOMI)
        DIREC  = SIGN( C1P000, T0I )
        TTT_CSC(1) = T0I + DIREC*DTT_CSC
        TTT_CSC(2) = T0I - DIREC*DTT_CSC
        CA0 = COS(G0I)
        SA0 = SIN(G0I)
        IF( MBPri >= 3 ) THEN
          PRINT 7771,CARISC(ISC),ICI,INOFI,INOZI
        ENDIF
!
300     CTOT_m = 999999999.D0
        ITZ1_m = 0
        ITZ2_m = 0
        ITS1_m = 0
        ITS2_m = 0
        PENA_m = 0.D0
        Z1_m   = 0.D0
        Z2_m   = 0.D0
        S1_m   = 0.D0
        S2_m   = 0.D0
        CTOT_s = 999999999.D0
        ITZ1_s = 0
        ITZ2_s = 0
        ITS1_s = 0
        ITS2_s = 0
        PENA_s = 0.D0
        Z1_s   = 0.D0
        Z2_s   = 0.D0
        S1_s   = 0.D0
        S2_s   = 0.D0
!-- Loop on all (but the last) layers of multilayer I
        DO 351 IP1=1,NLTUB(IMDTI)-1
          TL1 = T0I + DIREC * (TLTUB(IMDTI,IP1,1)+TLTUB(IMDTI,IP1,2))/2.d0
          ITDEB1 = NTUBH0(ICI,IWI,IP1) + 1
          ITFIN1 = NTUBH0(ICI,IWI,IP1) + NTUBHI(ICI,IWI,IP1)
!-- Loop on all the Z hits of this layer
          DO 350 ITZ1=ITDEB1,ITFIN1
            IF( JFLAGI(ITZ1) /= 1 )                    CYCLE
            ZL1 = ZTUBHI(ITZ1)
!-- Loop on all the other layers of multilayer I
            DO 341 IP2=IP1+1,NLTUB(IMDTI)
              TL2 = T0I + DIREC * (TLTUB(IMDTI,IP2,1)+TLTUB(IMDTI,IP2,2))/2.d0
              ITDEB2 = NTUBH0(ICI,IWI,IP2) + 1
              ITFIN2 = NTUBH0(ICI,IWI,IP2) + NTUBHI(ICI,IWI,IP2)
!-- Loop on all the Z hits of this layer
              DO 340 ITZ2=ITDEB2,ITFIN2
                IF( JFLAGI(ITZ2) /= 1 )                  CYCLE
                ZL2 = ZTUBHI(ITZ2)
                IF( ABS(ZL2-ZL1*TL2/TL1) > DZTOL/2.D0 )  CYCLE
                IF( ABS(ZL2-ZL1) > ABS(TL2-TL1) )        CYCLE
!-- do not allow to start with two hits altready used by a segment
                IF( LTUBHI(ITZ1) /= 0 .AND. LTUBHI(ITZ1) /= 10 ) THEN
                  IF( LTUBHI(ITZ2) /= 0 .AND. LTUBHI(ITZ2) /= 10 ) THEN
                    IF( JFLAGO(ITZ1) < 2 .AND. JFLAGO(ITZ2) < 2 ) CYCLE
                  ENDIF
                ENDIF
!
!-- Loop on all the coresponding S hits of the first layer
                DO 330 ITS1=ITDEB1,ITFIN1
                  IF( JFLAGI(ITS1) /= 2 )                  CYCLE
                  SL1 = STUBHI(ITS1)
                  IF( NOKCHA(ICI,INOZI,IGEOMI,ZL1,SL1,tolz,tols) ) CYCLE
!-- Loop on all the coresponding S hits of the second layer
                  DO 320 ITS2=ITDEB2,ITFIN2
                    IF( JFLAGI(ITS2) /= 2 )                  CYCLE
                    SL2 = STUBHI(ITS2)
                    IF( ABS(SL2-SL1*TL2/TL1) > DSTOL )       CYCLE
                    IF( ABS(SL2-SL1) > ABS(TL2-TL1) )        CYCLE
                    IF( NOKCHA(ICI,INOZI,IGEOMI,ZL2,SL2,tolz,tols) ) CYCLE
                    CALL Pos_Csc_from_Layer(Jcsc,IP1,SL1,ZL1,TL1,SH1,ZH1,TH1)
                    CALL Pos_Csc_from_Layer(Jcsc,IP2,SL2,ZL2,TL2,SH2,ZH2,TH2)
                    V3(1:3) = (/ ZH2-ZH1, TH2-TH1, SH2-SH1 /)
!-- force segments to be pointing in S as no precision
                    SH0I  = 0.5d0 * ( SH1/TH1 + SH2/TH2 ) * T0I
                    V3(3) = (TH2-TH1) * SH0I / T0I
                    CALL NORM3V( V3 )
                    CALL U3V( V3, VZ,VT,VS )
                    ZH0I = ZH1 + (T0I-TH1) * VZ/VT
!!!                 SH0I = SH1 + (T0I-TH1) * VS/VT
                    CALL CHICSC(.true., ICI,INOFI,INOZI, ZH0I,SH0I, VZ,VT,VS, NTOT,CTOT, dcapoin)
! add small penalty for non pointing segments
                    PENA = MAX( 0.D0 , 1.D0-(VT*T0I+VZ*ZH0I+VS*SH0I)**2/(T0I**2+ZH0I**2+SH0I**2) )
                    PENA = 10.D0 * SQRT( PENA )
                    IF( CTOT + PENA < CTOT_m + PENA_m ) THEN
                      ITZ1_s = ITZ1_m
                      ITZ2_s = ITZ2_m
                      ITS1_s = ITS1_m
                      ITS2_s = ITS2_m
                      Z1_s   = Z1_m
                      Z2_s   = Z2_m
                      S1_s   = S1_m
                      S2_s   = S2_m
                      PENA_s = PENA_m
                      CTOT_s = CTOT_m
                      ITZ1_m = ITZ1
                      ITZ2_m = ITZ2
                      ITS1_m = ITS1
                      ITS2_m = ITS2
                      Z1_m   = ZH0I + (TTT_CSC(1)-T0I)*VZ/VT
                      Z2_m   = ZH0I + (TTT_CSC(2)-T0I)*VZ/VT
                      S1_m   = SH0I + (TTT_CSC(1)-T0I)*VS/VT
                      S2_m   = SH0I + (TTT_CSC(2)-T0I)*VS/VT
                      PENA_m = PENA
                      CTOT_m = CTOT
                      IF( CTOT_s <= CTOT_ok )  GO TO 360
                    ELSE IF( CTOT + PENA < CTOT_s + PENA_s ) THEN
                      ITZ1_s = ITZ1
                      ITZ2_s = ITZ2
                      ITS1_s = ITS1
                      ITS2_s = ITS2
                      Z1_s   = ZH0I + (TTT_CSC(1)-T0I)*VZ/VT
                      Z2_s   = ZH0I + (TTT_CSC(2)-T0I)*VZ/VT
                      S1_s   = SH0I + (TTT_CSC(1)-T0I)*VS/VT
                      S2_s   = SH0I + (TTT_CSC(2)-T0I)*VS/VT
                      PENA_s = PENA
                      CTOT_s = CTOT
                      IF( CTOT_s <= CTOT_ok )  GO TO 360
                    ENDIF
320               ENDDO
330             ENDDO
!
340           ENDDO
341         ENDDO
350       ENDDO
351     ENDDO
!
360     CONTINUE
        IF( MBPri >= 3 ) THEN
          PRINT 7776,CTOT_m,Pena_m,ITZ1_m,ITZ2_m,ITS1_m,ITS2_m,Z1_m,Z2_m,S1_m,S2_m  &
                    ,CTOT_s,Pena_s,ITZ1_s,ITZ2_s,ITS1_s,ITS2_s,Z1_s,Z2_s,S1_s,S2_s
7776      FORMAT('      Chi2,Pena,IT_z1_z2_s1_s2,Z1,Z2,S1,S2 =',F14.3,F6.3,4I5,4F12.4,/,   &
                 ' next-Chi2,Pena,IT_z1_z2_s1_s2,Z1,Z2,S1,S2 =',F14.3,F6.3,4I5,4F12.4)
        ENDIF
!
        IF( CTOT_m < 3.1D0*CHIMA_CSC .OR. (CTOT_m < 4.6D0*CHIMA_CSC .AND. Isingle(IGEOMI) == 0) ) THEN
          ngoodz = 0
          IF( LTUBHI(ITZ1_m) == 0 .OR. LTUBHI(ITZ1_m) == 10 ) ngoodz = ngoodz + 1
          IF( LTUBHI(ITZ2_m) == 0 .OR. LTUBHI(ITZ2_m) == 10 ) ngoodz = ngoodz + 1
          CALL fitSegmentCSC( Z1_m,Z2_m,S1_m,S2_m, CSCMIN, ZP,ZM,SP, ngoodz )
          ZFIT(1) = (ZP+ZM) * 0.5d0
          ZFIT(2) = (ZP-ZM) * 0.5d0
!         SFIT(1) = (SP+SM) * 0.5d0
!         SFIT(2) = (SP-SM) * 0.5d0
          SFIT(1) = 0.5d0 * SP * TTT_CSC(1) / T0I
          SFIT(2) = 0.5d0 * SP * TTT_CSC(2) / T0I
          V3(1:3) = (/ ZFIT(2)-ZFIT(1), TTT_CSC(2)-TTT_CSC(1), SFIT(2)-SFIT(1) /)
          CALL NORM3V( V3 )
          CALL U3V( V3, VZ,VT,VS )
          ZH0I = 0.5d0 * ( ZFIT(1) + ZFIT(2) )
          SH0I = 0.5d0 * ( SFIT(1) + SFIT(2) )
! add small penalty for non pointing segments
          PENA = MAX( 0.D0 , 1.D0-(VT*T0I+VZ*ZH0I+VS*SH0I)**2/(T0I**2+ZH0I**2+SH0I**2) )
          PENA = 10.D0 * SQRT( PENA )
          IF( MBPri >= 3 ) THEN
            PRINT 7777,CSCMIN,PENA,ZFIT(1),ZFIT(2),SFIT(1),SFIT(2)
7777        FORMAT('  After fit :        Chi2,Pena,Z1,Z2,S1,S2 =',F14.3,F6.3,20X,4F12.4)
          ENDIF
          IF( CTOT_s < 3.1D0*CHIMA_CSC .OR. (CTOT_s < 4.6D0*CHIMA_CSC .AND. Isingle(IGEOMI) == 0) ) THEN
            ngoodz = 0
            IF( LTUBHI(ITZ1_s) == 0 .OR. LTUBHI(ITZ1_s) == 10 ) ngoodz = ngoodz + 1
            IF( LTUBHI(ITZ2_s) == 0 .OR. LTUBHI(ITZ2_s) == 10 ) ngoodz = ngoodz + 1
            CALL fitSegmentCSC( Z1_s,Z2_s,S1_s,S2_s, CSCMIN_s, ZP,ZM,SP, ngoodz )
            ZFIT_s(1) = (ZP+ZM) * 0.5d0
            ZFIT_s(2) = (ZP-ZM) * 0.5d0
!           SFIT_s(1) = (SP+SM) * 0.5d0
!           SFIT_s(2) = (SP-SM) * 0.5d0
            SFIT_s(1) = 0.5d0 * SP * TTT_CSC(1) / T0I
            SFIT_s(2) = 0.5d0 * SP * TTT_CSC(2) / T0I
            V3(1:3) = (/ ZFIT_s(2)-ZFIT_s(1), TTT_CSC(2)-TTT_CSC(1), SFIT_s(2)-SFIT_s(1) /)
            CALL NORM3V( V3 )
            CALL U3V( V3, VZ_s,VT_s,VS_s )
            ZH0I = 0.5d0 * ( ZFIT_s(1) + ZFIT_s(2) )
            SH0I = 0.5d0 * ( SFIT_s(1) + SFIT_s(2) )
! add small penalty for non pointing segments
            PENA_s = MAX( 0.D0 , 1.D0-(VT_s*T0I+VZ_s*ZH0I+VS_s*SH0I)**2/(T0I**2+ZH0I**2+SH0I**2) )
            PENA_s = 10.D0 * SQRT( PENA_s )
            IF( MBPri >= 3 ) THEN
              PRINT 7778,CSCMIN_s,PENA_s,ZFIT_s(1),ZFIT_s(2),SFIT_s(1),SFIT_s(2)
7778          FORMAT('  After fit :   next-Chi2,Pena,Z1,Z2,S1,S2 =',F14.3,F6.3,20X,4F12.4)
            ENDIF
            IF( CSCMIN_s + PENA_s < CSCMIN + PENA ) THEN
              CSCMIN = CSCMIN_s
              ZFIT(1:2) = ZFIT_s(1:2)
              SFIT(1:2) = SFIT_s(1:2)
              VZ = VZ_s
              VT = VT_s
              VS = VS_s
              ITZ1_m = ITZ1_s
              ITZ2_m = ITZ2_s
              ITS1_m = ITS1_s
              ITS2_m = ITS2_s
            ENDIF
          ENDIF
          IF( CSCMIN < 2.1D0*CHIMA_CSC .OR. (CSCMIN < 3.1D0*CHIMA_CSC .AND. Isingle(IGEOMI) == 0) ) THEN
            CALL Pos_True_From_Ideal(Itrans, SFIT(1),ZFIT(1),TTT_CSC(1), SCOR(1),ZCOR(1),TCOR(1))
            CALL Pos_True_From_Ideal(Itrans, SFIT(2),ZFIT(2),TTT_CSC(2), SCOR(2),ZCOR(2),TCOR(2))
            NBDRIT = NBDRIT + 1
            segm(NBDRIT)%chi2 = CSCMIN + CHIMA_CSC
            segm(NBDRIT)%phi0 = F0I
            segm(NBDRIT)%alfa = G0I
            segm(NBDRIT)%jff  = INOFI
            segm(NBDRIT)%nu   = 0
            segm(NBDRIT)%no   = 0
            segm(NBDRIT)%nh   = 0
            JFLAG_POS = 0
            JFLAG_NEG = 0
            segm(NBDRIT)%iw(1:2) = IWW_CSC(1:2)
            segm(NBDRIT)%is(1:2) = 0
            segm(NBDRIT)%spos(1:2) = SCOR(1:2)
            segm(NBDRIT)%zpos(1:2) = - SA0*ZCOR(1:2) + CA0*TCOR(1:2)
            segm(NBDRIT)%tpos(1:2) =   CA0*ZCOR(1:2) + SA0*TCOR(1:2)
            ZFITM = (ZFIT(1)+ZFIT(2)) * 0.5d0
            SFITM = (SFIT(1)+SFIT(2)) * 0.5d0
            saveDcaSeg = .true.
            CALL CHICSC(.true., ICI,INOFI,INOZI, ZFITM,SFITM, VZ,VT,VS, NTOT,CTOT, dcapoin)
            saveDcaSeg = .false.
            ngoodz = 0
            DO IP=1,NLTUB(IMDTI)
              DO K=1,2
                ITT = dcapoin%III(IP,K)
                IF( ITT >= 1 ) THEN
                  IF( LTUBHI(ITT) == 0 .OR. LTUBHI(ITT) == 10 ) ngoodz = ngoodz + 1
                  CALL AddHitToSeg( IP, K, dcapoin, segm(NBDRIT) )
                  IF( JFLAGI(ITT) > 0 ) THEN
                    JFLAG_POS = JFLAG_POS + 1
                    JFLAGO(ITT) = JFLAGO(ITT) - 1
                    IF( JFLAGO(ITT) == 0 ) JFLAGI(ITT) = 0
                    IF( K == 1 ) THEN
                      IF( ITT == ITZ1_m ) ITZ1_m = 0
                      IF( ITT == ITZ2_m ) ITZ2_m = 0
                    ELSE
                      IF( ITT == ITS1_m ) ITS1_m = 0
                      IF( ITT == ITS2_m ) ITS2_m = 0
                    ENDIF
                  ELSE
                    JFLAG_NEG = JFLAG_NEG + 1
                  ENDIF
                ELSE IF( dcapoin%JJJ(IP,K) <= -20000000 ) THEN
                  CALL AddHoleToSeg( IP, K, dcapoin, segm(NBDRIT) )
                ENDIF
              ENDDO
            ENDDO
            IF( ngoodz >= 4 ) THEN
              segm(NBDRIT)%errz = 2.d0 * RESTUB(IMDTI)
            ELSE IF( ngoodz == 3 ) THEN
              segm(NBDRIT)%errz = 3.d0 * RESTUB(IMDTI)
            ELSE IF( ngoodz == 2 ) THEN
              segm(NBDRIT)%errz = 6.d0 * RESTUB(IMDTI)
            ELSE
              segm(NBDRIT)%errz = 0.25d0 * PASTUB(IMDTI)
            ENDIF
            IF( JFLAG_NEG > JFLAG_POS ) NBDRIT = NBDRIT - 1
            IF( NBDRIT >= MACADO .OR. NBDRIT >= NDIM1 ) GO TO 900
          ENDIF
          IF( ITZ1_m > 0 ) THEN
            JFLAGO(ITZ1_m) = JFLAGO(ITZ1_m) - 1
            IF( JFLAGO(ITZ1_m) == 0 ) JFLAGI(ITZ1_m) = 0
          ENDIF
          IF( ITZ2_m > 0 ) THEN
            JFLAGO(ITZ2_m) = JFLAGO(ITZ2_m) - 1
            IF( JFLAGO(ITZ2_m) == 0 ) JFLAGI(ITZ2_m) = 0
          ENDIF
          IF( ITS1_m > 0 ) THEN
            JFLAGO(ITS1_m) = JFLAGO(ITS1_m) - 1
            IF( JFLAGO(ITS1_m) == 0 ) JFLAGI(ITS1_m) = 0
          ENDIF
          IF( ITS2_m > 0 ) THEN
            JFLAGO(ITS2_m) = JFLAGO(ITS2_m) - 1
            IF( JFLAGO(ITS2_m) == 0 ) JFLAGI(ITS2_m) = 0
          ENDIF
          GO TO 300
        ENDIF
!
400   ENDDO
500 ENDDO
!
!----------------------------------------------------------------------------------
!
900 CONTINUE
! cleaning of the segments
    reject(1:NBDRIT) = .false.
bi: DO i=1,NBDRIT
      IF( reject(i) ) CYCLE
      ss = segm(i)%spos(1) + segm(i)%spos(2)
      zz = segm(i)%zpos(1) + segm(i)%zpos(2)
      tt = segm(i)%tpos(1) + segm(i)%tpos(2)
      ds = segm(i)%spos(1) - segm(i)%spos(2)
      dz = segm(i)%zpos(1) - segm(i)%zpos(2)
      dt = segm(i)%tpos(1) - segm(i)%tpos(2)
      cosi = abs(ss*ds+zz*dz+tt*dt)/sqrt(ss*ss+zz*zz+tt*tt)/sqrt(ds*ds+dz*dz+dt*dt)
      if( cosi > 1.d0 ) cosi = 1.d0
      angi = acos(cosi) * 180.d0 / Pi
      IF( angi > angcut ) THEN
        reject(i) = .true.
        CYCLE
      ENDIF
bj:   DO j=i+1,NBDRIT
        IF( reject(j) ) CYCLE
        ss = segm(j)%spos(1) + segm(j)%spos(2)
        zz = segm(j)%zpos(1) + segm(j)%zpos(2)
        tt = segm(j)%tpos(1) + segm(j)%tpos(2)
        ds = segm(j)%spos(1) - segm(j)%spos(2)
        dz = segm(j)%zpos(1) - segm(j)%zpos(2)
        dt = segm(j)%tpos(1) - segm(j)%tpos(2)
        cosj = abs(ss*ds+zz*dz+tt*dt)/sqrt(ss*ss+zz*zz+tt*tt)/sqrt(ds*ds+dz*dz+dt*dt)
        if( cosj > 1.d0 ) cosj = 1.d0
        angj = acos(cosj) * 180.d0 / Pi
        IF( angj > angcut ) THEN
          reject(j) = .true.
          CYCLE
        ENDIF
! stricter selection when shared hits
        DO k=1,segm(i)%nu
          IF( intpos(segm(i)%iu(k),segm(j)%iu(1:segm(j)%nu),segm(j)%nu) > 0 ) THEN  ! shared hit found
            IF( MBPri >= 3 ) WRITE(*,'(" segments with shared hit found ",2I4," with angles ",2F7.2)') &
                 i,j,angi,angj
            IF( angi > angj + dangcut ) THEN
              reject(i) = .true.
              IF( MBPri >= 3 ) WRITE(*,'("   segment rejected ",I4)') i
              CYCLE bi
            ELSE IF( angj > angi + dangcut ) THEN
              reject(j) = .true.
              IF( MBPri >= 3 ) WRITE(*,'("   segment rejected ",I4)') j
              CYCLE bj
            ELSE
              EXIT
            ENDIF
          ENDIF
        ENDDO
      ENDDO bj
    ENDDO bi
!
   j = 0
   DO i=1,NBDRIT
     IF( reject(i) ) CYCLE
     j = j + 1
     IF( j == i )    CYCLE
     segm(j) = segm(i)
   ENDDO
   NBDRIT = j
!
!----------------------------------------------------------------------------------
!
   IF( MBPri >= 3 ) THEN
     WRITE(*,'(" AT the end of FILICSC, NBDRIT =",I5)') NBDRIT
     DO i=1,NBDRIT
       PRINT 7775,i,segm(i)%chi2,segm(i)%nu,(segm(i)%iu(k),k=1,segm(i)%nu)
7775   FORMAT(7X,'i,CHI2,NUDRIT(i),IUDRIT(.,i) =',I3,F7.2,I8,3X,8I6)
     ENDDO
   ENDIF
!
 END SUBROUTINE FILICSC
!
!
!
 SUBROUTINE fitSegmentCSC(z1,z2,s1,s2, cscmin, zp,zm,sp, ngoodz)
 USE M_MB_Control, ONLY : MBPri
 IMPLICIT NONE
 INTEGER, PARAMETER   :: liv=60, lv=220
 INTEGER, INTENT(IN)  :: ngoodz
 REAL(8), INTENT(IN)  :: z1, z2, s1, s2
 REAL(8), INTENT(OUT) :: cscmin, zp, zm, sp
 INTEGER, SAVE :: iv(liv)
 REAL(8), SAVE :: v(lv)
 INTEGER :: n, uip(1)
 REAL(8) :: x(3), d(3), urp(1)
 LOGICAL, SAVE :: first=.true.
 EXTERNAL :: fcncsc
!
   IF( first ) THEN
     first = .false.
     call deflt( 2, iv, liv, lv, v)
     v(31) = 1.0d-5    ! afctol absolute tol on f
!    v(32) = 1.0d-7    ! rfctol relative tol on f
     v(32) = 1.0d-4    ! rfctol absolute tol on f
     v(33) = 1.0d-6    ! xctol
     v(34) = 1.0d-8    ! xftol
     IF( MBPri <= 4 ) iv(21) = 0
   ENDIF
   iv( 1) = 12
   iv(17) = 200 ! mxfcal max # of function call
   n    = 3
   x(1) = z1 + z2
   x(2) = z1 - z2
   x(3) = s1 + s2
   if( ngoodz >= 2 ) then
     d(1:2) = 100.d0
   else if( ngoodz == 1 ) then
     d( 1 ) = 20.d0
     d( 2 ) = 5.d0
   else
     d(1:2) = 1.d0
   endif
   d(3)   = 1.d0
   uip(1) = 0
   urp(1) = 0.d0
   call smsno( n, d, x, fcncsc, iv, liv, lv, v, uip, urp, fcncsc ) 
   cscmin = v(10)
   zp = x(1)
   zm = x(2)
   sp = x(3) 
!
 END SUBROUTINE fitSegmentCSC   
!
!
!
 SUBROUTINE fcncsc(n, x, nf, f, uip, urp, ufp)
 USE M_MB_Types
 USE M_MB_Digis
 USE M_MB_Fit, ONLY : ICC_CSC, IWW_CSC, TTT_CSC
 IMPLICIT NONE
 INTEGER :: n, nf, uip(*)
 REAL(8) :: urp(*), x(n), f
 INTEGER :: J, IW, IC, INOF, INOZ, NTOT
 REAL(8) :: ZH(2), SH(2), V3, VZ, VT, VS, CTOT, ZHJ,SHJ
 TYPE(DcaPoint) :: dcapoin
 EXTERNAL :: ufp
!
   f = 0.d0
!
   ZH(1) = (x(1)+x(2)) * 0.5d0
   ZH(2) = (x(1)-x(2)) * 0.5d0
!   SH(1) = (A(3)+A(4)) * 0.5d0
!   SH(2) = (A(3)-A(4)) * 0.5d0
   SH(1) = x(3) * TTT_CSC(1) / ( TTT_CSC(1) + TTT_CSC(2) ) 
   SH(2) = x(3) * TTT_CSC(2) / ( TTT_CSC(1) + TTT_CSC(2) )
!
   V3 = 1.d0 / SQRT( (ZH(2)-ZH(1))**2 + (TTT_CSC(2)-TTT_CSC(1))**2 + (SH(2)-SH(1))**2 )
   VZ =           (ZH(2)-ZH(1)) * V3
   VT = (TTT_CSC(2)-TTT_CSC(1)) * V3
   VS =           (SH(2)-SH(1)) * V3
!
   DO J=1,2
     IW = IWW_CSC(J)
     IF( IW >= 1 ) THEN
       IF(IWW_CSC(1) > 0 .AND. IWW_CSC(2) > 0) THEN
         ZHJ = ZH(J)
         SHJ = SH(J)
       ELSE
         ZHJ = (ZH(1)+ZH(2)) * 0.5d0
         SHJ = (SH(1)+SH(2)) * 0.5d0
       ENDIF
       IC    = ICC_CSC(J)
       INOF  = IFCHA(IC,IW)
       INOZ  = IZCHA(IC,IW)
       CALL CHICSC(.true., IC,INOF,INOZ, ZHJ,SHJ, VZ,VT,VS, NTOT,CTOT, dcapoin)
       f = f + CTOT
     ENDIF
   ENDDO
!
 END SUBROUTINE fcncsc
!
