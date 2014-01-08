!
!> \author M.Virchaux
!
 SUBROUTINE TRAFAX_P(ICAN,NSENS,RTOWIR,FTOWIR,ETOWIR,WTOWIR,STOWIR,UTOWIR)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 USE M_MB_General_CONSTANTS, ONLY : C1P000
 USE M_MB_MuGeom
 USE M_MB_CTRABAZ
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ICAN, NSENS
 REAL(8), INTENT(OUT) :: RTOWIR(*), FTOWIR(*), ETOWIR(*), WTOWIR(*), STOWIR(*), UTOWIR(*)
 INTEGER :: IZTPLA, L, ICODE, IC, IW, IP, INOF, INOZ, I, IGEOM
 INTEGER :: IDU1,IDU2,IDU3,IDU4,IDU5, IC0, INOF0, INOZ0, ICT0
 INTEGER :: IZTPLA0, ICT, IGEOM0, IMDT, Itrans, IT, IRPC, IS
 INTEGER :: Izba1, Jz, Isba1, Js, Jcsc, intpos
 REAL(8) :: TTZZ, ZZTT, VVTZ, VVZT, SSSS, VVSS, DIREC
 REAL(8) :: SS, ZZ, TT, WZ, WT, UZT, UST, Z0
 REAL(8) :: S1P, S1N, S2P, S2N, AA, AM1, AP1, ASS, SQR12
 REAL(8) :: TTUT, T, Z, S, DZ, Slayer, Zlayer, Tlayer
!REAL(8) :: ABSTOWIR, FREREC
 REAL(8), PARAMETER :: BIG = 9999999.d0
 REAL(8) :: consecutiveStrips
!
!----Ali
 INTEGER :: ISC, itemp
 REAL(8) :: ZTUT
 REAL(8) :: Zfix, Tfix
 REAL(8) :: Ali_Zshit, Ali_Angle
 REAL(8) :: Ali_cos, Ali_sin , T0
 REAL(8) :: ZZ_L, TT_L, UZT_L 
 REAL(8) :: WT_L, WZ_L
!---- 
   SQR12 = SQRT(12.d0)
   RTOWIR(1:NZXCAN(ICAN)) = BIG
   FTOWIR(1:NZXCAN(ICAN)) = BIG
   ETOWIR(1:NZXCAN(ICAN)) = BIG
   WTOWIR(1:NZXCAN(ICAN)) = BIG
   STOWIR(1:NSXCAN(ICAN)) = BIG
   UTOWIR(1:NSXCAN(ICAN)) = BIG
!
!>> Set common CTRABAZ
     IZTPLA = 0
b10: DO 10 L=1,NZXCAN(ICAN)
       ICODE = KTUBHI(IZXCAN(L,ICAN))
       IC =  ICODE / 10000
       IW = (ICODE - IC*10000) / 10
       INOF = IFCHA(IC,IW)
       INOZ = IZCHA(IC,IW)
       IGEOM = IGEOMA(IC,INOF,INOZ)
!----Ali
         if (DoAligFit > 0) then
           if ( TrafaxAli_CollectFix == 1) then
             if( IC <= NCOUCH .OR. RTUBHI(IZXCAN(L,ICAN)) > -10.d0 ) then    ! only Z strips for CSC
               ISC = ( IC + 1 )/2
               TrafaxAli_NStations(ISC,INOF,INOZ) = TrafaxAli_NStations(ISC,INOF,INOZ) + 1
               TrafaxAli_Zfix     (ISC,INOF,INOZ) = TrafaxAli_Zfix     (ISC,INOF,INOZ) + ZTUBHI(IZXCAN(L,ICAN))
               TrafaxAli_Tfix     (ISC,INOF,INOZ) = TrafaxAli_Tfix     (ISC,INOF,INOZ) + TCECHAV(IGEOM)
             endif
           endif
         endif
!----
       DO I=1,IZTPLA
         CALL DECOIN(I, IDU1,IDU2,IDU3,IDU4,IDU5,IC0,INOF0,INOZ0,ICT0)
         IF(IC == IC0 .AND. INOF == INOF0 .AND. INOZ == INOZ0) CYCLE b10
       ENDDO
       IZTPLA = MIN( IZTPLA+1 , MAXPLA )
       CF0PLA(IZTPLA) = COSCHAV(IGEOM)
       SF0PLA(IZTPLA) = SINCHAV(IGEOM)
       TT0PLA(IZTPLA) = TCECHAV(IGEOM)
       IF( IC <= NCOUCH ) THEN
         CA0PLA(IZTPLA) = 1.D0
         SA0PLA(IZTPLA) = 0.D0
       ELSE
         CA0PLA(IZTPLA) = COS(GCECHAV(IGEOM))
         SA0PLA(IZTPLA) = SIN(GCECHAV(IGEOM))
       ENDIF
       CALL CODEIN(0,0,0,0,0,0,IC,INOF,INOZ,  0, IZTPLA)
