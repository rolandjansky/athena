!
!> \author M.Virchaux
!
 SUBROUTINE CHIZZZ(IC,INOF,INOZ,ZZtrue,SStrue,VZtrue,VTtrue,VStrue, NTOT,CTOT, dcapoin)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_COSURE
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IC,INOF,INOZ
 REAL(8), INTENT(IN)  :: ZZtrue, SStrue, VZtrue, VTtrue, VStrue
 INTEGER, INTENT(OUT) :: NTOT
 REAL(8), INTENT(OUT) :: CTOT
!
 TYPE(DcaPoint), INTENT(OUT) :: dcapoin
 INTEGER,SAVE :: IGEOM, IMDT, L, ITS, IWS, IP, IA,IB
 INTEGER,SAVE :: Itrans, IZ0LIM, NEGA, NM2000, NINEFF, INOZMI, INOZMA, I
 INTEGER,SAVE :: INOZA, INOZB, IGEOM_M1, ItransA, IGEOM_P1, ItransB, NTUT
 INTEGER,SAVE :: NM200T, NINEFT, IZ, INOZI, IGEOMI, IMDTI, IWI, ItransI
 INTEGER,SAVE :: J, JJJJ, IIII, ITDEB, ITFIN, IT, IZ0, IEndPlu
 REAL(8),SAVE :: TOLZ, TOLS, SIGT, TTtrue, AAZT, ZLIM, ZOLTUB0, TTUT, STUT
 REAL(8),SAVE :: ZTUT, RTAT, ARTAT, RTUT, TUBRES
 REAL(8),SAVE :: RDIG, RRR, SS, ZZ, TT, WZ, WT, UST, SWIZZN,SWIZZP
 REAL(8),SAVE :: ZOLTUB, TWZ, DDDD, RDED, DTEST, CCCINE, RRRMIN, CCCMIN
 REAL(8),SAVE :: SBt,ZBt,TBt
 REAL(8),SAVE :: SSidea(-1:1), ZZidea(-1:1), TTidea(-1:1)
 REAL(8),SAVE :: WZidea(-1:1), WTidea(-1:1), UZTidea(-1:1), USTidea(-1:1)
 REAL(8),SAVE :: SiFFi(-1:1), CoFFi(-1:1)
 REAL(8),SAVE :: SIGZ(-1:1)
 INTEGER, PARAMETER :: MAINEF = 2, MA2000 = 3
 REAL(8), PARAMETER :: WZLIM = 0.85D0
!REAL(8), PARAMETER :: deltaS = 2.0d0
 REAL(8), PARAMETER :: InvdeltaS = 0.5d0
 REAL(8), PARAMETER :: strictTolZ = -0.2d0
 INTEGER :: intpos, JFIRST
 REAL(8) :: RFTfast, CHICUR, FREREC
 LOGICAL :: NOKCHA, NAKCHA, IsTubeDead, IsTubeDisabled
 LOGICAL,SAVE :: NotSure, HasDeadTube
!
 REAL(8),SAVE :: Z1DCA, T1DCA, S1DCA 
 REAL(8),SAVE :: XXXDCA, YYYDCA, ZZZDCA
 REAL(8),SAVE :: XXRDCA, YYRDCA, ZZRDCA, SRDCA
 REAL(8) :: dirx, diry, dirz
!
 REAL(8),SAVE :: RadTrackTest
 REAL(8),SAVE :: RadDigitTest
 LOGICAL,SAVE :: isBarrel
 REAL(8),SAVE :: TWOPASTUB
 REAL(8),SAVE :: NotOnTrack
 REAL(8),SAVE :: InvPASTUB
 REAL(8),SAVE :: InvTUBRES
!
!>> Go to special treatement for CSC's ==============================
   IF( IC > NCOUCH ) THEN
     CALL CHICSC(.false., IC,INOF,INOZ,ZZtrue,SStrue,VZtrue,VTtrue,VStrue, NTOT,CTOT, dcapoin)
     RETURN
   ENDIF
!>> =================================================================
!
   NTOT = 0
   CTOT = 0.D0
   CALL ResetDCA( dcapoin )
!
   IGEOM = IGEOMA(IC,INOF,INOZ)
   IMDT  = ISTMDTV(IGEOM)
