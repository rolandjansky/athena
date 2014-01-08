!
!> Load the Amdc geometry and put it in a form suitable for Muonboy
!> \author M.Virchaux
!
 SUBROUTINE AMDCOX
 USE M_MB_Control
 USE M_MB_Reco_PARAMETERS
 USE M_MB_General_CONSTANTS
 USE M_MB_MuGeom
 USE M_MB_Fit
 IMPLICIT NONE
!
#include "AmdcStand/amdcsim_com.inc"
!
      INTEGER :: NCOUBI, NTRIBI, JTYP, ISC, NMDTTO, NRPCTO, NCUTTO
      INTEGER :: JFF, JZZ, INOF, INOZ, JGEO, JCUT
      INTEGER :: NTOMDT, NTOCSC, JOB, JTEC, JSHA
      INTEGER :: NBMDT, JSTA, N1BABA, N2BABA, L, JRPC, IC, NTUTU
      INTEGER :: NstripSBABA, NstripZBABA, NsplitBABA, NgazvolBABA
      INTEGER :: JMDT, JBID, IGEOM, KCUT, JrtTUTU
      INTEGER :: JTOTCU, JOBCUT, NBK, L1
      INTEGER :: NgangTotPre, NgangTot, I, JTGC, ICT
      INTEGER :: IAVAR, IZZDEB, IZZFIN, IZZ, LPASS, NbCAR3
      INTEGER :: IML, ICO, IGEOI, NSZB
      INTEGER :: ISCTYP(MTYP), ITYPRI(MTYP)
      INTEGER :: IZZBAR(-MZZ:MZZ), IZZBOF(-MZZ:MZZ), IZZBOG(-MZZ:MZZ), IZZBXL(-MZZ:MZZ), IZZBXE(-MZZ:MZZ) 
      INTEGER :: IZZECA(-MZZ:MZZ)
      REAL(8) :: SBAREL, F0,S0,Z0,T0,SIZ0,SIS0, RABABA, TBABAM
      REAL(8) :: DSBABA, DZBABA, TSBABA, TZBABA, DTBABA
      REAL(8) :: TotWidthBABA, TotLengthBABA, DeadSideBABA
      REAL(8) :: DeadCenterBABA, xxxBABA
      REAL(8) :: Soffset1BABA, Soffset2BABA, ZoffsetBABA
      REAL(8) :: PTUTU, RTUTU, DTUTU, ETUTU, XTUTU, ATUTU
      REAL(8) :: XLAYER, UTUTU, TTUTUM
      REAL(8) :: S0obj, S0MEX, S0PEX, ENDPLU, HAWIDS, HAWIDL
      REAL(8) :: SC_Station, ZC_Station, TC_Station
      REAL(8) :: DS_Station, DZ_Station, DT_Station
      REAL(8) :: ZICUR, ZACUR, ZCUR
      REAL(8) :: S1P, S2Pi, S2Po1, S2Po2, S2Pa, S2Px, Zii, Zoo, Zaa
      REAL(8) :: ZMI, S0CUT, SCUMEX, SCUPEX, ZTBABA
      REAL(8) :: ZZI, DZZ, ZZZ
      REAL(8) :: OTUTU1(N4M), OTUTU2(N4M), TTUTU1(N4M), TTUTU2(N4M)
      REAL(8) :: OBABA1(N6M), OBABA2(N6M), TBABA(N6M)
      INTEGER :: NSZBABA(N6M)
      REAL(8) :: SZBABA(200,N6M)
      REAL(8) :: XBUFF(NTRIMA)
      INTEGER :: IBUFF(NTRIMA), NORDRE(NTRIMA)
      CHARACTER(3) :: CATISC(NMATMA), CAR3(NMATMA)
      CHARACTER(5) :: CATOHI(2)
      CHARACTER(1) :: CAR1
      LOGICAL :: switchTGC
      INTEGER :: ier
      REAL(8) :: Tpoints(4)
      LOGICAL :: hasBOE
!**********************************************************
!JFL Start 29/01/05
!
      CHARACTER(3) :: Ctyp
      REAL(8) :: XYZabs(3,5),SZTloc(3,5)
      INTEGER :: Jsl,Jstrip
!
      INTEGER :: Iprint
      INTEGER :: IAmdcVersion
      REAL(8) :: SZTFixed(3),SZTCc(3)
      REAL(8) :: SzzttCc(3),SzzttCcF(3),SzzttCcR(3)
      REAL(8) :: S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab
      REAL(8) :: Ziib,Zoob,Zaab
      REAL(8) :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
      REAL(8) :: Ziip,Zoop,Zaap
      REAL(8) :: PitchEta1stStrip,PitchPhi2ndStrip
      INTEGER :: NberWireLayer
      REAL(8) :: CUFOAMG10,GASGAP
      REAL(8) :: Gama0,FOs
!
      REAL(8) :: ttLayer(2,4)
!
      REAL(8) :: ShiftStripCsc2nd
      REAL(8) :: ShiftStripCsc1st
!
!JFL End   29/01/05
!**********************************************************
!
!
!-- CATISC contains the names of the NMATMA station-types of MUONBOY
      DATA CATISC/                                                   &
!-->>   1     2     3     4     5     6     7     8
      'BIS','BIL','BMS','BML','BOS','BOL','BIR','BEE',               &
!-->>   9    10    11    12    13    14    15    16    17    18
      'EIS','EIL','EES','EEL','EMS','EML','EOS','EOL','FOS','FOL',   &
!-->>  19    20
      'CSS','CSL' /
!
!-- ISCTYP contains the address (1 to NMATMA) of the station-type of MUONBOY
!-- for each of the station-type of AMDC-SIMREC (array CARTYP of size MTYP)
      DATA ISCTYP/                                                   &
!-->>  BIL  BIS  BML  BMS  BOL  BOS  BEE  BIR  BMF  BOF  BOG  BME
        2,   1,   4,   3,   6,   5,   8,   7,   3,   5,   5,   4,    &
!-->>  EIC  EIL  EEL  EES  EMC  EML  EMS  EOC  EOL  EOS
       10,  10,  12,  11,  14,  14,  13,  18,  16,  15,              &
!-->>  T1C  T1L  T1S  T2C  T2L  T2S  T3C  T3L  T3S
       99,  99,  99,  99,  99,  99,  99,  99,  99,                   &
!-->>  CI1  CI2  CI3  CI4
        0,   0,   0,   0,                                            &
!-->>  FIL  FIS  FML  FMS  FOL  FOS
       10,   9,  14,  13,  18,  17,                                  &
!-->>  T1F  T1E  T2F  T2E  T3F  T3E  T4F  T4E
       99,  99,  99,  99,  99,  99,  99,  99,                        &
!-->>  EIS       CSS  CSL       BIM
        9,       19,  20,         2,                    7*0 /
!
!
      DATA CATOHI/'Lower','Upper'/
!
!
!-->>                   -8 -7 -6 -5 -4 -3 -2 -1  0  1  2  3  4  5  6  7  8
      DATA IZZBAR/ 7*0,  1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,  7*0 /
      DATA IZZBOF/11*0,              2, 4, 6, 8, 0,10,12,14,16,             11*0 /
      DATA IZZBOG/11*0,              1, 3, 5, 7, 9,11,13,15,17,             11*0 /
      DATA IZZBXL/ 8*0,     1, 2, 3, 4, 6, 7, 8, 9,10,11,12,14,15,16,17,     8*0 /
      DATA IZZBXE/ 7*0,  5, 5, 0, 0, 0, 0, 0, 5, 0,13, 0, 0, 0, 0, 0,13,13,  7*0 /
      DATA IZZECA/ 7*0, 17,16,15,14,13,12,11,10, 0, 1, 2, 3, 4, 5, 6, 7, 8,  7*0 /
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
!
!
!-- NCHAMA cannot be > 999 because of the coding of IW1 and IW2 in IKINTR....
      IF( NCHAMA > 999 ) THEN
         PRINT 7720
7720     FORMAT(/' Parameter NCHAMA is > 999 ======> STOP in AMDCOX of MUONBOY !!!')
         STOP
      ENDIF
!
!
!
!-- Number of different types of stations of MDT
!-- Number of different types of multi-layers of MDT
      NMATTR = 18
      NCOUCH = 2 * NMATTR
      NMACSC = NMATTR + 2
      NCOCSC = NCOUCH + 4
      IF( NMACSC > NMATMA ) THEN
        PRINT '(" NMATMA too small, increase it ===> STOP ")'
        STOP
      ENDIF
      CALOHI(1:2)      = CATOHI(1:2)
      CARISC(1:NMACSC) = CATISC(1:NMACSC)
      DO ISC=1,NMACSC
        DO JTYP=1,MTYP
          IF( CARISC(ISC) == CARTYP(JTYP) ) ItypMboyAmdc(ISC) = JTYP
        ENDDO
      ENDDO
!-- Barrel   : IC =        1  to  NCOUBA    ICT =        1  to  NTRIBA
!-- End caps : IC = NCOUBA+1  to  NCOUCH    ICT = NTRIBA+1  to  NTRITO
      NCOUBA = 0
!**** NCOUBI = NCOUCH + 1
      NCOUBI = 999999
      NTRIBA = 0
      NTRIBI = 999999
      NChaCSC = 0
!-- ICHPHI = 1 for stations in sectors between two barrel coils
!-- ICHPHI = 2 for stations in barrel coil sectors
      DO JTYP=1,MTYP
!-- Define internal ISC code of MUONBOY
        ISCISC(JTYP) = ISCTYP(JTYP)
        ISC          = ISCISC(JTYP)
        IF(ISC < 1 .OR. ISC > NMACSC)  CYCLE
        CAR1 = CARTYP(JTYP)(3:3)
        IF( CAR1 == 'L' .OR. CAR1 == 'C' .OR. CAR1 == 'R' .OR. CAR1 == 'M' .OR. & 
          CARTYP(JTYP) == 'BME') THEN
          ICHPHI(ISC) = 1
        ELSEIF( CAR1 == 'S' .OR. CAR1 == 'F' .OR. CAR1 == 'G' .OR. CAR1 == 'H' .OR. &
          ( CAR1 == 'E' .AND. (CARTYP(JTYP) .NE. 'BME'))) THEN
          ICHPHI(ISC) = 2
        ELSE
          PRINT 7700,CARTYP(JTYP)
7700      FORMAT(/' Station type ',A3,' is unknown ======> STOP in AMDCOX of MUONBOY !!!')
          STOP
        ENDIF
      ENDDO
