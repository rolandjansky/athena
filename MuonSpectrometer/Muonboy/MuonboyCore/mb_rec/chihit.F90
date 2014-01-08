!
!> \author M.Virchaux  
!
 SUBROUTINE CHIHIT(JKI0,Ztrue,Ttrue,Strue,WZtrue,WTtrue,VSTtrue, KMA, NTOT,CTOT, dcapoin)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS, ONLY : C1P000
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_COTEMH
 IMPLICIT NONE
 INTEGER,        INTENT(IN)    :: JKI0, KMA
 REAL(8),        INTENT(IN)    :: Ztrue, Ttrue, Strue, WZtrue, WTtrue, VSTtrue
 INTEGER,        INTENT(OUT)   :: NTOT
 REAL(8),        INTENT(OUT)   :: CTOT
 TYPE(DcaPoint), INTENT(INOUT) :: dcapoin
 LOGICAL :: NAKCHA, HasDeadTube, IsTubeDead, IsTubeDisabled
 INTEGER :: NEGA, IC, IMDT0, Itrans, JKIMI, JKIMA, NTUT
 INTEGER :: IP, JKI, INOZ, IMDT, IGEOM, IZ0, IZ0LIM, IZ, IIII, KKKK
 INTEGER :: ITDEB, ITFIN, IT, KTEST, ITUT
 REAL(8) :: T0, SIGT, VStrue, ZZ, TOLZ, TOLS, TOLZM, TOLSM, Z, T
 REAL(8) :: WZ, WT, VST, SIGZ, ZOLTUB, TTUT, STUT, TWZ
 REAL(8) :: ZTUT, RTAT, ARTAT, RTUT, TUBRES, FRESEG, DDDD 
 REAL(8) :: RRR,RDED, DTEST, RDIG, CHICUR, SBt,ZBt,TBt 
 REAL(8) :: Zk6(6),Tk6(6),Sk6(6),WZk6(6),WTk6(6),VSTk6(6),VZTk6(6)
 REAL(8) :: RFTfast
 REAL(8), PARAMETER :: WZLIM = 0.85D0
!
!>> Warning :  JKI0  must be equal to  2  or  5   !!!
!>>           ******                  ***    ***
!>>
!>>     and :  WTtrue  must be  > 0
!>>           ********         *****
!
 REAL(8) :: TWOPASTUB
 REAL(8) :: InvTUBRES
 REAL(8) :: Z1DCA, T1DCA, S1DCA 
 REAL(8) :: XXXDCA, YYYDCA, ZZZDCA
 REAL(8) :: XXRDCA, YYRDCA, ZZRDCA, SRDCA
 REAL(8) :: dirx, diry, dirz
 LOGICAL :: isBarrel
!
   NTOT = 0
   CTOT = 0.D0
   NEGA = 0
!
   IC    = ITEMC(JKI0)
   IMDT0 = ITEMM(JKI0)
   T0    = TEMTT(JKI0)
!>> SIGT < 0 if one is in the "negative" end cap
   SIGT  = SIGN( C1P000, T0 )
   isBarrel = IC <= NCOUBA
   CALL MuboSZTtoXYZ( isBarrel,SiTemFF(JKI0),CoTEMFF(JKI0),WTtrue*VSTtrue,WZtrue,WTtrue,dirX,dirY,dirZ)
!
   Itrans  = ITEMA(JKI0)
   VStrue  = VSTtrue*WTtrue
!>> Vector  VStrue,WZtrue,WTtrue  correspond to  Vs,Vz,Vt  with norm = sqrt( 1. + VStrue**2 )
   IF( Itrans < 1 ) THEN
     Zk6(JKI0)   = Ztrue
     Tk6(JKI0)   = Ttrue
     Sk6(JKI0)   = Strue
     WZk6(JKI0)  = WZtrue
     WTk6(JKI0)  = WTtrue
     VZTk6(JKI0) = WZtrue / WTtrue
     VSTk6(JKI0) = VSTtrue
   ELSE
     CALL SZTWU_Ideal_from_True(Itrans, Strue,Ztrue,Ttrue,VStrue,WZtrue,WTtrue,   &
                                Sk6(JKI0),Zk6(JKI0),Tk6(JKI0),WZk6(JKI0),WTk6(JKI0),VZTk6(JKI0),VSTk6(JKI0) )
   ENDIF