!
   isBarrel = IC<=NCOUBA
   TWOPASTUB = 2.d0*PASTUB(IMDT)
   InvPASTUB = PASTUB_inv(IMDT)
!
   SIGZ(:) = 1.d0
   IF( isBarrel ) Then
     IF( INOZ < NZMS2 )  SIGZ(:) = MDTnu
   endif
   TTtrue = TCECHAV(IGEOM)
!>> SIGT < 0 if one is in the "negative" end cap
   SIGT   = SIGN( 1.D0 , TTtrue )
   Itrans = ItransCHA(IGEOM)
   IF( Itrans <= 0 ) THEN
     SSidea(0)  = SStrue
     ZZidea(0)  = ZZtrue
     TTidea(0)  = TTtrue
     AAZT       = SQRT( VZtrue**2 + VTtrue**2 )
     WZidea(0)  = VZtrue / AAZT
     WTidea(0)  = VTtrue / AAZT
     UZTidea(0) = VZtrue / VTtrue
     USTidea(0) = VStrue / VTtrue
   ELSE
     CALL SZTWU_Ideal_from_True(Itrans, SStrue,ZZtrue,TTtrue,VStrue,VZtrue,VTtrue,    &
                      SSidea(0),ZZidea(0),TTidea(0),WZidea(0),WTidea(0),UZTidea(0),USTidea(0))
   ENDIF
   SiFFi(0) = SINCHAV(IGEOM)
   CoFFi(0) = COSCHAV(IGEOM)
   CALL MuboSZTtoXYZ( isBarrel,SiFFi(0),CoFFi(0), VStrue,VZtrue,VTtrue, dirx,diry,dirz)
!
   NotSure = .true.
   DO L=IFSure(IC),NZSure
     IF( ICSure(L) /= IC )            EXIT
     IWS = IWSure(L)
     IF( IFCHA(IC,IWS) /= INOF )      CYCLE
     IF( IZCHA(IC,IWS) /= INOZ )      CYCLE
     NotSure = .false.
     EXIT
   ENDDO
   IF( NotSure ) THEN
     TOLZ = TWOPASTUB
     TOLS = MAX( PASTUB(IMDT) , SEndPlu )
     ZZ = ZZidea(0) + UZTidea(0)*(TTtrue-TTidea(0))
     SS = SSidea(0) + USTidea(0)*(TTtrue-TTidea(0))
     IF( NOKCHA(IC,INOZ,IGEOM,ZZ,SS,TOLZ,TOLS) ) THEN
       NTOT = NLTUB(IMDT)
       NotOnTrack = 1.75d0*CHINAZ
       CTOT = NTOT * NotOnTrack
       DO IP=1,NLTUB(IMDT)
         dcapoin%CCC(IP,1) = NotOnTrack
       ENDDO
       RETURN
     ENDIF
   ENDIF
   TOLZ = 0.0d0
   TOLS = 0.0d0
!
   IZ0LIM = 0
   IF( ABS(WZidea(0)) > WZLIM ) IZ0LIM = 1
   NEGA   = 0
   NM2000 = 0
   NINEFF = 0