!-- MDT multilayer IC,INOF,INOZ is, a priori, absent (IGEOMA(IC,INOF,INOZ)=0)
!-- and has no internal structure,  no link to trigger chambers,  nor any cutouts.
      NGEOMT = 0
      IGEOMA(:,:,:) = 0
      ZLOCHAV(:) = 0.D0
      ISTMDTV(:) = 0
      ISTLINV(:) = 0
      JSTLINV(:) = 0
      ISTCUTV(:) = 0
!-- Stations are, a priori, in their nominal positions
!-- (i.e. no "transformations" to their true positions)
      ItransCHA(:) = 0
      ItransTRI(:) = 0
!-- Trigger chamber ICT is, a priori, absent (ZLOTRI(ICT)=0.),
!-- has no internal structure,  no cutouts,  nor any "amdb" type,
!-- and is in its nominal position (i.e. no "transformations")
      ZLOTRI(:)    = 0.D0
      IRPTGC(:)    = 0
      ITRCUT(:)    = 0
      JDBTRI(:)    = 0
!-- Initialise number of monolayers of tubes in multilayer of type JMDT
      NLTUB(:)  = 0
      JrtTUB(:) = 0
!-- Initialise number of MDT "stacking" types
      NMDTTO = 0
!-- Initialise links to trigger chambers
      NBLINK(:) = 0
      DSLINK(:) = 999.D0
      DZLINK(:) = 999.D0
      MBLINK(:) = 0
!-- Initialise internal structures of trigger chambers
      N1BAND(:)     = 0
      N2BAND(:)     = 0
      ISZBAND(:,:,:)= 0
      NSZBANDtot    = 0
      DSBAND(:)     = 9999.D0
      DZBAND(:)     = 9999.D0
      RABAND(:)     =    0.D0
      OBAND (:,:,:) = 0.D0
      TBAND (:,:)   = 0.D0
!-- Initialise number of RPC "stacking" types
      NRPCTO = 0
!-- Initialise total number of cutouts
      NCUTTO = 0
!-- Initialise number of sub-cutouts in cutout JCUT
      NBCUT(:) = 0
!-- Initialize total number of links to trigger chambers
      NLINTO = 0
      MLINTO = 0
!-- Initialize total number of trigger chambers
      NTRITO = 0
!
      hasBOE = .false.
!
!-- Define the AMDB Version Index ( A -> 65  to Z -> 90 )
      IVersAmdc = ICHAR( C_AMDC_NAME(1:1) )
!--  ICHAR( 'Q' 'Z' 'q' 'z' ) = 81 90 113 122
      IF( IVersAmdc >= 113 ) IVersAmdc = IVersAmdc - 32
      WRITE(*,'(/" ===>"/" ===> MuonBoy is using the AMDB version :  ",A16)') C_AMDC_NAME
!!$      I_AMDCOX_WARNING = 0
!!$!
!!$      IF( IVersAmdc < 81 .OR. 90 < IVersAmdc ) THEN    !--   CHAR( 81 90 ) = 'Q' 'Z'
!!$        PRINT 7725,C_AMDC_NAME
!!$7725    FORMAT(/' ===>'/' ===> MuonBoy is using the AMDB version :  ',   &
!!$            A16,'  which is ANTERIOR to versions Q ,'/                   &
!!$                ' ===> it therefore assumes that the CSC 2nd coord ',    &
!!$                'strips are pointing  !!!'/' ===>')
!!$        CSCpa = 0
!!$      ELSE
!!$        PRINT 7726,C_AMDC_NAME
!!$7726    FORMAT(/' ===>'/' ===> MuonBoy is using the AMDB version :  ',   &
!!$            A16,'  which is POSTERIOR to versions P ,'/                  &
!!$                ' ===> it therefore assumes that the CSC 2nd coord ',    &
!!$                'strips are parallel  !!!'/' ===>')
!!$        CSCpa = 1
!!$      ENDIF
!
!
!-----------------------------------------------------------------------
!-- Loops over all the stations of the data base
      DO 502 JTYP=1,MTYP
      DO 501 JFF=1,8
      DO 500 JZZ=-MZZ,MZZ
      JGEO = IGEO(JTYP,JFF,JZZ)
      IF( JGEO < 1 ) CYCLE
      JCUT = ICUT(JTYP,JFF,JZZ)
      ISC  = ISCISC(JTYP)
      IF( ISC  < 1 ) CYCLE
!
!     print *,' AMDCOX JTYP, JFF, JZZ ', JTYP, JFF, JZZ
!-- Is the current station a "Barrel" or "End-Cap" station ?
      SBAREL = 1.D0
      IF( IBAREL(JTYP,JFF,JZZ) < 1 ) SBAREL = -1.D0
!
      INOF = JFF
      IF(CARTYP(JTYP) == 'BOF') THEN
        INOZ = IZZBOF(JZZ)
      ELSEIF(CARTYP(JTYP) == 'BOG') THEN
        INOZ = IZZBOG(JZZ)
      ELSEIF(CARTYP(JTYP) == 'BML' .AND. JFF == 7 ) THEN
        INOZ = IZZBXL(JZZ)
      ELSEIF(CARTYP(JTYP) == 'BME' .AND. JFF == 7) THEN
        INOZ = IZZBXE(JZZ)
      ELSEIF(CARTYP(JTYP) == 'BOL' .AND. JFF == 7 .AND. &
        (ABS(JZZ) == 1 .OR. ABS(JZZ) == 2 .OR. ABS(JZZ) == 3 .OR. &
         ABS(JZZ) == 4 .OR. ABS(JZZ) == 5 .OR. ABS(JZZ) == 6) ) THEN
        INOZ = IZZBXL(JZZ)
      ELSEIF(CARTYP(JTYP) == 'BOL' .AND. JFF == 7 .AND. (ABS(JZZ) == 7 .OR. ABS(JZZ) == 8) ) THEN
        INOZ = IZZBXE(JZZ)
        IF(CARTYP(JTYP) == 'BOL')  hasBOE = .true. 
      ELSEIF(SBAREL > 0.D0) THEN
        INOZ = IZZBAR(JZZ)
      ELSE
        INOZ = IZZECA(JZZ)
      ENDIF
#if !defined(ATLAS_NOCHECKMU)
      IF(INOZ < 1 .OR. INOZ > NZM) THEN
        PRINT 7711,CARTYP(JTYP),JFF,JZZ,INOZ
7711    FORMAT(/' For : Type,Jff,Jzz = ',A3,2I3,', one gets : Inoz =',I3,   &
                ' ======> STOP in AMDCOX of MUONBOY !!!')
        STOP
      ENDIF
#endif
!
!-- Get the station dimentions
      F0 = Pos_Phi(JTYP,JFF,JZZ)
      S0 = Pos_S(JTYP,JFF,JZZ)/10.D0
      Z0 = Pos_Z(JTYP,JFF,JZZ)/10.D0
      T0 = Pos_R(JTYP,JFF,JZZ)/10.D0
      SIZ0 = SIGN(ONE,Z0)
      SIS0 = 1.D0
      IF( IVERSION_AMDC > 6 .AND. IBAREL(JTYP,JFF,JZZ) < 1 .AND. JZZ > 0 ) SIS0 = -1.D0
      Call Dim_Station(JTYP,JFF,JZZ, SC_Station,ZC_Station,TC_Station,   &
                                     DS_Station,DZ_Station,DT_Station)
!
!-- Get the number of MDT and CSC multilayers in the station
      NTOMDT = 0
      NTOCSC = 0
!     print *,' JTYP, CARTYP, JGEO, NOBJ ', 
!    &  JTYP, CARTYP(JTYP), JGEO, NOBJ(JTYP,JGEO)
      DO JOB=1,NOBJ(JTYP,JGEO)
        JTEC = ITEC(JTYP,JGEO,JOB)
        JSHA = ISHAPE(JTYP,JGEO,JOB)
        IF( CARTEC(JTEC) == 'MDT' .AND. JSHA == 0 ) NTOMDT = NTOMDT + 1
        IF( CARTEC(JTEC) == 'CSC' .AND. JSHA == 2 ) NTOCSC = NTOCSC + 1
      ENDDO
#if !defined(ATLAS_NOCHECKMU)
      IF( NTOMDT > 2 .OR. NTOCSC > 2 ) THEN
        PRINT 7701,CARTYP(JTYP),JGEO
7701    FORMAT(/' There are more than 2 multilayers of MDT or CSC ','in station Type,Jgeo = ',A3,I3,  &
                ' ======> STOP in AMDCOX of MUONBOY !!!')
        STOP
      ENDIF
#endif
!
      if ( NTOMDT .ne.0 ) then
        CALL dim_t_mdt(JTYP,JFF,JZZ,Tpoints)
        Tpoints(1:4) = Tpoints(1:4)/10.d0
      endif
!
!-- Loops over all the "objects" of one station
      NBMDT = 0
      DO 350 JOB=1,NOBJ(JTYP,JGEO)
      JTEC = ITEC(JTYP,JGEO,JOB)
      JSTA = ISTA(JTYP,JGEO,JOB)
      JSHA = ISHAPE(JTYP,JGEO,JOB)
!-- In the station JTYP,JGEO which is in position JTYP,JFF,JZZ
!-- and in which are possibly made the cut-outs JCUT ,
!-- the object of serial number JOB is of shape JSHA and is
!-- made of technology JTEC with stacking type JSTA
      S0obj  = S0    + SIS0*Geo_dx(JTYP,JGEO,JOB)/10.D0
      S0MEX  = S0obj + SIS0*Geo_Ex(JTYP,JGEO,JOB)/20.D0
      S0PEX  = S0obj - SIS0*Geo_Ex(JTYP,JGEO,JOB)/20.D0
!!$      I_AMDCOX_WARNING = I_AMDCOX_WARNING + 1
!!$      IF( IVersAmdc < 81 .OR. 90 < IVersAmdc ) THEN
!!$!--   CHAR( 81 90 ) = 'Q' 'Z'
!!$        IF( I_AMDCOX_WARNING < 2 ) PRINT 7715,C_AMDC_NAME
!!$7715    FORMAT(/' ===>'/' ===> MuonBoy is using the AMDB version :  ',        &
!!$            A16,'  which is ANTERIOR to versions Q ,'/                        &
!!$                ' ===> it therefore assumes that the MDT tubes are fully  ',  &
!!$                'EFFICIENT  in their  END-PLUGS  !!!'/' ===>')
!!$        ENDPLU = 0.D0
!!$      ELSE
!!$        IF( I_AMDCOX_WARNING < 2 ) PRINT 7716,C_AMDC_NAME
!!$7716    FORMAT(/' ===>'/' ===> MuonBoy is using the AMDB version :  ',          &
!!$            A16,'  which is POSTERIOR to versions P ,'/                         &
!!$                ' ===> it therefore (rightly) assumes that the MDT tubes are',  &
!!$                '  INefficient  in their  END-PLUGS  !!!'/' ===>')
!!$        ENDPLU = Geo_D1(JTYP,JGEO,JOB)/10.D0
!!$      ENDIF
      ENDPLU = Geo_D1(JTYP,JGEO,JOB)/10.D0
      HAWIDS = Geo_Ws(JTYP,JGEO,JOB)/20.D0 - ENDPLU
      HAWIDL = Geo_Wl(JTYP,JGEO,JOB)/20.D0 - ENDPLU
