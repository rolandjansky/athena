!
!> Put the trigger hits (RPC and TGC) information in a form suitable for Muonboy,
!> and store it in Muonboy commons by calling abanhi.F90
!
 SUBROUTINE DIGIBA(Ident,Jtyp,Jff,Jzz,Job,Jspli, Jsl,Jsz, Jstri,  &
                   Temps, XXsimu,YYsimu,ZZsimu,JJJ, AthPosOff)
 USE M_MB_Control
 USE M_MB_MuGeom
 IMPLICIT NONE 
 INTEGER, INTENT(IN)  :: Ident, Jtyp, Jff, Jzz, Job, Jspli
 INTEGER, INTENT(IN)  :: Jsl, Jsz, Jstri, JJJ, AthPosOff
 REAL(8), INTENT(IN)  :: Temps, XXsimu, YYsimu, ZZsimu
 REAL(8), PARAMETER :: EpsChk=0.0030D0
 REAL(8), PARAMETER :: VLum=29.9792458d0
 CHARACTER(32) :: WARN_mubo_amdb, WARN_simu_amdb, WARN_simu_mubo
 CHARACTER(3)  :: NameTYP, NameTEC
 LOGICAL :: NUKTRI, NOKTRI, OK_Warning
 LOGICAL :: L_RPC1, L_RPC2, L_TGC1, L_TGC2
 LOGICAL :: L_mubo_amdb, L_simu_amdb, L_simu_mubo
 REAL(8) :: XYZabs(3,4), SZTloc(3,4)
 REAL(8) :: ZZ0,TT0, DZZ, S1P,S1N,S2P,S2N, S0P,S0N,S00
 REAL(8) :: DCF,DSF, SIGT, DT0, Tcor
 REAL(8) :: XXamdb,YYamdb,ZZamdb,SSamdb,TTamdb,ZTamdb,TZamdb
 REAL(8) :: SSsimu,TTsimu,ZTsimu,TZsimu
 REAL(8) :: SSidea,ZTidea,TZidea, AA,AM1,AP1,ASS
 REAL(8) :: SSdigi,ZTdigi,TZdigi, WidthStri
 REAL(8) :: SSmubo,ZTmubo,TZmubo
 INTEGER :: IC,INOF,INOZ, ICT, IRPC, IP
 INTEGER :: Itran, IPside, JstriMax, IP1, Jz,Js
 INTEGER, SAVE :: Nb_Warn_RPC1 = 0, Max_Warn_RPC1 = 200
 INTEGER, SAVE :: Nb_Warn_RPC2 = 0, Max_Warn_RPC2 = 200
 INTEGER, SAVE :: Nb_Warn_TGC1 = 0, Max_Warn_TGC1 = 200
 INTEGER, SAVE :: Nb_Warn_TGC2 = 0, Max_Warn_TGC2 = 200