10   ENDDO b10
     IZTPLA0 = IZTPLA + 1
b20: DO 20 L=1,NSXCAN(ICAN)
       ICODE = KBANHI(ISXCAN(L,ICAN)) / 10
       ICT =  ICODE / 100
       DO I=IZTPLA0,IZTPLA
         CALL DECOIN(I, IDU1,IDU2,IDU3,IDU4,IDU5,IC0,INOF0,INOZ0,ICT0)
         IF(ICT == ICT0) CYCLE b20
       ENDDO
       IZTPLA = MIN( IZTPLA+1 , MAXPLA )
       CF0PLA(IZTPLA) = COSTRI(ICT)
       SF0PLA(IZTPLA) = SINTRI(ICT)
       TT0PLA(IZTPLA) = TCETRI(ICT)
       CA0PLA(IZTPLA) = 1.D0
       SA0PLA(IZTPLA) = 0.D0
       CALL CODEIN(0,1,0,0,0,0, 0,   0,   0,ICT, IZTPLA)
20   ENDDO b20
!>> Total number of planes
     NZTPLA = IZTPLA
!
!>> Track and catch the crossings
   CALL TRABAZ(1,NSENS,1)
!
!>> Loop on crossings
   DO 80 IZTPLA=1,NZTPLA
     IF( ICRPLA(IZTPLA) < 1 ) CYCLE
     CALL DECOIN(IZTPLA,IDU1,IDU2,IDU3,IDU4,IDU5,IC0,INOF0,INOZ0,ICT0)
     TTZZ  = TT0PLA(IZTPLA)
     ZZTT  = ZTTPLA(IZTPLA,1)
     VVTZ  = VTZPLA(IZTPLA,1)
     VVZT  = VZTPLA(IZTPLA,1)
     SSSS  = SSSPLA(IZTPLA,1)
     VVSS  = VSSPLA(IZTPLA,1)
     IF( ICT0 < 1 ) THEN
       IGEOM0= IGEOMA(IC0,INOF0,INOZ0)
       IMDT  = ISTMDTV(IGEOM0)
       DIREC = SIGN( C1P000, TCECHAV(IGEOM0) )
       Itrans= ItransCHA(IGEOM0)
       CALL SZTWU_Ideal_from_True(Itrans,SSSS,ZZTT,TTZZ,VVSS,VVZT,VVTZ, SS,ZZ,TT,WZ,WT,UZT,UST)
       IF( WT < 0.D0 ) THEN ! Ensure that direction is always outgoing (WT > 0)
         WZ = -WZ
         WT = -WT
       ENDIF
       Jcsc = intpos(IGEOM0,IGEOMCSC,NChaCSC)
       DO L=1,NZXCAN(ICAN)
         IT = IZXCAN(L,ICAN)
         ICODE = KTUBHI(IT)
         IC    =  ICODE / 10000
         IF( IC /= IC0 )     CYCLE
         IW    = (ICODE - IC*10000) / 10
         INOF  = IFCHA(IC,IW)
         IF( INOF /= INOF0 ) CYCLE
         INOZ  = IZCHA(IC,IW)
         IF( INOZ /= INOZ0 ) CYCLE
         IP    =  ICODE - IC*10000 - IW*10
         IF( IC <= NCOUCH ) THEN
!>> Compute RTOWIR,ETOWIR for the MDTs
           TTUT = TTZZ + DIREC*TLTUB(IMDT,IP,1)
           ZTUT = ZTUBHI(IT)
           ZZ_L  = ZZ
           TT_L  = TT
           WT_L  = WT
           WZ_L  = WZ
