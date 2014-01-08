!
!> Fit the t0 of a segment
!> \author A.Ouraou
!
 MODULE M_MB_SegmentT0 
   INTEGER, PARAMETER :: maxPrevious=50
   INTEGER, SAVE :: NhitPrevious
   INTEGER, SAVE :: HitPrevious(maxPrevious), StaPrevious(maxPrevious)
   REAL(8), SAVE :: RadPrevious(maxPrevious)
 END MODULE M_MB_SegmentT0
!
!
!
 SUBROUTINE fitSegmentT0(isc,segm, t0)
 USE M_MB_Types
 USE M_MB_MuGeom
 IMPLICIT NONE
 TYPE(Segment), INTENT(IN) :: segm
 INTEGER, INTENT(IN)  :: ISC
 REAL(8), INTENT(OUT) :: t0
 REAL(8) :: ss, tt, zz, ds, dt, dz
 INTEGER :: icc(2), iww(2)
!
   t0 = 9999.d0
   if( isc > NMATTR ) return  ! only MDT segments are fitted
   if( segm%nu < 3  ) return  ! require at least 3 hits for the t0 fit
   ss = 0.5d0 * ( segm%spos(1) + segm%spos(2) )
   ds =           segm%spos(2) - segm%spos(1)
   IF( isc <= NCOUBAS2 ) THEN
     tt = 0.5d0 * ( segm%tpos(1) + segm%tpos(2) )
     dt =           segm%tpos(2) - segm%tpos(1)
     zz = 0.5d0 * ( segm%zpos(1) + segm%zpos(2) )
     dz =           segm%zpos(2) - segm%zpos(1)
   ELSE
     tt = 0.5d0 * ( segm%zpos(1) + segm%zpos(2) )
     dt =           segm%zpos(2) - segm%zpos(1)
     zz = 0.5d0 * ( segm%tpos(1) + segm%tpos(2) )
     dz =           segm%tpos(2) - segm%tpos(1)
   ENDIF
   iww(1:2) = abs(segm%iw(1:2))
   icc(1)   = 2*isc
   icc(2)   = 2*isc-1
   CALL filtem(icc,iww) 
   CALL fitSegmentMDT(tt,zz,ss,dt,dz,ds,t0)
!
 END SUBROUTINE fitSegmentT0
!
!
!
 SUBROUTINE fitSegmentMDT(tt,zz,ss,dt,dz,ds,t0)
 USE M_MB_Control, ONLY : MBPri
 IMPLICIT NONE
 INTEGER, PARAMETER   :: liv=60, lv=220
 REAL(8), INTENT(IN)  :: tt, zz, ss, dt, dz, ds
 REAL(8), INTENT(OUT) :: t0
 INTEGER, SAVE :: iv(liv)
 REAL(8), SAVE :: v(lv)
 INTEGER :: n, uip(1)
 REAL(8) :: x(3), d(3), urp(6)
 LOGICAL, SAVE :: first=.true.
 EXTERNAL :: fcnmdt, gcnmdt
!
   IF( first ) THEN
     first = .false.
     call deflt( 2, iv, liv, lv, v)
     v(31) = 1.0d-4    ! afctol absolute tol on f
!    v(32) = 1.0d-5    ! rfctol relative tol on f
     v(32) = 1.0d-3    ! rfctol absolute tol on f
     v(33) = 1.0d-8    ! xctol
     v(34) = 1.0d-10   ! xftol
     IF( MBPri <= 4 ) iv(21) = 0
   ENDIF
!
   CALL setenabletomap
!
   iv( 1) = 12
   iv(17) = 100 ! mxfcal max # of function call
   n    = 3
   x(1) = 0.d0
   x(2) = 0.d0
   x(3) = 0.d0
   d(1:2) = 100.d0
   d( 3 ) = 0.1d0
   uip(1) = 0
   urp(1) = tt
   urp(2) = zz
   urp(3) = ss
   urp(4) = dt
   urp(5) = dz
   urp(6) = ds / dt
   call sumsl( n, d, x, fcnmdt, gcnmdt, iv, liv, lv, v, uip, urp, fcnmdt ) 
   t0 = x(3)
!
   call ResetSetTShift(0.d0)
   CALL setdisabletomap
!
 END SUBROUTINE fitSegmentMDT  
!
!
!
 SUBROUTINE fcnmdt(n, x, nf, f, uip, urp, ufp)
 IMPLICIT NONE
 INTEGER :: n, nf, uip(*)
 REAL(8) :: urp(*), x(n), f, wz, wt, wn, tt, zz, ss, dt, dz, vst
 EXTERNAL :: ufp
!
   tt  = urp(1)
   zz  = urp(2) + x(1)
   ss  = urp(3)
   dt  = urp(4)
   dz  = urp(5) + x(2)
   vst = urp(6)
   call ResetSetTShift(x(3))  
   wn = 1.d0 / sqrt( dz**2 + dt**2 )
   wz = dz * wn
   wt = dt * wn
   call droikiforT0( zz, tt, ss, wz, wt, vst, f, 0)
