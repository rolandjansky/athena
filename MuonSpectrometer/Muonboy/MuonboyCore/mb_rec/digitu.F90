!
!> Put the hits (MDT and CSC) information in a form suitable for Muonboy,
!> and store it in Muonboy commons by calling atubhi.F90
!
 SUBROUTINE DIGITU(Ident,Jtyp,Jff,Jzz,Job,Jsl,Jtube,                      &
                   XXsimu,YYsimu,ZZsimu,CCC,Time,EEE,JJJ,Status,AthPosOff)
 USE M_MB_Control
 USE M_MB_General_CONSTANTS, ONLY : Zero
 USE M_MB_Digis
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Ident, Jtyp, Jff, Jzz, Job, Jsl, Jtube
 INTEGER, INTENT(IN)  :: JJJ
 REAL(8), INTENT(IN)  :: XXsimu, YYsimu, ZZsimu, CCC, Time, EEE
 INTEGER, INTENT(IN)  :: Status, AthPosOff
 REAL(8), PARAMETER :: EpsChk=0.0030D0
 CHARACTER (32) :: WARN_mubo_amdb,WARN_simu_amdb,WARN_simu_mubo
 CHARACTER (3)  :: NameTYP, NameTEC
 LOGICAL :: NUKCHA, NAKCHA, OK_Warning
 LOGICAL :: L_MDT, L_CSC1, L_CSC2
 LOGICAL :: L_mubo_amdb, L_simu_amdb, L_simu_mubo
 REAL(8) :: XYZabs(3,5), SZTloc(3,5)
 REAL(8) :: AA0,ZZ0,TT0,DZZ, S1P,S1N,S2P,S2N, SIGZ, ZZ1
 REAL(8) :: DCF,DSF, SIGT, ZLONG
 REAL(8) :: XXamdb,YYamdb,ZZamdb,SSamdb,TTamdb,ZTamdb,TZamdb
 REAL(8) :: SSsimu,TTsimu,ZTsimu,TZsimu, DCA,DSA
 REAL(8) :: SSidea,ZTidea,TZidea, SSdigi,ZTdigi,TZdigi
 REAL(8) :: SScsc, ZZcsc, TTcsc, SSlayer, ZZlayer, TTlayer
 REAL(8) :: SSmubo,ZTmubo,TZmubo, SZflag, Xflag, Radius
 INTEGER :: IC,INOF,INOZ, ICT, IGEOM, IMDT, IP
 INTEGER :: Itran, JtubeMax, IW, Iquali, Jcsc
 INTEGER, SAVE :: Nb_Warn_MDT  = 0, Max_Warn_MDT  = 200
 INTEGER, SAVE :: Nb_Warn_CSC1 = 0, Max_Warn_CSC1 = 200
 INTEGER, SAVE :: Nb_Warn_CSC2 = 0, Max_Warn_CSC2 = 100
 REAL(8), PARAMETER :: CSCchargeMin=9000.D0  !<  Cut-Out For Minimal CSC deposited charge
 INTEGER :: intpos