!
!
      IF( CARTEC(JTEC) == 'RPC' .AND. JSHA <= 0 ) THEN
!-- Rectangular (or trapezoidal) Barrel (or End-Cap) RPC chambers <<<<<<<<<<<<<<
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         N1BABA = NLAS(JTEC,JSTA)
         N2BABA = NLAS(JTEC,JSTA) + NLAZ(JTEC,JSTA)
         RABABA = STAX0(JTEC,JSTA)
         TBABAM = 0.D0
         OBABA2(1:N2BABA) = -999.D0
         IF( IFORMA(JTEC,JSTA) == 0 ) THEN
            DSBABA = STAPP(JTEC,JSTA)/10.D0
            DZBABA = STARR(JTEC,JSTA)/10.D0
            DO L=1,N2BABA
             OBABA1(L)= STAOO(JTEC,JSTA,L)/10.D0
             TBABA(L) = STATT(JTEC,JSTA,L)/10.D0
             TBABAM   = TBABAM + TBABA(L)
            ENDDO
         ELSEIF( IFORMA(JTEC,JSTA) == 1 ) THEN
            DSBABA = STAPP(JTEC,JSTA)/10.D0
            DZBABA = STAPP(JTEC,JSTA)/10.D0
            TSBABA = STAOO(JTEC,JSTA,1)/10.D0+STAOO(JTEC,JSTA,2)/20.D0
            TZBABA = TSBABA
            DTBABA = STAOO(JTEC,JSTA,2)/10.D0+STAOO(JTEC,JSTA,3)/10.D0
            DO L=1,N1BABA
!*********** OBABA1(L)= DSBABA/2.D0 * MOD(L-1,2)
             OBABA1(L)= 0.D0
             TBABA(L) = TSBABA + DTBABA * (L-1.D0)
             TBABAM   = TBABAM + TBABA(L)
            ENDDO
            DO L=N1BABA+1,N2BABA
!*********** OBABA1(L)= DZBABA/2.D0 * MOD(L-N1BABA-1,2)
             OBABA1(L)= 0.D0
             TBABA(L) = TZBABA + DTBABA * (L-N1BABA-1.D0)
             TBABAM   = TBABAM + TBABA(L)
            ENDDO
         ELSEIF( IFORMA(JTEC,JSTA) == 2.OR.IFORMA(JTEC,JSTA) == 4 ) THEN
            DSBABA = STATT(JTEC,JSTA,10)/10.D0
            DZBABA = STATT(JTEC,JSTA,11)/10.D0
            TSBABA = STATT(JTEC,JSTA,1)/10.D0+STATT(JTEC,JSTA,7)/10.D0   &
                   + STATT(JTEC,JSTA,4)/20.D0
            TZBABA = TSBABA
            DTBABA = STATT(JTEC,JSTA,4)/10.D0+STATT(JTEC,JSTA,5)/10.D0
            IF( JSHA == -1 ) THEN
              TSBABA = STAEE(JTEC,JSTA)/10.D0 -TSBABA-(N1BABA-1)*DTBABA
              TZBABA = TSBABA
            ENDIF
            NstripSBABA    = NINT( STAOO(JTEC,JSTA,14) )
            NstripZBABA    = NINT( STAOO(JTEC,JSTA,15) )
            NsplitBABA     = NINT( STAOO(JTEC,JSTA, 2) )
            NgazvolBABA    = NINT( STAOO(JTEC,JSTA, 3) )
            TotWidthBABA   = (Geo_Wl(JTYP,JGEO,JOB)+Geo_Ws(JTYP,JGEO,JOB))/20.D0
            TotLengthBABA  = Geo_Le(JTYP,JGEO,JOB)/10.D0
            DeadSideBABA   = Geo_D1(JTYP,JGEO,JOB)/10.D0
            DeadCenterBABA = STATT(JTEC,JSTA,17)/10.D0
            IF( NsplitBABA == 2 ) THEN
               IF( NgazvolBABA == 2 ) THEN
                  xxxBABA = ( TotWidthBABA - 2.D0*NstripSBABA*DSBABA - 2.D0*DeadSideBABA - DeadCenterBABA ) / 4.D0
                  Soffset1BABA = DeadSideBABA + xxxBABA
                  Soffset2BABA = DeadCenterBABA/2.D0 + xxxBABA
               ELSE
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!JFL mercredi 30 juillet 2008, 15:02:55 (UTC+0200)
!                 xxxBABA = ( TotWidthBABA - 2.D0*NstripSBABA*DSBABA
!    +                    - 2.D0*DeadSideBABA ) / 4.D0
!                 Soffset1BABA = DeadSideBABA + xxxBABA
                  xxxBABA = ( TotWidthBABA - 2.D0*NstripSBABA*DSBABA ) / 4.D0
                  Soffset1BABA =                xxxBABA
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                  Soffset2BABA = xxxBABA
               ENDIF
            ELSE
               Soffset1BABA = (TotWidthBABA-NstripSBABA*DSBABA) / 2.D0
               Soffset2BABA = 0.D0
            ENDIF
            ZoffsetBABA     = (TotLengthBABA-NstripZBABA*DZBABA) / 2.D0
            DO L=1,N1BABA
             IF( NstripSBABA < 2 ) THEN
                OBABA1(L) = DeadCenterBABA/2.D0 - DSBABA/2.D0
             ELSE
                OBABA1(L) = Soffset1BABA - DSBABA/2.D0
                IF( NsplitBABA == 2 ) OBABA2(L) = Soffset2BABA - DSBABA/2.D0
             ENDIF
             TBABA(L) = TSBABA + DTBABA * (L-1.D0)
             TBABAM   = TBABAM + TBABA(L)
            ENDDO
            DO L=N1BABA+1,N2BABA
             IF( NstripZBABA < 2 ) THEN
                OBABA1(L) = DeadCenterBABA/2.D0 - DZBABA/2.D0
             ELSE
                OBABA1(L) = ZoffsetBABA         - DZBABA/2.D0
             ENDIF
             TBABA(L) = TZBABA + DTBABA * (L-N1BABA-1.D0)
             TBABAM   = TBABAM + TBABA(L)
            ENDDO
         ELSE
            GO TO 350
         ENDIF
         TBABAM   = TBABAM / N2BABA
         TBABA(1:N2BABA) = TBABA(1:N2BABA) - TBABAM
         NSZBABA(1:N6M) = 0
         CALL CRERPC(N1BABA,N2BABA,DSBABA,DZBABA,RABABA,                 &
                     OBABA1,OBABA2,TBABA, NSZBABA,SZBABA, NRPCTO, JRPC)
!***************************************
!******* ISPLIX(JTYP,JGEO,JOB) *********
!******* ISPLIY(JTYP,JGEO,JOB) *********
!***************************************
         NTRITO = NTRITO + 1
#if !defined(ATLAS_NOCHECKMU)
         IF( NTRITO > NTRIMA ) THEN
           PRINT 7704,NTRITO
7704       FORMAT(/' Atempt to create a total number of trigger chambers >',I5,' ======> STOP in AMDCOX of MUONBOY !!!')
           STOP
         ENDIF
         IF( NBMDT /= 0 .AND. NBMDT /= 2 ) THEN
           PRINT 7705,CARTYP(JTYP),JGEO
7705       FORMAT(/' In station Type,Jgeo =',A3,I3,' there is RPC inbetween MDTs',    &
                   ' ======> STOP in AMDCOX of MUONBOY !!!')
           STOP
         ENDIF
#endif
         IRPTGC(NTRITO)      = JRPC
         ITRCUT(NTRITO)      = 0
         CALL CDECI(JTYP,JFF,JZZ,JOB, JDBTRI(NTRITO)) 
         FCETRI(NTRITO)      = F0
         ZLOTRI(NTRITO)      = Geo_Le(JTYP,JGEO,JOB)/10.D0
         IF( SBAREL > 0.D0 ) THEN
            NTRIBA = MAX( NTRIBA , NTRITO )
            TCETRI(NTRITO) =    T0 + Geo_dz(JTYP,JGEO,JOB)/10.D0 + TBABAM
            ZCETRI(NTRITO) =    Z0 + Geo_dy(JTYP,JGEO,JOB)/10.D0 + Geo_Le(JTYP,JGEO,JOB)/20.D0
            SNITRI(NTRITO) = S0MEX - Geo_Ws(JTYP,JGEO,JOB)/20.D0
            SNATRI(NTRITO) = S0PEX - Geo_Wl(JTYP,JGEO,JOB)/20.D0
            SPITRI(NTRITO) = S0MEX + Geo_Ws(JTYP,JGEO,JOB)/20.D0
            SPATRI(NTRITO) = S0PEX + Geo_Wl(JTYP,JGEO,JOB)/20.D0
         ELSE
            NTRIBI = MIN( NTRIBI , NTRITO )
            ZCETRI(NTRITO) =    T0 + Geo_dy(JTYP,JGEO,JOB)/10.D0 + Geo_Le(JTYP,JGEO,JOB)/20.D0
            TCETRI(NTRITO) =    Z0 + SIZ0 * (Geo_dz(JTYP,JGEO,JOB)/10.D0+TBABAM)
            SNITRI(NTRITO) = S0MEX - Geo_Ws(JTYP,JGEO,JOB)/20.D0
            SNATRI(NTRITO) = S0PEX - Geo_Wl(JTYP,JGEO,JOB)/20.D0
            SPITRI(NTRITO) = S0MEX + Geo_Ws(JTYP,JGEO,JOB)/20.D0
            SPATRI(NTRITO) = S0PEX + Geo_Wl(JTYP,JGEO,JOB)/20.D0
         ENDIF
         IF( NTRIBA >= NTRIBI ) THEN
            PRINT 7719
7719        FORMAT(/' Mismatch between BARREL and END-CAP trigger chambers ======> STOP in AMDCOX of MUONBOY !!!')
            STOP
         ENDIF
!
      ELSEIF(SBAREL > 0..AND.CARTEC(JTEC) == 'MDT'.AND.JSHA == 0) THEN
!-- Rectangular (or trapezoidal) Barrel MDT chambers <<<<<<<<<<<<<<<<<<<<<<<<<<<
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         NCOUBA = MAX( NCOUBA , 2*ISC )
         IF( NCOUBA >= NCOUBI ) THEN
            PRINT 7718