!
!>> Find the nb. in Z of the MDT chamber(s) to be considered
   INOZMI =  1
   INOZMA = NZM
   IF( IC > NCOUBA ) THEN
     IF( SIGT > 0.D0 ) THEN
       INOZMA = NZMS2
     ELSE
       INOZMI = NZMS2 + 1
     ENDIF
   ENDIF
   INOZA = INOZ
   INOZB = INOZ
   IF( INOZ-1 >= INOZMI ) THEN
     IGEOM_M1 = IGEOMA(IC,INOF,INOZ-1)
     IF( IGEOM_M1 > 0 ) THEN
       ZLIM = ZCECHAV(IGEOM_M1) + ZLOCHAV(IGEOM_M1)/2.D0
       IF( ZZtrue < ZLIM+TWOPASTUB ) THEN
         IF( ABS(TCECHAV(IGEOM_M1)-TCECHAV(IGEOM)) < DTcut ) THEN
           INOZA = INOZ - 1
           ItransA = ItransCHA(IGEOM_M1)
           CALL SZTWU_Ideal_from_True(ItransA, SStrue,ZZtrue,TTtrue,VStrue,VZtrue,VTtrue,     &
                            SSidea(-1),ZZidea(-1),TTidea(-1),WZidea(-1),WTidea(-1),UZTidea(-1),USTidea(-1))
           IF( isBarrel .AND. INOZA < NZMS2 ) SIGZ(-1) = MDTnu
           SiFFi(-1) = SINCHAV(IGEOM_M1)
           CoFFi(-1) = COSCHAV(IGEOM_M1)
         ENDIF
       ENDIF
     ENDIF
   ENDIF
   IF( INOZ+1 <= INOZMA ) THEN
     IGEOM_P1 = IGEOMA(IC,INOF,INOZ+1)
     IF( IGEOM_P1 > 0 ) THEN
       ZLIM = ZCECHAV(IGEOM_P1) - ZLOCHAV(IGEOM_P1)/2.D0
       IF( ZZtrue > ZLIM-TWOPASTUB ) THEN
         IF( ABS(TCECHAV(IGEOM_P1)-TCECHAV(IGEOM)) < DTcut ) THEN
           INOZB = INOZ + 1
           ItransB = ItransCHA(IGEOM_P1)
           CALL SZTWU_Ideal_from_True(ItransB,SStrue,ZZtrue,TTtrue,VStrue,VZtrue,VTtrue,     &
                            SSidea(1),ZZidea(1),TTidea(1),WZidea(1),WTidea(1),UZTidea(1),USTidea(1))
           IF( isBarrel .AND. INOZB < NZMS2 ) SIGZ(1) = MDTnu
           SiFFi(1) = SINCHAV(IGEOM_P1)
           CoFFi(1) = COSCHAV(IGEOM_P1)
         ENDIF
       ENDIF
     ENDIF
   ENDIF
!
!>> Ensure that direction is always outgoing (WT > 0)
   IF( WTidea(0) < 0.D0 ) THEN
     IA = INOZA - INOZ
     IB = INOZB - INOZ
     WZidea(IA:IB) = - WZidea(IA:IB)
     WTidea(IA:IB) = - WTidea(IA:IB)
     dirx = - dirx
     diry = - diry
     dirz = - dirz
   ENDIF
   dirx = SIGT * dirx
   diry = SIGT * diry
   dirz = SIGT * dirz
!
!>> One loops over the hits from the NLTUB(IMDT) layers of tubes of
!>> hit chamber(s) IWCHA(IC,INOF,INOZA) to IWCHA(IC,INOF,INOZB) of
!>> multi-layer type IC
   DO 80 IP=1,NLTUB(IMDT)
     ZOLTUB0 = ZCECHAV(IGEOM) + SIGZ(0) *( OLTUB(IMDT,IP,1) - ZLOCHAV(IGEOM)/2.D0 )
     NTUT = 0
     NM200T = 0
     NINEFT = 0
!
     DO L=IFSure(IC),NZSure
       IF( ICSure(L) /= IC )                              EXIT
       IF( IPSure(L) /= IP )                              CYCLE       
       IWS = IWSure(L)
       IF( IFCHA(IC,IWS) /= INOF )                        CYCLE
       IF( IZCHA(IC,IWS) /= INOZ )                        CYCLE
       ITS = IZSure(L)
       TTUT = TTtrue + SIGT*TLTUB(IMDT,IP,1)
       STUT = SSidea(0) + (TTUT-TTidea(0)) * USTidea(0)
       ZTUT = ZTUBHI(ITS)
       RTAT = (ZTUT-ZZidea(0))*WTidea(0) - (TTUT-TTidea(0))*WZidea(0)
       ARTAT = ABS( RTAT )
       RTUT = ARTAT
       TUBRES = FREREC( IGEOM,IC,INOF,INOZ , ARTAT )
       IF( IadDisChan(IGEOM) > 0 ) THEN
         IZ = NINT( SIGZ(0)*(ZTUT-ZOLTUB0) * InvPASTUB )
         IF( IsTubeDisabled(IGEOM,IP,IZ+1) ) TUBRES = 999.D0
       ENDIF