!===========================================
!> Jtyp,Jff,Jzz,Job  are the AMDC_SIMREC indexes defining the MDT (or CSC) multilayer
!!  Jsl   is the tube (or strips, for CSC) monolayer index (1 to 3 or 4)
!! Jtube  is the hit tube (or strips, for CSC) number in the monolayer
!! XXsimu,YYsimu,ZZsimu  are the absolute coordinates of the center of the hit tube
!!        (i.e. the wire) at the S position (i.e. the position along
!!        its length) of closest distance to the particle for MDT
!!        or the absolute coordinates of the center of the hit strip for CSC
!!  CCC   is the deposited charge for CSC
!!        or the ADC count for MDT
!!  Time  is the TDC count for MDT, 0. for CSC
!!  EEE   is the drift time for MDT
!!        or a flag which is  <0  for the 2nd coordinate strips of CSC
!!                       and  >0  for the 1rst coordinate strips of CSC
!!  JJJ   is the "particle code" (5 or 6 if the digi. originate from a muon,
!!                                2 or 3 if from an electron,
!!                               99 or 88 if from gamma-neutron background... etc)
!!  AthPosOff = 0 means keep the position from Athena
!!            otherwise recompute positions of digits
!!
!!  SS,ZT,TZ :
!! **********
!!
!! From Simulation :
!!    idea  <--TransfBak--  simu
!!   ******                ******
!!
!! From Amdb Indexes :
!!                          amdb
!!                         ******
!!
!! From Muonboy Indexes :
!!    digi  --TransfDir-->  mubo
!!   ******                ******
!===========================================
!
    CALL FGetStationType(Jtyp, NameTYP)
    CALL FGetObjectTech(Jtyp,Jff,Jzz,Job, NameTEC)
!
!- Get the MUONBOY MDT indexes from the AMDC_SIMREC ones
    CALL GETIND(Jtyp,Jff,Jzz,Job, IC,INOF,INOZ, ICT)
    IF( IC+INOF+INOZ <= 0 .OR. ( NAmeTEC /= 'MDT' .AND. NAmeTEC /= 'CSC') ) THEN
      PRINT 7770,NameTYP,Jff,Jzz,Job,NAmeTEC
7770  FORMAT(/' In Muonboy/DIGITU : ',                                 &
             'The object Typ,Jff,Jzz,Job,Tec =  ',A3,3I4,2X,A3,        &
             '  does not correspond to a Muonboy MDT/CSC multilayer',  &
             '  ===>  STOP !!!',/)
      RETURN
    ENDIF
    IGEOM = IGEOMA(IC,INOF,INOZ)
    IF( isOffChav(IGEOM) ) THEN
      IF( RecovDead ) THEN
        IF( NRecovDeadMDT < NRecovDeadMax ) THEN
          NRecovDeadMDT = NRecovDeadMDT + 1
          wasOffChav(NRecovDeadMDT) = IGEOM
          isOffChav(IGEOM) = .false.
        ENDIF
      ENDIF
      WRITE(*,*) ' Muonboy/digitu ERROR of MDT/CSC station Off ',IC,INOF,INOZ,' which has hits !!!'
    ENDIF
    IMDT  = ISTMDTV(IGEOM)
    IP    = Jsl
    IF( IP < 1 .OR. IP > NLTUB(IMDT) ) THEN
      PRINT 7780,NameTYP,Jff,Jzz,Job,NAmeTEC,Jsl,NLTUB(IMDT)
7780  FORMAT(/' In Muonboy/DIGITU : ',                                 &
              'For  Typ,Jff,Jzz,Job,Tec =  ',A3,3I4,2X,A3,             &
              '  the index  Jsl =',I2,'  is outside range :  1  to',   &
           I3,' <=========  do not use it !!!',/)
      RETURN
    ENDIF
    L_MDT  = .FALSE.
    L_CSC1 = .FALSE.
    L_CSC2 = .FALSE.
    IF( NAmeTEC == 'MDT' ) THEN
!- MDT tube
      L_MDT   = .TRUE.
      OK_Warning = Nb_Warn_MDT  <= Max_Warn_MDT
    ELSEIF( EEE < 0.D0 ) THEN
!- CSC 2nd coordinate strip
      L_CSC2 = .TRUE.
      OK_Warning = Nb_Warn_CSC2 <= Max_Warn_CSC2
    ELSE
!- CSC 1rst coordinate group of strips
      L_CSC1 = .TRUE.
      OK_Warning = Nb_Warn_CSC1 <= Max_Warn_CSC1
    ENDIF
!
    Iquali = 1    !< quality flag
    IF( L_MDT ) THEN
      Iquali = Status