7718        FORMAT(/' Mismatch between BARREL and END-CAP stations ======> STOP in AMDCOX of MUONBOY !!!')
            STOP
         ENDIF
         NBMDT = NBMDT + 1
!-- Number of tube layers
         NTUTU     = NLAZ(JTEC,JSTA)
!-- Tube pitch
         PTUTU     = STAPP(JTEC,JSTA)/10.D0
!-- Tube internal radius
         RTUTU     = STARR(JTEC,JSTA)/10.D0
!-- Tube dead time
         DTUTU     = DeTub
!-- Tube "effective" resolution (if <= 0  it will be overwritten !)
         ETUTU     = - 0.0160D0
!-- Multilayer radiative thickness
!******* XTUTU     = 0.011D0 * NLAZ(JTEC,JSTA) + 0.020D0
         XTUTU     = STAX0(JTEC,JSTA)
!-- Maximum drift time
         ATUTU     = TDRMAX
!-- Multilayer "effective" resolution (for fast resolution computation with RESOMU)
         XLAYER = NTUTU
         UTUTU     = SQRT( STOKAS**2/XLAYER + ALIPRE**2 )
!-- Position of first tube of each layer
         OTUTU1(1:NTUTU) = STAOO(JTEC,JSTA,1:NTUTU) / 10.D0
         OTUTU2(1:NTUTU) = 0.D0
         TTUTU1(1:NTUTU) = STATT(JTEC,JSTA,1:NTUTU) / 10.D0
         TTUTU2(1:NTUTU) = 0.D0
         TTUTUM = SUM( TTUTU1(1:NTUTU) ) / NTUTU
         TTUTU1(1:NTUTU) = TTUTU1(1:NTUTU) - TTUTUM
!-- Index of R-T relation
         JrtTUTU    = NLAS(JTEC,JSTA)
!--                        PAStub      DEDtub      RADtub      ACUtub
!--                              RAYtub      REStub      DRItub
         CALL CREMDT(NTUTU,PTUTU,RTUTU,DTUTU,ETUTU,XTUTU,ATUTU,UTUTU,    &
                     OTUTU1,OTUTU2,TTUTU1,TTUTU2,JrtTUTU, NMDTTO, JMDT)
         IF( NTUTU == 4 ) CALL CREMDT(3,PTUTU,RTUTU,DTUTU,ETUTU,XTUTU,ATUTU,                   &
                                      UTUTU,OTUTU1,OTUTU2,TTUTU1,TTUTU2,JrtTUTU, NMDTTO, JBID)
         NGEOMT = NGEOMT + 1
#if !defined(ATLAS_NOCHECKMU)
         IF( NGEOMT > NGEOMA ) THEN
           PRINT 7712,NGEOMA
7712       FORMAT(/' NGEOMA =',I6,' is not big enough ======> STOP in AMDCOX of MUONBOY !!!')
           STOP
         ENDIF
#endif
         IF( NBMDT == 1 .AND. NTOMDT == 2 ) THEN
           IC = 2*ISC - 1
         ELSE
           IC = 2*ISC
         ENDIF
         IGEOMA(IC,INOF,INOZ) = NGEOMT
         SNICHAV(NGEOMT) = S0MEX - HAWIDS
         SNACHAV(NGEOMT) = S0PEX - HAWIDL
         SPICHAV(NGEOMT) = S0MEX + HAWIDS
         SPACHAV(NGEOMT) = S0PEX + HAWIDL
!!$         TREFABSO(NGEOMT) = Tpoints(1) 
!!$         TREFSIZE(NGEOMT) = abs(Tpoints(4)-Tpoints(1))
!!$         IF( NBMDT == 1 ) THEN
!!$           TMLADOWN(NGEOMT) = Tpoints(1)
!!$           TMLASIZE(NGEOMT) = abs(Tpoints(2)-Tpoints(1))
!!$         ELSE
!!$           TMLADOWN(NGEOMT) = Tpoints(3)
!!$           TMLASIZE(NGEOMT) = abs(Tpoints(4)-Tpoints(3))
!!$         ENDIF

!--      JFL; mercredi 12 mai 2010, 15:19:09 (UTC+0200)
!--      When one moves the next 3 lines after the CDECI line, results change for unknown reasons
!--      ZCUR vakue change by at most 1E-013 while  ZICUR and ZACUR are apparently unchanged   
         ZICUR  = Z0 + Geo_dy(JTYP,JGEO,JOB)/10.D0
         ZACUR    = ZICUR + Geo_Le(JTYP,JGEO,JOB)/10.D0
         ZCUR     = (ZACUR + ZICUR) / 2.D0

         CALL CDECI(JTYP,JFF,JZZ,JOB,ISTTYPV(NGEOMT))

         FCECHAV(NGEOMT) = F0
         GCECHAV(NGEOMT) = 0.D0
         TCECHAV(NGEOMT) = T0 + Geo_dz(JTYP,JGEO,JOB)/10.D0 + TTUTUM
         TCUTOUT(NGEOMT) = T0 + Geo_dz(JTYP,JGEO,JOB)/10.D0
         ZCECHAV(NGEOMT) = ZCUR
         ZLOCHAV(NGEOMT) = ZACUR - ZICUR
         STACHAV(NGEOMT) = Geo_D3(JTYP,JGEO,JOB)/10.D0
         ISTMDTV(NGEOMT) = JMDT
!
      ELSEIF(SBAREL < 0..AND.CARTEC(JTEC) == 'MDT'.AND.JSHA == 0) THEN
!-- Trapezoidal (or rectangular) End-Cap MDT chambers <<<<<<<<<<<<<<<<<<<<<<<<<<
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         NCOUBI = MIN( NCOUBI , 2*ISC-1 )
         IF( NCOUBA >= NCOUBI ) THEN
            PRINT 7718
            STOP
         ENDIF
         NBMDT = NBMDT + 1
!-- Number of tube layers
         NTUTU     = NLAZ(JTEC,JSTA)
!-- Tube pitch
         PTUTU     = STAPP(JTEC,JSTA)/10.D0
!-- Tube internal radius
         RTUTU     = STARR(JTEC,JSTA)/10.D0
!-- Tube dead time
         DTUTU     = DeTub
!-- Tube "effective" resolution (if <= 0  it will be overwritten !)
         ETUTU     = - 0.0160D0
!-- Multilayer radiative thickness
!******* XTUTU     = 0.011D0 * NLAZ(JTEC,JSTA) + 0.020D0
         XTUTU     = STAX0(JTEC,JSTA)
!-- Maximum drift time
         ATUTU     = TDRMAX
!-- Multilayer "effective" resolution (for fast resolution computation with RESOMU)
         XLAYER    = NTUTU
         UTUTU     = SQRT( STOKAS**2/XLAYER + ALIPRE**2 )
!-- Position of first tube of each layer
         OTUTU1(1:NTUTU) = STAOO(JTEC,JSTA,1:NTUTU) / 10.D0
         OTUTU2(1:NTUTU) = 0.D0
         TTUTU1(1:NTUTU) = STATT(JTEC,JSTA,1:NTUTU) / 10.D0
         TTUTU2(1:NTUTU) = 0.D0
         TTUTUM = SUM( TTUTU1(1:NTUTU) ) / NTUTU
         TTUTU1(1:NTUTU)  = TTUTU1(1:NTUTU) - TTUTUM
!-- Index of R-T relation
         JrtTUTU    = NLAS(JTEC,JSTA)
!--                        PAStub      DEDtub      RADtub      ACUtub
!--                              RAYtub      REStub      DRItub
         CALL CREMDT(NTUTU,PTUTU,RTUTU,DTUTU,ETUTU,XTUTU,ATUTU,                &
                     UTUTU,OTUTU1,OTUTU2,TTUTU1,TTUTU2,JrtTUTU, NMDTTO, JMDT)
         IF( NTUTU == 4 ) CALL CREMDT(3,PTUTU,RTUTU,DTUTU,ETUTU,XTUTU,ATUTU,                    &
                                      UTUTU,OTUTU1,OTUTU2,TTUTU1,TTUTU2,JrtTUTU, NMDTTO, JBID)
         NGEOMT = NGEOMT + 1
#if !defined(ATLAS_NOCHECKMU)
         IF( NGEOMT > NGEOMA ) THEN
           PRINT 7712,NGEOMA
           STOP
         ENDIF
#endif
         IC = 2*ISC - 2 + NBMDT
         IGEOMA(IC,INOF,INOZ) = NGEOMT
         TCECHAV(NGEOMT) = Z0 + SIZ0 * (Geo_dz(JTYP,JGEO,JOB)/10.D0+TTUTUM)
         TCUTOUT(NGEOMT) = Z0 + SIZ0 *  Geo_dz(JTYP,JGEO,JOB)/10.D0
         SNICHAV(NGEOMT) = S0MEX - HAWIDS
         SNACHAV(NGEOMT) = S0PEX - HAWIDL
         SPICHAV(NGEOMT) = S0MEX + HAWIDS
         SPACHAV(NGEOMT) = S0PEX + HAWIDL
!!$         TREFABSO(NGEOMT) = Tpoints(1) 
!!$         TREFSIZE(NGEOMT) = abs(Tpoints(4)-Tpoints(1))
!!$         IF( NBMDT == 1 ) THEN
!!$           TMLADOWN(NGEOMT) = Tpoints(1)
!!$           TMLASIZE(NGEOMT) = abs(Tpoints(2)-Tpoints(1))
!!$         ELSE
!!$           TMLADOWN(NGEOMT) = Tpoints(3)
!!$           TMLASIZE(NGEOMT) = abs(Tpoints(4)-Tpoints(3))
!!$         ENDIF
         CALL CDECI(JTYP,JFF,JZZ,JOB, ISTTYPV(NGEOMT))
         FCECHAV(NGEOMT)  = F0
         GCECHAV(NGEOMT)  = 0.D0
         ZCECHAV(NGEOMT)  = T0 + Geo_dy(JTYP,JGEO,JOB)/10.D0 + Geo_Le(JTYP,JGEO,JOB)/20.D0
         ZLOCHAV(NGEOMT)  = Geo_Le(JTYP,JGEO,JOB)/10.D0
         STACHAV(NGEOMT)  = Geo_D3(JTYP,JGEO,JOB)/10.D0
         ISTMDTV(NGEOMT)  = JMDT
!
!TAGJFLA
      ELSEIF(SBAREL < 0..AND.CARTEC(JTEC) == 'CSC'.AND.JSHA == 2) THEN