!
 END SUBROUTINE fcnmdt
!
!
!
 SUBROUTINE gcnmdt(n, x, nf, g, uip, urp, ufp)
 IMPLICIT NONE
 INTEGER :: n, nf, uip(*)
 REAL(8) :: urp(*), x(n), g(n), wz, wt, wn, tt, zz, ss, dt, dz, vst, t0
 REAL(8) :: zzi, dzi, fp, fm
 REAL(8), PARAMETER :: derzz=0.0002d0, derdz=0.0002d0, dert0=0.5d0
 EXTERNAL :: ufp
!
   tt  = urp(1)
   zz  = urp(2) + x(1)
   ss  = urp(3)
   dt  = urp(4)
   dz  = urp(5) + x(2)
   vst = urp(6)
   t0  = x(3) 
   wn = 1.d0 / sqrt( dz**2 + dt**2 )
   wz = dz * wn
   wt = dt * wn
   call ResetSetTShift(t0+dert0)
   call droikiforT0( zz, tt, ss, wz, wt, vst, fp, 0)
   call ResetSetTShift(t0-dert0)
   call droikiforT0( zz, tt, ss, wz, wt, vst, fm, 0) 
   g(3) = ( fp - fm ) / dert0 / 2.d0
   call ResetSetTShift(t0)
   zzi = zz + derzz
   call droikiforT0( zzi, tt, ss, wz, wt, vst, fp, 1)
   zzi = zz - derzz
   call droikiforT0( zzi, tt, ss, wz, wt, vst, fm, 2)
   g(1) = ( fp - fm ) / derzz / 2.d0
   dzi = dz + derdz
   wn = 1.d0 / sqrt( dzi**2 + dt**2 )
   wz = dzi * wn
   wt = dt  * wn   
   call droikiforT0( zz, tt, ss, wz, wt, vst, fp, 2)
   dzi = dz - derdz
   wn = 1.d0 / sqrt( dzi**2 + dt**2 )
   wz = dzi * wn
   wt = dt  * wn   
   call droikiforT0( zz, tt, ss, wz, wt, vst, fm, 2)
   g(2) = ( fp - fm ) / derdz / 2.d0
!
 END SUBROUTINE gcnmdt
!
!
!> copy of droiki.F90, with some simplifications for the t0 fit
!
 SUBROUTINE DROIKIforT0(ZZ,TT,SS,WZ0,WT0,VST, CTOT, iflg)
 USE M_MB_PACONS
 USE M_MB_COTEMH
 USE M_MB_SegmentT0
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: iflg
 REAL(8), INTENT(IN)  :: ZZ, TT, SS, WZ0, WT0, VST
 REAL(8), INTENT(OUT) :: CTOT
 REAL(8) :: WZ, WT, CCC1, CCC2, CCCC
 INTEGER :: NNN1, NNN2, NNNN, II, JKI0
!
   if( iflg == 1 ) NhitPrevious = 0
   CTOT = 999999.D0
   IF( WT0 < 0.D0 ) THEN
     WZ = - WZ0
     WT = - WT0
   ELSE
     WZ =   WZ0
     WT =   WT0
   ENDIF
   IF( ITEMC(2)*ITEMC(5) >= 1 ) THEN
     CALL CHIHITforT0(2, ZZ,TT,SS,WZ,WT,VST, 10, NNN1,CCC1, iflg)
     CALL CHIHITforT0(5, ZZ,TT,SS,WZ,WT,VST, 10, NNN2,CCC2, iflg)
     NNNN = NNN1 + NNN2
     CTOT = (CCC1+CCC2) / (NNNN-2.D0)
   ELSE
     IF( ITEMC(2) >= 1 ) THEN
       II = 1
     ELSEIF( ITEMC(5) >= 1 ) THEN
       II = 2
     ELSE
       RETURN
     ENDIF
     JKI0 = 3*II-1
     CALL CHIHITforT0(JKI0,ZZ,TT,SS,WZ,WT,VST,  1, NNNN,CCCC, iflg)
     CTOT = CCCC / (NNNN-2.D0) + 0.15D0*SIGMOK**2
   ENDIF
!
 END SUBROUTINE DROIKIforT0