!--------------------------------
       Z1DCA= ZTUT - RTAT*WTidea(0)
       T1DCA= TTUT + RTAT*WZidea(0)
       S1DCA= STUT + RTAT*WZidea(0) * USTidea(0)
       IF( Itrans >= 1 ) THEN
         CALL Pos_True_from_Ideal( Itrans, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
         Z1DCA= ZBt
         T1DCA= TBt
         S1DCA= SBt
       ENDIF
       CALL MuboSZTtoXYZ( isBarrel, SiFFi(0),CoFFi(0), S1DCA,Z1DCA,T1DCA, XXXDCA,YYYDCA,ZZZDCA )
       IF( doRTmiddle ) THEN
         Z1DCA= ZTUT - RTAT*WTidea(0)
         T1DCA= TTUT + RTAT*WZidea(0)
         SRDCA= 0.d0
         IF( Itrans >= 1 ) THEN
           CALL Pos_True_from_Ideal( Itrans, SRDCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
           Z1DCA= ZBt
           T1DCA= TBt
           SRDCA= SBt
         ENDIF
         CALL MuboSZTtoXYZ( isBarrel, SiFFi(0),CoFFi(0), SRDCA,Z1DCA,T1DCA, XXRDCA,YYRDCA,ZZRDCA )
       ELSE
         SRDCA  = S1DCA
         XXRDCA = XXXDCA
         YYRDCA = YYYDCA
         ZZRDCA = ZZZDCA
       ENDIF
!--------------------------------
       RRR = RFTfast(ITS, SRDCA, RTUT,        &
                     XXRDCA, YYRDCA, ZZRDCA , &
                     dirx  , diry  , dirz  )
       NTUT = MIN( NTUT+1 , 4 )
       dcapoin%CCC(IP,NTUT) = ( (RTUT-RRR) / TUBRES )**2
       dcapoin%JJJ(IP,NTUT) = JTUBHI(ITS)
       dcapoin%III(IP,NTUT) = ITS
       CALL Get_Tube_Lateral_Dim(IGEOM,ZTUT, SWIZZN,SWIZZP)
       IF( STUT < SWIZZN ) dcapoin%CCC(IP,NTUT) = dcapoin%CCC(IP,NTUT) + (SIGMAK*(STUT-SWIZZN)*InvdeltaS)**2
       IF( STUT > SWIZZP ) dcapoin%CCC(IP,NTUT) = dcapoin%CCC(IP,NTUT) + (SIGMAK*(STUT-SWIZZP)*InvdeltaS)**2
       dcapoin%dRodZ(IP,NTUT) = - WTidea(0)
       dcapoin%dRodR(IP,NTUT) = - (ZTUT-ZZidea(0))*WZidea(0) - (TTUT-TTidea(0))*WTidea(0)
       dcapoin%scal(IP,NTUT) = 1.d0
       dcapoin%d2RodZdR(IP,NTUT) = WZidea(0)
       dcapoin%d2RodRdR(IP,NTUT) = - RTAT
       dcapoin%RHIT(IP,NTUT) = SIGN( RRR , RTAT )
       dcapoin%RTRA(IP,NTUT) = RTAT
       dcapoin%RDCA(IP,NTUT) = TUBRES
       IF( saveDcaTrk ) THEN
         dcapoin%IDE(IP,NTUT)  = 0 ! used only for holes, and sure hits can not be holes
         dcapoin%XDCA(IP,NTUT) = XXXDCA
         dcapoin%YDCA(IP,NTUT) = YYYDCA
         dcapoin%ZDCA(IP,NTUT) = ZZZDCA
         dcapoin%EDCA(IP,NTUT) = 99999.D0
         dcapoin%SDCA(IP,NTUT) = S1DCA
         dcapoin%XDIR(IP,NTUT) = dirx
         dcapoin%YDIR(IP,NTUT) = diry
         dcapoin%ZDIR(IP,NTUT) = dirz
       ENDIF
     ENDDO
!
     DO 70 INOZI=INOZA,INOZB
       IGEOMI = IGEOMA(IC,INOF,INOZI)
       IF( isOffChav(IGEOMI) )  CYCLE
       IMDTI  = ISTMDTV(IGEOMI)
       IF(IP > NLTUB(IMDTI))    CYCLE
       IWI    = IWCHA(IC,INOF,INOZI)
       ItransI= ItransCHA(IGEOMI)
       J = INOZI - INOZ
       SS     = SSidea(J)
       ZZ     = ZZidea(J)
       TT     = TTidea(J)
       WZ     = WZidea(J)
       WT     = WTidea(J)
       UST    = USTidea(J)
       ZOLTUB = ZCECHAV(IGEOMI) + SIGZ(J) * ( OLTUB(IMDTI,IP,1) - ZLOCHAV(IGEOMI)*0.5d0 )
       TTUT   = TCECHAV(IGEOMI) + SIGT*TLTUB(IMDTI,IP,1)
       STUT   = SS + (TTUT-TT) * UST
       TWZ    = (TTUT-TT) * WZ
       IZ0    = NINT( SIGZ(J)*(ZZ-ZOLTUB+TWZ/WT)*PASTUB_inv(IMDTI) - 0.5D0 )
       HasDeadTube = IadDedChan(IGEOMI) > 0 .OR. IadDisChan(IGEOMI) > 0
b60:   DO 60 IZ=IZ0-IZ0LIM,IZ0+1+IZ0LIM
         ZTUT   = SIGZ(J)*IZ*PASTUB(IMDTI) + ZOLTUB
         RTAT   = (ZTUT-ZZ) * WT - TWZ
         ARTAT  = ABS( RTAT )
         TUBRES = FREREC( IGEOMI,IC,INOF,INOZI , ARTAT )
         IF( ARTAT  >=  RAYTUB(IMDTI)+SIGMAK*TUBRES )              CYCLE
         IF( NAKCHA(IC,INOZI,IGEOMI,ZTUT,STUT,TTUT,TOLZ,SEndPlu) ) CYCLE
         IEndPlu = 0
         IF( NAKCHA(IC,INOZI,IGEOMI,ZTUT,STUT,TTUT,TOLZ,TOLS) ) IEndPlu = 1
         IF( HasDeadTube ) THEN
           IF( IsTubeDead(IGEOMI,IP,IZ+1) )                        CYCLE
           IF( IsTubeDisabled(IGEOMI,IP,IZ+1) ) TUBRES = 999.D0
         ENDIF
         InvTUBRES = 1.d0/TUBRES
         IF( ARTAT >= RAYTUB(IMDTI) ) THEN
           DDDD = SIGMAK2
           JJJJ = -10000000
         ELSE
           DDDD =( (RAYTUB(IMDTI)-ARTAT)*InvTUBRES + SIGMAK )**2
           JJJJ = -20000000
         ENDIF
         IIII = 0
         RDIG = 99999.D0
!--------------------------------
         Z1DCA= ZTUT - RTAT*WT
         T1DCA= TTUT + RTAT*WZ
         S1DCA= STUT + RTAT*WZ * UST
         IF( ItransI >= 1 ) THEN
           CALL Pos_True_from_Ideal( ItransI, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
           Z1DCA= ZBt
           T1DCA= TBt
           S1DCA= SBt
         ENDIF
         CALL MuboSZTtoXYZ( isBarrel, SiFFi(J),CoFFi(J), S1DCA,Z1DCA,T1DCA, XXXDCA,YYYDCA,ZZZDCA )
         IF( doRTmiddle ) THEN
           Z1DCA= ZTUT - RTAT*WT
           T1DCA= TTUT + RTAT*WZ
           SRDCA= 0.d0
           IF( ItransI >= 1 ) THEN
             CALL Pos_True_from_Ideal( ItransI, SRDCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
             Z1DCA= ZBt
             T1DCA= TBt
             SRDCA= SBt
           ENDIF
           CALL MuboSZTtoXYZ( isBarrel, SiFFi(J),CoFFi(J), SRDCA,Z1DCA,T1DCA, XXRDCA,YYRDCA,ZZRDCA )
         ELSE
           SRDCA  = S1DCA
           XXRDCA = XXXDCA
           YYRDCA = YYYDCA
           ZZRDCA = ZZZDCA
         ENDIF
!--------------------------------
         ITDEB = NTUBH0(IC,IWI,IP) + 1
         ITFIN = NTUBH0(IC,IWI,IP) + NTUBHI(IC,IWI,IP)
         DO 50 IT=ITDEB,ITFIN
           IF( ABS(ZTUBHI(IT)-ZTUT)  >  0.1D0 )  CYCLE
           IF( IFSure(IC) <= NZSure ) THEN
             DO L=1,NTUT
               IF( abs(dcapoin%III(IP,L)) == IT) CYCLE b60
             ENDDO
           ENDIF
           RTUT = ARTAT
           IF( LTUBHI(IT) == 1 ) THEN
             RRR = RFTfast(IT, SRDCA, RTUT,         &
                           XXRDCA, YYRDCA, ZZRDCA , &
                           dirx  , diry  , dirz  )
           ELSE
             RRR = 99999.D0
           ENDIF
           IF( LTUBHI(IT) >= 3 )  CYCLE   ! warning : should not be done before as RFTfast may modify LTUBHI 
           RDED = RRR + REDTUB(IMDTI)
           IF( LTUBHI(IT) == 1 .AND. RTUT < RRR + SIGMAK*TUBRES ) THEN
             DTEST = ( (RTUT-RRR) *InvTUBRES )**2
             IF(DTEST > DDDD) CYCLE
             JJJJ = JTUBHI(IT)
             IIII = IT
           ELSEIF( LTUBHI(IT) /= 1 .OR. RTUT < RDED ) THEN
             DTEST = SIGMAK2
             IF(DTEST > DDDD) CYCLE
             JJJJ = -JTUBHI(IT)
             IIII = -IT
           ELSE
             DTEST =((RTUT + SIGMAK*TUBRES - RDED) *InvTUBRES)**2
             DTEST = CHICUR( DTEST , CHINAX )
             IF(DTEST > DDDD) CYCLE
             JJJJ = -JTUBHI(IT)
             IIII = -IT
           ENDIF
           DDDD = DTEST
           RDIG = RRR
50       ENDDO
         IF( JJJJ == -10000000 )            CYCLE
         IF( JJJJ <= 0 .AND. IEndPlu == 1 ) CYCLE
!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
         IF( JJJJ == -20000000 ) THEN
!  Special case where MUONBOY thinks wrongly that an additional tube
!  exists at one extremity of the multilayer
           IF( NAKCHA(IC,INOZI,IGEOMI,ZTUT,STUT,TTUT,strictTolZ,TOLS) ) CYCLE
         ENDIF
!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
         IF( IEndPlu == 1 ) JJJJ = JJJJ + 10000000
         NTUT = MIN( NTUT+1 , 4 )
         dcapoin%CCC(IP,NTUT)  = DDDD
         dcapoin%JJJ(IP,NTUT)  = JJJJ
         dcapoin%III(IP,NTUT)  = IIII
         dcapoin%dRodZ(IP,NTUT) = - WT
         dcapoin%dRodR(IP,NTUT) = - (ZTUT-ZZ)*WZ - (TTUT-TT)*WT
         dcapoin%scal(IP,NTUT) = 1.d0
         dcapoin%d2RodZdR(IP,NTUT) = WZ
         dcapoin%d2RodRdR(IP,NTUT) = - RTAT
         dcapoin%RHIT(IP,NTUT) = SIGN( RDIG , RTAT )
         dcapoin%RTRA(IP,NTUT) = RTAT
         dcapoin%RDCA(IP,NTUT) = TUBRES
         IF( saveDcaTrk ) THEN
           dcapoin%IDE(IP,NTUT)  = IGEOMI + 100000*IP + 1000000*(IZ+1)
           dcapoin%XDCA(IP,NTUT) = XXXDCA
           dcapoin%YDCA(IP,NTUT) = YYYDCA
           dcapoin%ZDCA(IP,NTUT) = ZZZDCA
           dcapoin%EDCA(IP,NTUT) = 99999.D0
           dcapoin%SDCA(IP,NTUT) = S1DCA
           dcapoin%XDIR(IP,NTUT) = dirx
           dcapoin%YDIR(IP,NTUT) = diry
           dcapoin%ZDIR(IP,NTUT) = dirz
         ENDIF
         IF( dcapoin%JJJ(IP,NTUT) == -20000000 )  NM200T = NM200T + 1
         IF( dcapoin%CCC(IP,NTUT) > CHINAZ)      NINEFT = NINEFT + 1
60     ENDDO b60
70   ENDDO
!
     NM2000 = NM2000 + NM200T
     NINEFF = NINEFF + NINEFT
     IF( NTUT <= 0 ) THEN
!>> Total geometrical inefficiency in layer IP
       NTUT = 1
       dcapoin%CCC(IP,NTUT) = SIGMAK2
       dcapoin%JJJ(IP,NTUT) = -10000000
       dcapoin%III(IP,NTUT) = 0
       IF( saveDcaTrk ) THEN
         dcapoin%IDE(IP,NTUT) = 0
         dcapoin%RHIT(IP,NTUT) = 999999.D0
         dcapoin%RTRA(IP,NTUT) = 999999.D0
         dcapoin%XDCA(IP,NTUT) = 999999.D0
         dcapoin%YDCA(IP,NTUT) = 999999.D0
         dcapoin%ZDCA(IP,NTUT) = 999999.D0
         dcapoin%EDCA(IP,NTUT) = 999999.D0
         dcapoin%RDCA(IP,NTUT) = 999999.D0
         dcapoin%SDCA(IP,NTUT) = 999999.D0
         dcapoin%XDIR(IP,NTUT) = dirx
         dcapoin%YDIR(IP,NTUT) = diry
         dcapoin%ZDIR(IP,NTUT) = dirz
       ENDIF
     ENDIF
     NTOT = NTOT + NTUT
     DO I=1,NTUT
       IF( dcapoin%JJJ(IP,I) < 0 ) NEGA = NEGA + 1
       CTOT = CTOT + dcapoin%CCC(IP,I)
     ENDDO
!
80 ENDDO
!
!
   IF( ItypFit <= 2 ) THEN
!>> Pattern recognition fit type <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!>> Almost redhibitory case !
     IF( NEGA >= NTOT ) THEN
       CTOT = NTOT * 2.D0*DCLOOK
       WHERE( dcapoin%JJJ(1:NLTUB(IMDT),1:4) /= 0 ) dcapoin%CCC(1:NLTUB(IMDT),1:4) = 2.D0*DCLOOK
       RETURN
     ENDIF
!>> Here, one treats the possible inefficiencies
     IF( NINEFF >= 1 .AND. NINEFF <= MAINEF .AND. NM2000 < MA2000 ) THEN
       DO IP=1,NLTUB(IMDT)
         DO I=1,4
           IF( dcapoin%JJJ(IP,I) == 0 )      EXIT
           IF( dcapoin%CCC(IP,I) <= CHINAZ ) CYCLE
           IF( dcapoin%CCC(IP,I) > 2.D0*CHINAZ ) THEN
             IF( dcapoin%JJJ(IP,I) > 0 )  dcapoin%JJJ(IP,I)= -20000000-JFIRST(dcapoin%JJJ(IP,I))
             dcapoin%III(IP,I) = 0
             IF( saveDcaTrk ) THEN
               dcapoin%IDE(IP,I) = 0  ! holes are usefull only on last fit (ItypFit = 6)
               dcapoin%RHIT(IP,I) = 999999.D0
               dcapoin%RTRA(IP,I) = 999999.D0
               dcapoin%XDCA(IP,I) = 999999.D0
               dcapoin%YDCA(IP,I) = 999999.D0
               dcapoin%ZDCA(IP,I) = 999999.D0
               dcapoin%EDCA(IP,I) = 999999.D0
               dcapoin%RDCA(IP,I) = 999999.D0
               dcapoin%SDCA(IP,I) = 999999.D0
             ENDIF
           ENDIF
           CCCINE            = CHICUR( dcapoin%CCC(IP,I) , CHINAZ )
           CTOT              = CTOT + CCCINE - dcapoin%CCC(IP,I)
           dcapoin%CCC(IP,I) = CCCINE
         ENDDO
       ENDDO
!>> Here is the case when one is far away from the hits
     ELSE IF( NM2000 >= MA2000 ) THEN
       DO IP=1,NLTUB(IMDT)
         DO 300 I=1,4
           IF( dcapoin%JJJ(IP,I) /= -20000000 ) CYCLE
           RRRMIN = 999999.D0
           DO INOZI=INOZA,INOZB
             IGEOMI = IGEOMA(IC,INOF,INOZI)
             IMDTI  = ISTMDTV(IGEOMI)
             ItransI= ItransCHA(IGEOMI)
             IWI    = IWCHA(IC,INOF,INOZI)
             J = INOZI - INOZ
             SS    = SSidea(J)
             ZZ    = ZZidea(J)
             TT    = TTidea(J)
             WZ    = WZidea(J)
             WT    = WTidea(J)
             UST   = USTidea(J)
             TTUT  = TCECHAV(IGEOMI) + SIGT*TLTUB(IMDTI,IP,1)
             STUT  = SS + (TTUT-TT) * UST
             ZTUT  = ZZ + (TTUT-TT) * WZ / WT
!--------------------------------
             Z1DCA= ZTUT
             T1DCA= TTUT
             S1DCA= STUT
             CALL Pos_True_from_Ideal( ItransI, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
             CALL MuboSZTtoXYZ( isBarrel, SiFFi(J),CoFFi(J), SBt,ZBt,TBt, XXXDCA,YYYDCA,ZZZDCA )
             IF( doRTmiddle ) THEN
               Z1DCA= ZTUT
               T1DCA= TTUT
               SRDCA= 0.d0
               CALL Pos_True_from_Ideal( ItransI, SRDCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
               CALL MuboSZTtoXYZ( isBarrel, SiFFi(J),CoFFi(J), SBt,ZBt,TBt, XXRDCA,YYRDCA,ZZRDCA )
             ELSE
               SRDCA  = STUT
               XXRDCA = XXXDCA
               YYRDCA = YYYDCA
               ZZRDCA = ZZZDCA
             ENDIF
!--------------------------------
             ITDEB = NTUBH0(IC,IWI,IP) + 1
             ITFIN = NTUBH0(IC,IWI,IP) + NTUBHI(IC,IWI,IP)
             DO IT=ITDEB,ITFIN
               IF( LTUBHI(IT) /= 1 ) CYCLE
               RadTrackTest = ABS( (ZTUBHI(IT)-ZZ)*WT - (TTUT-TT)*WZ )
               RadDigitTest = RFTfast(IT, SRDCA, RadTrackTest, &
                                      XXRDCA, YYRDCA, ZZRDCA , &
                                      dirx  , diry  , dirz  )
               RRR = ABS( RadTrackTest - RadDigitTest )
               IF(RRR < RRRMIN) RRRMIN = RRR
             ENDDO
           ENDDO
           IF( RRRMIN > 6.d0*PASTUB(IMDT) ) CYCLE
           CCCMIN            = ( RRRMIN / RESTUB(IMDT) )**2
           IF( RRRMIN > 4.d0*PASTUB(IMDT) ) THEN
             RRRMIN = RRRMIN * PASTUB_inv(IMDT)
             CCCMIN = 0.5d0 * ( ( RRRMIN - 4.d0 ) * dcapoin%CCC(IP,I) + ( 6.d0 - RRRMIN ) * CCCMIN )
           ENDIF
           CTOT              = CTOT + CCCMIN - dcapoin%CCC(IP,I)
           dcapoin%CCC(IP,I) = CCCMIN
300      ENDDO
       ENDDO
     ENDIF
   ELSE
!>> Statistical fit type <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!>> Here, one treats the possible inefficiencies
     IF( NINEFF >= 1 ) THEN
       DO IP=1,NLTUB(IMDT)
         DO I=1,4
           IF( dcapoin%JJJ(IP,I) == 0 )                      EXIT
           IF( dcapoin%CCC(IP,I) <= CHINAZ )                 CYCLE
           IF( intpos(dcapoin%III(IP,I),IZSure,NZSure) > 0 ) CYCLE 
           IF( dcapoin%CCC(IP,I) > 2.D0*CHINAZ ) THEN
             IF( dcapoin%JJJ(IP,I) > 0 )  dcapoin%JJJ(IP,I)= -20000000-JFIRST(dcapoin%JJJ(IP,I))
             dcapoin%III(IP,I) = 0
           ENDIF
           CCCINE = CHICUR( dcapoin%CCC(IP,I) , CHINAZ )
           CTOT   = CTOT + CCCINE - dcapoin%CCC(IP,I)
           dcapoin%scal(IP,I) = dcapoin%scal(IP,I) * CCCINE / dcapoin%CCC(IP,I)
           dcapoin%CCC(IP,I) = CCCINE
         ENDDO
       ENDDO
     ENDIF
   ENDIF
!
 END SUBROUTINE CHIZZZ
!