! Rejection of problematic hits
      IF( Iquali == 999 ) RETURN  ! problematic hit, should never happen
      IF( Iquali == 3   ) RETURN  ! out-of-time (late) hit
! Rejection of Electronic noise (for MDT CCC=ADCcount Time=TDCcount)
! beware : simul uses CCC=0 and Time=0 to flag masked tubes
      IF( Iquali >= 1 .AND. CCC < MdtAdc ) THEN
        IF( CCC < MdtCut ) RETURN
        Iquali = 10   ! will be treated as a masked (by electronic noise) one
      ENDIF
    ENDIF
!
!- Get some Muonboy variables
    AA0   = GCECHAV(IGEOM)
    ZZ0   = ZCECHAV(IGEOM)
    TT0   = TCECHAV(IGEOM)
    DZZ   = ZLOCHAV(IGEOM)/2.D0
    S1P   = SPICHAV(IGEOM)
    S1N   = SNICHAV(IGEOM)
    S2P   = SPACHAV(IGEOM)
    S2N   = SNACHAV(IGEOM)
    Itran = ItransCHA(IGEOM)
    SIGZ  = 1.d0
    IF( IC <= NCOUBA .AND. INOZ < NZMS2 ) SIGZ = MDTnu
    ZZ1   = ZZ0 - SIGZ*DZZ
    DCF   = COSCHAV(IGEOM)
    DSF   = SINCHAV(IGEOM)
    SIGT  = SIGN( 1.D0, TT0 )
    IF( .NOT.L_MDT ) THEN
      Jcsc = intpos(IGEOM,IGEOMCSC,NChaCSC)
    ELSE
      Jcsc = 0
    ENDIF
!
!- Get Tube or Strip position directly from the Amdb Indexes
!- after displacements+deformations (amdb)
    IF( .NOT.L_MDT ) THEN
      CALL FGetStripPositionFromCSCindex( NameTYP,Jff,Jzz,Job,Jsl,Jtube,EEE, XYZabs, SZTloc )
      XXamdb = XYZabs(1,5) / 10.D0
      YYamdb = XYZabs(2,5) / 10.D0
      ZZamdb = XYZabs(3,5) / 10.D0
      SSamdb = DCF * YYamdb - DSF * XXamdb
      TTamdb = DCF * XXamdb + DSF * YYamdb 
      DCA = COS(AA0)
      DSA = SIN(AA0)
      ZTamdb = - DSA * ZZamdb + DCA * TTamdb
      TZamdb =   DCA * ZZamdb + DSA * TTamdb
    ENDIF
!
!- Get the input (i.e. from simulation) Tube or Strip position
!- both after (simu) and before (idea) displacements+deformations
    SSsimu   = DCF * YYsimu - DSF * XXsimu
    TTsimu   = DCF * XXsimu + DSF * YYsimu
    IF(IC <= NCOUBA) THEN
      ZTsimu = ZZsimu
      TZsimu = TTsimu
    ELSEIF(IC <= NCOUCH) THEN
      ZTsimu = TTsimu
      TZsimu = ZZsimu
    ELSE
      DCA = COS(AA0)
      DSA = SIN(AA0)
      ZTsimu = - DSA * ZZsimu + DCA * TTsimu
      TZsimu =   DCA * ZZsimu + DSA * TTsimu
    ENDIF
    CALL Pos_Ideal_from_True(Itran,SSsimu,ZTsimu,TZsimu,SSidea,ZTidea,TZidea)
    IF( Jcsc > 0 ) THEN !< take possible layer-to-layer misalignement into account
      CALL Pos_Layer_from_Csc(Jcsc,IP, SSidea,ZTidea,TZidea, SSlayer,ZZlayer,TTlayer)
      SSidea = SSlayer
      ZTidea = ZZlayer
      TZidea = TTlayer
    ENDIF
!
!- Get the Muonboy Tube or Strip position
!- both before (digi) and after (mubo) displacements+deformations
    IF( L_CSC2 ) THEN