!
   TWOPASTUB = 2.D0*PASTUB(IMDT0)
!
   ZZ = Zk6(JKI0) + (T0-Tk6(JKI0))*VZTk6(JKI0)
   IF( ZZ < TEMZI(JKI0)-TWOPASTUB ) GO TO 90
   IF( ZZ > TEMZA(JKI0)+TWOPASTUB ) GO TO 90
   JKIMI = JKI0
   JKIMA = JKI0
   IF( ITEMC(JKI0-1) > 0  ) THEN
     IF( ZZ < TEMZA(JKI0-1)+TWOPASTUB ) THEN
       JKIMI = JKI0 - 1
       Itrans = ITEMA(JKIMI)
       CALL SZTWU_Ideal_from_True(Itrans, Strue,Ztrue,Ttrue,VStrue,WZtrue,WTtrue,   &
                                Sk6(JKIMI),Zk6(JKIMI),Tk6(JKIMI),WZk6(JKIMI),WTk6(JKIMI),VZTk6(JKIMI),VSTk6(JKIMI) )
     ENDIF
   ENDIF
   IF( ITEMC(JKI0+1) > 0 ) THEN
     IF( ZZ > TEMZI(JKI0+1)-TWOPASTUB ) THEN
       JKIMA = JKI0 + 1
       Itrans = ITEMA(JKIMA)
       CALL SZTWU_Ideal_from_True(Itrans, Strue,Ztrue,Ttrue,VStrue,WZtrue,WTtrue,   &
                                Sk6(JKIMA),Zk6(JKIMA),Tk6(JKIMA),WZk6(JKIMA),WTk6(JKIMA),VZTk6(JKIMA),VSTk6(JKIMA) )
     ENDIF
   ENDIF
!
   TOLZ  =  0.00d0
   TOLS  =  SEndPlu
   TOLZM = -0.20d0
   TOLSM =  0.00d0
!
!>> One loops over the single layers of the multilayer
   DO 40 IP=1,NLTUB(IMDT0)
     NTUT = 0
!
     DO 30 JKI=JKIMI,JKIMA
       INOZ   = ITEMZ(JKI)
       IMDT   = ITEMM(JKI)
       IF( IP > NLTUB(IMDT) ) CYCLE
       Itrans = ITEMA(JKI)
       IGEOM  = ITEMG(JKI)
       Z      = Zk6(JKI)
       T      = Tk6(JKI)
       WZ     = WZk6(JKI)
       WT     = WTk6(JKI)
       VST    = VSTk6(JKI)
       SIGZ   = TEMSI(JKI)
       ZOLTUB = TEMZO(JKI) + SIGZ*OLTUB(IMDT,IP,1)
       TTUT   = TEMTT(JKI) + SIGT*TLTUB(IMDT,IP,1)
       STUT   = Sk6(JKI) + (TTUT-T) * VST
       TWZ    = (TTUT-T) * WZ
       IZ0    = NINT( SIGZ*(Z-ZOLTUB+TWZ/WT) * PASTUB_inv(IMDT) - 0.5D0 )
       IZ0LIM = 0
       IF( ABS(WZ) > WZLIM ) IZ0LIM = 1
       HasDeadTube = LTEMP(JKI)
       DO 20 IZ=IZ0-IZ0LIM,IZ0+1+IZ0LIM
         ZTUT  = SIGZ*IZ*PASTUB(IMDT) + ZOLTUB
         RTAT  = (ZTUT-Z) * WT - TWZ
         ARTAT = ABS( RTAT )
         IF( ARTAT >= RAYTUB(IMDT) )                          CYCLE
         IF( NAKCHA(IC,INOZ,IGEOM,ZTUT,STUT,TTUT,TOLZ,TOLS) ) CYCLE
         TUBRES = FRESEG( IGEOM, ARTAT )
         IF( HasDeadTube ) THEN
           IF( IsTubeDead(IGEOM,IP,IZ+1) )                    CYCLE
           IF( IsTubeDisabled(IGEOM,IP,IZ+1) ) TUBRES = 999.D0
         ENDIF
         InvTUBRES = 1.d0 / TUBRES
         DDDD = ( (RAYTUB(IMDT)-ARTAT) *InvTUBRES + SIGMOK )**2
         IIII = -20000000 - INT(10.d0*(RAYTUB(IMDT)-ARTAT)*InvTUBRES)
         RDIG = 99999.D0
         IF( RTAT < 0.D0 ) THEN
           KKKK = 1
         ELSE
           KKKK = 2
         ENDIF