!===========================================
!> Jtyp,Jff,Jzz,Job  are the AMDC_SIMREC indexes defining the trigger chamber
!! Jspli  is the splitting index (for RPC's splitted in 2 or 4)
!!        = 1 is low S,  low Z
!!        = 2 is high S, low Z
!!        = 3 is low S,  high Z
!!        = 4 is high S, high Z
!!  Jsl   is the sensitive layer index (1 to 2 or 3)
!!  Jsz   is the "type of strips" index
!!        = 1 for S strips (Phi coordinate)
!!        = 2 for Z strips (Eta coordinate)
!! Jstri  is the hit strip number
!! Temps  is the measured time of this strip
!! XXsimu,YYsimu,ZZsimu  are the absolute coordinates of the center of the
!!        hit strip
!!  JJJ   is the "particle code" (5 or 6 if the digi. originate from a muon,
!!                                2 or 3 if from an electron,
!!                               99 or 88 if from gamma-neutron background... etc)
!!  AthPosOff = 0 means keep the position from Athena
!!            otherwise recompute positions of digits
!===========================================
!
    CALL FGetStationType(Jtyp, NameTYP)
    CALL FGetObjectTech(Jtyp,Jff,Jzz,Job, NameTEC)
!
!- Get the MUONBOY Trigger Chamber indexes from the AMDC_SIMREC ones
    CALL GETIND(Jtyp,Jff,Jzz,Job, IC,INOF,INOZ, ICT)
!
    IF( ICT <= 0 .OR. ( NAmeTEC /= 'RPC' .AND. NAmeTEC /= 'TGC') ) THEN
      PRINT 7770,NameTYP,Jff,Jzz,Job,Jspli,NAmeTEC
7770  FORMAT(/' In Muonboy/DIGIBA : ',                                  &
              'The object Typ,Jff,Jzz,Job,Jspli,Tec =  ',A3,4I4,2X,A3,  &
              '  does not correspond to a Muonboy Trigger Chamber',     &
              '  ===>  do not use it !!!',/)
      RETURN
    ENDIF
    IF( isOffTrig(ICT) ) THEN
      WRITE(*,*) ' Muonboy/digiba ERROR of trigger station Off ',ICT,' which has hits !!!'
    ENDIF
!
    IRPC = IRPTGC(ICT)
    IP   = Jsl + (Jsz-1) * N1BAND(IRPC)
    L_RPC1 = .FALSE.
    L_RPC2 = .FALSE.
    L_TGC1 = .FALSE.
    L_TGC2 = .FALSE.
    IF( NAmeTEC == 'RPC' .AND. Jsz == 2 ) THEN
!- RPC 1rst coordinate strip
      L_RPC1 = .TRUE.
      OK_Warning = Nb_Warn_RPC1 <= Max_Warn_RPC1
    ELSEIF( NAmeTEC == 'RPC' .AND. Jsz == 1 ) THEN
!- RPC 2nd coordinate strip
      L_RPC2 = .TRUE.
      OK_Warning = Nb_Warn_RPC2 <= Max_Warn_RPC2
    ELSEIF( NAmeTEC == 'TGC' .AND. Jsz == 2 ) THEN
!- TGC 1rst coordinate wire gang
      L_TGC1 = .TRUE.
      OK_Warning = Nb_Warn_TGC1 <= Max_Warn_TGC1
    ELSEIF( NAmeTEC == 'TGC' .AND. Jsz == 1 ) THEN
!- TGC 2nd coordinate strip
      L_TGC2 = .TRUE.
      OK_Warning = Nb_Warn_TGC2 <= Max_Warn_TGC2
    ELSE
      PRINT 7769,Jsz
7769  FORMAT(/' In Muonboy/DIGIBA, the index :  Jsz =',I4,  &
              '  is unknown  ===>  do not use it !!!',/)
      RETURN
    ENDIF
    IF( Jsl < 1 .OR. Jsl > N1BAND(IRPC) ) THEN
      PRINT 7768,Jsl
7768  FORMAT(/' In Muonboy/DIGIBA, the index :  Jsl =',I4,     &
              '  is outside range  ===>  do not use it !!!',/)
      RETURN
    ENDIF
!
!- Get some Muonboy variables
    ZZ0   = ZCETRI(ICT)
    TT0   = TCETRI(ICT)
    DZZ   = ZLOTRI(ICT) * 0.5d0
    S1P   = SPITRI(ICT)
    S1N   = SNITRI(ICT)
    S2P   = SPATRI(ICT)
    S2N   = SNATRI(ICT)
    Itran = ItransTRI(ICT)
    S0P   = (S1P+S2P) * 0.5d0
    S0N   = (S1N+S2N) * 0.5d0
    S00   = (S0P+S0N) * 0.5d0
    DCF   = COSTRI(ICT)
    DSF   = SINTRI(ICT)
    SIGT  = SIGN( 1.D0, TT0 )
    DT0   = SIGT * TBAND(IRPC,IP)
!
!- Get the input (i.e. from simulation) Strip position
!- both after (simu) and before (idea) displacements+deformations
    SSsimu   = DCF * YYsimu - DSF * XXsimu
    TTsimu   = DCF * XXsimu + DSF * YYsimu
    IF(ICT <= NTRIBA) THEN
      ZTsimu = ZZsimu
      TZsimu = TTsimu
    ELSE
      ZTsimu = TTsimu
      TZsimu = ZZsimu
    ENDIF
    CALL Pos_Ideal_from_True(Itran,SSsimu,ZTsimu,TZsimu,SSidea,ZTidea,TZidea)
    IF( L_TGC2 ) THEN
      AA  = (S2P-S2N) / (S1P-S1N)
      AM1 = AA - 1.D0
      AP1 = AA + 1.D0
      ASS = AA * (S2P-S1P)
      SSidea = SSidea + (ZTidea-ZZ0) * (ASS+AM1*(SSidea-S2P)) / (AM1*(ZZ0-ZTidea)-AP1*DZZ)
      ZTidea = ZZ0
      CALL Pos_True_from_Ideal(Itran,SSidea,ZTidea,TZidea,SSsimu,ZTsimu,TZsimu)
    ENDIF
!
!- Get the Muonboy Tube or Strip position
!- both before (digi) and after (mubo) displacements+deformations
    IPside = 0
    TZdigi = TT0+DT0
    IF( L_RPC1 ) THEN
!     IF( ZZ0 < 0.D0 ) THEN    13-03-2010  problem with BOG0 which have ZZ0=-0.7
      IF( ZZ0 < -3.d0 ) THEN
        ZTdigi = ZZ0+DZZ - Jstri*DZBAND(IRPC) - OBAND(IRPC,IP,1)
      ELSE
        ZTdigi = ZZ0-DZZ + Jstri*DZBAND(IRPC) + OBAND(IRPC,IP,1)
      ENDIF
      SSdigi = S00
      WidthStri = 2.D0*(DZZ-OBAND(IRPC,IP,1)) - DZBAND(IRPC)
      JstriMax  = NINT( WidthStri / DZBAND(IRPC) )         
      IP1 =MAX( 1 , IP-N1BAND(IRPC) )
      IF( OBAND(IRPC,IP1,2) > -900.D0 ) THEN
        IF( Jspli == 1 .OR. Jspli == 3 ) THEN
          IPside = 1
          SSdigi = (S00-OBAND(IRPC,IP1,2)+S1N+OBAND(IRPC,IP1,1))/2.D0
        ELSE
          IPside = 2
          SSdigi = (S00+OBAND(IRPC,IP1,2)+S1P-OBAND(IRPC,IP1,1))/2.D0
        ENDIF
      ENDIF
    ELSEIF( L_TGC1 ) THEN
      IF( ISZBAND(IRPC,IP,2) > 0 ) THEN
!- Special case of TGC's 1st coordinate strips (along T/Z) corresponding to variable wire gang width
        Jz = ISZBAND(IRPC,IP,1) + Jstri
        ZTdigi = ZZ0-DZZ + (SZBAND(Jz-1)+SZBAND(Jz))/2.D0 + OBAND(IRPC,IP,1)
        JstriMax  = ISZBAND(IRPC,IP,2) - ISZBAND(IRPC,IP,1)
      ELSE
!!$     ZTdigi = ZZ0-DZZ + Jstri*DZBAND(IRPC) + OBAND(IRPC,IP,1)
!!$     WidthStri = 2.D0*(DZZ-OBAND(IRPC,IP,1)) - DZBAND(IRPC)
!!$     JstriMax  = NINT( WidthStri / DZBAND(IRPC) )
        WRITE(*,*) ' Muonboy:digiba error : eta-wires of TGC with ISZBAND(IRPC,IP,2)=0'
        WRITE(*,*) ' Jtyp Jff Jzz Job Jspli Jsl Jsz Jstri ',Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
      ENDIF
      SSdigi = S00
    ELSEIF( L_RPC2 ) THEN
      IF( (Jspli == 2 .OR. Jspli == 4) .AND. OBAND(IRPC,IP,2) > -900.D0 ) THEN
        SSdigi = S00 + Jstri*DSBAND(IRPC) + OBAND(IRPC,IP,2)
        WidthStri = S0P - S00 - OBAND(IRPC,IP,1) - OBAND(IRPC,IP,2) - DSBAND(IRPC)          
      ELSE
        SSdigi = S0N + Jstri*DSBAND(IRPC) + OBAND(IRPC,IP,1)
        IF( OBAND(IRPC,IP,2) > -900.D0 ) THEN
          WidthStri = S00 - S0N - OBAND(IRPC,IP,1) - OBAND(IRPC,IP,2) - DSBAND(IRPC)
        ELSE
          WidthStri = S0P - S0N - 2.D0*OBAND(IRPC,IP,1) - DSBAND(IRPC)
        ENDIF
      ENDIF
      JstriMax  = NINT( WidthStri / DSBAND(IRPC) )
      ZTdigi = ZZ0
    ELSEIF( L_TGC2 ) THEN
      IF( ISZBAND(IRPC,IP,2) > 0 ) THEN
!- Special case of TGC's 2nd coordinate strips (along S/Phi) with variable width
        Js = ISZBAND(IRPC,IP,1) + Jstri
        SSdigi = S00 + (SZBAND(Js-1)+SZBAND(Js))/2.D0
        JstriMax  = ISZBAND(IRPC,IP,2) - ISZBAND(IRPC,IP,1)
      ELSE
!!$     SSdigi = S0N + Jstri*DSBAND(IRPC) + OBAND(IRPC,IP,1)
!!$     WidthStri = S0P-S0N-2.D0*OBAND(IRPC,IP,1) - DSBAND(IRPC)
!!$     JstriMax  = NINT( WidthStri / DSBAND(IRPC) )    
        WRITE(*,*) ' Muonboy:digiba error : phi-strip of TGC with ISZBAND(IRPC,IP,2)=0'
        WRITE(*,*) ' Jtyp Jff Jzz Job Jspli Jsl Jsz Jstri ',Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
      ENDIF
      ZTdigi = ZZ0
    ENDIF
    CALL Pos_True_from_Ideal(Itran,SSdigi,ZTdigi,TZdigi,SSmubo,ZTmubo,TZmubo)
!
    IF( MBChkDig > 0 .AND. OK_WARNING ) THEN
!
!-   Get Tube or Strip position directly from the Amdb Indexes
!-   after displacements+deformations (amdb)
      IF( L_RPC1 .OR. L_RPC2 ) THEN
        CALL FGetStripPositionFromRPCindex( NameTYP,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, XYZabs, SZTloc )
      ELSE
        CALL FGetChannelPositionFromTGCind( NameTYP,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, XYZabs, SZTloc )
      ENDIF
      XXamdb = (XYZabs(1,1)+XYZabs(1,2)+XYZabs(1,3)+XYZabs(1,4)) / 40.D0
      YYamdb = (XYZabs(2,1)+XYZabs(2,2)+XYZabs(2,3)+XYZabs(2,4)) / 40.D0
      ZZamdb = (XYZabs(3,1)+XYZabs(3,2)+XYZabs(3,3)+XYZabs(3,4)) / 40.D0
      SSamdb   = DCF * YYamdb - DSF * XXamdb
      TTamdb   = DCF * XXamdb + DSF * YYamdb
      IF(ICT <= NTRIBA) THEN
        ZTamdb = ZZamdb
        TZamdb = TTamdb
      ELSE
        ZTamdb = TTamdb
        TZamdb = ZZamdb
      ENDIF
      L_mubo_amdb = ABS(SSmubo-SSamdb) > EpsChk .OR. ABS(ZTmubo-ZTamdb) > EpsChk  &
                                                .OR. ABS(TZmubo-TZamdb) > EpsChk
      L_simu_amdb = ABS(SSsimu-SSamdb) > EpsChk .OR. ABS(ZTsimu-ZTamdb) > EpsChk  &
                                                .OR. ABS(TZsimu-TZamdb) > EpsChk
      L_simu_mubo = ABS(SSsimu-SSmubo) > EpsChk .OR. ABS(ZTsimu-ZTmubo) > EpsChk  &
                                                .OR. ABS(TZsimu-TZmubo) > EpsChk
      IF( L_simu_amdb .OR. L_simu_mubo .OR. L_mubo_amdb ) THEN
        IF( L_RPC1 ) THEN
          Nb_Warn_RPC1 = Nb_Warn_RPC1 + 1
          PRINT 7701
7701      FORMAT(/'==>   RPC 1rst co. (Z) strip  Digi to Muonboy (DIGIBA) : ',  &
                  ' Typ  Jff Jzz Job Jspli Jsl Jsz Jstri  Tec')
        ELSEIF( L_RPC2 ) THEN
          Nb_Warn_RPC2 = Nb_Warn_RPC2 + 1
          PRINT 7702
7702      FORMAT(/'==>  RPC 2nd co. (Phi) strip  Digi to Muonboy (DIGIBA) : ',  &
                  ' Typ  Jff Jzz Job Jspli Jsl Jsz Jstri  Tec')
        ELSEIF( L_TGC1 ) THEN
          Nb_Warn_TGC1 = Nb_Warn_TGC1 + 1
          PRINT 7703
7703      FORMAT(/'==>   TGC 1rst co. wire gang  Digi to Muonboy (DIGIBA) : ',  &
                  ' Typ  Jff Jzz Job Jspli Jsl Jsz Jstri  Tec')
        ELSE
          Nb_Warn_TGC2 = Nb_Warn_TGC2 + 1
          PRINT 7704
7704      FORMAT(/'==>  TGC 2nd co. (Phi) strip  Digi to Muonboy (DIGIBA) : ',  &
                  ' Typ  Jff Jzz Job Jspli Jsl Jsz Jstri  Tec')
        ENDIF
        WARN_mubo_amdb = '  OK !'
        WARN_simu_amdb = '  OK !'
        WARN_simu_mubo = '  OK !'
        IF( L_mubo_amdb  ) WARN_mubo_amdb = '  <=========  WARNING !'
        IF( L_simu_amdb  ) WARN_simu_amdb = '  <=========  WARNING !'
        IF( L_simu_mubo  ) WARN_simu_mubo = '  <=========  WARNING !'
        PRINT 7705,NameTYP,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,NAmeTEC
7705    FORMAT('==>  ************************',29X,A3,3I4,2I5,I4,I5,4X,A3)
        IF( Jstri < 1 .OR. Jstri > JstriMax ) PRINT 7706,JstriMax
7706    FORMAT('==>  Index  Jstri  is outside range :  1 ,',I4,  &
               '  <=====================  WARNING !')
        PRINT 7707,SSsimu*10., ZTsimu*10., TZsimu*10.,                                               &
                   SSamdb*10., ZTamdb*10., TZamdb*10.,                                               &
                   SSmubo*10., ZTmubo*10., TZmubo*10.,                                               &
                  (SSmubo-SSamdb)*10.,(ZTmubo-ZTamdb)*10.,(TZmubo-TZamdb)*10.,TRIM(WARN_mubo_amdb),  &
                  (SSsimu-SSamdb)*10.,(ZTsimu-ZTamdb)*10.,(TZsimu-TZamdb)*10.,TRIM(WARN_simu_amdb)
7707    FORMAT('==>  Simulation  SZT =',3F12.3,   &
              /'==>      Amdb    SZT =',3F12.3,   &
              /'==>    Muonboy   SZT =',3F12.3,   &
              /'==>  Mubo - Amdb SZT =',3F12.3,A, &
              /'==>  Simu - Amdb SZT =',3F12.3,A)
        IF( L_mubo_amdb ) PRINT 7708,(SSsimu-SSmubo)*10., (ZTsimu-ZTmubo)*10., (TZsimu-TZmubo)*10.,TRIM(WARN_simu_mubo)
7708    FORMAT('==>  Simu - Mubo SZT =',3F12.3,A)
        IF( Itran > 0 ) PRINT 7709,SSidea*10., ZTidea*10., TZidea*10., SSdigi*10., ZTdigi*10., TZdigi*10.
7709    FORMAT('==>  Idea (Simu) SZT =',3F12.3, &
                /'==>  Digi (Mubo) SZT =',3F12.3)
      ENDIF
!
!- One checks whether the hit is inside the trigger chamber area
      IF(  NUKTRI(ICT,ZTidea,SSidea,1.D0,1.D0) ) THEN
        IF( L_RPC1 )  Nb_Warn_RPC1 = Nb_Warn_RPC1 + 1
        IF( L_RPC2 )  Nb_Warn_RPC2 = Nb_Warn_RPC2 + 1
        IF( L_TGC1 )  Nb_Warn_TGC1 = Nb_Warn_TGC1 + 1
        IF( L_TGC2 )  Nb_Warn_TGC2 = Nb_Warn_TGC2 + 1
        PRINT 7772,NameTYP,Jff,Jzz,Job,Jspli,Jsl, ICT,ZTidea*10.,SSidea*10.
7772    FORMAT(/' In DIGIBA :  For TYP,Jff,Jzz,Job,Jspli,Jsl = ',A3,5I4,' ,    ICT =',I5               &
               /'    and :  Hit Z,S =',2F10.3,'  is outside trigger chamber area  ===>  WARNING !!!'/)
      ELSE IF( NOKTRI(ICT,ZTidea,SSidea,0.5D0,0.5D0) ) THEN
!- and inside cutout area of the trigger chamber
        IF( L_RPC1 )  Nb_Warn_RPC1 = Nb_Warn_RPC1 + 1
        IF( L_RPC2 )  Nb_Warn_RPC2 = Nb_Warn_RPC2 + 1
        IF( L_TGC1 )  Nb_Warn_TGC1 = Nb_Warn_TGC1 + 1
        IF( L_TGC2 )  Nb_Warn_TGC2 = Nb_Warn_TGC2 + 1
        PRINT 7785,NameTYP,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,ICT,IP, SSidea*10.,ZTidea*10.
7785    FORMAT(/' In DIGIBA :  For TYP,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri =  ',A3,7I4,                   &
               /' i.e. in the trigger chamber ICT,IP =',I5,I3,',   the digi. ZTidea,SSidea =',2F12.3, &
               /' is in the cutout area of the trigger chamber  ======>  WARNING  !!!'/)
      ENDIF
!
    ENDIF
!
!- If Simu positions are given, one forces the digi position to the simulated one !
    IF( AthPosOff == 0 ) THEN
      SSdigi = SSidea
      ZTdigi = ZTidea
    ENDIF
!
!- One stores the digi in module M_MB_Digis
    IF( Temps > -900.d0 ) THEN
      IF( RpcTimeSchema == 2 ) THEN
        Tcor = Temps - SQRT(SSmubo**2+TZmubo**2+ZTmubo**2) / VLum
      ELSE
        Tcor = Temps
      ENDIF
    ELSE   ! TGC have no timing and were put to -999
      Tcor = StripTimeMoy
    ENDIF
    IF( AtCosm == 0 ) THEN   !< rejection of out-of-time strips
      IF( Tcor < StripTimeMin ) RETURN
      IF( Tcor > StripTimeMax ) RETURN
    ENDIF
    IF( L_RPC1 .OR. L_TGC1 ) THEN
      CALL ABANHI(Ident,ICT,IP,ZTdigi,Tcor,JJJ,IPside,Jstri,Jspli)
    ELSE
      CALL ABANHI(Ident,ICT,IP,SSdigi,Tcor,JJJ,IPside,Jstri,Jspli)
    ENDIF
!
 END SUBROUTINE DIGIBA
!