!- CSC 2nd coordinate strip
      SSdigi = OLTUB(IMDT,IP,2) + Jtube*RAYTUB(IMDT)
!     ZTdigi = ZZ0 ! old, now take the right value from Amdb
      ZTdigi = SZTloc(2,5) / 10.D0 !
      TZdigi = TT0 + SIGT*(TLTUB(IMDT,IP,1)+TLTUB(IMDT,IP,2))*0.5d0
      JtubeMax = NINT( (2.*ABS(OLTUB(IMDT,IP,2))-RAYTUB(IMDT)) / RAYTUB(IMDT) )
    ELSE
!- MDT tube  or  CSC 1rst coordinate strip
      SSdigi = (S1P+S1N+S2P+S2N) / 4.D0
      ZTdigi = ZZ1 + SIGZ*(OLTUB(IMDT,IP,1)+(Jtube-1)*PASTUB(IMDT))
      TZdigi = TT0 + SIGT*TLTUB(IMDT,IP,1)
      IF( L_CSC1 ) TZdigi = TT0 + SIGT*(TLTUB(IMDT,IP,1)+TLTUB(IMDT,IP,2))*0.5d0
      ZLONG = ZLOCHAV(IGEOM)
      IF( L_CSC1 ) ZLONG = ZLONG - 2.*OLTUB(IMDT,IP,1) + PASTUB(IMDT)
      JtubeMax = NINT( ZLONG * PASTUB_inv(IMDT) )
    ENDIF
    IF( Jcsc > 0 ) THEN !< take possible layer-to-layer misalignement into account
      CALL Pos_Csc_from_Layer(Jcsc,IP, SSdigi,ZTdigi,TZdigi, SScsc,ZZcsc,TTcsc)
    ELSE
      SScsc = SSdigi
      ZZcsc = ZTdigi
      TTcsc = TZdigi
    ENDIF
    CALL Pos_True_from_Ideal(Itran,SScsc,ZZcsc,TTcsc,SSmubo,ZTmubo,TZmubo)
!
    IF( Jtube < 1 .OR. Jtube > JtubeMax ) THEN
      SZflag = 0.
      IF( L_CSC1 .OR. L_CSC2 ) SZflag = EEE
      PRINT 7701
      PRINT 7704,NameTYP,Jff,Jzz,Job,Jsl,Jtube,NAmeTEC,SZflag
      PRINT 7733,JtubeMax
7733  FORMAT('==>  Index  Jtube  is outside range :  1  to',I4,  &
             ' <=========  do not use it !')
      RETURN
    ENDIF
!
    IF( MBChkDig > 0 .AND. OK_WARNING ) THEN
      IF( L_MDT ) THEN
        CALL FGetTubePositionFromMDTindexJtypBased( Jtyp,Jff,Jzz,Job,Jsl,Jtube, MDTnu, XYZabs, SZTloc, Radius )
        XXamdb = (XYZabs(1,1)+XYZabs(1,2)) / 20.D0
        YYamdb = (XYZabs(2,1)+XYZabs(2,2)) / 20.D0
        ZZamdb = (XYZabs(3,1)+XYZabs(3,2)) / 20.D0
        SSamdb   = DCF * YYamdb - DSF * XXamdb
        TTamdb   = DCF * XXamdb + DSF * YYamdb
        IF(IC <= NCOUBA) THEN
          ZTamdb = ZZamdb
          TZamdb = TTamdb
        ELSE
          ZTamdb = TTamdb
          TZamdb = ZZamdb
        ENDIF
      ENDIF
      L_mubo_amdb = ABS(SSmubo-SSamdb) > EpsChk .OR. ABS(ZTmubo-ZTamdb) > EpsChk  &
                                                .OR. ABS(TZmubo-TZamdb) > EpsChk
      L_simu_amdb = ABS(SSsimu-SSamdb) > EpsChk .OR. ABS(ZTsimu-ZTamdb) > EpsChk  &
                                                .OR. ABS(TZsimu-TZamdb) > EpsChk
      L_simu_mubo = ABS(SSsimu-SSmubo) > EpsChk .OR. ABS(ZTsimu-ZTmubo) > EpsChk  & 
                                                .OR. ABS(TZsimu-TZmubo) > EpsChk
      IF( L_simu_amdb .OR. L_simu_mubo .OR. L_mubo_amdb ) THEN
        IF( L_MDT  ) THEN
          Nb_Warn_MDT  = Nb_Warn_MDT  + 1
          PRINT 7701