!
         IF( NTEMHI(JKI,IP,IZ) >= 1 .OR. saveDcaSeg ) THEN
           Z1DCA= ZTUT - RTAT*WT
           T1DCA= TTUT + RTAT*WZ
           S1DCA= STUT + RTAT*WZ * VST
           IF( Itrans > 0 ) THEN
             CALL Pos_True_from_Ideal( Itrans, S1DCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
             Z1DCA= ZBt
             T1DCA= TBt
             S1DCA= SBt
           ENDIF
           CALL MuboSZTtoXYZ( isBarrel, SiTEMFF(JKI),CoTEMFF(JKI), S1DCA,Z1DCA,T1DCA, XXXDCA,YYYDCA,ZZZDCA )
           IF( doRTmiddle ) THEN
             Z1DCA= ZTUT - RTAT*WT
             T1DCA= TTUT + RTAT*WZ
             SRDCA= 0.d0
             IF( Itrans > 0 ) THEN
               CALL Pos_True_from_Ideal( Itrans, SRDCA,Z1DCA,T1DCA, SBt,ZBt,TBt )
               Z1DCA= ZBt
               T1DCA= TBt
               SRDCA= SBt
             ENDIF
             CALL MuboSZTtoXYZ( isBarrel, SiTEMFF(JKI),CoTEMFF(JKI), SRDCA,Z1DCA,T1DCA, XXRDCA,YYRDCA,ZZRDCA )
           ELSE
             SRDCA  = S1DCA
             XXRDCA = XXXDCA
             YYRDCA = YYYDCA
             ZZRDCA = ZZZDCA
           ENDIF
         ENDIF
!
         ITDEB = NTEMH0(JKI,IP,IZ) + 1
         ITFIN = NTEMH0(JKI,IP,IZ) + NTEMHI(JKI,IP,IZ)
         DO IT=ITDEB,ITFIN
           RTUT  = ARTAT
           IF( LTUBHI(IT) == 1 ) THEN
             RRR = RFTfast(IT, SRDCA, RTUT,         &
                           XXRDCA, YYRDCA, ZZRDCA , &
                           dirx  , diry  , dirz)
           ELSE
             RRR = 99999.D0
           ENDIF
           IF( LTUBHI(IT) >= 3 )     CYCLE   ! warning : should not be done before as RFTfast may modify LTUBHI 
           RDED = RRR + REDTUB(IMDT)
           IF( LTUBHI(IT) == 1 .AND. RTUT < RRR + SIGMOK*TUBRES ) THEN
             DTEST = ( (RTUT-RRR) *InvTUBRES )**2
           ELSEIF( LTUBHI(IT) /= 1 .OR. RTUT < RDED ) THEN
             DTEST =  SIGMOK**2
           ELSE
             DTEST =((RTUT + SIGMOK*TUBRES - RDED) *InvTUBRES)**2
             DTEST = CHICUR( DTEST , CHINEF*0.5d0 )
           ENDIF
           KTEST = KTUBHI(IT) / KMA
           IF( KTEST >= 3 .OR. KTEST == KKKK )  DTEST = MAX( DTEST , 1.1D0*SIGMOK**2 )
           IF( DTEST >= DDDD ) CYCLE
           DDDD = DTEST
           RDIG = RRR
           IIII =  IT
           IF( DTEST > SIGMOK**2-0.001D0 ) IIII = -IT
           IF( LTUBHI(IT) == 1  ) then
             IF( RTUT < RRR - 2.d0*SIGMOK*TUBRES ) IIII = -IT -15000000  ! not compatible
           endif
         ENDDO
         IF( IIII <= -20000000 ) THEN
           IF( NAKCHA(IC,INOZ,IGEOM,ZTUT,STUT,TTUT,TOLZM,TOLSM) ) CYCLE
         ENDIF
         NTUT = MIN( NTUT+1 , 4 )
         dcapoin%CCC(IP,NTUT) = DDDD
         dcapoin%III(IP,NTUT) = IIII
         dcapoin%JJJ(IP,NTUT) = KKKK
         IF( saveDcaSeg ) THEN
           dcapoin%IDE(IP,NTUT) = IGEOM + 100000*IP + 1000000*(IZ+1)
           dcapoin%RHIT(IP,NTUT) = SIGN( RDIG , RTAT )
           dcapoin%RTRA(IP,NTUT) = RTAT
           dcapoin%XDCA(IP,NTUT) = XXXDCA
           dcapoin%YDCA(IP,NTUT) = YYYDCA
           dcapoin%ZDCA(IP,NTUT) = ZZZDCA
           dcapoin%XDIR(IP,NTUT) = dirx
           dcapoin%YDIR(IP,NTUT) = diry
           dcapoin%ZDIR(IP,NTUT) = dirz
           dcapoin%EDCA(IP,NTUT) = 99999.D0
           dcapoin%RDCA(IP,NTUT) = TUBRES
           dcapoin%SDCA(IP,NTUT) = S1DCA
         ENDIF
20     ENDDO
30   ENDDO
!
     IF( NTUT < 1 ) THEN
       NTUT = 1
       dcapoin%CCC(IP,NTUT) = SIGMOK*SIGMOK
       dcapoin%III(IP,NTUT) = -10000000
       dcapoin%JJJ(IP,NTUT) = 0
       IF( saveDcaSeg ) THEN
         dcapoin%IDE(IP,NTUT) = 0
         dcapoin%RHIT(IP,NTUT) = 999999.D0
         dcapoin%RTRA(IP,NTUT) = 999999.D0
         dcapoin%XDCA(IP,NTUT) = 999999.D0
         dcapoin%YDCA(IP,NTUT) = 999999.D0
         dcapoin%ZDCA(IP,NTUT) = 999999.D0
         dcapoin%XDIR(IP,NTUT) = 999999.D0
         dcapoin%YDIR(IP,NTUT) = 999999.D0
         dcapoin%ZDIR(IP,NTUT) = 999999.D0
         dcapoin%EDCA(IP,NTUT) = 999999.D0
         dcapoin%RDCA(IP,NTUT) = 999999.D0
         dcapoin%SDCA(IP,NTUT) = 999999.D0
       ENDIF
     ENDIF
!
     NTOT = NTOT + NTUT
     DO ITUT=1,NTUT
       IF( dcapoin%III(IP,ITUT) <= 0 ) NEGA = NEGA + 1
       IF( dcapoin%CCC(IP,ITUT) > CHINEF )  dcapoin%CCC(IP,ITUT) = CHICUR( dcapoin%CCC(IP,ITUT) , CHINEF )
       CTOT = CTOT + dcapoin%CCC(IP,ITUT)
     ENDDO
!
40 ENDDO
   IF( NEGA >= NTOT ) GO TO 90
   IF( NEGA == NTOT-1 ) THEN
     IF( KMA == 1 )   GO TO 90
   ENDIF
!
   RETURN
!
!>> Redhibitory case !
90 NTOT = NLTUB(IMDT0)
   CTOT = NTOT * 999999.D0
   dcapoin%CCC(1:NTOT,1) = 999999.D0
!
   RETURN
!
 END SUBROUTINE CHIHIT
!
