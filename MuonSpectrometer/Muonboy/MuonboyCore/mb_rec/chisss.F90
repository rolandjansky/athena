!
!> Compute the crossing points of tracks with trigger stations (RPC and TGC)
!> and their contributions to the Minuit chi2
!> \author M.Virchaux
!
SUBROUTINE CHISSS(ICT,ZZtrue,SStrue,VZtrue,VTtrue,VStrue,  NTOT,CTOT, dcastr)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_Types
 USE M_MB_General_CONSTANTS
 USE M_MB_MuGeom
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ICT
 REAL(8), INTENT(IN)  :: ZZtrue,SStrue,VZtrue,VTtrue,VStrue
 INTEGER, INTENT(OUT) :: NTOT
 REAL(8), INTENT(OUT) :: CTOT
 TYPE(DcaStrip), INTENT(OUT) :: dcastr
 REAL(8), PARAMETER :: TolAliZ=0.1d0  !< tolerance on Z alignement
 REAL(8), PARAMETER :: TolAliS=0.2d0  !< tolerance on S alignement
!
!
 INTEGER :: Irpc, Itrans, Ip, IPside, Ip1, ISDEB, ISFIN, Is, Icode
 INTEGER :: IPsideIS, Izba1, JzMin, JzNex, nedge, iedge
 REAL(8) :: TTtrue, SS, ZZ, TT, WZ, WT, UZT, UST
 REAL(8) :: TOLZ, TOLS, Z0, DZ, S1P, S1N, S2P, S2N, S00
 REAL(8) :: AA, AM1, AP1, ASS, SIGT, S, Z, T, Xs, Ds, DT
 REAL(8) :: Stest, DiffMin, DiffNex, Diff, DsMin, HalfDsMin, HalfDsNex
 REAL(8) :: Xr, Xt, transition
 LOGICAL :: NOKTRI, NAKTRI
 INTEGER :: JFIRST
 REAL(8) :: CHICUR, Resol
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 REAL(8) :: SBt,ZBt,TBt
 REAL(8) :: SiFFi,CoFFi
 REAL(8) :: XXXDCA, YYYDCA, ZZZDCA
 REAL(8) :: dirx, diry, dirz
 LOGICAL :: isBarrel
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
   NTOT    = 0
   CTOT    = 0.d0
!
   IF( isOffTrig(ICT) )              RETURN
   Irpc  = IRPTGC(ICT)
   IF( N2BAND(Irpc) < 1 )            RETURN
   Itrans = ItransTRI(ICT)
   TTtrue = TCETRI(ICT)
   CALL SZTWU_Ideal_from_True(Itrans, SStrue,ZZtrue,TTtrue,VStrue,VZtrue,VTtrue, SS,ZZ,TT,WZ,WT,UZT,UST)
   TOLZ = TolAliZ
   TOLS = TolAliS
   ZZ = ZZ + UZT*(TTtrue-TT)
   SS = SS + UST*(TTtrue-TT)
   TT = TTtrue
   IF( NOKTRI(ICT,ZZ,SS,TOLZ,TOLS) ) RETURN
!
   Z0  = ZCETRI(ICT)
   DZ  = ZLOTRI(ICT) *0.5D0
   S1P = SPITRI(ICT)
   S1N = SNITRI(ICT)
   S2P = SPATRI(ICT)
   S2N = SNATRI(ICT)
   S00 = (S1P+S2P+S1N+S2N) * 0.25D0
   AA  = (S2P-S2N) / (S1P-S1N)
   AM1 = AA - 1.D0
   AP1 = AA + 1.D0
   ASS = AA * (S2P-S1P)
!>> SIGT < 0 if one is in the "negative" end cap
   SIGT = SIGN( C1P000, TTtrue )