!-- CSC chambers <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!**********************************************************
!JFL Start 29/01/05
!
      Ctyp   = CARTYP(Jtyp)
      Jsl    = 1
      JStrip = 1
      Iprint = 0
!
!--Basics of Csc Geometry
      call CscGeoBasics(Iprint,                                         &
                        Ctyp,Jff,Jzz,Job,                               &
                        IAmdcVersion,                                   &
                        SZTFixed,SZTCc,                                 &
                        SzzttCc,SzzttCcF,SzzttCcR,                      &
                        S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab,Ziib,Zoob,Zaab,  &
                        S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                        PitchEta1stStrip,PitchPhi2ndStrip,              &
                        NberWireLayer,CUFOAMG10,GASGAP,                 &
                        Gama0,FOs)
!
!--Wire Layer Model
      call CscWireLayerModel(Iprint,                           &
                             IAmdcVersion,                     &
                             Ctyp,Jff,Jzz,Job,                 &
                             SzzttCc,SzzttCcF,SzzttCcR,        &
                             NberWireLayer,CUFOAMG10,GASGAP,   &
                             ttLayer)
!
!-- Strip Model
!--  Phi-strip 
       call CscPhi2ndCoStripModel(Iprint,                                           &
                                  IAmdcVersion,                                     &
                                  Ctyp,Jff,Jzz,Job,Jsl,JStrip,                      &
                                  SzzttCc,SzzttCcF,SzzttCcR,                        &
                                  S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,    &
                                  PitchPhi2ndStrip,                                 &
                                  ttLayer,                                          &
                                  ShiftStripCsc2nd,                                 &
                                  SZTloc,XYZabs)
!--  R-strip 
       call CscEta1stCoStripModel(Iprint,                                           &
                                  IAmdcVersion,                                     &
                                  Ctyp,Jff,Jzz,Job,Jsl,JStrip,                      &
                                  SzzttCc,SzzttCcF,SzzttCcR,                        &
                                  S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,    &
                                  PitchEta1stStrip,                                 &
                                  ttLayer,                                          &
                                  ShiftStripCsc1st,                                 &
                                  SZTloc,XYZabs)
!
!JFL End   29/01/05
!**********************************************************
!******* IF( SBAREL > 0.D0 ) THEN
!*******    NCOUBA = MAX( NCOUBA , 2*ISC )
!******* ELSE
!*******    NCOUBI = MIN( NCOUBI , 2*ISC-1 )
!******* ENDIF
!******* IF( NCOUBA >= NCOUBI ) THEN
!*******    PRINT 7718
!*******    STOP
!******* ENDIF
         NBMDT = NBMDT + 1
         IF( NBMDT == 1 ) THEN
           IC = 2*ISC - 1
         ELSE
           IC = 2*ISC
         ENDIF
         NGEOMT = NGEOMT + 1
#if !defined(ATLAS_NOCHECKMU)
           IF( NGEOMT > NGEOMA ) THEN
             PRINT 7712,NGEOMA
             STOP
           ENDIF
#endif
         CALL Get_CSC_SZdim(JTYP,JFF,JZZ,JOB, S1P, S2Pi, S2Po1,S2Po2, S2Pa, Zii,Zoo,Zaa )
         S2Px = S1P + (S2Pi-S1P) * Zoo/2.D0/Zii
!-- Number of sensitive layers in one CSC multilayer
!**********************************************************
!JFL Start 29/01/05
!*       NTUTU     = NLAZ(JTEC,JSTA)
         NTUTU     = NberWireLayer
!JFL End   29/01/05
!**********************************************************
!-- Cathode readout pitch along the "precise coordinate" (in cm)
!**********************************************************
!JFL Start 29/01/05
!*       PTUTU     = STATT(JTEC,JSTA,6)/10.D0
         PTUTU     = PitchEta1stStrip/10.D0
!JFL End   29/01/05
!**********************************************************
!-- Strip pitch along the "second coordinate" (in cm)
!**********************************************************
!JFL Start 29/01/05
!*       RTUTU     = STAOO(JTEC,JSTA,4)/10.D0
!*       IF( RTUTU < 0.01D0 ) RTUTU = 2.00D0
         RTUTU     = PitchPhi2ndStrip/10.D0
!JFL End   29/01/05
!**********************************************************
!-- Here for CSCs, two tracks separation i.e.
!-- width of the induced charge distr. on the strips (in cm)
!--                 @@@@@@@@
!@@@@@@@ DTUTU     = 0.70D0
         DTUTU     = 0.52D0
!--                 @@@@@@@@
!-- Accuracy of measurement (1rst coordinate), for Chi2 computation (in CHICSC)
         ETUTU     = 0.0060D0
!-- Radiative thickness
         XTUTU     = STAX0(JTEC,JSTA)
!-- Maximum integration time (in ns)
!--                 @@@@@@@@
!@@@@@@@ ATUTU     =  70.D0
         ATUTU     =  35.D0
!--                 @@@@@@@@
!-- Multilayer "effective" resolution (for fast resolution computation with RESOMU)
         UTUTU     = 0.0050D0
!-- Position of first tube of each layer
!**********************************************************
!JFL Start 29/01/05
!*         TTUTUM     = 0.D0
!*         IF( CSCpa > 0 ) THEN
!*            NbStrips2ndCo = NINT(S2Pi/RTUTU) - 1
!*         ELSE
!*            NbStrips2ndCo = NINT(S2Px/RTUTU) - 1
!*         ENDIF
!*         DO L=1,NTUTU
!*          OTUTU1(L) = STATT(JTEC,JSTA,11)/10.D0
!*!-- Second coordinate strips offset
!*!--                  @@@@@@@@@@@@@@@@@@@@@@@
!********** OTUTU2(L) = 0.D0
!*          OTUTU2(L) = - NbStrips2ndCo*RTUTU
!*!--                  @@@@@@@@@@@@@@@@@@@@@@@
!*          TTUTU(L)  = L*( 2.D0*STATT(JTEC,JSTA,5) + STATT(JTEC,JSTA,2)
!*     +                 + 2.D0*STATT(JTEC,JSTA,3) )/10.D0
!*          TTUTUM    = TTUTUM + TTUTU(L)
!*         ENDDO
!*         TTUTUM     = TTUTUM / NTUTU
!*         DO L=1,NTUTU
!*          TTUTU(L)  = TTUTU(L) - TTUTUM
!*         ENDDO
         DO L=1,NTUTU
!
            OTUTU1(L) = ShiftStripCsc1st
            OTUTU1(L) = OTUTU1(L) - ( SzzttCc(2) - Zoob/2.d0 )
            OTUTU1(L) = OTUTU1(L)/10.D0
!
            OTUTU2(L) = ShiftStripCsc2nd
            OTUTU2(L) = OTUTU2(L) - SzzttCc(1)
            OTUTU2(L) = OTUTU2(L) - PitchPhi2ndStrip
            OTUTU2(L) = OTUTU2(L)/10.D0
!
            TTUTU1(L) = ttLayer(2,L) 
            TTUTU1(L) = TTUTU1(L) - SzzttCc(3)
            TTUTU1(L) = TTUTU1(L) * SIGN( 1.D0 , SzzttCc(3) )
            TTUTU1(L) = TTUTU1(L)/10.D0
!
            TTUTU2(L) = ttLayer(1,L)
            TTUTU2(L) = TTUTU2(L) - SzzttCc(3)
            TTUTU2(L) = TTUTU2(L) * SIGN( 1.D0 , SzzttCc(3) )
            TTUTU2(L) = TTUTU2(L)/10.D0
!
         ENDDO
!JFL End  29/01/05
!**********************************************************
         JrtTUTU    = 0
!--                        PAStub      DEDtub      RADtub      ACUtub
!--                                        RAYtub      REStub      DRItub
         CALL CREMDT(NTUTU,PTUTU,RTUTU,DTUTU,ETUTU,XTUTU,ATUTU,UTUTU,    &
                     OTUTU1,OTUTU2,TTUTU1,TTUTU2,JrtTUTU, NMDTTO, JMDT)
         IGEOMA(IC,INOF,INOZ) = NGEOMT
         CALL CDECI(JTYP,JFF,JZZ,JOB, ISTTYPV(NGEOMT))
!**********************************************************
!JFL Start  29/01/05
!*       IF( IVERSION_AMDC < 7 ) THEN
!*          ZZC = Z0 + SIZ0 * TC_Station
!*          TTC = T0 +        ZC_Station
!*       ELSE
!*          ZZC = Z0
!*          TTC = T0
!*       ENDIF
!*       ZZCP = Z0 + SIZ0 * Geo_dz(JTYP,JGEO,JOB)/10.D0
!*   +             + SIZ0 * STAEE(JTEC,JSTA)/20.D0
!*       TTCP = T0 + Geo_dy(JTYP,JGEO,JOB)/10.D0 + Zoo/2.D0
!*       AA = - SIZ0 * Pos_Gama(JTYP,JFF,JZZ) * PIS180
!*       ZZC  = SZTFixed(2)/10.d0
!*       TTC  = SZTFixed(3)/10.d0
!*       ZZCP = SZTCc(2)/10.d0
!*       TTCP = SZTCc(3)/10.d0
!*       AA = - Gama0
!*       SINAA = SIN( AA )
!*       COSAA = COS( AA )
!*       FCECHAV(NGEOMT) = F0
!*       GCECHAV(NGEOMT) = AA
!*       TCECHAV(NGEOMT) =   COSAA*ZZC + SINAA*TTC - ZZC + ZZCP
!*       ZCECHAV(NGEOMT) = - SINAA*ZZC + COSAA*TTC - TTC + TTCP
!*       ZLOCHAV(NGEOMT) = Zoo
!*       SNICHAV(NGEOMT) = S0obj - S1P
!*       SNACHAV(NGEOMT) = S0obj - S2Po2
!*       SPICHAV(NGEOMT) = S0obj + S1P
!*       SPACHAV(NGEOMT) = S0obj + S2Po2
         S0obj = SZTCc(1)/10.d0
         FCECHAV(NGEOMT) = FOs
         GCECHAV(NGEOMT) = - Gama0
         TCECHAV(NGEOMT) = SzzttCc(3)/10.d0
         TCUTOUT(NGEOMT) = TCECHAV(NGEOMT)
         ZCECHAV(NGEOMT) = SzzttCc(2)/10.d0
         ZLOCHAV(NGEOMT) = Zoob/10.d0
         SNICHAV(NGEOMT) = S0obj - S1Pb/10.d0
         SNACHAV(NGEOMT) = S0obj - S2Po2b/10.d0
         SPICHAV(NGEOMT) = S0obj + S1Pb/10.d0
         SPACHAV(NGEOMT) = S0obj + S2Po2b/10.d0