7701      FORMAT(/'==>         MDT tube          Digi to Muonboy (DIGITU) : ',  &
                  ' Typ  Jff Jzz Job Jsl Jcha  Tec SZflag')
        ELSEIF( L_CSC1 ) THEN
          PRINT 7702
          Nb_Warn_CSC1 = Nb_Warn_CSC1 + 1
7702      FORMAT(/'==>    CSC 1rst co. strips    Digi to Muonboy (DIGITU) : ',  &
                  ' Typ  Jff Jzz Job Jsl Jcha  Tec SZflag')
        ELSE
          PRINT 7703
          Nb_Warn_CSC2 = Nb_Warn_CSC2 + 1
7703      FORMAT(/'==>     CSC 2nd co. strip     Digi to Muonboy (DIGITU) : ',  &
                  ' Typ  Jff Jzz Job Jsl Jcha  Tec SZflag')
        ENDIF
        WARN_mubo_amdb = '  OK !'
        WARN_simu_amdb = '  OK !'
        WARN_simu_mubo = '  OK !'
        IF( L_mubo_amdb  ) WARN_mubo_amdb = '  <=========  WARNING !'
        IF( L_simu_amdb  ) WARN_simu_amdb = '  <=========  WARNING !'
        IF( L_simu_mubo  ) WARN_simu_mubo = '  <=========  WARNING !'
        SZflag = 0.
        IF( L_CSC1 .OR. L_CSC2 ) SZflag = EEE
        PRINT 7704,NameTYP,Jff,Jzz,Job,Jsl,Jtube,NAmeTEC,SZflag
7704    FORMAT('==>  ************************',29X,A3,4I4,I5,3X,A3,F6.1)
        PRINT 7714,SSsimu*10., ZTsimu*10., TZsimu*10.,                                              &
                   SSamdb*10., ZTamdb*10., TZamdb*10.,                                              &
                   SSmubo*10., ZTmubo*10., TZmubo*10.,                                              &
                  (SSmubo-SSamdb)*10.,(ZTmubo-ZTamdb)*10.,(TZmubo-TZamdb)*10.,TRIM(WARN_mubo_amdb), &
                  (SSsimu-SSamdb)*10.,(ZTsimu-ZTamdb)*10.,(TZsimu-TZamdb)*10.,TRIM(WARN_simu_amdb)
7714    FORMAT('==>  Simulation  SZT =',3F12.3,    &
              /'==>      Amdb    SZT =',3F12.3,    &
              /'==>    Muonboy   SZT =',3F12.3,    &
              /'==>  Mubo - Amdb SZT =',3F12.3,A,  &
              /'==>  Simu - Amdb SZT =',3F12.3,A)
        IF( L_mubo_amdb ) PRINT 7705,(SSsimu-SSmubo)*10., (ZTsimu-ZTmubo)*10., (TZsimu-TZmubo)*10.,TRIM(WARN_simu_mubo)
7705    FORMAT('==>  Simu - Mubo SZT =',3F12.3,A)     
        IF( Itran > 0 ) PRINT 7706, SSidea*10., ZTidea*10., TZidea*10., SSdigi*10., ZTdigi*10., TZdigi*10.