!
   SiFFi = SINTRI(ICT)
   CoFFi = COSTRI(ICT)
   isBarrel = ICT<=NTRIBA
   CALL MuboSZTtoXYZ( isBarrel,SiFFi,CoFFi, VStrue,VZtrue,VTtrue, dirx,diry,dirz)
   IF( WT < 0.d0) THEN
     dirx = - dirx
     diry = - diry
     dirz = - dirz
   endif
   dirx = SIGT * dirx
   diry = SIGT * diry
   dirz = SIGT * dirz
!
   nedge = 0
   DO Ip=1,N2BAND(Irpc)
     IF( .NOT.isBarrel ) THEN
       IF( ISZBAND(Irpc,Ip,2) <= 0 )    CYCLE
     ENDIF
     DT = SIGT*TBAND(Irpc,Ip)
     T  = TT + DT
     Z  = ZZ + DT*UZT
     S  = SS + DT*UST
     TOLZ =  TolAliZ
     TOLS =  TolAliS
     IF( NAKTRI(ICT,Ip,Z,S,TOLZ,TOLS) ) CYCLE
     TOLZ = -TolAliZ
     TOLS = -TolAliS
     iedge = 0
     IF( NAKTRI(ICT,Ip,Z,S,TOLZ,TOLS) ) iedge = 1
     NTOT = NTOT + 1
!
     IPside = 0
     IF( Ip <= N1BAND(Irpc) ) THEN
!>> 2nd coordinate strips are along S
       Xs = S + (Z-Z0) * (ASS+AM1*(S-S2P)) / (AM1*(Z0-Z)-AP1*DZ)
       Ds = DSBAND(Irpc)
       Resol = 1.01D0 ! not clear which reso should be taken ... use it to distinguish S and Z strips
     ELSE
!>> 1st coordinate strips are along Z
       Xs = Z
       Ds = DZBAND(Irpc)
       Ip1 = MAX( 1 , Ip-N1BAND(Irpc) )
       IF( OBAND(Irpc,Ip1,2) > -900.D0 ) THEN
         Stest = S + (Z-Z0) * (ASS+AM1*(S-S2P)) / (AM1*(Z0-Z)-AP1*DZ)
         IF( Stest < S00-Ds ) IPside = 1
         IF( Stest > S00+Ds ) IPside = 2
       ENDIF
       Resol = 0.99D0 ! not clear which reso should be taken ... use it to distinguish S and Z strips
     ENDIF
!
     IF( saveDcaTrk ) THEN
       CALL Pos_True_from_Ideal( Itrans, S,Z,T, SBt,ZBt,TBt )
       CALL MuboSZTtoXYZ( isBarrel, SiFFi ,CoFFi , SBt,ZBt,TBt, XXXDCA,YYYDCA,ZZZDCA )
       dcastr%XDCA(NTOT)   = XXXDCA
       dcastr%YDCA(NTOT)   = YYYDCA
       dcastr%ZDCA(NTOT)   = ZZZDCA
       dcastr%XDir(NTOT)   = dirx
       dcastr%YDir(NTOT)   = diry
       dcastr%ZDir(NTOT)   = dirz
       dcastr%EneDCA(NTOT) = 99999.D0
       dcastr%ResDCA(NTOT) = resol
     ENDIF
!
     JzMin = -99
     JzNex = -99
     DiffMin = 999.D0
     DiffNex = 999.D0
     HalfDsMin = 999.D0
     ISDEB = NBANH0(ICT,Ip) + 1
     ISFIN = NBANH0(ICT,Ip) + NBANHI(ICT,Ip)
     IF( ISZBAND(Irpc,Ip,2) < 1 ) THEN
!>> General case
       DO Is=ISDEB,ISFIN