!JFL End   29/01/05
!**********************************************************
! save the details of active area of CSC to be used in NAKCSC
         CALL saveActiveCSC(NGEOMT,Zoop,Ziip,PitchEta1stStrip,S1Pp,S2Pip,S2Po1p,PitchPhi2ndStrip)
!**********************************************************
!-- There is NO "staircasing" in CSCs !!!
         STACHAV(NGEOMT) = 0.D0
         ISTMDTV(NGEOMT) = JMDT
         ZMI = ZCECHAV(NGEOMT) - ZLOCHAV(NGEOMT)/2.D0
         IF( Zaa/Zii > 1.001D0 ) THEN
!-- When "diamond" shaped insert one cut with two sub-cuts
           NCUTTO = NCUTTO + 1
#if !defined(ATLAS_NOCHECKMU)
           IF( NCUTTO > NCUTMA ) THEN
             PRINT 7703,NCUTTO
             STOP
           ENDIF
#endif
           ISTCUTV(NGEOMT) = NCUTTO
           NBCUT(NCUTTO) = 2
           DO L=1,2
!**********************************************************
!JFL Start  29/01/05
!*          ZTCUT(1,NCUTTO,L) = ZMI + Zii
!*          ZTCUT(2,NCUTTO,L) = ZMI + Zii
!*          ZTCUT(3,NCUTTO,L) = ZMI + Zaa
!*          ZTCUT(4,NCUTTO,L) = ZMI + Zaa
!*          ZTCUT(5,NCUTTO,L) = ZMI + Zii
!*          SSCUT(1,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pi
!*          SSCUT(2,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pa
!*          SSCUT(3,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pa
!*          SSCUT(4,NCUTTO,L) = S0obj
!*          SSCUT(5,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pi
            ANCUT(  NCUTTO,L) = 0.d0
            ZTCUT(1,NCUTTO,L) = ZMI + Ziib/10.d0
            ZTCUT(2,NCUTTO,L) = ZMI + Ziib/10.d0
            ZTCUT(3,NCUTTO,L) = ZMI + Zaab/10.d0
            ZTCUT(4,NCUTTO,L) = ZMI + Zaab/10.d0
            ZTCUT(5,NCUTTO,L) = ZMI + Ziib/10.d0
            SSCUT(1,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pib/10.d0
            SSCUT(2,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pab/10.d0
            SSCUT(3,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pab/10.d0
            SSCUT(4,NCUTTO,L) = S0obj
            SSCUT(5,NCUTTO,L) = S0obj + (-1.D0)**L * S2Pib/10.d0
!JFL End   29/01/05
!**********************************************************
           ENDDO
         ENDIF
         IF( JCUT >= 1 ) THEN
!-- When there is an explicit cut-out (e.g. to get the "coffin" shape), insert it
           IF( ISTCUTV(NGEOMT) <= 0 ) THEN
             NCUTTO = NCUTTO + 1
#if !defined(ATLAS_NOCHECKMU)
             IF( NCUTTO > NCUTMA ) THEN
               PRINT 7703,NCUTTO
               STOP
             ENDIF
#endif
             ISTCUTV(NGEOMT) = NCUTTO
             NBCUT(NCUTTO) = 0
           ENDIF
           JTOTCU = INOCUT(JTYP,JGEO,JCUT)
           DO 155 JOBCUT=1,NOBCUT(JTOTCU)
             IF( IOBCUT(JTOTCU,JOBCUT) /= JOB ) CYCLE
             IF( NBCUT(NCUTTO) >= NSUCUM )      CYCLE
             NBCUT(NCUTTO) = NBCUT(NCUTTO) + 1
             NBK = NBCUT(NCUTTO)
               S0CUT = S0 - SIS0*Cut_dx(JTOTCU,JOBCUT)/10.D0
             SCUMEX = S0CUT + SIS0*Cut_Ex(JTOTCU,JOBCUT)/20.D0
             SCUPEX = S0CUT - SIS0*Cut_Ex(JTOTCU,JOBCUT)/20.D0
             ANCUT(  NCUTTO,NBK)  = Cut_An(JTOTCU,JOBCUT)*PiS180
             ZTCUT(1,NCUTTO,NBK)  = ZMI - Geo_dy(JTYP,JGEO,JOB)/10.D0 + Cut_dy(JTOTCU,JOBCUT)/10.D0
             ZTCUT(2,NCUTTO,NBK)  = ZTCUT(1,NCUTTO,NBK)
             ZTCUT(3,NCUTTO,NBK)  = ZTCUT(1,NCUTTO,NBK) + Cut_Le(JTOTCU,JOBCUT)/10.D0
             ZTCUT(4,NCUTTO,NBK)  = ZTCUT(3,NCUTTO,NBK)
              SSCUT(1,NCUTTO,NBK) = SCUMEX+Cut_Ws(JTOTCU,JOBCUT)/20.D0
              SSCUT(2,NCUTTO,NBK) = SCUMEX-Cut_Ws(JTOTCU,JOBCUT)/20.D0
              SSCUT(3,NCUTTO,NBK) = SCUPEX-Cut_Wl(JTOTCU,JOBCUT)/20.D0
              SSCUT(4,NCUTTO,NBK) = SCUPEX+Cut_Wl(JTOTCU,JOBCUT)/20.D0
             ZTCUT(5,NCUTTO,NBK)  = ZTCUT(1,NCUTTO,NBK)
             SSCUT(5,NCUTTO,NBK)  = SSCUT(1,NCUTTO,NBK)
155        CONTINUE
           IF( NBCUT(NCUTTO) <= 0 ) THEN
             NCUTTO = NCUTTO - 1
             ISTCUTV(NGEOMT) = 0
           ENDIF
         ENDIF
!
      ELSEIF( CARTEC(JTEC) == 'TGC' .AND. JSHA == 0 ) THEN
!-- Trapezoidal End-Cap (or Barrel) TGC chambers <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         TBABAM        = 0.D0
         RABABA        = STAX0(JTEC,JSTA)
         NSZBABA(1:N6M)= 0
         IF( IFORMA(JTEC,JSTA) == 1 ) THEN
           N1BABA   = NLAS(JTEC,JSTA)
           N2BABA   = NLAS(JTEC,JSTA) + NLAZ(JTEC,JSTA)
           DSBABA   = STAPP(JTEC,JSTA)/10.D0
           DZBABA   = STAPP(JTEC,JSTA)/10.D0
           TSBABA   = STAOO(JTEC,JSTA,1)/10.D0+STAOO(JTEC,JSTA,2)/20.D0
           TZBABA   = TSBABA
           DTBABA   = STAOO(JTEC,JSTA,2)/10.D0+STAOO(JTEC,JSTA,3)/10.D0
           OBABA2(1:N2BABA) = -999.D0
           DO L=1,N1BABA
            OBABA1(L)= DSBABA/2.D0 * MOD(L-1,2)
            TBABA(L) = TSBABA + DTBABA * (L-1.D0)
            TBABAM   = TBABAM + TBABA(L)
           ENDDO
           DO L=N1BABA+1,N2BABA
            OBABA1(L)= DZBABA/2.D0 * MOD(L-N1BABA-1,2)
            TBABA(L) = TZBABA + DTBABA * (L-N1BABA-1.D0)
            TBABAM   = TBABAM + TBABA(L)
           ENDDO
         ELSEIF( IFORMA(JTEC,JSTA) >= 2 ) THEN
           N1BABA   = ( NLAZ(JTEC,JSTA) - 3 ) / 4
           N2BABA   = 2 * N1BABA
           OBABA2(1:N2BABA) = -999.D0
           IF(     IFORMA(JTEC,JSTA) == 2 ) THEN
              DSBABA = 3.0D0
              DZBABA = 3.0D0
           ELSEIF( IFORMA(JTEC,JSTA) == 3 ) THEN
              DSBABA = STATT(JTEC,JSTA,NLAZ(JTEC,JSTA)+1)/10.D0
              DZBABA = STAOO(JTEC,JSTA,NLAZ(JTEC,JSTA)+1)/10.D0
           ELSEIF( IFORMA(JTEC,JSTA) == 4 .OR. IFORMA(JTEC,JSTA) == 5 ) THEN
              DSBABA = XtgcAdd(JSTA,5) / 10.D0
              DZBABA = 0.0D0
              IF( DSBABA < 0.2D0 ) DSBABA = 3.0D0
           ELSEIF( IFORMA(JTEC,JSTA) == 6 ) THEN
              DSBABA = 0.0D0
              DZBABA = 0.0D0
           ENDIF
           DO L=1,N1BABA
            OBABA1(L)= DSBABA/2.D0 * MOD(L-1,2)
            TBABA(L)=(STATT(JTEC,JSTA,4*L)+STATT(JTEC,JSTA,4*L+1))/20.D0
            TBABAM  = TBABAM + TBABA(L)
           ENDDO
           DO L=N1BABA+1,N2BABA
            L1 = L-N1BABA
            OBABA1(L)= OBABA1(L1)
            TBABA(L) = TBABA(L1)
            TBABAM   = TBABAM + TBABA(L)
           ENDDO
           IF( IFORMA(JTEC,JSTA) >= 4 ) THEN
             IF( IFORMA(JTEC,JSTA) >= 6 ) THEN
               DO L=1,MIN(N1BABA,3)
                 IF( NtgcAdd(JSTA,L+3) < 2 ) CYCLE
                 NSZB = NtgcAdd(JSTA,L+3) + 1
                 SZBABA(1:NSZB,L) = ( XtgcStrp(JSTA,1:NSZB,1) + XtgcStrp(JSTA,1:NSZB,2) ) / 20.D0
                 switchTGC = .FALSE.
                 IF( JZZ < 0 ) switchTGC = .NOT.switchTGC
                 IF( L /= 1  ) switchTGC = .NOT.switchTGC 
                 IF( switchTGC ) THEN
                   SZBABA(1:NSZB,L) = - SZBABA(NSZB:1:-1,L) + SZBABA(1,L) + SZBABA(NSZB,L)
                 ENDIF
                 DSBABA = ( SZBABA(NSZB,L) - SZBABA(1,L) ) / NSZB
                 NSZBABA(L) = NSZB
               ENDDO
               OBABA1(1:N1BABA) = 0.D0
             ENDIF
             NgangTotPre = 0
             DO L=N1BABA+1,MIN(N2BABA,N1BABA+3)
               IF( NtgcAdd(JSTA,L-N1BABA) < 1 ) CYCLE
               NSZBABA(L) = NtgcAdd(JSTA,L-N1BABA) + 1
               NgangTot  = 0
               DO I=1,NSZBABA(L)
                 SZBABA(I,L) = NgangTot * XtgcAdd(JSTA,4) / 10.D0
                 IF( I == NSZBABA(L) ) EXIT
                 NgangTot = NgangTot + NtgcGang(JSTA,I,L-N1BABA)
               ENDDO
               IF( NgangTotPre > 0 .AND. NgangTot /= NgangTotPre) THEN
                 PRINT 7721,JSTA,L-N1BABA-1,L-N1BABA
7721             FORMAT(/' For stacking type : TGC',I2,' there are',          &
                         ' different total number of wires in layers',I2,     &
                         ' and',I2,' ======> STOP in AMDCOX of MUONBOY !!!')
                 STOP
               ENDIF
               NgangTotPre = NgangTot
             ENDDO
             ZTBABA = NgangTotPre * XtgcAdd(JSTA,4) / 10.D0
             DO L=N1BABA+1,N2BABA
               OBABA1(L) = (Geo_Le(JTYP,JGEO,JOB)/10.D0-ZTBABA) / 2.D0
             ENDDO
             DZBABA = ZTBABA / MAX( NtgcAdd(JSTA,1) , 1 )
           ENDIF
         ELSE
           GO TO 350
         ENDIF
         TBABAM   = TBABAM / N2BABA
         TBABA(1:N2BABA) = TBABA(1:N2BABA) - TBABAM
         NTRITO = NTRITO + 1
#if !defined(ATLAS_NOCHECKMU)
         IF( NTRITO > NTRIMA ) THEN
           PRINT 7704,NTRITO
           STOP
         ENDIF
#endif
         CALL CRERPC(N1BABA,N2BABA,DSBABA,DZBABA,RABABA,                 &
                     OBABA1,OBABA2,TBABA, NSZBABA,SZBABA, NRPCTO, JTGC)
         IRPTGC(NTRITO) = JTGC
         ITRCUT(NTRITO) = 0
         CALL CDECI(JTYP,JFF,JZZ,JOB, JDBTRI(NTRITO))
!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
         IF(Z0 > 0.D0) THEN
           FCETRI(NTRITO) = F0 + Geo_dx(JTYP,JGEO,JOB)*PI/180.D0
         ELSE
           FCETRI(NTRITO) = F0 - Geo_dx(JTYP,JGEO,JOB)*PI/180.D0
         ENDIF
!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
         ZLOTRI(NTRITO) = Geo_Le(JTYP,JGEO,JOB)/10.D0
         IF( SBAREL > 0.D0 ) THEN
            NTRIBA = MAX( NTRIBA , NTRITO )
            TCETRI(NTRITO) = T0 + Geo_dz(JTYP,JGEO,JOB)/10.D0 + TBABAM
            ZCETRI(NTRITO) = Z0 + Geo_dy(JTYP,JGEO,JOB)/10.D0 + Geo_Le(JTYP,JGEO,JOB)/20.D0
         ELSE
            NTRIBI = MIN( NTRIBI , NTRITO )
            TCETRI(NTRITO) = Z0 + SIZ0 * (Geo_dz(JTYP,JGEO,JOB)/10.D0+TBABAM)
            ZCETRI(NTRITO) = T0 + Geo_dy(JTYP,JGEO,JOB)/10.D0 + Geo_Le(JTYP,JGEO,JOB)/20.D0
         ENDIF
         SNITRI(NTRITO) = - Geo_Ws(JTYP,JGEO,JOB)/20.D0
         SNATRI(NTRITO) = - Geo_Wl(JTYP,JGEO,JOB)/20.D0
         SPITRI(NTRITO) = + Geo_Ws(JTYP,JGEO,JOB)/20.D0
         SPATRI(NTRITO) = + Geo_Wl(JTYP,JGEO,JOB)/20.D0
         IF( NTRIBA >= NTRIBI ) THEN
            PRINT 7719
            STOP
         ENDIF
!
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      ENDIF
!
!
      IF( JCUT >= 1 .AND. ( CARTEC(JTEC) == 'MDT' .OR. CARTEC(JTEC) == 'RPC' .OR. CARTEC(JTEC) == 'TGC') ) THEN
!-- Possible cut-outs (in MDTs, RPCs or TGCs) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         IF( CARTEC(JTEC) == 'MDT' ) THEN
            IGEOM = IGEOMA(IC,INOF,INOZ)
#if !defined(ATLAS_NOCHECKMU)
            IF( IGEOM <= 0 ) THEN
              PRINT 7710,IC,INOF,INOZ
7710          FORMAT(/' Atempt to create a cut-out on multilayer ',           &
                      'Ic,Inof,Inoz =',3I3,' which does not already exists',  &
                      ' ======> STOP in AMDCOX of MUONBOY !!!')
              STOP
            ENDIF
#endif
            IF(ISTCUTV(IGEOM) <= 0) THEN
              NCUTTO = NCUTTO + 1
#if !defined(ATLAS_NOCHECKMU)
              IF( NCUTTO > NCUTMA ) THEN
                PRINT 7703,NCUTTO-1
7703            FORMAT(/' Atempt to create a total number of cut-outs >',I5,' ======> STOP in AMDCOX of MUONBOY !!!')
                STOP
              ENDIF
#endif
              ISTCUTV(IGEOM) = NCUTTO
              NBCUT(NCUTTO) = 0
            ENDIF
            KCUT = ISTCUTV(IGEOM)
         ELSE
#if !defined(ATLAS_NOCHECKMU)
            IF( NTRITO <= 0 ) THEN
              PRINT 7713
7713          FORMAT(/' Atempt to create a cut-out on a trigger ',   &
                      'chamber which does not already exists',       &
                      ' ======> STOP in AMDCOX of MUONBOY !!!')
              STOP
            ENDIF
#endif
            IF(ITRCUT(NTRITO) <= 0) THEN
              NCUTTO = NCUTTO + 1
#if !defined(ATLAS_NOCHECKMU)
              IF( NCUTTO > NCUTMA ) THEN
                PRINT 7703,NCUTTO-1
                STOP
              ENDIF
#endif
              ITRCUT(NTRITO) = NCUTTO
              NBCUT(NCUTTO) = 0
            ENDIF
            KCUT = ITRCUT(NTRITO)
         ENDIF
         JTOTCU = INOCUT(JTYP,JGEO,JCUT)
         DO 210 JOBCUT=1,NOBCUT(JTOTCU)
           IF( IOBCUT(JTOTCU,JOBCUT) /= JOB ) CYCLE
           IF( NBCUT(KCUT) >= NSUCUM )        CYCLE
           NBCUT(KCUT) = NBCUT(KCUT) + 1
           NBK = NBCUT(KCUT)
                S0CUT = S0 +  SIS0 *Cut_dx(JTOTCU,JOBCUT)/10.D0
           SCUMEX = S0CUT + SIS0*Cut_Ex(JTOTCU,JOBCUT)/20.D0
           SCUPEX = S0CUT - SIS0*Cut_Ex(JTOTCU,JOBCUT)/20.D0
           ANCUT(KCUT,NBK) = Cut_An(JTOTCU,JOBCUT)*PiS180
             IF(SBAREL > 0.D0) THEN
               ZTCUT(1,KCUT,NBK) =  Z0    + Cut_dy(JTOTCU,JOBCUT)/10.D0
             ELSE   
               ZTCUT(1,KCUT,NBK) =  T0    + Cut_dy(JTOTCU,JOBCUT)/10.D0
             ENDIF
             SSCUT(1,KCUT,NBK)   = SCUMEX + Cut_Ws(JTOTCU,JOBCUT)/20.D0
             SSCUT(2,KCUT,NBK)   = SCUMEX - Cut_Ws(JTOTCU,JOBCUT)/20.D0
             SSCUT(3,KCUT,NBK)   = SCUPEX - Cut_Wl(JTOTCU,JOBCUT)/20.D0
             SSCUT(4,KCUT,NBK)   = SCUPEX + Cut_Wl(JTOTCU,JOBCUT)/20.D0 
           SSCUT(5,KCUT,NBK)     = SSCUT(1,KCUT,NBK)
           ZTCUT(2,KCUT,NBK)     = ZTCUT(1,KCUT,NBK)
           ZTCUT(3,KCUT,NBK)     = ZTCUT(1,KCUT,NBK) + Cut_Le(JTOTCU,JOBCUT)/10.D0
           ZTCUT(4,KCUT,NBK)     = ZTCUT(3,KCUT,NBK)
           ZTCUT(5,KCUT,NBK)     = ZTCUT(1,KCUT,NBK)
210      CONTINUE
         IF( NBCUT(KCUT) <= 0 ) THEN
           NCUTTO = NCUTTO - 1
           IF( CARTEC(JTEC) == 'MDT' ) THEN
              ISTCUTV(IGEOM) = 0
           ELSE
              ITRCUT(NTRITO) = 0
           ENDIF
         ENDIF
      ENDIF
!
350   CONTINUE
!
!
!
500   CONTINUE
501   CONTINUE
502   CONTINUE
!-----------------------------------------------------------------------
!
      NCOUBAS2 = NCOUBA / 2
!
!-- Rearange Z ordering of Barrel multilayers
      DO IC=1,NCOUBA
        DO INOF=1,8
          INOZ = NZMS2 - 1
          DO IZZ=NZMS2,NZM
            IF( IGEOMA(IC,INOF,IZZ) <= 0 )  CYCLE
            INOZ = INOZ + 1
            IGEOMA(IC,INOF,INOZ) = IGEOMA(IC,INOF,IZZ)
          ENDDO
          IF(INOZ < NZM ) IGEOMA(IC,INOF,INOZ+1:NZM) = 0
          INOZ = NZMS2
          DO IZZ=NZMS2-1,1,-1
            IF( IGEOMA(IC,INOF,IZZ) <= 0 )  CYCLE
            INOZ = INOZ - 1
            IGEOMA(IC,INOF,INOZ) = IGEOMA(IC,INOF,IZZ)
          ENDDO
          IF( INOZ > 1 )  IGEOMA(IC,INOF,1:INOZ-1) = 0
        ENDDO
      ENDDO
!
!-- Rearange Z ordering of End-Cap multilayers
      DO IC=NCOUBA+1,NCOCSC
        DO INOF=1,8
          DO 850 IAVAR=1,2
            INOZ   = (IAVAR-1)*NZMS2
            IZZDEB = (IAVAR-1)*NZMS2 + 1
            IZZFIN =   IAVAR  *NZMS2
            DO IZZ=IZZDEB,IZZFIN
              IF( IGEOMA(IC,INOF,IZZ) <= 0 )  CYCLE
              INOZ = INOZ + 1
              IGEOMA(IC,INOF,INOZ) = IGEOMA(IC,INOF,IZZ)
            ENDDO
            IGEOMA(IC,INOF,INOZ+1:IZZFIN) = 0
850       ENDDO
        ENDDO
      ENDDO
!
!-- Re-order trigger chambers
      CALL KROISS(JDBTRI,NTRITO,NORDRE)
      ICT = 1
      DO WHILE( ICT == NORDRE(ICT) )
        ICT = ICT + 1
        IF( ICT > NTRITO ) EXIT
      ENDDO
      IF(ICT < NTRITO) THEN
            IBUFF(ICT:NTRITO) = IRPTGC(NORDRE(ICT:NTRITO))
           IRPTGC(ICT:NTRITO) = IBUFF(ICT:NTRITO)
            IBUFF(ICT:NTRITO) = ITRCUT(NORDRE(ICT:NTRITO))
           ITRCUT(ICT:NTRITO) = IBUFF(ICT:NTRITO)
            IBUFF(ICT:NTRITO) = JDBTRI(NORDRE(ICT:NTRITO))
           JDBTRI(ICT:NTRITO) = IBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = FCETRI(NORDRE(ICT:NTRITO))
           FCETRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = TCETRI(NORDRE(ICT:NTRITO))
           TCETRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = ZCETRI(NORDRE(ICT:NTRITO))
           ZCETRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = ZLOTRI(NORDRE(ICT:NTRITO))
           ZLOTRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = SPATRI(NORDRE(ICT:NTRITO))
           SPATRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = SPITRI(NORDRE(ICT:NTRITO))
           SPITRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = SNATRI(NORDRE(ICT:NTRITO))
           SNATRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
            XBUFF(ICT:NTRITO) = SNITRI(NORDRE(ICT:NTRITO))
           SNITRI(ICT:NTRITO) = XBUFF(ICT:NTRITO)
      ENDIF
!
!-- Store some often used quantities
!
      COSCHAV(1:NGEOMT)     = COS(FCECHAV(1:NGEOMT))
      SINCHAV(1:NGEOMT)     = SIN(FCECHAV(1:NGEOMT))
      ZLOCHAV_inv(1:NGEOMT) = 1.d0 / ZLOCHAV(1:NGEOMT)
      WHERE( STACHAV(1:NGEOMT) > 1.d0 )
        XSTCHAV(1:NGEOMT) = 1.d0 + 1.d0 / ( NINT(ZLOCHAV(1:NGEOMT)/STACHAV(1:NGEOMT)) - 1 ) 
      ELSEWHERE
        XSTCHAV(1:NGEOMT) = 0.d0
      END WHERE
      COSTRI(1:NTRITO) = COS(FCETRI(1:NTRITO))
      SINTRI(1:NTRITO) = SIN(FCETRI(1:NTRITO))
      WHERE( PASTUB(1:NMDTMA) > 0.d0 ) 
        PASTUB_inv(1:NMDTMA) = 1.d0 / PASTUB(1:NMDTMA)
      ELSEWHERE
        PASTUB_inv(1:NMDTMA) = 0.d0
      END WHERE
!
!
      IF( Ignore_Misali == 0 ) THEN
!--- Load position adjustments (A & B lines)
        call load_position_adjustment(ier)        
        if ( ier /= 0 ) then
          print *,' *  ======> STOP in AMDCOX of MUONBOY !!!'
          stop
        endif

      ENDIF
!
!-- Create links between MDT and trigger chambers
!
      CALL createTrigLinks
!
!-- Then flag single multi-layer MDT :
!--    O = single multi-layer
!--    1 = double multi-layer
!--   +2 = double multi-layer, but no adjacent chamber on smaller INOZ
!--   +4 = double multi-layer, but no adjacent chamber on greater INOZ
      DZZ = 10.D0
      ISingle(:) = 0
      DO ISC=1,NMACSC
        DO INOF=1,8
          DO IML=1,2
            IC  = (ISC-1)*2 + IML
            ICO = (ISC-1)*2 + 3 - IML
            DO INOZ=1,NZM
              IGEOM = IGEOMA(IC,INOF,INOZ) 
              IF( IGEOM <= 0 )                 CYCLE
              IF( IGEOMA(ICO,INOF,INOZ) <= 0 ) CYCLE
              ISingle(IGEOM) = 1
              IF(ISC > NMATTR)                 CYCLE
              IF(INOZ == 1) THEN
                ISingle(IGEOM) = ISingle(IGEOM) + 2
              ELSE IF(IGEOMA(ICO,INOF,INOZ-1) <= 0) THEN
                ISingle(IGEOM) = ISingle(IGEOM) + 2
              ELSE          ! trous potentiels
                IGEOI = IGEOMA(ICO,INOF,INOZ)
                ZZZ = ZCECHAV(IGEOI) - ZLOCHAV(IGEOI)/2.
                IGEOI = IGEOMA(ICO,INOF,INOZ-1)
                ZZI = ZCECHAV(IGEOI) + ZLOCHAV(IGEOI)/2.
                IF(ZZZ-ZZI > DZZ) ISingle(IGEOM) = ISingle(IGEOM) + 2
              ENDIF
              IF(INOZ == NZM) THEN
                ISingle(IGEOM) = ISingle(IGEOM) + 4
              ELSE IF(IGEOMA(ICO,INOF,INOZ+1) <= 0) THEN
                ISingle(IGEOM) = ISingle(IGEOM) + 4
              ELSE          ! trous potentiels
                IGEOI = IGEOMA(ICO,INOF,INOZ)
                ZZZ = ZCECHAV(IGEOI) + ZLOCHAV(IGEOI)/2.
                IGEOI = IGEOMA(ICO,INOF,INOZ+1)
                ZZI = ZCECHAV(IGEOI) - ZLOCHAV(IGEOI)/2.
                IF(ZZI-ZZZ > DZZ) ISingle(IGEOM) = ISingle(IGEOM) + 4
              ENDIF
            ENDDO
          ENDDO
        ENDDO
      ENDDO
!
! do not apply cutout if not in Simulation
!
      IF( NoCutOut ) CALL DontApplyCutOut( CARTYP )
!
      PRINT 8000
8000  FORMAT(/' ===> Chamber layout and geometry',                  &
              ' has been transfered to MUONBOY by routine AMDCOX',  &
              ' with the following conventions :'                   &
             /' ===>',51X,'*********',10X,'********'/)
      WRITE(*,*)
      WRITE(*,'("  Muonboy name  :",22(2X,A3))') CARISC(1:NMACSC)
      WRITE(*,*)
      WRITE(*,'("  Station nb.   :",I4,21I5  )') (ISC,ISC=1,NMACSC)
      WRITE(*,*)
      IF( NCOCSC <= 44 ) THEN
        PRINT 8003,CALOHI(1),(IC,IC =1,NCOCSC,2)
        PRINT 8003,CALOHI(2),(IC,IC =2,NCOCSC,2)
      ELSE
        PRINT 8003,CALOHI(1),(IC,IC =1,43,2)
        PRINT 8003,CALOHI(1),(IC,IC =45,NCOCSC,2)
        PRINT 8003,CALOHI(2),(IC,IC =2,44,2)
        PRINT 8003,CALOHI(2),(IC,IC =46,NCOCSC,2)
      ENDIF
      WRITE(*,*)
8003  FORMAT(1X,A5,' m.l. nb. :',I4,21I5  )
      ITYPRI(:) = 0
      DO 880 LPASS=1,6
        NbCAR3 = 0
        DO 870 ISC=1,NMACSC
          CAR3(ISC) = '   '
          DO JTYP=1,MTYP
            IF(ISCISC(JTYP) /= ISC .OR. ITYPRI(JTYP) >= 1) CYCLE
            NbCAR3 = NbCAR3 + 1
            CAR3(ISC) = CARTYP(JTYP)
            ITYPRI(JTYP) = 1
            EXIT
          ENDDO
870     ENDDO
        IF(LPASS == 1)THEN
          WRITE(*,'(" Amdb stations  :",22(2X,A3))') CAR3(1:NMACSC)
        ELSEIF( NbCAR3 > 0 ) THEN
          WRITE(*,'("                 ",22(2X,A3))') CAR3(1:NMACSC)
        ENDIF
880   ENDDO
      WRITE(*,'(/" Max. Barrel m.l. nb. (NCOUBA) :",I4)') NCOUBA
      WRITE(*,'(///" Where all amdb station types are :"/)')
      WRITE(*,'(" Name :",23(2X,A3),4(/7X,23(2X,A3)))') CARTYP(1:MTYP)
      WRITE(*,'(" Jtyp :",I4,22I5,4(/7X,I4,22I5))') (JTYP,JTYP=1,MTYP)
      WRITE(*,*)
!
      IF( MBPri >= 1 ) THEN
        WRITE(*,'(31X,"used  reserved",/,                            &
                &     " number of precision stations",2I6,/,         &
                &     "             trigger stations",2I6,/,         &
                &     "               standard links",2I6,/,         &
                &     "                special links",2I6,/)')       &
          NGEOMT,NGEOMA, NTRITO,NTRIMA, NLINTO,N1LIMA, MLINTO,M1LIMA  
      ENDIF
!     
      isLarge(1:NMACSC) = (ICHPHI(1:NMACSC) == 1)     
      isBOG(:) = .false.
      DO JZZ=-MZZ,MZZ
        INOZ = IZZBOG(JZZ)
        IF( INOZ /= 0 ) isBOG(INOZ) = .true.
      ENDDO
!
!-- Redefine external radius if BOE stations
    IF( hasBOE ) THEN
      RCYLIN(4) = 1400.d0
    ELSE
      RCYLIN(4) = 1260.d0
    ENDIF
    RCYLI2(4) = RCYLIN(4)**2
    RCHAMA    = RCYLIN(4)
    IF( MBPri >= 1 ) WRITE(*,*) ' External radius of spectrometer ',RCYLIN(4),' cm'
!
!-- If asked, print the geometry that will be assumed in MUONBOY
      IF(MBPri >= 4 .OR. LunGeo /= 6)  CALL printGeom(CARTYP)
!
! Do a list of non-cabled channels
!
      isOffChav(1:NGEOMA) = .false.
      isOffTrig(1:NTRITO) = .false.
      CALL FillDeadChannels
!
! Additionnals dead channels
!!$      DO IC=1,NCOUCH
!!$        ISC = (IC+1)/2
!!$        IF( CARISC(ISC)(1:2) /= 'EM' ) CYCLE
!!$        DO INOF=1,8
!!$          DO INOZ=1,NZM
!!$            IGEOM = IGEOMA(IC,INOF,INOZ)
!!$            IF( IGEOM <= 0 )               CYCLE      
!!$            isOffChav(IGEOM) = .true.
!!$          ENDDO
!!$        ENDDO
!!$      ENDDO
!
 END SUBROUTINE AMDCOX
!