!----Ali
           if (DoAligFit > 0) then
             ISC = ( IC + 1 )/2
             iTemp = TrafaxAli_2Valid(ISC,INOF,INOZ)
             if ( itemp .ne. 0 ) then
               if  ( TrafaxAli_ApplyAliCor(itemp) == 1 ) then
                 if  ( TrafaxAli_IZ ( iTemp) == 1 .or. TrafaxAli_IA ( iTemp) == 1 ) then
                   Zfix = TrafaxAli_Zfix(ISC,INOF,INOZ)
                   Tfix = TrafaxAli_Tfix(ISC,INOF,INOZ)
                   Ali_Zshit = 0.d0
                   if  ( TrafaxAli_IZ ( iTemp)  == 1 ) then
                     Ali_Zshit = TrafaxAli_WDZ(iTemp) * TrafaxAli_DZ(iTemp)
                   endif
                   Ali_Angle = 0.d0
                   if ( TrafaxAli_IA ( iTemp) == 1 ) then
                     Ali_Angle = TrafaxAli_WDA(iTemp) * TrafaxAli_DA(iTemp)
                   endif
                   Ali_cos = cos(Ali_Angle)
                   Ali_sin = sin(Ali_Angle)
                   ZZ_L = Zfix + Ali_cos*( ZZ - Zfix ) + Ali_sin*( TT - Tfix )
                   TT_L = Tfix - Ali_sin*( ZZ - Zfix ) + Ali_cos*( TT - Tfix )
                   ZZ_L = ZZ_L + Ali_Zshit
                   WZ_L =  Ali_cos*WZ + Ali_sin*WT
                   WT_L = -Ali_sin*WZ + Ali_cos*WT
                 endif
               endif
             endif
           endif
!----           
           RTOWIR(L) = (ZTUT-ZZ_L)*WT_L - (TTUT-TT_L)*WZ_L
           FTOWIR(L) = SS + (TTUT-TT_L) * UST
!          ABSTOWIR  = ABS( RTOWIR(L) )
!          ETOWIR(L) = FREREC( IGEOM0,IC,INOF,INOZ , ABSTOWIR )
           ETOWIR(L) = 0.d0  ! not used any more, so do not loose time computing it
           WTOWIR(L) = WT
         ELSE
!>> Compute RTOWIR,ETOWIR for the CSCs
           T0 = TTZZ + DIREC*(TLTUB(IMDT,IP,1)+TLTUB(IMDT,IP,2))/2.d0
           ZZ_L  = ZZ
           TT_L  = TT
           UZT_L = UZT
!----Ali
           if (DoAligFit > 0) then
             ISC = ( IC + 1 )/2
             iTemp = TrafaxAli_2Valid(ISC,INOF,INOZ)
             if ( itemp .ne. 0 ) then
               if  ( TrafaxAli_ApplyAliCor(itemp) == 1 ) then
                 if  ( TrafaxAli_IZ ( iTemp) == 1 .or. TrafaxAli_IA ( iTemp) == 1 ) then
                   Zfix = TrafaxAli_Zfix(ISC,INOF,INOZ)
                   Tfix = TrafaxAli_Tfix(ISC,INOF,INOZ)
                   Ali_Zshit = 0.d0
                   if  ( TrafaxAli_IZ ( iTemp)  == 1 ) then
                     Ali_Zshit = TrafaxAli_WDZ(iTemp) * TrafaxAli_DZ(iTemp)
                   endif
                   Ali_Angle = 0.d0
                   if ( TrafaxAli_IA ( iTemp) == 1 ) then
                     Ali_Angle = TrafaxAli_WDA(iTemp) * TrafaxAli_DA(iTemp)
                   endif 
                   Ali_cos = cos(Ali_Angle)
                   Ali_sin = sin(Ali_Angle)
                   ZZ_L = Zfix + Ali_cos*( ZZ - Zfix ) + Ali_sin*( TT - Tfix )
                   TT_L = Tfix - Ali_sin*( ZZ - Zfix ) + Ali_cos*( TT - Tfix )
                   ZZ_L = ZZ_L + Ali_Zshit
                   UZT_L =             Ali_cos*UZT + Ali_sin 
                   UZT_L =  UZT_L / (  Ali_cos     - Ali_sin*UZT )              
                 endif
               endif
             endif
           endif
!----
           T = T0 
           Z = ZZ_L + (T0-TT_L) * UZT_L
           S = SS   + (T0-TT_L) * UST
           CALL Pos_Layer_from_Csc(Jcsc,IP,S,Z,T,Slayer,Zlayer,Tlayer)
           IF( RTUBHI(IT) < -10.D0 ) THEN
             RTOWIR(L) = Slayer
             FTOWIR(L) = Zlayer
           ELSE IF( RTUBHI(IT) < 0.D0 ) THEN
             RTOWIR(L) = Zlayer
             FTOWIR(L) = Slayer
           ENDIF
           ETOWIR(L) = DTUBHI(IT)
           WTOWIR(L) = 1.d0
         ENDIF
       ENDDO
     ELSE