!        IF( rejectStrip(Is) )      CYCLE
         Icode    = KBANHI(Is) / 10
         IPsideIS = KBANHI(Is) - 10*Icode
         IF( IPsideIS+IPside == 3 ) CYCLE
         Diff = ABS( Xs - SBANHI(Is) )
         IF( Diff < DiffMin ) THEN
           DiffNex = DiffMin
           DiffMin = Diff
           dcastr%JJJ(NTOT) = JBANHI(Is)
           dcastr%III(NTOT) = Is
           JzNex   = JzMin
           JzMin   = 1
         ELSE IF( Diff < DiffNex ) THEN
           DiffNex = Diff
           JzNex   = 1
         ENDIF
       ENDDO
       DsMin     = Ds
       HalfDsMin = Ds * 0.5D0
       HalfDsNex = HalfDsMin
     ELSE
!>> Special case of TGC's with variable  wire gang  or  strip  widths
       Izba1 = ISZBAND(Irpc,Ip,1)
       DO Is=ISDEB,ISFIN
!        IF( rejectStrip(Is) )      CYCLE
         Diff = ABS( Xs - SBANHI(Is) )
         IF( Diff < DiffMin ) THEN
           DiffNex = DiffMin
           DiffMin = Diff
           dcastr%JJJ(NTOT) = JBANHI(Is)
           dcastr%III(NTOT) = Is
           JzNex = JzMin
           JzMin = ABS(LBANHI(Is)) / 10
           IF( JzMin < 1 ) THEN
             DsMin = Ds
           ELSE
             DsMin = SZBAND(Izba1+JzMin)-SZBAND(Izba1+JzMin-1)
           ENDIF
           HalfDsNex = HalfDsMin
           HalfDsMin = DsMin * 0.5D0
         ELSE IF( Diff < DiffNex ) THEN
           DiffNex = Diff
           JzNex = ABS(LBANHI(Is)) / 10
           IF( JzNex < 1 ) THEN
             HalfDsNex = DZBAND(Irpc) * 0.5D0
           ELSE
             HalfDsNex = (SZBAND(Izba1+JzNex)-SZBAND(Izba1+JzNex-1)) * 0.5D0
           ENDIF
         ENDIF
       ENDDO
     ENDIF
     IF( JzMin >= 0 ) THEN
       IF( isBarrel ) THEN
         IF( Ip <= N1BAND(Irpc) ) THEN
           transition = AligRPCphi
         ELSE
           transition = AligRPCeta
         ENDIF
       ELSE
         IF( Ip <= N1BAND(Irpc) ) THEN
           transition = AligTGCphi
         ELSE
           transition = AligTGCeta
         ENDIF
       ENDIF
       IF( JzNex >= 0 .AND. DiffMin <= HalfDsMin .AND. DiffNex <= HalfDsMin+HalfDsNex ) THEN
         dcastr%CCC(NTOT) = 0.d0
       ELSE
         Xr =   DiffMin / HalfDsMin
         Xt = ( DiffMin - HalfDsMin ) / transition
         dcastr%CCC(NTOT) = Xr**2 * (2.D0+TANH(REAL(Xt,10)))    ! differences between slc5 and slc6 in 17.2.7.4 
       ENDIF
     ELSE
       dcastr%III(NTOT) = 0
       dcastr%JJJ(NTOT) = 0
       dcastr%CCC(NTOT) = 2.D0*CHINAS
     ENDIF
!
!- Possible inefficiencies
     IF( dcastr%CCC(NTOT) > CHINAS ) THEN
       nedge = nedge + iedge
       IF(JzMin >= 0)  dcastr%CCC(NTOT) = CHICUR( dcastr%CCC(NTOT) , CHINAS )
       dcastr%JJJ(NTOT) = -20000000 - JFIRST(dcastr%JJJ(NTOT))
       dcastr%III(NTOT) = - ICT - 100000*Ip
     ENDIF
     CTOT = CTOT + dcastr%CCC(NTOT)
   ENDDO
!
   IF( nedge >= NTOT-1 ) THEN
     NTOT = 0
     CTOT = 0.d0
   ENDIF
!
 END SUBROUTINE CHISSS
!