!
!
!> copy of chihit.F90, with some simplifications for the t0 fit
!>   - do not save DCApoin
!>   - do not test on KTUBHI
!>   - looser cut on LTUBHI, and restore the previous value at each iteration
!>   - use rfttub instead of rftfast
!> iflg = 0  means recompute radius
!> iflg /= 0 means recompute and reuse radius for following use
!> iflg = 1  means reset the number of radius kept
!
 SUBROUTINE CHIHITforT0(JKI0,Ztrue,Ttrue,Strue,WZtrue,WTtrue,VSTtrue, KMA, NTOT,CTOT, iflg)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS, ONLY : C1P000
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_COTEMH
 USE M_MB_SegmentT0
 IMPLICIT NONE
 INTEGER,        INTENT(IN)  :: JKI0, KMA, iflg
 REAL(8),        INTENT(IN)  :: Ztrue, Ttrue, Strue, WZtrue, WTtrue, VSTtrue
 INTEGER,        INTENT(OUT) :: NTOT
 REAL(8),        INTENT(OUT) :: CTOT
 TYPE(DcaPoint) :: dcapoin
 LOGICAL :: NAKCHA, HasDeadTube, IsTubeDead, IsTubeDisabled
 INTEGER :: NEGA, IC, IMDT0, Itrans, JKIMI, JKIMA, NTUT
 INTEGER :: IP, JKI, INOZ, IMDT, Ident, IGEOM, IZ0, IZ0LIM, IZ, IIII, KKKK
 INTEGER :: ITDEB, ITFIN, IT, ITUT, Status, k
 REAL(8) :: T0, SIGT, VStrue, ZZ, TOLZ, TOLS, TOLZM, TOLSM, Z, T
 REAL(8) :: WZ, WT, VST, SIGZ, ZOLTUB, TTUT, STUT, TWZ
 REAL(8) :: ZTUT, RTAT, ARTAT, RTUT, TUBRES, FRESEG, DDDD 
 REAL(8) :: RRR,RDED, DTEST, CHICUR, SBt,ZBt,TBt 
 REAL(8) :: Zk6(6),Tk6(6),Sk6(6),WZk6(6),WTk6(6),VSTk6(6),VZTk6(6)
 REAL(8) :: RFTTUB
 REAL(8), PARAMETER :: WZLIM = 0.85D0
 REAL(8) :: TWOPASTUB
 REAL(8) :: InvTUBRES
 REAL(8) :: Z1DCA, T1DCA, S1DCA 
 REAL(8) :: XXXDCA, YYYDCA, ZZZDCA
 REAL(8) :: XXRDCA, YYRDCA, ZZRDCA, SRDCA
 REAL(8) :: XWiCen, YWiCen, ZWiCen
 REAL(8) :: XWiDir, YWiDir, ZWiDir
 REAL(8) :: dirx, diry, dirz
 LOGICAL :: isBarrel
 INTEGER :: intpos
!
!>> Warning :  JKI0  must be equal to  2  or  5   !!!
!>>           ******                  ***    ***
!>>
!>>     and :  WTtrue  must be  > 0
!>>           ********         *****
!
   NTOT = 0
   CTOT = 0.D0
   NEGA = 0
   CALL ResetDCA(dcapoin)
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
         IF( RTAT < 0.D0 ) THEN
           KKKK = 1
         ELSE
           KKKK = 2
         ENDIF
!
         IF( NTEMHI(JKI,IP,IZ) >= 1 ) THEN
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
           Status = LTUBHI(IT)
           RTUT   = ARTAT
           IF( Status == 1 ) THEN
             IF( iflg >= 2 ) THEN
               k = intpos(IT, HitPrevious, NhitPrevious)
               IF( k >= 1 ) THEN
                 RRR    = RadPrevious(k)
                 Status = StaPrevious(k)
                 GO TO 10
               ENDIF
             ENDIF
             Ident = ITUBHI(IT)
             RRR  = RFTTUB(Ident, Status, RTUT,                              &
                           XXRDCA, YYRDCA, ZZRDCA, dirx  , diry  , dirz  ,  &
                           XWiCen, YWiCen, ZWiCen, XWiDir, YWiDir, ZWiDir)
             IF( iflg >= 1 ) THEN
               IF( NHitPrevious < maxPrevious ) THEN
                 NHitPrevious = NHitPrevious + 1
                 HitPrevious(NHitPrevious) = IT
                 RadPrevious(NHitPrevious) = RRR
                 StaPrevious(NHitPrevious) = Status
               ENDIF
             ENDIF
           ELSE
             RRR = 99999.D0
           ENDIF
10         IF( Status >= 3 )     CYCLE
           RDED = RRR + REDTUB(IMDT)
           IF( Status == 1 .AND. RTUT < RRR + SIGMOK*TUBRES ) THEN
             DTEST = ( (RTUT-RRR) *InvTUBRES )**2
           ELSEIF( Status /= 1 .OR. RTUT < RDED ) THEN
             DTEST =  SIGMOK**2
           ELSE
             DTEST =((RTUT + SIGMOK*TUBRES - RDED) *InvTUBRES)**2
             DTEST = CHICUR( DTEST , CHINEF*0.5d0 )
           ENDIF
           IF( DTEST >= DDDD ) CYCLE
           DDDD = DTEST
           IIII =  IT
           IF( DTEST > SIGMOK**2-0.001D0 ) IIII = -IT
           IF( Status == 1  ) then
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
20     ENDDO
30   ENDDO
!
     IF( NTUT < 1 ) THEN
       NTUT = 1
       dcapoin%CCC(IP,NTUT) = SIGMOK*SIGMOK
       dcapoin%III(IP,NTUT) = -10000000
       dcapoin%JJJ(IP,NTUT) = 0
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
!
 END SUBROUTINE CHIHITforT0
!