!>> Compute STOWIR,UTOWIR for the RPC/TGCs
       IRPC  = IRPTGC(ICT0)
       Itrans= ItransTRI(ICT0)
       DIREC = SIGN( C1P000, TCETRI(ICT0) )
       CALL SZTWU_Ideal_from_True(Itrans,SSSS,ZZTT,TTZZ,VVSS,VVZT,VVTZ, SS,ZZ,TT,WZ,WT,UZT,UST)
       Z0    = ZCETRI(ICT0)
       DZ    = ZLOTRI(ICT0) / 2.D0
       S1P   = SPITRI(ICT0)
       S1N   = SNITRI(ICT0)
       S2P   = SPATRI(ICT0)
       S2N   = SNATRI(ICT0)
       AA    = (S2P-S2N) / (S1P-S1N)
       AM1   = AA - 1.D0
       AP1   = AA + 1.D0
       ASS   = AA * (S2P-S1P)
       DO L=1,NSXCAN(ICAN)
         IS = ISXCAN(L,ICAN)
         ICODE = KBANHI(IS) / 10
         ICT =  ICODE / 100
         IF( ICT /= ICT0 )   CYCLE
         IP  =  ICODE - ICT*100
         T = TTZZ + DIREC*TBAND(IRPC,IP)
         S = SS + (T-TT) * UST
         Z = ZZ + (T-TT) * UZT
         IF( ISZBAND(IRPC,IP,2) < 1 ) THEN
           IF( IP <= N1BAND(IRPC) ) THEN
             STOWIR(L) = S + (Z-Z0) * (ASS+AM1*(S-S2P)) / (AM1*(Z0-Z)-AP1*DZ)
             UTOWIR(L) = DSBAND(IRPC) / SQR12
           ELSE
             STOWIR(L) = Z
             UTOWIR(L) = DZBAND(IRPC) / SQR12
           ENDIF
         ELSE
!>> Special case of variable  strip  or  wire gang  width
           IF( IP <= N1BAND(IRPC) ) THEN
             STOWIR(L) = S + (Z-Z0) * (ASS+AM1*(S-S2P)) / (AM1*(Z0-Z)-AP1*DZ)
             Isba1 = ISZBAND(IRPC,IP,1)
             Js    = ABS(LBANHI(Is)) / 10
             IF( Js < 1 ) THEN
               UTOWIR(L) = DSBAND(IRPC) / SQR12
             ELSE
               UTOWIR(L) = (SZBAND(Isba1+Js)-SZBAND(Isba1+Js-1)) / SQR12
             ENDIF
           ELSE
             STOWIR(L) = Z
             Izba1 = ISZBAND(IRPC,IP,1)
             Jz    = ABS(LBANHI(Is)) / 10
             IF( Jz < 1 ) THEN
               UTOWIR(L) = DZBAND(IRPC) / SQR12
             ELSE
               UTOWIR(L) = (SZBAND(Izba1+Jz)-SZBAND(Izba1+Jz-1)) / SQR12
             ENDIF
           ENDIF
         ENDIF
         UTOWIR(L) = UTOWIR(L) * consecutiveStrips(IS,ICT,IP)
       ENDDO
     ENDIF
80 ENDDO
!
 END SUBROUTINE TRAFAX_P
!
!
!> give the number of consecutive hit strips
!
 REAL(8) FUNCTION consecutiveStrips(is,ict,ip)
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: is, ict, ip
 INTEGER :: strip, ipside, i, j
!
  consecutiveStrips = 1.d0
  strip  = abs(lbanhi(is))/10
  ipside = mod(kbanhi(is),10)
  j = strip + 1
  do i=is+1,nbanh0(ict,ip)+nbanhi(ict,ip)
    if( mod(kbanhi(i),10) /= ipside ) exit
    if( abs(lbanhi(i))/10 == j ) then
      consecutiveStrips = consecutiveStrips + 1.d0
      j = j + 1
    else
      if( abs(lbanhi(i))/10 < j ) write(*,*) ' MuonboyCore/consecutiveStrips problem + : ',strip,j,lbanhi(i)/10
      exit
    endif
  enddo
  j = strip - 1
  do i=is-1,nbanh0(ict,ip)+1,-1
    if( mod(kbanhi(i),10) /= ipside ) exit
    if( abs(lbanhi(i))/10 == j ) then
      consecutiveStrips = consecutiveStrips + 1.d0
      j = j - 1
    else
      if( abs(lbanhi(i))/10 > j ) write(*,*) ' MuonboyCore/consecutiveStrips problem - : ',strip,j,lbanhi(i)/10
      exit
    endif
  enddo
!
 END FUNCTION consecutiveStrips
!