7706    FORMAT('==>  Idea (Simu) SZT =',3F12.3,/'==>  Digi (Mubo) SZT =',3F12.3)
      ENDIF
!
!- One checks whether the hit is inside the multilayer area
      IF( NUKCHA(IC,INOZ,IGEOM,ZTidea,SSidea,1.0D0,1.0D0) ) THEN
        IF( L_MDT  )  Nb_Warn_MDT  = Nb_Warn_MDT  + 1
        IF( L_CSC1 )  Nb_Warn_CSC1 = Nb_Warn_CSC1 + 1
        IF( L_CSC2 )  Nb_Warn_CSC2 = Nb_Warn_CSC2 + 1
        PRINT 7772,NameTYP,Jff,Jzz,Job,Jsl, IC,INOF,INOZ,IP,Jtube,ZTidea*10.,SSidea*10.
7772    FORMAT(/' In DIGITU :  For Typ,Jff,Jzz,Job,Jsl = ',A3,4I4,' ,   IC,INOF,INOZ,IP,Jchan =',5I4/  &
                '    and    :  Hit Z,S =',2F10.3,'  is outside multilayer area  ===>  WARNING !!!',/)
      ELSE IF( NAKCHA(IC,INOZ,IGEOM,ZTidea,SSidea,TZidea,0.5D0,0.5D0) ) THEN
!- and if the digi is in cutout area of the multilayer
        IF( L_MDT  )  Nb_Warn_MDT  = Nb_Warn_MDT  + 1
        IF( L_CSC1 )  Nb_Warn_CSC1 = Nb_Warn_CSC1 + 1
        IF( L_CSC2 )  Nb_Warn_CSC2 = Nb_Warn_CSC2 + 1
        PRINT 7785,NameTYP,Jff,Jzz,Job,Jsl,Jtube,IC,INOF,INOZ, ZTidea*10.,SSidea*10.
7785    FORMAT(/' In DIGITU :  For TYP,Jff,Jzz,Job,Jsl,Jtube = ',A3,5I4,              &
               /' i.e. in the multilayer IC,INOF,INOZ =',3I4,                         &
                ',  the digi. ZTidea,SSidea =',2F12.3,                                &
               /' is in the cutout area of the chamber','  ======>  WARNING  !!!'/)
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
    IF( IWCHA(IC,INOF,INOZ) >= 1 ) THEN
!- Here, the multilayer was already hit
      IW = IWCHA(IC,INOF,INOZ)
    ELSE
!- Here, on the contrary, the multilayer was not hit up to now
      IF( NCHAHI(IC) >= NCHAMA ) THEN
        PRINT 7774,IC,NCHAMA
7774    FORMAT(/' WARNING : The number of Munoboy hit chambers in m.l.',I3,  &
                ' is limited to the parameter NCHAMA =',I8/)
        RETURN
      ENDIF
      NCHAHI(IC) = NCHAHI(IC) + 1
      IW = NCHAHI(IC)
      IFCHA(IC,IW) = INOF
      IZCHA(IC,IW) = INOZ
      IWCHA(IC,INOF,INOZ) = IW
    ENDIF
    IF( L_MDT ) THEN
!- MDT tube
      Xflag  = -1.D0
      CALL ATUBHI(Ident,IC,IW,IP, ZTdigi,CCC,Xflag, Zero,JJJ, Iquali)
    ELSEIF( L_CSC2 ) THEN
!- CSC 2nd coordinate strip
      Xflag  = 1.D0
      CALL ATUBHI(Ident,IC,IW,IP,  Zero, CCC,Xflag,SSdigi,JJJ, Iquali)
    ELSEIF( CCC > CSCchargeMin ) THEN
!- CSC 1rst coordinate strip
      Xflag  = 11.D0
      CALL ATUBHI(Ident,IC,IW,IP, ZTdigi,CCC,Xflag, Zero,JJJ, Iquali)
    ENDIF
!
 END SUBROUTINE DIGITU
!
