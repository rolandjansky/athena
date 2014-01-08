!
 SUBROUTINE AmdcBarrelToroid
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 CHARACTER(8) :: CAR8
 INTEGER :: ICIR(50,20,2)
 REAL(8) :: XYZCIR(3,2000)
 INTEGER :: ICON( 4,30)
 INTEGER :: JJJA(6), JJJU(6), JJJO(40)
 REAL(8) :: XYZCON(3,120)
 REAL(8) :: VV(400)
 REAL(8) :: XPLAK(6), YPLAK(6)
 REAL(8) :: XPLUK(6), YPLUK(6)
 REAL(8) :: XPLOK(40),YPLOK(40)
 INTEGER :: IBUF(100)
 INTEGER :: ICA, ICB, IFA, IFB, I03, I0C, IFI, INEX, LLL
 INTEGER :: ILATT, IMANAT, IO, IP, IT0, IV, NPLAK, NPLUK
 INTEGER :: IVMDEB_F, IVMDEB_Z, IVMFIN_F, IVMFIN_Z, IVOUS
 INTEGER :: JFF, JZZ, JJ
 INTEGER :: L, NAMATO
 INTEGER :: NCIC, NCIR, NCIRTOT, NCONTOT, NN, NPLOK
 REAL(8) :: COPIS8, COPIS4, COSA, COSF, DAC, DAR, DCIC, DCIR
 REAL(8) :: DYBIEL, DYCLAT, DYLATT, DZCUR
 REAL(8) :: ECRYAT, F0, FF, FPIS4
 REAL(8) :: FPIS2, ANG, A0, AA, RADI, RADIX, RADIY, SINA, SINF, SIPIS4
 REAL(8) :: FPIS8 , RMOY, ROBIEL, TGDAC, TGDAR, TVOUSA, Y0, YATTI, Z0
 REAL(8) :: SIPIS8, SSTRUA, SSTRUI
 REAL(8) :: XATTA, XHEADO
 REAL(8) :: XVOUS1, XVOUS2, XVOUS3, XVOUS4
 REAL(8) :: XVOUSA, XVOUSB, XVOUSC, XVOUSD, XVOUSE, XVOUSF, XVOUSG, XVOUSH
 REAL(8) :: YATTA, YBIELA, YBIELI, YBIELO, YCATTI, YHEADA, YHEADI
 REAL(8) :: YVOUS1, YVOUS2, YVOUS3
 REAL(8) :: YVOUS4, YVOUSA, YVOUSB, YVOUSC, YVOUSD, YVOUSE, YVOUSF, YVOUSG
 REAL(8) :: YVOUSH, YVOUSSOIR, Z1
 REAL(8) :: Z2, ZBIELO, ZHEAD, ZLATT1, ZVOUS1
!
#include "AmdcStand/comamu.inc"
!
      FPIS2  = Pi/2.D0
      FPIS4  = Pi/4.D0
      FPIS8  = Pi/8.D0
      SIPIS4 = SIN(FPIS4)
      COPIS4 = COS(FPIS4)
      SIPIS8 = SIN(FPIS8)
      COPIS8 = COS(FPIS8)
      F0 = - FPIS2 + FPIS8
!
      RMOY   = (RBAMAX+RBAMIN) / 2.D0
      YATTI  = RBAMIN + RCRYO + HCOND/2.D0 + 0.1D0
      XATTA  = XATTI
      YATTA  = YATTI + DYATT
      DYBIEL = 2.5D0 * RRBIEL
      YBIELI = YATTI + DYBIEL
      YBIELA = YATTI + DYBIEL + DLBIEL - 2.D0*EXBIEL
      ZBIELO = SQRT( RRBIEL**2 - EXBIEL**2 )
      ROBIEL = RRBIEL - 0.5D0
      YBIELO = SQRT( ROBIEL**2 - EPBIEL**2 )
      YCATTI = RBAMIN + RCRYO + SQRT( RCRYO**2 - XCATTI**2 ) - 1.D0
      ECRYAT = ECRYOB * 1.3D0
      YHEADI = YATTA
      ZHEAD  = ZATTA
      ZBIELA = MIN( ZHEAD-1.D0 , ZBIELA )
      YHEADA = YHEADI + DYHEAD
      DYLATT = (RBAMAX-RBAMIN-2.D0*RCRYO-HCOND-1.D0) / 2.D0
      DYCLAT = (RBAMAX-RBAMIN-2.D0*RCRYO -2.D0*SQRT(RCRYO**2-DXCLAT**2)+1.D0) / 2.D0
!
      XVOUS1 = XHEADA
      YVOUS1 = YHEADA
      XVOUS2 = XHEADA + (XHEADI-XHEADA)*DTVOUS/(YHEADA-YHEADI)
      YVOUS2 = YHEADA - DTVOUS
      XVOUS3 = - COPIS4*XVOUS2 + SIPIS4*YVOUS2
      YVOUS3 =   SIPIS4*XVOUS2 + COPIS4*YVOUS2
      XVOUS4 = - COPIS4*XVOUS1 + SIPIS4*YVOUS1
      YVOUS4 =   SIPIS4*XVOUS1 + COPIS4*YVOUS1
!
      XVOUSA = XHEADI
      YVOUSA = YHEADI
      XVOUSB = XHEADI + (XHEADA-XHEADI)*DTVOUS/(YHEADA-YHEADI)
      YVOUSB = YHEADI + DTVOUS
      XVOUSC = (TVOUSI+DTVOUS)*SIPIS8 - SVOUSI*COPIS8
      YVOUSC = (TVOUSI+DTVOUS)*COPIS8 + SVOUSI*SIPIS8
      XVOUSD = (TVOUSI+DTVOUS)*SIPIS8 + SVOUSI*COPIS8
      YVOUSD = (TVOUSI+DTVOUS)*COPIS8 - SVOUSI*SIPIS8
      XVOUSE = - COPIS4*XVOUSB + SIPIS4*YVOUSB
      YVOUSE =   SIPIS4*XVOUSB + COPIS4*YVOUSB
      XVOUSF = - COPIS4*XVOUSA + SIPIS4*YVOUSA
      YVOUSF =   SIPIS4*XVOUSA + COPIS4*YVOUSA
      XVOUSG = TVOUSI*SIPIS8 + SVOUSI*COPIS8
      YVOUSG = TVOUSI*COPIS8 - SVOUSI*SIPIS8
      XVOUSH = TVOUSI*SIPIS8 - SVOUSI*COPIS8
      YVOUSH = TVOUSI*COPIS8 + SVOUSI*SIPIS8
!
      NPLAK = 6
      XPLAK(1:6) = (/ XVOUS2, XVOUS3, XVOUSE, XVOUSD, XVOUSC, XVOUSB /)
      YPLAK(1:6) = (/ YVOUS2, YVOUS3, YVOUSE, YVOUSD, YVOUSC, YVOUSB /)
      JJJA(1:NPLAK) = 1
      TVOUSA = YVOUS1*COPIS8 + XVOUS1*SIPIS8
      NPLUK = 6
      XPLUK(1) =   (TVOUSI+DTTROU)   *SIPIS8 - S1TROU*COPIS8
      YPLUK(1) =   (TVOUSI+DTTROU)   *COPIS8 + S1TROU*SIPIS8
      XPLUK(2) = (TVOUSI+TVOUSA)/2.D0*SIPIS8 - S2TROU*COPIS8
      YPLUK(2) = (TVOUSI+TVOUSA)/2.D0*COPIS8 + S2TROU*SIPIS8
      XPLUK(3) =   (TVOUSA-DTTROU)   *SIPIS8 - S1TROU*COPIS8
      YPLUK(3) =   (TVOUSA-DTTROU)   *COPIS8 + S1TROU*SIPIS8
      XPLUK(4) =   (TVOUSA-DTTROU)   *SIPIS8 + S1TROU*COPIS8
      YPLUK(4) =   (TVOUSA-DTTROU)   *COPIS8 - S1TROU*SIPIS8
      XPLUK(5) = (TVOUSI+TVOUSA)/2.D0*SIPIS8 + S2TROU*COPIS8
      YPLUK(5) = (TVOUSI+TVOUSA)/2.D0*COPIS8 - S2TROU*SIPIS8
      XPLUK(6) =   (TVOUSI+DTTROU)   *SIPIS8 + S1TROU*COPIS8
      YPLUK(6) =   (TVOUSI+DTTROU)   *COPIS8 - S1TROU*SIPIS8
      JJJU(1:NPLUK) = 1
      CALL CUTOUT( NPLAK, JJJA,XPLAK,YPLAK, NPLUK, JJJU,XPLUK,YPLUK,   &
                                40, 1,  LLL, IBUF, JJJO,XPLOK,YPLOK )
      NPLOK = IBUF(1)
!
      SSTRUI = TSTRUI*SIPIS8/COPIS8 - ECOND/2.D0/COPIS8
      SSTRUA = TSTRUA*SIPIS8/COPIS8 - ECOND/2.D0/COPIS8
!
      NCIR  = 0
      NCIC  = 0
      NCIRTOT = 0
      NCONTOT = 0
      DAR   = FPIS4/NCURV
      DAC   = FPIS4/NCURC
      TGDAR = TAN(DAR)
      TGDAC = TAN(DAC)
      DCIR = FPIS2/NCURV
      DCIC = FPIS2/NCURC
!
      DO 40 I03=0,3
        A0 = I03*FPIS2
        IF(I03.EQ.0) THEN
          Y0 =   RBAMAX - RCRYO - RCURV
          Z0 =   ZBAMAX - RCRYO - RCURV
        ELSEIF(I03.EQ.1) THEN
          Y0 =   RBAMAX - RCRYO - RCURV
          Z0 = - ZBAMAX + RCRYO + RCURV
        ELSEIF(I03.EQ.2) THEN
          Y0 =   RBAMIN + RCRYO + RCURV
          Z0 = - ZBAMAX + RCRYO + RCURV
        ELSEIF(I03.EQ.3) THEN
          Y0 =   RBAMIN + RCRYO + RCURV
          Z0 =   ZBAMAX - RCRYO - RCURV
        ENDIF
!
        DO 25 I0C=0,NCURV-1
          NCIR = NCIR + 1
          AA = A0 + I0C*DCIR
          SINA = SIN(AA)
          COSA = COS(AA)
          DO 21 IFI=1,NCRYO
            FF = (IFI-1.D0)*2.D0*Pi/NCRYO
            SINF = SIN(FF)
            COSF = COS(FF)
            DO INEX=1,2
              RADI = RCRYO - (INEX-1)*ECRYOB
              NCIRTOT = NCIRTOT + 1
              ICIR(IFI,NCIR,INEX) = NCIRTOT
              XYZCIR(1,NCIRTOT) = RADI*COSF
              XYZCIR(2,NCIRTOT) = Y0 + (RCURV+RADI*SINF)*(SINA+TGDAR*COSA)
              XYZCIR(3,NCIRTOT) = Z0 + (RCURV+RADI*SINF)*(COSA-TGDAR*SINA)
            ENDDO
21        ENDDO
25      ENDDO
!
        DO 35 I0C=0,NCURC-1
          NCIC = NCIC + 1
          AA = A0 + I0C*DCIC
          SINA = SIN(AA)
          COSA = COS(AA)
          DO IFI=1,4
            IF(IFI == 1) THEN
              RADIX =  ECOND/2.D0
              RADIY =  HCOND/2.D0
            ELSEIF( IFI == 2 ) THEN
              RADIX =  ECOND/2.D0
              RADIY = -HCOND/2.D0
            ELSEIF( IFI == 3 ) THEN
              RADIX = -ECOND/2.D0
              RADIY = -HCOND/2.D0
            ELSEIF( IFI == 4 ) THEN
              RADIX = -ECOND/2.D0
              RADIY =  HCOND/2.D0
            ENDIF
            NCONTOT = NCONTOT + 1
            ICON(IFI,NCIC) = NCONTOT
            XYZCON(1,NCONTOT) = RADIX
            XYZCON(2,NCONTOT) = Y0 + (RCURV+RADIY)*(SINA+TGDAC*COSA)
            XYZCON(3,NCONTOT) = Z0 + (RCURV+RADIY)*(COSA-TGDAC*SINA)
          ENDDO
35      ENDDO
!
40    ENDDO
!
!
!
      IVMDEB_F = NVMATI + 1
!
!
!-- Bypass all the toroid but the coils for the magnet system visualization
      IF( IFLAG_MATIERE_MAGNETSYSTEM(1) == 1 ) GO TO 75
!
      NAMATO = NAMATI
      CALL ADEDMAT( F0, NCIRTOT, XYZCIR )
!-- Loop over the NCIR=4*NCURV straight segments of one cryostat
      DO 70 ICA=1,NCIR
        ICB = MOD(ICA,NCIR) + 1
!-- Define each straight segment of cryostat as a new volume
        CALL NEWMAT( 'CryoBar_', MATCRY, 1, 11 )
!-- Loop over 2*NCRYO faces of one straight segment of one cryostat
        DO 61 L=1,2
          DO IFA=1,NCRYO
            IFB = MOD(IFA,NCRYO) + 1
            IBUF(1) = NAMATO + ICIR(IFA,ICA,L)
            IBUF(2) = NAMATO + ICIR(IFA,ICB,L)
            IBUF(3) = NAMATO + ICIR(IFB,ICB,L)
            IBUF(4) = NAMATO + ICIR(IFB,ICA,L)
            CALL PLAMATII( 4, IBUF )
          ENDDO
61      ENDDO
        CALL ENDMAT
70    ENDDO
!
75    CONTINUE
!
!
      NAMATO = NAMATI
      CALL ADEDMAT( F0, NCONTOT, XYZCON )
!-- Loop over the NCIC=4*NCURC straight segments of conductor (+ cold mass)
      DO 90 ICA=1,NCIC
        ICB = MOD(ICA,NCIC) + 1
!-- Define each straight segment of conductor (+ cold mass) as a new volume
        CALL NEWMAT( 'CondBar_', MATCON, 1, 12 )
!-- Loop over the 4 faces of one straight segment of conductor (+ cold mass)
        DO IFA=1,4
          IFB = MOD(IFA,4) + 1
          IBUF(1) = NAMATO + ICON(IFA,ICA)
          IBUF(2) = NAMATO + ICON(IFA,ICB)
          IBUF(3) = NAMATO + ICON(IFB,ICB)
          IBUF(4) = NAMATO + ICON(IFB,ICA)
          CALL PLAMATII( 4, IBUF )
        ENDDO
        CALL ENDMAT
90    ENDDO
!
!
!-- Bypass the rest of the toroid for the magnet system visualization
      IF( IFLAG_MATIERE_MAGNETSYSTEM(1) == 1 ) GO TO 380
!
!-- The first "voussoir"  ( JFF=1 , JZZ=1 )
      IVMDEB_Z = NVMATI + 1
!
      ZVOUS1 = ZVOUS(1)
!-- Define each cryostat of one attachement of voussoir as a new volume
      CALL NEWMAT( 'CryoAtt_', MATCRY, 1, 1 )
      CALL DVTUBE( F0, 2,                                       &
       Zero,YCATTI,ZVOUS1,  XCATTI,       Zero,ZCATTI,          &
       Zero,YCATTI,ZVOUS1,  XCATTI-ECRYAT,Zero,ZCATTI-ECRYAT,   &
       Zero,YATTA, ZVOUS1,  XATTA,        Zero,ZATTA,           &
       Zero,YATTA, ZVOUS1,  XATTA -ECRYAT,Zero,ZATTA -ECRYAT )
      CALL ENDMAT
!-- Define each attachement of voussoir as a new volume
      CALL NEWMAT( 'AttVous_', MATATT, 1, 1 )
!**** CALL DVBARR(F0, 2, Zero,YATTI,ZVOUS1, XATTI,Zero,ZATTI,
!****+                   Zero,YATTA,ZVOUS1, XATTA,Zero,ZATTA )
      VV( 1: 3) = (/ Zero, YBIELI,        ZVOUS1-ZBIELO /)
      VV( 4: 6) = (/ Zero, YBIELI,        ZVOUS1-ZBIELI /)
      VV( 7: 9) = (/ Zero, YBIELI-DYBIEL, ZVOUS1-ZATTI  /)
      VV(10:12) = (/ Zero, YBIELI-DYBIEL, ZVOUS1+ZATTI  /)
      VV(13:15) = (/ Zero, YBIELI,        ZVOUS1+ZBIELI /)
      VV(16:18) = (/ Zero, YBIELI,        ZVOUS1+ZBIELO /)
      NN = 6
      DO JJ=1,10
        ANG = (JJ-1.D0)*TwoPi/10.D0
        IF( RRBIEL*COS(ANG) >= EXBIEL )  CYCLE
        NN = NN + 1
        VV(3*NN-1) = YBIELI - EXBIEL + RRBIEL*COS(ANG)
        VV(3*NN  ) =     ZVOUS1      + RRBIEL*SIN(ANG)
      ENDDO
      CALL DVGENE(F0, NN, VV, 1,-XATTI,+XATTI)
      VV( 1: 3) = (/ Zero, YBIELA,        ZVOUS1+ZBIELO /)
      VV( 4: 6) = (/ Zero, YBIELA,        ZVOUS1+ZBIELA /)
      VV( 7: 9) = (/ Zero, YBIELA+DYBIEL, ZVOUS1+ZBIELA /)
      VV(10:12) = (/ Zero, YBIELA+DYBIEL, ZVOUS1-ZBIELA /)
      VV(13:15) = (/ Zero, YBIELA,        ZVOUS1-ZBIELA /)
      VV(16:18) = (/ Zero, YBIELA,        ZVOUS1-ZBIELO /)
      NN = 6
      DO JJ=1,10
        ANG = (JJ-1.D0)*TwoPi/10.D0
        IF( RRBIEL*COS(ANG) >= EXBIEL )  CYCLE
        NN = NN + 1
        VV(3*NN-1) = YBIELA + EXBIEL - RRBIEL*COS(ANG)
        VV(3*NN  ) =     ZVOUS1      - RRBIEL*SIN(ANG)
      ENDDO
      CALL DVGENE(F0, NN, VV, 1,-XATTA,+XATTA)
      CALL ENDMAT
!-- Define each "biellette" as a new volume
      CALL NEWMAT( 'AttRod__', MATROD, 1, 1 )
      VV(1:3) = (/ Zero, YBIELA+EXBIEL-YBIELO, ZVOUS1+EPBIEL /)
      VV(4:6) = (/ Zero, YBIELI-EXBIEL+YBIELO, ZVOUS1+EPBIEL /)
      NN = 2
      DO JJ=1,10
        ANG = (JJ-1.D0)*TwoPi/10.D0
        IF( ROBIEL*COS(ANG) >= YBIELO )  CYCLE
        NN = NN + 1
        VV(3*NN-1) = YBIELI - EXBIEL + ROBIEL*COS(ANG)
        VV(3*NN  ) =     ZVOUS1      + ROBIEL*SIN(ANG)
      ENDDO
      NN = NN + 1
      VV(3*NN-2:3*NN) = (/ Zero, YBIELI-EXBIEL+YBIELO, ZVOUS1-EPBIEL /)
      NN = NN + 1
      VV(3*NN-2:3*NN) = (/ Zero, YBIELA+EXBIEL-YBIELO, ZVOUS1-EPBIEL /)
      DO JJ=1,10
        ANG = (JJ-1.D0)*TwoPi/10.D0
        IF( ROBIEL*COS(ANG) >= YBIELO )  CYCLE
        NN = NN + 1
        VV(3*NN-1) = YBIELA + EXBIEL - ROBIEL*COS(ANG)
        VV(3*NN  ) =     ZVOUS1      - ROBIEL*SIN(ANG)
      ENDDO
      CALL DVGENE(F0, NN, VV, 1,-XATTI,+XATTI)
      CALL ENDMAT
      IF( TVOUSI > 400.D0 ) THEN
!-- Version 1 (old) of voussoirs !!!!
!-- Define each head of voussoir as a new volume
        CALL NEWMAT( 'VousHead', MATHEA, 1, 1 )
!****** CALL DVBARR(F0, 2, Zero,YHEADI,ZVOUS1, XHEADI,Zero,ZHEAD,
!******+                   Zero,YHEADA,ZVOUS1, XHEADA,Zero,ZHEAD )
        DZCUR = ZHEAD - ZBIELA
        CALL DVTUBE( F0, 2,                               &
          ZERO,YHEADI,ZVOUS1, XHEADI      ,Zero,ZHEAD,    &
          ZERO,YHEADI,ZVOUS1, XHEADI-DZCUR,Zero,ZBIELA,   &
          ZERO,YHEADA,ZVOUS1, XHEADA      ,Zero,ZHEAD,    &
          ZERO,YHEADA,ZVOUS1, XHEADA-DZCUR,Zero,ZBIELA )
        CALL ENDMAT
!-- Define each top of head of voussoir as a new volume
        CALL NEWMAT( 'VousHeTo', MATHEA, 1, 1 )
        XHEADO = XHEADA - (XHEADA-XHEADI)*DZCUR/(YHEADA-YHEADI)
        CALL DVBARR( F0, 2,                                    &
          ZERO,YHEADA,      ZVOUS1, XHEADA-DZCUR,Zero,ZBIELA,  &
          ZERO,YHEADA-DZCUR,ZVOUS1, XHEADO-DZCUR,Zero,ZBIELA )
        CALL ENDMAT
!-- Define each voussoir as a new volume
        CALL NEWMAT( 'Voussoir', MATVOU, 1, 1 )
!-- Top of voussoir
        VV( 1:12) = (/ XVOUS1, YVOUS1, Zero, XVOUS2, YVOUS2, Zero,    &
                       XVOUS3, YVOUS3, Zero, XVOUS4, YVOUS4, Zero /)
        CALL DVGENE(F0, 4,VV, 3,ZVOUS1-ZHEAD,ZVOUS1+ZHEAD)
!-- Bottom of voussoir
        VV( 1: 6) = (/ XVOUSA, YVOUSA, Zero, XVOUSB, YVOUSB, Zero /)
        VV( 7:12) = (/ XVOUSC, YVOUSC, Zero, XVOUSD, YVOUSD, Zero /)
        VV(13:18) = (/ XVOUSE, YVOUSE, Zero, XVOUSF, YVOUSF, Zero /)
        VV(19:24) = (/ XVOUSG, YVOUSG, Zero, XVOUSH, YVOUSH, Zero /)
        CALL DVGENE(F0, 8,VV, 3,ZVOUS1-ZHEAD,ZVOUS1+ZHEAD)
!-- Center plane of voussoir
        VV(1:3*NPLOK) = (/ (XPLOK(JJ),YPLOK(JJ),Zero, JJ=1,NPLOK) /)
        Z1 = ZVOUS1 - DZVOUS/2.D0
        Z2 = ZVOUS1 + DZVOUS/2.D0
        CALL DVGENE( F0, NPLOK, VV, 3, Z1,Z2 )
        CALL ENDMAT
      ELSE
!-- Version 2 (new) of voussoirs !!!!
        CALL MATVOUSSOIRHEAD(-FPIS2, 1, 1, YVOUSSOIR)
        CALL MATVOUSSOIRS(-FPIS2, 1, 1, ZHEAD, YVOUSSOIR)
      ENDIF
!-- Define each strut as a new volume
      CALL NEWMAT( 'Strut___', MATSTR, 1, 1 )
      CALL DVBARR( F0-FPIS8, 2,                        &
        Zero,TSTRUA,ZVOUS1, SSTRUA,Zero,DZSTRU/2.D0,   &
        Zero,TSTRUI,ZVOUS1, SSTRUI,Zero,DZSTRU/2.D0 )
      CALL ENDMAT
!
      IVMFIN_Z = NVMATI
!
!-- Copy the first voussoir (+attachement+head+strut)  NVOUS-1 times along Z
      DO IVOUS=2,NVOUS
        CALL INCR_NTMATI
        TRANSMATI(3,4,NTMATI) = ZVOUS(IVOUS) - ZVOUS1
        IT0 = NTMATI
        DO IV=IVMDEB_Z,IVMFIN_Z
          CALL DECODE_JMATI( IV, CAR8,IMANAT,JFF,JZZ )
          CALL COPYMAT( IV, IT0, CAR8,IMANAT,JFF,IVOUS )
        ENDDO
      ENDDO
!
!
!-- The first "latte" or "rib"  ( JFF=1 , JZZ=1 )
      IVMDEB_Z = NVMATI + 1
!
      ZLATT1 = ZLATT(1)
!-- Define each cryostat of one "latte" as a new volume
      CALL NEWMAT( 'CryoRib_', MATCRY, 1, 1 )
      CALL DVTUBE( F0, 2,                                        &
        Zero,RMOY,ZLATT1, DXCLAT,       -DYCLAT,DZCLAT,          &
        Zero,RMOY,ZLATT1, DXCLAT-ECRYOB,-DYCLAT,DZCLAT-ECRYOB,   &
        Zero,RMOY,ZLATT1, DXCLAT,        DYCLAT,DZCLAT,          &
        Zero,RMOY,ZLATT1, DXCLAT-ECRYOB, DYCLAT,DZCLAT-ECRYOB )
      CALL ENDMAT
!-- Define each "latte" as a new volume
      CALL NEWMAT( 'Rib_____', MATLAT, 1, 1 )
      CALL DVBARR(F0, 3, Zero,RMOY,ZLATT1, DXLATT,DYLATT,-DZLATT,   &
                         Zero,RMOY,ZLATT1, DXLATT,DYLATT, DZLATT )
      CALL ENDMAT
!
      IVMFIN_Z = NVMATI
!
!-- Copy the first latte  NLATT-1 times along Z
      DO ILATT=2,NLATT
        CALL INCR_NTMATI
        TRANSMATI(3,4,NTMATI) = ZLATT(ILATT) - ZLATT1
        IT0 = NTMATI
        DO IV=IVMDEB_Z,IVMFIN_Z
          CALL DECODE_JMATI( IV, CAR8,IMANAT,JFF,JZZ )
          CALL COPYMAT( IV, IT0, CAR8,IMANAT,JFF,ILATT )
        ENDDO
      ENDDO
!
!
!-- Define straight section of cryogenic ring as a new volume
      IF( NANNO > 48 ) THEN
        PRINT 7771
7771    FORMAT(' In MATIERE ; NANNO is too big for IBUF  =====>  STOP !')
        STOP
      ENDIF
      CALL NEWMAT( 'CryoRing', MATANN, 1, 9 )
      DO IO=1,NANNO
        ANG = (IO-1) * TwoPi / NANNO
        VV(3*IO-1) = TANNO + RANNO * COS(ANG)
        VV(3*IO  ) = ZANNO + RANNO * SIN(ANG)
        VV(3*IO-2) = VV(3*IO-1) * SIPIS8/COPIS8 - ECOND/2.D0
        IBUF(IO)     = NAMATI + IO
      ENDDO
      IBUF(NANNO+1) = NAMATI + 1
      DO IO=1,NANNO
        ANG = (NANNO+1-IO) * TwoPi / NANNO
        VV(3*IO-1+3*NANNO) = TANNO + (RANNO-ECRYOB) * COS(ANG)
        VV(3*IO  +3*NANNO) = ZANNO + (RANNO-ECRYOB) * SIN(ANG)
        VV(3*IO-2+3*NANNO) = VV(3*IO-1+3*NANNO)*SIPIS8/COPIS8-ECOND/2.D0
        IBUF(NANNO+1+IO) = NAMATI + NANNO + IO
      ENDDO
      IBUF(2*NANNO+2)   = NAMATI + NANNO + 1
      CALL ADEDMAT( F0-FPIS8, 2*NANNO, VV )
      CALL PLAMATII( 2*NANNO+2, IBUF )
      DO IO=1,2*NANNO
        VV(3*IO-2) = -VV(3*IO-2)
      ENDDO
      DO IP=1,2*NANNO+2
        IBUF(IP) = IBUF(IP) + 2*NANNO
      ENDDO
      CALL ADEDMAT( F0-FPIS8, 2*NANNO, VV )
      CALL PLAMATII( 2*NANNO+2, IBUF )
      CALL CLOMAT
      CALL ENDMAT
!
380   CONTINUE
!
!
!-- Copy volumes 8-1 times around Phi !!!
      IVMFIN_F = NVMATI
      CALL SYFMAT(IVMDEB_F,IVMFIN_F)
!
 END SUBROUTINE AmdcBarrelToroid
!
!
 SUBROUTINE AmdcEndcapToroid
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 REAL(8) :: VV(400), UU(3)
 REAL(8) :: XEC(20),YEC(20)
 REAL(8) :: XSNAK(20),YSNAK(20),ZSNAK(20)
 INTEGER :: I, I03, IP, ITECAI
 INTEGER :: IVMDEB_F, IVMFIN_F, IVMFIN_Z
 INTEGER :: JJ, NN
 REAL(8) :: ANG, ANG0, DZECA, F0, COSEC, ANGEC, DSEC1, DSEC2, DTECA
 REAL(8) :: FPIS2, FPIS4, FPIS8, COPIS4, COPIS8, SIPIS4, SIPIS8
 REAL(8) :: SECA1, SECA1P, SECA2, SECA2P, SECA3, SECA4
 REAL(8) :: TCO_FECTU, TECA1P, TSI_FECTU
 REAL(8) :: Y0, Z0, ZCUR, ZECAMO
!
#include "AmdcStand/comamu.inc"
!
      FPIS2  = Pi/2.D0
      FPIS4  = Pi/4.D0
      FPIS8  = Pi/8.D0
      SIPIS4 = SIN(FPIS4)
      COPIS4 = COS(FPIS4)
      SIPIS8 = SIN(FPIS8)
      COPIS8 = COS(FPIS8)
      F0 = - FPIS2 + FPIS8
!
      XEC( 1) =   TECMA1*SIPIS4 - SECMA1*COPIS4
      YEC( 1) =   TECMA1*COPIS4 + SECMA1*SIPIS4
      XEC( 2) =   TECMA2*SIPIS8 + SECMA2*COPIS8
      YEC( 2) =   TECMA2*COPIS8 - SECMA2*SIPIS8
      XEC( 3) =   TECMA2*SIPIS8 - SECMA2*COPIS8
      YEC( 3) =   TECMA2*COPIS8 + SECMA2*SIPIS8
      XEC( 4) =                   SECMA1
      YEC( 4) =   TECMA1
      XEC( 5) =                 - SECMA1
      YEC( 5) =   TECMA1
      VV(1:3) = (/ XEC(2)-XEC(1), YEC(2)-YEC(1), CP0000 /)
      UU(1:3) = (/ XEC(3)-XEC(2), YEC(3)-YEC(2), CP0000 /)
      CALL NORM3V(VV)
      CALL NORM3V(UU)
      COSEC = DOT_PRODUCT( VV(1:3), UU(1:3))
      ANGEC = ACOS( COSEC )
      DSEC2 = ECRYOE * TAN( ANGEC/2.D0 )
      VV(1:3) = (/ XEC(4)-XEC(3), YEC(4)-YEC(3), CP0000 /)
      UU(1:3) = (/ XEC(5)-XEC(4), YEC(5)-YEC(4), CP0000 /)
      CALL NORM3V(VV)
      CALL NORM3V(UU)
      COSEC = DOT_PRODUCT( VV(1:3), UU(1:3))
      ANGEC = ACOS( COSEC )
      DSEC1 = ECRYOE * TAN( ANGEC/2.D0 )
      XEC( 6) =                          - (SECMA1+DSEC1)
      YEC( 6) =   (TECMA1-ECRYOE)
      XEC( 7) =                            (SECMA1+DSEC1)
      YEC( 7) =   (TECMA1-ECRYOE)
      XEC( 8) =   (TECMA2-ECRYOE)*SIPIS8 - (SECMA2-DSEC2)*COPIS8
      YEC( 8) =   (TECMA2-ECRYOE)*COPIS8 + (SECMA2-DSEC2)*SIPIS8
      XEC( 9) =   (TECMA2-ECRYOE)*SIPIS8 + (SECMA2-DSEC2)*COPIS8
      YEC( 9) =   (TECMA2-ECRYOE)*COPIS8 - (SECMA2-DSEC2)*SIPIS8
      XEC(10) =   (TECMA1-ECRYOE)*SIPIS4 - (SECMA1+DSEC1)*COPIS4
      YEC(10) =   (TECMA1-ECRYOE)*COPIS4 + (SECMA1+DSEC1)*SIPIS4
!
      ZECAMO = (ZECAMA+ZECAMI) / 2.D0
      DZECA  = (ZECAMA-ZECAMI) / 2.D0 - EPZECA
      SECA1  = TECA1*SIPIS8/COPIS8 - EPECO/2.D0/COPIS8
      SECA2  = TECA2*SIPIS8/COPIS8 - EPECO/2.D0/COPIS8
      SECA3  = TECA3*SIPIS8/COPIS8 - EPECO/2.D0/COPIS8
      SECA4  = TECA4*SIPIS8/COPIS8 - EPECO/2.D0/COPIS8
      ITECAI = 0
      IF( ABS(TECA2-TECA1-TECA4+TECA3) > 4.D0 ) THEN
        ITECAI = 1
        DTECA  = TECA4 - TECA3
        TECA1P = TECA1 + DTECA
        SECA1P = TECA1P*SIPIS8/COPIS8 - (EPECO/2.D0+DTECA)/COPIS8
        SECA2P = TECA2 *SIPIS8/COPIS8 - (EPECO/2.D0+DTECA)/COPIS8
      ENDIF
!
!
!
      IVMDEB_F = NVMATI + 1
!
!
!-- Bypass all elements but the coils for the magnet system visualization
      IF( IFLAG_MATIERE_MAGNETSYSTEM(2) == 1 ) GO TO 470
!
!-- Define each outer section of the End-Cap cryostats as a new volume
      CALL NEWMAT( 'CryoEout', MATECR, 1, 21 )
      VV(1:30) = (/ (XEC(IP),YEC(IP),Zero, IP=1,10) /)
      CALL DVGENE( F0, 10, VV, 3, ZECMI+ECRYOF,ZECMA-ECRYOF )
      CALL ENDMAT
!
!
!-- Define each inner section of the End-Cap cryostats as a new volume
      CALL NEWMAT( 'CryoEin_', MATECR, 1, 21 )
      DO I=1,5
        ANG = FPIS4 * ( 1.D0 + I/4.D0 )
        VV(3*I-2)  =  RECMI         * COS(ANG)
        VV(3*I-1)  =  RECMI         * SIN(ANG)
        VV(31-3*I) = (RECMI+ECRYOR) * COS(ANG)
        VV(32-3*I) = (RECMI+ECRYOR) * SIN(ANG)
      ENDDO
      CALL DVGENE( F0, 10, VV, 3, ZECMI+ECRYOF,ZECMA-ECRYOF )
      CALL ENDMAT
!
!
!-- Define each side section of the End-Cap cryostats as a new volume
      TSI_FECTU = TECTU * SIN(FECTU*PiS180)
      TCO_FECTU = TECTU * COS(FECTU*PiS180)
      DO 460 JJ=0,1
        ZCUR = ZECMI + JJ*(ZECMA-ECRYOF-ZECMI)
        CALL NEWMAT( 'CryoEsid', MATECR, 1, 21+JJ )
        DO I=1,5
          ANG = FPIS4 * ( 1.D0 + I/4.D0 )
          VV(3*I-2) = RECMI * COS(ANG)
          VV(3*I-1) = RECMI * SIN(ANG)
        ENDDO
        VV(16:30) = (/ (XEC(IP),YEC(IP),Zero, IP=5,1,-1) /)
        VV(31:33) = VV(1:3)
        DO I=1,NECTU+1
          ANG = (I-1.D0)*TwoPi/NECTU
          VV(31+3*I) = TSI_FECTU + RECTU * COS(ANG)
          VV(32+3*I) = TCO_FECTU + RECTU * SIN(ANG)
        ENDDO
        CALL DVGENE( F0, 12+NECTU, VV, 3, ZCUR,ZCUR+ECRYOF )
        CALL ENDMAT
460   ENDDO
!
!
!-- Define each stay tube of the End-Cap cryostats as a new volume
      CALL NEWMAT( 'StayTube', MATECR, 1, 21 )
      DO I=1,NECTU+1
        ANG = (I-1.D0)*TwoPi/NECTU
        VV(3*I-2) = TSI_FECTU + RECTU * COS(ANG)
        VV(3*I-1) = TCO_FECTU + RECTU * SIN(ANG)
      ENDDO
      DO I=1,NECTU+1
        ANG = - (I-1.D0)*TwoPi/NECTU
        VV(3*NECTU+1+3*I) = TSI_FECTU + (RECTU+ECRYOE) * COS(ANG)
        VV(3*NECTU+2+3*I) = TCO_FECTU + (RECTU+ECRYOE) * SIN(ANG)
      ENDDO
      CALL DVGENE( F0, 2*NECTU+2, VV, 3, ZECMI+ECRYOF,ZECMA-ECRYOF )
      CALL ENDMAT
!
470   CONTINUE
!
!
!-- Define each coil of the End-Cap as a new volume
      CALL NEWMAT( 'CondEndC', MATECO, 1, 21 )
      NN = 0
      DO 490 I03=0,3
        ANG0 = I03 * FPIS2
        IF( I03.EQ.0 ) THEN
          Y0 = TECOMA - RCUECO
          Z0 = ZECOMA - RCUECO
        ELSEIF( I03.EQ.1 ) THEN
          Y0 = TECOMA - RCUECO
          Z0 = ZECOMI + RCUECO
        ELSEIF( I03.EQ.2 ) THEN
          Y0 = TECOMI + RCUECO
          Z0 = ZECOMI + RCUECO
        ELSEIF( I03.EQ.3 ) THEN
          Y0 = TECOMI + RCUECO
          Z0 = ZECOMA - RCUECO
        ENDIF
        DO I=1,NCUECO
          NN = NN + 1
          ANG = ANG0 + (I-1.D0)*FPIS2/(NCUECO-1.D0)
          VV(3*NN-1) = Y0 + RCUECO * SIN(ANG)
          VV(3*NN  ) = Z0 + RCUECO * COS(ANG)
        ENDDO
490   ENDDO
      CALL DVGENE( F0-FPIS8, NN, VV, 1, EPECO/2.D0,-EPECO/2.D0 )
      CALL ENDMAT
!-------------------------------------------------------------------
!**      IV1 = NVMATI
!**      CALL NEWMAT( 'Tempo_01', MATGIRD, 1, 21 )
!**      X0 = EPECO
!**      Y0 = (TECOMA+TECOMI) / 2.D0
!**      Z0 = (ZECOMA+ZECOMI) / 2.D0
!**      R0 = 100.D0
!**      CALL DVPIPE( F0-FPIS8, 16, ZERO,Z360, -X0,Y0,Z0, R0,Zero,   &
!**                                             X0,Y0,Z0, R0,Zero )
!**      CALL ENDMAT
!**      IV2 = NVMATI
!**      CALL BOOLMAT( 1,0, IV1,IV2, 'CondEndC',MATECO,1,21, IVC )
!-------------------------------------------------------------------
!
!
!-- Bypass the rest of the EC toroid for the magnet system visualization
      IF( IFLAG_MATIERE_MAGNETSYSTEM(2) == 1 ) GO TO 500
!
!-- Define each Z,S stiffening plate of the End-Cap as a new volume
      CALL NEWMAT( 'EndStifZ', MATEPL, 1, 21 )
      IF( ITECAI.LE.0 ) THEN
        CALL DVBARR( F0, 2, Zero,TECA1,ZECAMO, SECA1,Zero,DZECA,   &
                            Zero,TECA2,ZECAMO, SECA2,Zero,DZECA )
      ELSE
        VV( 1: 6) = (/  SECA1,  TECA1, Zero, -SECA1,  TECA1, Zero /)
        VV( 7:12) = (/ -SECA2,  TECA2, Zero,-SECA2P,  TECA2, Zero /)
        VV(13:18) = (/-SECA1P, TECA1P, Zero, SECA1P, TECA1P, Zero /)
        VV(19:24) = (/ SECA2P,  TECA2, Zero,  SECA2,  TECA2, Zero /)
        CALL DVGENE( F0, 8, VV, 3, ZECAMO-DZECA,ZECAMO+DZECA )
      ENDIF
      CALL ENDMAT
      CALL NEWMAT( 'EndStifZ', MATEPL, 1, 22 )
      CALL DVBARR( F0, 2, Zero,TECA3,ZECAMO, SECA3,Zero,DZECA,   &
                          Zero,TECA4,ZECAMO, SECA4,Zero,DZECA )
      CALL ENDMAT
!
!
!-- Define each T,S stiffening plate of the End-Cap as a new volume
      CALL NEWMAT( 'EndStifT', MATEPL, 1, 21 )
      VV( 1: 6) = (/ SECA1, TECA1, Zero,-SECA1, TECA1, Zero /)
      VV( 7:12) = (/-SECA4, TECA4, Zero, SECA4, TECA4, Zero /)
      VV(13:15) = (/ SECA1, TECA1, Zero /)
      DO I=1,NECTU+1
        ANG = (I-1.D0)*TwoPi/NECTU
        VV(3*I+13) = TSI_FECTU + (RECTU+DRECTU) * COS(ANG)
        VV(3*I+14) = TCO_FECTU + (RECTU+DRECTU) * SIN(ANG)
      ENDDO
      CALL DVGENE( F0, NECTU+6, VV, 3, ZECAMI,ZECAMI+EPZECA )
      CALL ENDMAT
      CALL NEWMAT( 'EndStifT', MATEPL, 1, 22 )
      CALL DVGENE( F0, NECTU+6, VV, 3, ZECAMA-EPZECA,ZECAMA )
      CALL ENDMAT
!
500   CONTINUE
!
!
!-- Copy volumes 8-1 times around Phi !!!
      IVMFIN_F = NVMATI
      CALL SYFMAT(IVMDEB_F,IVMFIN_F)
!
!
!-- Bypass the service turret for the magnet system visualization
      IF( IFLAG_MATIERE_MAGNETSYSTEM(2) == 1 ) GO TO 510
!
!-- Define service turret of the End-Cap as a new volume
      IF( MATETU >= 1 ) THEN
        CALL NEWMAT( 'ECTurret', MATETU, 3, 23 )
        CALL DVPIPE( FPIS2, 20, Zero,Z360,                  &
          TECMA2       ,Zero,ZECMA-DZ1ETU, RO1ETU,RI1ETU,   &
          TECMA2+DY1ETU,Zero,ZECMA-DZ1ETU, RO1ETU,RI1ETU )
        XSNAK(1:4) = Zero
        YSNAK(1) = TECMA2+DY1ETU
        YSNAK(2) = TECMA2+DY1ETU+DY2ETU
        YSNAK(3) = TECMA2+DY1ETU+DY2ETU
        YSNAK(4) = TECMA2+DY1ETU+DY2ETU+DY3ETU
        ZSNAK(1) = ZECMA -DZ2ETU
        ZSNAK(2) = ZECMA -DZ2ETU
        ZSNAK(3) = ZECMA -DZ3ETU
        ZSNAK(4) = ZECMA -DZ3ETU
        CALL DVSNAK( 4, XSNAK,YSNAK,ZSNAK, 12, RR2ETU )
        CALL ENDMAT
      ENDIF
!
510   CONTINUE
!
!
!-- Copy volumes of the current (Z+) end cap octant to get the (Z-) ones
      IVMFIN_Z = NVMATI
      CALL SYZMAT(IVMDEB_F,IVMFIN_Z)
!
 END SUBROUTINE AmdcEndcapToroid
!
 SUBROUTINE AmdcFeet
 USE M_MB_General_CONSTANTS, Epsil=>Eps   ! EPS deja utilise
 USE M_MB_COMATI
 IMPLICIT NONE
 REAL(8) :: VV(400)
 INTEGER :: JJJA(100),JJJU(100),JJJO(100),JJJB(100)
 REAL(8) :: XAFEB(100),YAFEB(100)
 REAL(8) :: XUFEB(100),YUFEB(100)
 REAL(8) :: XOFEB(100),YOFEB(100)
 REAL(8) :: XBFEB(100),YBFEB(100)
 REAL(8) :: XSNAK(20),YSNAK(20)
 INTEGER :: IBUF(100)
 INTEGER :: I
 INTEGER :: IVMDEB_Z, IVMFIN_Z
 INTEGER :: JZZ, JFF67
 INTEGER :: J, LLL, NAFEB, NBFEB
 INTEGER :: NOFEB, NSNAK, NUFEB
 REAL(8) :: COPIS8, SIPIS8
 REAL(8) :: DBEFS2, DBXFS2, DBYFEX, DBYFS2
 REAL(8) :: DDZ, DL1FEX, DXXP
 REAL(8) :: DYYP, DZ1FEX, DZ2FEX, DZ3FEX
 REAL(8) :: F67, FPIS4, FPIS8 , OV, S
 REAL(8) :: TA3FEX, TANG1, TANG2
 REAL(8) :: UMU, VX, VY, XCUR, XP5, XP6, XP7
 REAL(8) :: XT1, XT2, XT5, XT6, XT7, XT8
 REAL(8) :: XXP5, XXP6, XXPB, XXPD, XXS1, XXS2, Y1, Y1P, Y2, Y2P, Y3, Y3P
 REAL(8) :: YP5, YP6, YP7, YSM, YT1, YT4, YT6, YT8
 REAL(8) :: YYP5, YYP6, YYP7, YYPB, YYPM, YYSM
 REAL(8) :: ZCUR, ZZFEX_CUR
 REAL(8) :: ZZFEX_PRE
 REAL(8), PARAMETER :: EPS=0.0100D0
!
#include "AmdcStand/comamu.inc"
!
    FPIS4  = Pi/4.D0
    FPIS8  = Pi/8.D0
    SIPIS8 = SIN(FPIS8)
    COPIS8 = COS(FPIS8)
!
    DO 650 JFF67=6,7
      IF( JFF67.EQ.6 ) THEN
        S = -1.D0
      ELSE
        S =  1.D0
      ENDIF
      F67  = (JFF67-1)*FPIS4 + FPIS8
!
!
      DO 640 JZZ=1,NFEE
        IF( NFEE.GT.7 .AND. (JZZ.EQ.1 .OR. JZZ.EQ.NFEE) ) THEN
          DZ1FEX = DZ1FEE + DZ7FEE
          DZ2FEX = DZ2FEE + DZ7FEE
          DZ3FEX = DZ3FEE + DZ7FEE
          DL1FEX = DL1FEE + DL7FEE
          DBYFEX = DBYFEE - DL7FEE
          TA3FEX = TA3FEE + DT7FEE
        ELSE
          DZ1FEX = DZ1FEE
          DZ2FEX = DZ2FEE
          DZ3FEX = DZ3FEE
          DL1FEX = DL1FEE
          DBYFEX = DBYFEE
          TA3FEX = TA3FEE
        ENDIF
!
!-- Define 6 stiffening iron plates of each foot as a new volume
        CALL NEWMAT( 'Foot6Pla', MATFEE, JFF67, JZZ )
        YSM = -S * (DSSFEE + EP1FEE/2.D0)
        CALL DVBARR( F67, 1,                              &
         TI1FEE,YSM,ZFEE(JZZ), Zero,EP1FEE/2.D0,DZ2FEX,   &
         TA1FEE,YSM,ZFEE(JZZ), Zero,EP1FEE/2.D0,DZ2FEX )
        YSM =  S * (DSSFEE + EP1FEE/2.D0)
        CALL DVBARR( F67, 1,                              &
         TI2FEE,YSM,ZFEE(JZZ), Zero,EP1FEE/2.D0,DZ2FEX,   &
         TA2FEE,YSM,ZFEE(JZZ), Zero,EP1FEE/2.D0,DZ2FEX )
        CALL DVBARR( F67, 1,                                &
         TA2FEE-EP1FEE,Zero,ZFEE(JZZ), Zero,DSSFEE,DZ2FEX,  &
         TA2FEE       ,Zero,ZFEE(JZZ), Zero,DSSFEE,DZ2FEX )
        XP5 = TI2FEE - COPIS8*DL1FEX
        YP5 = DSSFEE + SIPIS8*DL1FEX
        XP6 = XP5 - SIPIS8*DL2FEE
        YP6 = YP5 - COPIS8*DL2FEE
        UMU = (TI1FEE-XP6)*SIPIS8 + (-DSSFEE-YP6)*COPIS8
        XP7 =  TI1FEE - UMU*SIPIS8
        YP7 = -DSSFEE - UMU*COPIS8
        VV( 1: 6) = (/ TI1FEE, -S*DSSFEE,Zero,TA3FEX,-S*DSSFEE,Zero/)
        VV( 7:12) = (/ TA3FEX,  S*DSSFEE,Zero,TI2FEE, S*DSSFEE,Zero/)
        VV(13:18) = (/    XP5,  S*YP5   ,Zero,   XP6, S*YP6   ,Zero/)
        VV(19:21) = (/    XP7,  S*YP7   ,Zero /)
        ZCUR = ZFEE(JZZ) + DZ1FEX
        CALL DVGENE( F67, 7, VV, 3, ZCUR,ZCUR+EP2FEE )
        ZCUR = ZFEE(JZZ) - DZ1FEX
        CALL DVGENE( F67, 7, VV, 3, ZCUR,ZCUR-EP2FEE )
        XXP6 =  SIPIS8*XP6 + COPIS8*YP6
        YYP6 = -COPIS8*XP6 + SIPIS8*YP6
        YYP7 = -COPIS8*XP7 + SIPIS8*YP7
        YYPM = (YYP6+YYP7)/2.D0
        DYYP = (YYP6-YYP7)/2.D0
        CALL DVBARR( ZERO, 1,                                       &
         S* XXP6        ,YYPM,ZFEE(JZZ), Zero,DYYP,DZ1FEX+EP2FEE,   &
         S*(XXP6-EP1FEE),YYPM,ZFEE(JZZ), Zero,DYYP,DZ1FEX+EP2FEE )
        CALL ENDMAT
!
!-- Define each "bridge" between two feeet (supporting the
!-- voussoir heads) as a new volume
        IF( JZZ >= 2 .AND. ABS(XFEEBRI) > 1.D0 ) THEN
         CALL NEWMAT( 'FootBrid', MATFEE, JFF67, JZZ )
         ZZFEX_CUR = ZFEE(JZZ) - DZ1FEX - EP2FEE - EPS
         VV(1:3) = (/ Zero, YFEEBRI          , ZZFEX_PRE           /)
         VV(4:6) = (/ Zero, YFEEBRI+DY1FEEBRI, ZZFEX_PRE           /)
         VV(7:9) = (/ Zero, YFEEBRI+DY1FEEBRI, ZZFEX_PRE+DZ1FEEBRI /)
         XCUR = S*XFEEBRI
         CALL DVGENE( Zero, 3, VV, 1, XCUR,XCUR+S*DX1FEEBRI )
         XCUR = S*(XFEEBRI+DX2FEEBRI)
         CALL DVGENE( Zero, 3, VV, 1, XCUR,XCUR-S*DX1FEEBRI )
         VV(1:3) = (/ Zero, YFEEBRI          , ZZFEX_CUR           /)
         VV(4:6) = (/ Zero, YFEEBRI+DY1FEEBRI, ZZFEX_CUR           /)
         VV(7:9) = (/ Zero, YFEEBRI+DY1FEEBRI, ZZFEX_CUR-DZ1FEEBRI /)
         XCUR = S*XFEEBRI
         CALL DVGENE( Zero, 3, VV, 1, XCUR,XCUR+S*DX1FEEBRI )
         XCUR = S*(XFEEBRI+DX2FEEBRI)
         CALL DVGENE( Zero, 3, VV, 1, XCUR,XCUR-S*DX1FEEBRI )
         TANG1 = (DY3FEEBRI-DY2FEEBRI) / DZ2FEEBRI
         TANG2 = ( SQRT(1.D0+TANG1**2) - 1.D0 ) / TANG1
         Y1    = YFEEBRI + DY1FEEBRI
         Y2    = Y1 - (DY3FEEBRI-DY2FEEBRI)
         Y1P   = Y1 + DY4FEEBRI
         Y2P   = Y2 + DY4FEEBRI
         DDZ   = TANG2 * DY4FEEBRI
         VV( 1: 3) = (/Zero, Y1P, ZZFEX_CUR                        /)
         VV( 4: 6) = (/Zero, Y1P, ZZFEX_CUR-DZ1FEEBRI          -DDZ/)
         VV( 7: 9) = (/Zero, Y2P, ZZFEX_CUR-DZ1FEEBRI-DZ2FEEBRI-DDZ/)
         VV(10:12) = (/Zero, Y2P, ZZFEX_PRE+DZ1FEEBRI+DZ2FEEBRI+DDZ/)
         VV(13:15) = (/Zero, Y1P, ZZFEX_PRE+DZ1FEEBRI          +DDZ/)
         VV(16:18) = (/Zero, Y1P, ZZFEX_PRE                        /)
         VV(19:21) = (/Zero, Y1 , ZZFEX_PRE                        /)
         VV(22:24) = (/Zero, Y1 , ZZFEX_PRE+DZ1FEEBRI              /)
         VV(25:27) = (/Zero, Y2 , ZZFEX_PRE+DZ1FEEBRI+DZ2FEEBRI    /)
         VV(28:30) = (/Zero, Y2 , ZZFEX_CUR-DZ1FEEBRI-DZ2FEEBRI    /)
         VV(31:33) = (/Zero, Y1 , ZZFEX_CUR-DZ1FEEBRI              /)
         VV(34:36) = (/Zero, Y1 , ZZFEX_CUR                        /)
         XCUR = S*( XFEEBRI - (DX3FEEBRI-DX2FEEBRI)/2.D0 )
         CALL DVGENE( ZERO, 12, VV, 1, XCUR,XCUR+S*DX3FEEBRI )
         Y3    = YFEEBRI + DY1FEEBRI + DY2FEEBRI
         Y3P   = Y3 - DY4FEEBRI
         VV(19:21) = (/Zero, Y3P, ZZFEX_PRE                        /)
         VV(22:24) = (/Zero, Y3P, ZZFEX_CUR                        /)
         XCUR = S*( XFEEBRI + DX2FEEBRI/2.D0 - DX4FEEBRI/2.D0 )
         CALL DVGENE( ZERO, 8, VV, 1, XCUR,XCUR+S*DX4FEEBRI )
         VV(25:27) = (/Zero, Y3 , ZZFEX_CUR                        /)
         VV(28:30) = (/Zero, Y3 , ZZFEX_PRE                        /)
         XCUR = S*( XFEEBRI - (DX3FEEBRI-DX2FEEBRI)/2.D0 )
         CALL DVGENE( Zero, 4, VV(19), 1, XCUR,XCUR+S*DX3FEEBRI )
         CALL ENDMAT
        ENDIF
        ZZFEX_PRE = ZFEE(JZZ) + DZ1FEX + EP2FEE + EPS
!
!-- Define each foot "socles" (3 iron plates) as a new volume
        CALL NEWMAT( 'Foot3Pla', MATFEE, JFF67, JZZ )
        XXS1 =  SIPIS8*TA1FEE + COPIS8*(-DSSFEE-EP1FEE) - DX1FEE
        XXS2 =  SIPIS8*TA1FEE + COPIS8*(-DSSFEE-EP1FEE) + DX2FEE
        YYSM = -COPIS8*TA1FEE + SIPIS8*(-DSSFEE-EP1FEE) + EP1FEE/2.D0
        CALL DVBARR( Zero, 1,                              &
         S*XXS1,YYSM,ZFEE(JZZ), Zero,EP1FEE/2.D0,DZ3FEX,   &
         S*XXS2,YYSM,ZFEE(JZZ), Zero,EP1FEE/2.D0,DZ3FEX )
        XT1 =       TA1FEE    + SIPIS8*DX2FEE - COPIS8*EP1FEE
        YT1 =  -DSSFEE-EP1FEE + COPIS8*DX2FEE + SIPIS8*EP1FEE
        XT8 =       TA1FEE    - SIPIS8*DX1FEE - COPIS8*EP1FEE
        YT8 =  -DSSFEE-EP1FEE - COPIS8*DX1FEE + SIPIS8*EP1FEE
        XT2 = XT1 - (DSSFEE+EP1FEE-YT1)*COPIS8/SIPIS8
        YT4 = -DSSFEE          + DD1FEE * (DD2FEE+EP1FEE)/DD2FEE
        XT5 =  TA2FEE - EP1FEE - DD2FEE * (DD1FEE+EP1FEE)/DD1FEE
        XT6 =   TM1FEE + DSPFEE*COPIS8/SIPIS8
        YT6 = - DSSFEE - EP1FEE - DSPFEE
        XT7 = XT8 - (YT6-YT8)*COPIS8/SIPIS8
        VV( 1: 3) = (/    XT1, S*YT1            , Zero /)
        VV( 4: 6) = (/    XT2, S*(DSSFEE+EP1FEE), Zero /)
        VV( 7: 9) = (/ TA2FEE, S*(DSSFEE+EP1FEE), Zero /)
        VV(10:12) = (/ TA2FEE, S*YT4            , Zero /)
        VV(13:15) = (/    XT5,-S*(DSSFEE+EP1FEE), Zero /)
        VV(16:18) = (/ TM1FEE,-S*(DSSFEE+EP1FEE), Zero /)
        VV(19:21) = (/    XT6, S*YT6            , Zero /)
        VV(22:24) = (/    XT7, S*YT6            , Zero /)
        VV(25:27) = (/    XT8, S*YT8            , Zero /)
        ZCUR = ZFEE(JZZ) + DZ1FEX
        CALL DVGENE( F67, 9, VV, 3, ZCUR,ZCUR+EP1FEE )
        ZCUR = ZFEE(JZZ) - DZ1FEX
        CALL DVGENE( F67, 9, VV, 3, ZCUR,ZCUR-EP1FEE )
        CALL ENDMAT
!
!-- Define each small "box" on top of the feet as a new volume
        CALL NEWMAT( 'FootBox_', MATFEE, JFF67, JZZ )
        DBXFS2 = DBXFEE / 2.D0
        DBYFS2 = DBYFEX / 2.D0
        DBEFS2 = DBEFEE / 2.D0
        XXP5 =  SIPIS8*XP5 + COPIS8*YP5
        YYP5 = -COPIS8*XP5 + SIPIS8*YP5
        XXPB = XXP5 - DBXFS2
        YYPB = YYP5 + DBYFS2
        CALL DVTUBE( Zero, 3,                                           &
         S* XXPB        ,YYPB,ZFEE(JZZ),DBXFS2,DBYFS2       ,-DZ2FEX,   &
         S*(XXPB+DBEFEE),YYPB,ZFEE(JZZ),DBXFS2,DBYFS2-DBEFEE,-DZ2FEX,   &
         S* XXPB        ,YYPB,ZFEE(JZZ),DBXFS2,DBYFS2       , DZ2FEX,   &
         S*(XXPB+DBEFEE),YYPB,ZFEE(JZZ),DBXFS2,DBYFS2-DBEFEE, DZ2FEX )
        XXPD = XXP5 - DBXFEE + DBEFEE
        CALL DVBARR( Zero, 1,                                           &
         S*XXPD,YYPB,ZFEE(JZZ)-DZ2FEX/2.D0,Zero,DBYFS2-DBEFEE,DBEFS2,   &
         S*XXP5,YYPB,ZFEE(JZZ)-DZ2FEX/2.D0,Zero,DBYFS2-DBEFEE,DBEFS2 )
        CALL DVBARR( Zero, 1,                                           &
         S*XXPD,YYPB,ZFEE(JZZ)+DZ2FEX/2.D0,Zero,DBYFS2-DBEFEE,DBEFS2,   &
         S*XXP5,YYPB,ZFEE(JZZ)+DZ2FEX/2.D0,Zero,DBYFS2-DBEFEE,DBEFS2 )
        CALL ENDMAT
!
!-- Define the barr connecting two feet as a new volume
        IF( JFF67 == 7 .AND. MATBAR >= 1 ) THEN
         CALL NEWMAT( 'FootBarO', MATBAR, JFF67, JZZ )
         DXXP = XXP6 - EP1FEE
         CALL DVTUBE( Zero, 3,                                     &
          ZERO,YYPM,ZFEE(JZZ), DXXP       ,DYYP       ,-DZ1FEB,    &
          ZERO,YYPM,ZFEE(JZZ), DXXP-EP1FEB,DYYP-EP1FEB,-DZ1FEB,    &
          ZERO,YYPM,ZFEE(JZZ), DXXP       ,DYYP       , DZ1FEB,    &
          ZERO,YYPM,ZFEE(JZZ), DXXP-EP1FEB,DYYP-EP1FEB, DZ1FEB )
         NAFEB = 20
         NUFEB =  8
         JJJA(1:NAFEB) = 1
         JJJU(1:NUFEB) = 1
         XAFEB( 1) = -DX1FEB/2.D0
         XAFEB( 2) = XAFEB( 1) - DX2FEB
         XAFEB( 3) = XAFEB( 2) - DX3FEB
         XAFEB( 4) = XAFEB( 3) - DX4FEB
         XAFEB( 5) = -DXXP + EP1FEB
         YAFEB( 1) = YYPM + DYYP - EP1FEB
         YAFEB( 2) = YYPM + DYYP - EP1FEB - DY1FEB
         YAFEB( 3) = YYPM + DYYP - EP1FEB
         YAFEB( 4) = YYPM + DYYP - EP1FEB
         YAFEB( 5) = YYPM + DYYP - EP1FEB - DY2FEB
         DO I=6,10
           J = 11 - I
           XAFEB(I) = XAFEB(J)
           YAFEB(I) = 2.D0*YYPM - YAFEB(J)
         ENDDO
         DO I=11,20
           J = 21 - I
           XAFEB(I) = -XAFEB(J)
           YAFEB(I) =  YAFEB(J)
         ENDDO
         VV( 1: 3) = (/ XAFEB( 1), YAFEB( 1), Zero /)
         VV( 4: 6) = (/ XAFEB( 2), YAFEB( 2), Zero /)
         VV( 7: 9) = (/ XAFEB( 9), YAFEB( 9), Zero /)
         VV(10:12) = (/ XAFEB(10), YAFEB(10), Zero /)
         VV(13:15) = (/ XAFEB( 5), YAFEB(10), Zero /)
         VV(16:18) = (/ XAFEB( 5), YAFEB( 1), Zero /)
         ZCUR = ZFEE(JZZ) - EP2FEB/2.D0
         CALL DVGENE( Zero, 6, VV, 3, ZCUR,ZCUR+EP2FEB )
         VV(1:16:3) = -VV(1:16:3)
         CALL DVGENE( Zero, 6, VV, 3, ZCUR,ZCUR+EP2FEB )
         CALL ENDMAT
!-- Define the reinforcement of this barr as a new volume
         CALL NEWMAT( 'FootBarI', MATBAR, JFF67, JZZ )
         DO I=1,NUFEB
           J = I + 1
           VX = XAFEB(3) - DX4FEB/2.D0 - XAFEB(J)
           VY =         YYPM           - YAFEB(J)
           OV = SQRT( VX**2 + VY**2 )
           XUFEB(I) = XAFEB(J) + VX * EP2FEB / OV
           YUFEB(I) = YAFEB(J) + VY * EP2FEB / OV
         ENDDO
         CALL CUTOUT( NAFEB, JJJA,XAFEB,YAFEB,    &
                      NUFEB, JJJU,XUFEB,YUFEB,    &
          100, 1, LLL, IBUF, JJJO,XOFEB,YOFEB )
         NOFEB = IBUF(1)
         XUFEB(1:NUFEB) = -XUFEB(1:NUFEB)
         CALL CUTOUT( NOFEB, JJJO,XOFEB,YOFEB,    &
                      NUFEB, JJJU,XUFEB,YUFEB,    &
          100, 1, LLL, IBUF, JJJB,XBFEB,YBFEB )
         NBFEB = IBUF(1)
         DO I=1,NUFEB
           J = I
           IF( I >= 3 ) J = I + 6
           IF( I >= 7 ) J = I + 12
           VX =      - XAFEB(J)
           VY = YYPM - YAFEB(J)
           OV = SQRT( VX**2 + VY**2 )
           XUFEB(I) = XAFEB(J) + VX * EP2FEB / OV
           YUFEB(I) = YAFEB(J) + VY * EP2FEB / OV
         ENDDO
         CALL CUTOUT( NBFEB, JJJB,XBFEB,YBFEB,    &
                      NUFEB, JJJU,XUFEB,YUFEB,    &
          100, 1, LLL, IBUF, JJJO,XOFEB,YOFEB )
         NOFEB = IBUF(1)
         VV(1:3*NOFEB) = (/ (XOFEB(I),YOFEB(I),Zero, I=1,NOFEB) /)
         CALL DVGENE(Zero,NOFEB,VV,3,ZFEE(JZZ)-DZ2FEB,ZFEE(JZZ)+DZ2FEB)
         CALL ENDMAT
        ENDIF
!
640   ENDDO
!
!-- Rails
      IF( ZRAIL > 300.D0 ) THEN
        IF( CHAMFR > EPS ) THEN
           XSNAK((/ 1, 2      /)) = XXP5 - XRAIL1
           XSNAK((/ 3, 4, 9,10/)) = XSNAK(1) + (XRAIL1-XRAIL2)/2.D0
           XSNAK((/ 5, 8      /)) = XSNAK(4) + (XRAIL2-XRAIL3-2.D0*CHAMFR)/2.D0
           XSNAK((/ 6, 7      /)) = XSNAK(5) + CHAMFR
           XSNAK((/11,12,17,18/)) = XSNAK(3) + XRAIL2
           XSNAK((/13,16      /)) = XSNAK(5) + XRAIL3 + 2.D0*CHAMFR
           XSNAK((/14,15      /)) = XSNAK(6) + XRAIL3
           XSNAK((/19,20      /)) = XSNAK(1) + XRAIL1
           YSNAK((/ 1,20      /)) = YYP5 + DBYFEX
           YSNAK((/ 2, 3,18,19/)) = YSNAK(1) + YRAIL1
           YSNAK((/ 4, 5,16,17/)) = YSNAK(2) + YRAIL2
           YSNAK((/ 6,15      /)) = YSNAK(5) + CHAMFR
           YSNAK((/ 7,14      /)) = YSNAK(2) + YRAIL3 - CHAMFR - YRAIL2
           YSNAK((/ 8, 9,12,13/)) = YSNAK(7) + CHAMFR
           YSNAK((/10,11      /)) = YSNAK(2) + YRAIL3
           NSNAK = 20
        ELSE
           XSNAK((/ 1, 2      /)) = XXP5 - XRAIL1
           XSNAK((/ 3, 4      /)) = XSNAK(1) + (XRAIL1-XRAIL2)/2.D0
           XSNAK((/ 5, 6      /)) = XSNAK(3) - (XRAIL3-XRAIL2)/2.D0
           XSNAK((/ 7, 8      /)) = XSNAK(5) + XRAIL3
           XSNAK((/ 9,10      /)) = XSNAK(3) + XRAIL2
           XSNAK((/11,12      /)) = XSNAK(1) + XRAIL1
           YSNAK((/ 1,12      /)) = YYP5 + DBYFEX
           YSNAK((/ 2, 3,10,11/)) = YSNAK(1) + YRAIL1
           YSNAK((/ 4, 5, 8, 9/)) = YSNAK(2) + YRAIL2
           YSNAK((/ 6, 7      /)) = YSNAK(5) + YRAIL3
           NSNAK = 12
        ENDIF
        VV(1:3*NSNAK) = (/ (S*XSNAK(I),YSNAK(I),Zero, I=1,NSNAK) /)
        IVMDEB_Z = NVMATI + 1
        CALL NEWMAT( 'FootRail', MATRAI, JFF67, 1 )
        CALL DVGENE( Zero, NSNAK, VV, 3, Zero,ZRAIL )
        CALL ENDMAT
        IVMFIN_Z = NVMATI
        CALL SYZMAT(IVMDEB_Z,IVMFIN_Z)
      ENDIF
!
650 ENDDO
!
 END SUBROUTINE AmdcFeet
!
 SUBROUTINE AmdcTile
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: IVMDEB_F, IVMDEB_Z, IVMFIN_F, IVMFIN_Z
 INTEGER :: L
 REAL(8) :: UU(6), VV(6)
 REAL(8) :: ANGFIN, R0BAR, DRBAR, F0BAR, FANGFIN
 REAL(8) :: COPIS64, FPIS32, FPIS64
 REAL(8) :: THERA1, THERA2, THERA3, THERI1, THERI2, THERI3, THEZLO
!
#include "AmdcStand/comamu.inc"
!
      FPIS32 = Pi/32.D0
      FPIS64 = Pi/64.D0
      COPIS64 = COS(FPIS64)
      ANGFIN  = 45.D0
      FANGFIN = PiS180*ANGFIN
!
      IVMDEB_F = NVMATI + 1
!
!-- Iron Girder
      IF( MATGIRD.GE.1 ) THEN
        IF( ABS(DZHGIRD).LT.0.1D0 ) THEN
!-- Old Physics TDR Version
          CALL NEWMAT( 'IronGird', MATGIRD, 1, 28 )
          CALL DVPIPE( Zero, 8, Zero,ANGFIN,            &
             Zero, Zero, -ZMAGIRD, RAOGIRD, RIOGIRD,    &
             Zero, Zero,  ZMAGIRD, RAOGIRD, RIOGIRD )
          CALL DVPIPE( Zero, 8, Zero,ANGFIN,            &
             Zero, Zero, -ZMAGIRD, RAIGIRD, RIIGIRD,    &
             Zero, Zero,  ZMAGIRD, RAIGIRD, RIIGIRD )
          CALL ENDMAT
          CALL NEWMAT( 'IronGird', MATGIRD, 1, 28 )
          R0BAR = (RIOGIRD+RAIGIRD)/2.D0 * COPIS64
          DRBAR = (RIOGIRD-RAIGIRD)/2.D0 * COPIS64 - 0.1D0
          DO L=0,7
            F0BAR = FPIS64 + L*FPIS32
            CALL DVBARR( F0BAR, 3,                                     &
              R0BAR,-DS2GIRD/2.D0,-ZMAGIRD, DRBAR,DS1GIRD/2.D0,Zero,   &
              R0BAR,-DS2GIRD/2.D0, ZMAGIRD, DRBAR,DS1GIRD/2.D0,Zero )
            CALL DVBARR( F0BAR, 3,                                     &
              R0BAR, DS2GIRD/2.D0,-ZMAGIRD, DRBAR,DS1GIRD/2.D0,Zero,   &
              R0BAR, DS2GIRD/2.D0, ZMAGIRD, DRBAR,DS1GIRD/2.D0,Zero )
          ENDDO
          CALL ENDMAT
        ELSE
!-- New more detailed Version  (Perform Z symetry)
          IVMDEB_Z = NVMATI + 1
          CALL NEWMAT( 'IronGird', MATGIRD, 1, 27 )
          CALL DVPIPE( Zero, 8, Zero,ANGFIN,                        &
                       Zero, Zero,  ZMIGIRD, RAOGIRD, RIOGIRD,      &
                       Zero, Zero,  ZMAGIRD, RAOGIRD, RIOGIRD )
          CALL DVPIPE( Zero, 8, Zero,ANGFIN,                        &
                       Zero, Zero,  ZMIGIRD, RAIGIRD, RIIGIRD,      &
                       Zero, Zero,  ZMAGIRD, RAIGIRD, RIIGIRD )
          THEZLO = ZMIGIRD - DZHGIRD
          CALL DVPIPE( Zero, 8, Zero,ANGFIN,                        &
                       Zero, Zero,  Zero  , RAOGIRD, RIOGIRD,       &
                       Zero, Zero,  THEZLO, RAOGIRD, RIOGIRD )
          CALL DVPIPE( Zero, 8, Zero,ANGFIN,                        &
                       Zero, Zero,  Zero  , RAIGIRD, RIIGIRD,       &
                       Zero, Zero,  THEZLO, RAIGIRD, RIIGIRD )
          R0BAR = (RIOGIRD+RAIGIRD)/2.D0 * COPIS64
          DRBAR = (RIOGIRD-RAIGIRD)/2.D0 * COPIS64 - 0.1D0
          DO L=0,7
            F0BAR = FPIS64 + L*FPIS32
            CALL DVBARR( F0BAR, 3,                                                &
                         R0BAR,-DS2GIRD/2.D0, Zero   , DRBAR,DS1GIRD/2.D0,Zero,   &
                         R0BAR,-DS2GIRD/2.D0, ZMAGIRD, DRBAR,DS1GIRD/2.D0,Zero )
            CALL DVBARR( F0BAR, 3,                                                &
                         R0BAR, DS2GIRD/2.D0, Zero   , DRBAR,DS1GIRD/2.D0,Zero,   &
                         R0BAR, DS2GIRD/2.D0, ZMAGIRD, DRBAR,DS1GIRD/2.D0,Zero )
          ENDDO
          CALL ENDMAT
          IVMFIN_Z = NVMATI
          CALL SYZMAT(IVMDEB_Z,IVMFIN_Z)
        ENDIF
      ENDIF
!
!
!-- Bypass the HCAL for the magnet system visualization
      IF( IFLAG_MATIERE_MAGNETSYSTEM(4) == 1 ) GO TO 665
!
!-- Tile Calorimeter
      IF( MATTIL >= 1 ) THEN
         IF( ABS(DRBTIL1) < 0.1D0 ) THEN
!-- Old Physics TDR Version
            CALL NEWMAT( 'BarTileC', MATTIL, 1, 29 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                      &
                         Zero, Zero, -ZABATIL, RABATIL, RIBATIL,    &
                         Zero, Zero,  ZABATIL, RABATIL, RIBATIL )
            CALL ENDMAT
            CALL NEWMAT( 'ExTileC+', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                      &
                         Zero, Zero,  ZIEXTIL, RAEXTIL, RIEXTIL,    &
                         Zero, Zero,  ZAEXTIL, RAEXTIL, RIEXTIL )
            CALL ENDMAT
            CALL NEWMAT( 'ExTileC-', MATTIL, 1, 31 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                      &
                         Zero, Zero, -ZAEXTIL, RAEXTIL, RIEXTIL,    &
                         Zero, Zero, -ZIEXTIL, RAEXTIL, RIEXTIL )
            CALL ENDMAT
            CALL NEWMAT( 'InTileC+', MATTIL, 1, 30 )
            UU( 1) = RAEXTIL
            UU( 2) = RAEXTIL - DRITIL1
            UU( 3) = RAEXTIL - DRITIL1
            UU( 4) = RAEXTIL - DRITIL1 - DRITIL2
            UU( 5) = RAEXTIL - DRITIL1 - DRITIL2
            UU( 6) = RAEXTIL
            VV( 1) = ZIEXTIL - 0.1D0 - DZITIL1
            VV( 2) = ZIEXTIL - 0.1D0 - DZITIL1
            VV( 3) = ZIEXTIL - 0.1D0 - DZITIL2
            VV( 4) = ZIEXTIL - 0.1D0 - DZITIL2
            VV( 5) = ZIEXTIL - 0.1D0
            VV( 6) = ZIEXTIL - 0.1D0
            CALL DVRRZZ( Zero,FANGFIN,8, 6, UU,VV )
            CALL ENDMAT
            CALL NEWMAT( 'InTileC-', MATTIL, 1, 31 )
            VV(1:6) = -VV(1:6)
            CALL DVRRZZ( Zero,FANGFIN,8, 6, UU,VV )
            CALL ENDMAT
         ELSE
!-- New more detailed Version - Barrel Tile
            THERA1 = RIBATIL + DRBTIL1
            THERI1 = RIBATIL
            THERA2 = RIBATIL + DRBTIL1 + DRBTIL2
            THERI2 = RIBATIL + DRBTIL1
            THERA3 = RABATIL
            THERI3 = RIBATIL + DRBTIL1 + DRBTIL2
            CALL NEWMAT( 'BarTilC1', MATTIL, 1, 29 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                    &
                         Zero, Zero, -ZABATIL, THERA1, THERI1,    &
                         Zero, Zero,  ZABATIL, THERA1, THERI1 )
            CALL ENDMAT
            CALL NEWMAT( 'BarTilC2', MATTIL, 1, 29 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                    &
                         Zero, Zero, -ZABATIL, THERA2, THERI2,    &
                         Zero, Zero,  ZABATIL, THERA2, THERI2 )
            CALL ENDMAT
            CALL NEWMAT( 'BarTilC3', MATTIL, 1, 29 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                    &
                         Zero, Zero, -ZABATIL, THERA3, THERI3,    &
                         Zero, Zero,  ZABATIL, THERA3, THERI3 )
            CALL ENDMAT
!-- New more detailed Version - Ext. + Int. Tile  (Perform Z symetry)
            IVMDEB_Z = NVMATI + 1
            THERA1 = RIEXTIL + DRETIL1
            THERI1 = RIEXTIL
            THERA2 = RIEXTIL + DRETIL1 + DRETIL2
            THERI2 = RIEXTIL + DRETIL1
            THERA3 = RAEXTIL
            THERI3 = RIEXTIL + DRETIL1 + DRETIL2
            CALL NEWMAT( 'ExtTilC1', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                    &
                         Zero, Zero,  ZIEXTIL, THERA1, THERI1,    &
                         Zero, Zero,  ZAEXTIL, THERA1, THERI1 )
            CALL ENDMAT
            CALL NEWMAT( 'ExtTilC2', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                    &
                         Zero, Zero,  ZIEXTIL, THERA2, THERI2,    &
                         Zero, Zero,  ZAEXTIL, THERA2, THERI2 )
            CALL ENDMAT
            CALL NEWMAT( 'ExtTilC3', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                    &
                         Zero, Zero,  ZIEXTIL, THERA3, THERI3,    &
                         Zero, Zero,  ZAEXTIL, THERA3, THERI3 )
            CALL ENDMAT
            THERA1 = RAEXTIL
            THERI1 = RAEXTIL - DRITIL1
            THERA2 = RAEXTIL - DRITIL1
            THERI2 = RAEXTIL - DRITIL1 - DRITIL2
            CALL NEWMAT( 'IntTilP1', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                         &
                         Zero, Zero, ZIEXTIL-DZITIL1, THERA1, THERI1,  &
                         Zero, Zero, ZIEXTIL        ,THERA1, THERI1 )
            CALL ENDMAT
            CALL NEWMAT( 'IntTilP2', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                         &
                         Zero, Zero, ZIEXTIL-DZITIL2, THERA2, THERI2,  &
                         Zero, Zero, ZIEXTIL        , THERA2, THERI2 )
            CALL ENDMAT
            THERA1 = RAEXTIL - DRITIL1-DRITIL2
            THERI1 = RAEXTIL - DRITIL1-DRITIL2-DRITIL3
            THERA2 = RAEXTIL - DRITIL1-DRITIL2-DRITIL3
            THERI2 = RAEXTIL - DRITIL1-DRITIL2-DRITIL3-DRITIL4
            THERA3 = RAEXTIL - DRITIL1-DRITIL2-DRITIL3-DRITIL4
            THERI3 = RAEXTIL - DRITIL1-DRITIL2-DRITIL3-DRITIL4-DRITIL5
            CALL NEWMAT( 'IntTilS1', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                         &
                         Zero, Zero, ZIEXTIL-DZITIL3, THERA1, THERI1,  &
                         Zero, Zero, ZIEXTIL        , THERA1, THERI1 )
            CALL ENDMAT
            CALL NEWMAT( 'IntTilS2', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                         &
                         Zero, Zero, ZIEXTIL-DZITIL3, THERA2, THERI2,  &
                         Zero, Zero, ZIEXTIL        , THERA2, THERI2 )
            CALL ENDMAT
            CALL NEWMAT( 'IntTilS3', MATTIL, 1, 30 )
            CALL DVPIPE( Zero, 8, Zero,ANGFIN,                         &
                         Zero, Zero, ZIEXTIL-DZITIL3, THERA3, THERI3,  &
                         Zero, Zero, ZIEXTIL        , THERA3, THERI3 )
            CALL ENDMAT
          IVMFIN_Z = NVMATI
          CALL SYZMAT(IVMDEB_Z,IVMFIN_Z)
         ENDIF
      ENDIF
!
665   CONTINUE
!
!
!-- Copy volumes around Phi
      IVMFIN_F = NVMATI
      CALL SYFMAT(IVMDEB_F,IVMFIN_F)
!
 END SUBROUTINE AmdcTile
!
!
 SUBROUTINE AmdcLarg
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: IVMDEB_F, IVMDEB_Z, IVMFIN_F, IVMFIN_Z
 INTEGER :: I, II, LA, LF, NAF1, NAF2, NAF2P
 REAL(8) :: UU(100), VV(400), UUS(10), VVS(10)
 REAL(8) :: ANFEEC, ANFEED, ANG, ANG0, ANG1, ANG2, ANG3, ANGFIN, ANGP
 REAL(8) :: ATEMP, CANG, CC
 REAL(8) :: FPIS2, FPIS4, FPIS8, SIPIS4, SIPIS8, COPIS8
 REAL(8) :: ECELETAS1, ECELETAS2, ECELETAS3, F0FEEC, F0FEED, FANGFIN
 REAL(8) :: ANGS, R, SANG, SEP1, SEP2, SEP3, SEPC, SS, X, Z
!
#include "AmdcStand/comamu.inc"
!
      FPIS2  = Pi/2.D0
      FPIS4  = Pi/4.D0
      FPIS8  = Pi/8.D0
      SIPIS4 = SIN(FPIS4)
      SIPIS8 = SIN(FPIS8)
      COPIS8 = COS(FPIS8)
      ANGFIN  = 45.D0
      FANGFIN = PiS180*ANGFIN
!
      IVMDEB_F = NVMATI + 1
!
!-- Bypass the LAr calo for the magnet system visualization
      IF( IFLAG_MATIERE_MAGNETSYSTEM(3) == 1 ) GO TO 690
!
!-- Barrel e.m. liquid argon warm cryostat
      IF( MATCALCR.GE.1 ) THEN
        CALL NEWMAT( 'ECalCryo', MATCALCR, 1, 33 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZAECA2, RAECA1, RAECA1-EPECA1,                    &
                     Zero,Zero,  ZAECA2, RAECA1, RAECA1-EPECA1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZAECA2-EPECA1, RAECA2-EPECA1, RAECA1-EPECA1,      &
                     Zero,Zero, -ZAECA2       , RAECA2-EPECA1, RAECA1-EPECA1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero,  ZAECA2       , RAECA2-EPECA1, RAECA1-EPECA1,      &
                     Zero,Zero,  ZAECA2+EPECA1, RAECA2-EPECA1, RAECA1-EPECA1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZAECA1, RAECA2, RAECA2-EPECA1,                    &
                     Zero,Zero, -ZAECA2, RAECA2, RAECA2-EPECA1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero,  ZAECA2, RAECA2, RAECA2-EPECA1,                    &
                     Zero,Zero,  ZAECA1, RAECA2, RAECA2-EPECA1 )
        SEP1 = EPECA2/SIPIS4 - EPECA1
        SEP2 = EPECA2/SIPIS4 - EPECA2
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZAECA1      , RAECA2-EPECA1,RIECA1+COECA1,        &
                     Zero,Zero, -ZAECA1+EPECA1,RAECA2-EPECA1,RIECA1+COECA1+SEP1)
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero,  ZAECA1-EPECA1,RAECA2-EPECA1,RIECA1+COECA1+SEP1,   &
                     Zero,Zero,  ZAECA1      ,RAECA2-EPECA1,RIECA1+COECA1     )
        CALL DVPICO( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZAECA1+COECA1, RIECA1+EPECA2, RIECA1,  SEP2,      &
                     Zero,Zero,  ZAECA1-COECA1, RIECA1+EPECA2, RIECA1, -SEP2 )
        CALL DVPICO( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZAECA1, RIECA1+COECA1+SEP1,RIECA1+COECA1,EPECA1,  &
                     Zero,Zero, -ZAECA1+COECA1,RIECA1+EPECA2,RIECA1      , SEP2 )
        CALL DVPICO( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, ZAECA1-COECA1, RIECA1+EPECA2,RIECA1     , -SEP2,   &
                     Zero,Zero, ZAECA1, RIECA1+COECA1+SEP1,RIECA1+COECA1,-EPECA1)
         CALL ENDMAT
      ENDIF
!-- Barrel e.m. liquid argon cold cryostat
      IF( MATCOC.GE.1 ) THEN
        CALL NEWMAT( 'ECalCoCr', MATCOC, 1, 33 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZMCOCR       , RMCOCR, RMCOCR-EPCOCR,             &
                     Zero,Zero,  ZMCOCR       , RMCOCR, RMCOCR-EPCOCR )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZMCOCR-EPCOCR, RACOCR, RMCOCR-EPCOCR,             &
                     Zero,Zero, -ZMCOCR       , RACOCR, RMCOCR-EPCOCR )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero,  ZMCOCR       , RACOCR, RMCOCR-EPCOCR,             &
                     Zero,Zero,  ZMCOCR+EPCOCR, RACOCR, RMCOCR-EPCOCR )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZACOCR+EPCOCR, RACOCR, RACOCR-EPCOCR,             &
                     Zero,Zero, -ZMCOCR-EPCOCR, RACOCR, RACOCR-EPCOCR )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero,  ZMCOCR+EPCOCR, RACOCR, RACOCR-EPCOCR,             &
                     Zero,Zero,  ZACOCR-EPCOCR, RACOCR, RACOCR-EPCOCR )
        SEPC = EPCOCR*SIPIS8/COPIS8
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZACOCR       , RACOCR, RICOCR+COCOCR,             &
                     Zero,Zero, -ZACOCR+EPCOCR, RACOCR, RICOCR+COCOCR+SEPC )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero,  ZACOCR-EPCOCR, RACOCR, RICOCR+COCOCR+SEPC,        &
                     Zero,Zero,  ZACOCR       , RACOCR, RICOCR+COCOCR     )
        CALL DVPICO( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZACOCR+COCOCR, RICOCR+EPCOCR, RICOCR,  SEPC,      &
                     Zero,Zero,  ZACOCR-COCOCR, RICOCR+EPCOCR, RICOCR, -SEPC )
        CALL DVPICO( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, -ZACOCR, RICOCR+COCOCR+SEPC,RICOCR+COCOCR,EPCOCR,  &
                     Zero,Zero, -ZACOCR+COCOCR, RICOCR+EPCOCR,RICOCR      ,SEPC)
        CALL DVPICO( Zero, 4, Zero,ANGFIN,                                         &
                     Zero,Zero, ZACOCR-COCOCR, RICOCR+EPCOCR,RICOCR      ,-SEPC,   &
                     Zero,Zero, ZACOCR,RICOCR+COCOCR+SEPC, RICOCR+COCOCR,-EPCOCR)
         CALL ENDMAT
      ENDIF
      ANFEED = TWOPI / NNFEED
!-- Barrel e.m. liquid argon feed-throughs
      IF( MATFEED.GE.1 ) THEN
         CALL NEWMAT( 'ECaFeed-', MATFEED, 1, 33 )
         DO I=1,NNFEED/8
          F0FEED = (I-0.5D0)*ANFEED
          CALL DVPIPE( F0FEED, 6, Zero,Z360,                                       &
                       RAECA2-HHFEED, Zero, -(ZAECA1+ZAECA2)/2.D0, RRFEED, Zero,   &
                       RAECA2+HHFEED, Zero, -(ZAECA1+ZAECA2)/2.D0, RRFEED, Zero )
         ENDDO
         CALL ENDMAT
         CALL NEWMAT( 'ECaFeed+', MATFEED, 1, 33 )
         DO I=1,NNFEED/8
          F0FEED = (I-0.5D0)*ANFEED
          CALL DVPIPE( F0FEED, 6, Zero,Z360,                                       &
                       RAECA2-HHFEED, Zero,  (ZAECA1+ZAECA2)/2.D0, RRFEED, Zero,   &
                       RAECA2+HHFEED, Zero,  (ZAECA1+ZAECA2)/2.D0, RRFEED, Zero )
         ENDDO
         CALL ENDMAT
      ENDIF
!
!-- Barrel e.m. liquid argon calorimeter proper
      IVMDEB_Z = NVMATI + 1
!
      IF( MATPSBAR >= 1 ) THEN
        CALL NEWMAT( 'PrShBarr', MATPSBAR, 1, 34 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                    &
                     Zero, Zero,    Zero, RAPSBAR, RIPSBAR,   &
                     Zero, Zero, ZAPSBAR, RAPSBAR, RIPSBAR )
        CALL ENDMAT
      ENDIF
      ANG0 = FPIS2 - 2.D0*ATAN(EXP(-ETACORD0))
      ANG1 = FPIS2 - 2.D0*ATAN(EXP(-ETACORD1))
      ANG2 = FPIS2 - 2.D0*ATAN(EXP(-ETACORD2))
      ANG3 = FPIS2 - 2.D0*ATAN(EXP(-ETACORD3))
      ANGP = ATAN( ZAACORD /  RAACORD )
      ATEMP = ATAN(ZAACORD/(RIACORD+(DSAMPL21+DSAMPL22)*COS(ANGP)))
680   ANGS = ATAN(ZAACORD/(RIACORD+(DSAMPL21+DSAMPL22)*COS(ATEMP)))
      IF( ABS(ANGS-ATEMP) > 0.0001D0 ) THEN
        ATEMP = ANGS
        GO TO 680
      ENDIF
!
      IF( MATACORD1 >= 1 ) THEN
        CALL NEWMAT( 'ECalBa11', MATACORD1, 1, 34 )
        NAF1 = 3
        DO LF=0,NAF1
          ANG = ANG0 + LF*(ANG1-ANG0)/NAF1
          SANG = SIN(ANG)
          CANG = COS(ANG)
          IF( LF == 0 ) THEN
            VV(1)        = RIACORD
            UU(1)        = RIACORD*SANG/CANG
          ELSEIF( LF == NAF1 ) THEN
            VV(2)        = RIACORD
            UU(2)        = RIACORD*SANG/CANG
          ENDIF
          VV(  NAF1+3-LF) = RIACORD          + DSAMPL11*CANG
          UU(  NAF1+3-LF) = RIACORD*SANG/CANG+ DSAMPL11*SANG
        ENDDO
        CALL DVRRZZ( Zero,FANGFIN,4, NAF1+3, VV,UU )
        CALL ENDMAT
      ENDIF
!
      IF( MATACORD1 >= 1 ) THEN
        CALL NEWMAT( 'ECalBa12', MATACORD1, 1, 34 )
        DO LF=0,NAF1
          ANG = ANG0 + LF*(ANG1-ANG0)/NAF1
          SANG = SIN(ANG)
          CANG = COS(ANG)
          VV(LF+1)        = RIACORD          + DSAMPL11*CANG
          UU(LF+1)        = RIACORD*SANG/CANG+ DSAMPL11*SANG
          VV(2*NAF1+2-LF) = RIACORD          +(DSAMPL11+DSAMPL12)*CANG
          UU(2*NAF1+2-LF) = RIACORD*SANG/CANG+(DSAMPL11+DSAMPL12)*SANG
        ENDDO
        CALL DVRRZZ( Zero,FANGFIN,4, 2*NAF1+2, VV,UU )
        CALL ENDMAT
      ENDIF
!
      IF( MATACORD1 >= 1 ) THEN
        CALL NEWMAT( 'ECalBa13', MATACORD1, 1, 34 )
        DO LF=0,NAF1
          ANG = ANG0 + LF*(ANG1-ANG0)/NAF1
          SANG = SIN(ANG)
          CANG = COS(ANG)
          VV(LF+1)       = RIACORD          +(DSAMPL11+DSAMPL12)*CANG
          UU(LF+1)       = RIACORD*SANG/CANG+(DSAMPL11+DSAMPL12)*SANG
          IF( LF == 0 ) THEN
            VV(NAF1+3)   = RAACORD
            UU(NAF1+3)   = RAACORD*SANG/CANG
          ELSEIF( LF == NAF1 ) THEN
            VV(NAF1+2)   = RAACORD
            UU(NAF1+2)   = RAACORD*SANG/CANG
          ENDIF
        ENDDO
        CALL DVRRZZ( Zero,FANGFIN,4, NAF1+3, VV,UU )
        CALL ENDMAT
      ENDIF
!
      IF( MATACORD2 >= 1 ) THEN
         CALL NEWMAT( 'ECalBa21', MATACORD2, 1, 34 )
         NAF2 = 3
         DO LF=0,NAF2
           ANG = ANG1 + LF*(ANG2-ANG1)/NAF2
           SANG = SIN(ANG)
           CANG = COS(ANG)
           IF( LF == 0 ) THEN
              VV(1)        = RIACORD
              UU(1)        = RIACORD*SANG/CANG
           ELSEIF( LF == NAF2 ) THEN
              VV(2)        = RIACORD
              UU(2)        = RIACORD*SANG/CANG
           ENDIF
           VV(  NAF2+3-LF) = RIACORD          + DSAMPL21*CANG
           UU(  NAF2+3-LF) = RIACORD*SANG/CANG+ DSAMPL21*SANG
         ENDDO
         CALL DVRRZZ( Zero,FANGFIN,4, NAF2+3, VV,UU )
         CALL ENDMAT
      ENDIF
!
      IF( MATACORD2 >= 1 ) THEN
        CALL NEWMAT( 'ECalBa22', MATACORD2, 1, 34 )
        LA = 1
        DO LF=0,NAF2
          ANG = ANG1 + LF*(ANG2-ANG1)/NAF2
          SANG = SIN(ANG)
          CANG = COS(ANG)
          VV(LF+1)        = RIACORD          + DSAMPL21*CANG
          UU(LF+1)        = RIACORD*SANG/CANG+ DSAMPL21*SANG
          IF( ANG.GT.ANGS .AND. LA.GE.1 ) THEN
            SS = SIN(ANGS)
            CC = COS(ANGS)
            VV(2*NAF2+2+LA-LF)=RIACORD      +(DSAMPL21+DSAMPL22)*CC
            UU(2*NAF2+2+LA-LF)=RIACORD*SS/CC+(DSAMPL21+DSAMPL22)*SS
            LA = 0
          ENDIF
          VV(2*NAF2+2+LA-LF)=RIACORD         +(DSAMPL21+DSAMPL22)*CANG
          UU(2*NAF2+2+LA-LF)=RIACORD*SANG/CANG+(DSAMPL21+DSAMPL22)*SANG
          IF( UU(2*NAF2+2+LA-LF).GT.ZAACORD ) THEN
            VV(2*NAF2+2+LA-LF) = ZAACORD*CANG/SANG
            UU(2*NAF2+2+LA-LF) = ZAACORD
          ENDIF
        ENDDO
        CALL DVRRZZ( Zero,FANGFIN,4, 2*NAF2+3, VV,UU )
        CALL ENDMAT
      ENDIF
!
      IF( MATACORD2.GE.1 ) THEN
        CALL NEWMAT( 'ECalBa23', MATACORD2, 1, 34 )
        X = NAF2 * (ANGS-ANG1) / (ANG2-ANG1) + 0.9999D0
        NAF2P = X
        DO LF=0,NAF2P
          ANG = ANG1 + LF*(ANG2-ANG1)/NAF2
          IF( ANG.GT.ANGS ) ANG = ANGS
          SANG = SIN(ANG)
          CANG = COS(ANG)
          VV(LF+1)        = RIACORD          +(DSAMPL21+DSAMPL22)*CANG
          UU(LF+1)        = RIACORD*SANG/CANG+(DSAMPL21+DSAMPL22)*SANG
          IF( LF.EQ.0 ) THEN
            VV(NAF2P+3)   = RAACORD
            UU(NAF2P+3)   = RAACORD*SANG/CANG
            VV(NAF2P+2)   = RAACORD
            UU(NAF2P+2)   = ZAACORD
          ENDIF
        ENDDO
        CALL DVRRZZ( Zero,FANGFIN,4, NAF2P+3, VV,UU )
        CALL ENDMAT
      ENDIF
!
      IF( MATACORD3.GE.1 ) THEN
        CALL NEWMAT( 'ECalBa31', MATACORD3, 1, 34 )
        DO LF=0,1
          ANG = ANG2 + LF*(ANG3-ANG2)
          SANG = SIN(ANG)
          CANG = COS(ANG)
          VV(LF+1) = RIACORD
          UU(LF+1) = RIACORD*SANG/CANG
          VV(4-LF) = RIACORD          + DSAMPL31*CANG
          UU(4-LF) = RIACORD*SANG/CANG+ DSAMPL31*SANG
        ENDDO
        CALL DVRRZZ( Zero,FANGFIN,4, 4, VV,UU )
        CALL ENDMAT
      ENDIF
!
      IF( MATACORD3.GE.1 ) THEN
        CALL NEWMAT( 'ECalBa32', MATACORD3, 1, 34 )
        DO LF=0,1
          ANG = ANG2 + LF*(ANG3-ANG2)
          SANG = SIN(ANG)
          CANG = COS(ANG)
          VV(LF+1) = RIACORD          + DSAMPL31*CANG
          UU(LF+1) = RIACORD*SANG/CANG+ DSAMPL31*SANG
          VV(4-LF) = ZAACORD*CANG/SANG
          UU(4-LF) = ZAACORD
        ENDDO
        CALL DVRRZZ( Zero,FANGFIN,4, 4, VV,UU )
        CALL ENDMAT
      ENDIF
!
      IF( MATSURI.GE.1 ) THEN
        DO I=1,NNSURI
          CALL NEWMAT( 'ECalSuRi', MATSURI, 1, 35 )
          Z = ZZSURI(I)
          R = RAACORD + 0.1D0
          VV( 1) = Z - AASURI/2.D0
          VV( 2) = Z + AASURI/2.D0
          VV( 3) = Z + AASURI/2.D0
          VV( 4) = Z + EESURI/2.D0
          VV( 5) = Z + EESURI/2.D0
          VV( 6) = Z + AASURI/2.D0
          VV( 7) = Z + AASURI/2.D0
          VV( 8) = Z - AASURI/2.D0
          VV( 9) = Z - AASURI/2.D0
          VV(10) = Z - EESURI/2.D0
          VV(11) = Z - EESURI/2.D0
          VV(12) = Z - AASURI/2.D0
          UU( 1) = R
          UU( 2) = R
          UU( 3) = R + EESURI
          UU( 4) = R + EESURI
          UU( 5) = R + HHSURI - EESURI
          UU( 6) = R + HHSURI - EESURI
          UU( 7) = R + HHSURI
          UU( 8) = R + HHSURI
          UU( 9) = R + HHSURI - EESURI
          UU(10) = R + HHSURI - EESURI
          UU(11) = R + EESURI
          UU(12) = R + EESURI
          CALL DVRRZZ( Zero,FANGFIN,4, 12, UU,VV )
          CALL ENDMAT
        ENDDO
      ENDIF
!
      IVMFIN_Z = NVMATI
      CALL SYZMAT(IVMDEB_Z,IVMFIN_Z)
!
!
!-- End-cap cryostat + calorimeters
      IVMDEB_Z = NVMATI + 1
      ECELETAS1 = ECELOETA3 + (ECELOETA4-ECELOETA3)*3.0D0/8.0D0
      ECELETAS2 = ECELOETA3 + (ECELOETA4-ECELOETA3)*4.0D0/8.0D0
      ECELETAS3 = ECELOETA1 + (ECELOETA2-ECELOETA1)*4.0D0/5.0D0
!
!-- First, the cryostat
      IF( MATECCR.GE.1 ) THEN
        CALL NEWMAT( 'ECCalCry', MATECCR, 1, 40 )
        SEP3 = EPECC2 * SQRT(ZIECC1**2+RIECC1**2) / ZIECC1
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                               &
                     Zero,Zero, -ZAECC1       , RAECC2, RIECC3,          &
                     Zero,Zero, -ZAECC1+EPECC3, RAECC2, RIECC3 )
        CALL DVPIPE( Zero, 2, Zero,ANGFIN,                               &
                     Zero,Zero, -ZAECC1+EPECC3, RIECC3+EPECC2, RIECC3,   &
                     Zero,Zero, -ZIECC2-EPECC2, RIECC3+EPECC2, RIECC3 )
        CALL DVPIPE( Zero, 3, Zero,ANGFIN,                               &
                     Zero,Zero, -ZIECC2-EPECC2, RIECC2+SEP3, RIECC3,     &
                     Zero,Zero, -ZIECC2       , RIECC2+SEP3, RIECC3 )
        CALL DVPIPE( Zero, 3, Zero,ANGFIN,                               &
                     Zero,Zero, -ZIECC2       , RIECC2+SEP3, RIECC2,     &
                     Zero,Zero, -ZIECC1-EPECC2, RIECC1+SEP3, RIECC1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                               &
                     Zero,Zero, -ZIECC1-EPECC2, RAECC1, RIECC1,          &
                     Zero,Zero, -ZIECC1       , RAECC1, RIECC1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                               &
                     Zero,Zero, -ZIECC3       , RAECC1, RAECC1-EPECC1,   &
                     Zero,Zero, -ZIECC1-EPECC2, RAECC1, RAECC1-EPECC1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                               &
                     Zero,Zero, -ZIECC3-EPECC1, RAECC2, RAECC1-EPECC1,   &
                     Zero,Zero, -ZIECC3       , RAECC2, RAECC1-EPECC1 )
        CALL DVPIPE( Zero, 4, Zero,ANGFIN,                               &
                     Zero,Zero, -ZAECC1+EPECC3, RAECC2, RAECC2-EPECC1,   &
                     Zero,Zero, -ZIECC3-EPECC1, RAECC2, RAECC2-EPECC1 )
        CALL ENDMAT
      ENDIF
      ANFEEC = TWOPI / NNFEEC
      IF( MATFEEC.GE.1 ) THEN
        CALL NEWMAT( 'ECCalFee', MATFEEC, 1, 40 )
        DO I=1,NNFEEC/8
          F0FEEC = (I-0.5D0)*ANFEEC
          CALL DVPIPE( F0FEEC, 6, Zero,Z360,                                       &
                       RAECC2-HHFEEC, Zero, -(ZAECC1+ZIECC3)/2.D0, RRFEEC, Zero,   &
                       RAECC2+HHFEEC, Zero, -(ZAECC1+ZIECC3)/2.D0, RRFEEC, Zero )
        ENDDO
        CALL ENDMAT
      ENDIF
!-- Second, the 9 volumes of the end-cap accordion "spanish fan" e.m. calo
!-- Outer wheel, sampling 1 with eta between 1.35 and 1.5
      IF( MATECEL.GE.1 ) THEN
         CALL NEWMAT( 'ECalEC11', MATECEL, 1, 40 )
          VV(1) = ECELIZ
          UU(1) = tan(atan(dexp(-ECELOETA1))*2.0d0)*VV(1)
          UU(2) = ECELOR
          VV(2) = UU(2)/tan(atan(dexp(-ECELOETA1))*2.0d0)
          UU(3) = ECELOR
          VV(3) = ECELIZ+ECELDEEP*ECELRDP1
          UUS(1) = UU(3)
          VVS(1) = VV(3)
          VV(4) = VV(3)
          UU(4) = tan(atan(dexp(-ECELOETA2))*2.0d0)*VV(4)
          VV(5) = ECELIZ
          UU(5) = tan(atan(dexp(-ECELOETA2))*2.0d0)*VV(5)
          VV(5) = ECELIZ
          UU(5) = tan(atan(dexp(-ECELOETA2))*2.0d0)*VV(5)
          CALL DVRRZZ(Zero,FANGFIN,4,5,UU,VV)
         CALL ENDMAT
!-- Outer wheel, sampling 1 with eta between 1.5 and 1.8
         CALL NEWMAT( 'ECalEC21', MATECEL, 1, 40 )
          VV(1) = VV(5)
          UU(1) = UU(5)
          VV(2) = VV(4)
          UU(2) = UU(4)
          VV(3) = VV(1)+ECELDEEP*ECELRDP1
          UU(3) = tan(atan(dexp(-ECELOETA3))*2.0d0)*VV(3)
          VV(4) = ECELIZ
          UU(4) = tan(atan(dexp(-ECELOETA3))*2.0d0)*VV(4)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
!-- Outer wheel, sampling 1 with eta between 1.8 and 2.0
         CALL NEWMAT( 'ECalEC31', MATECEL, 1, 40 )
          VV(1) = VV(4)
          UU(1) = UU(4)
          VV(2) = VV(3)
          UU(2) = UU(3)
          VV(3) = VV(1)+ECELDEEP*ECELRDP1
          UU(3) = tan(atan(dexp(-ECELETAS1))*2.0d0)*VV(3)
          UUS(2) = UU(3)
          VVS(2) = VV(3)
          VV(4) = VV(1)+ECELDEEP*ECELRDP2
          UU(4) = tan(atan(dexp(-ECELETAS1))*2.0d0)*VV(4)
          UUS(3) = UU(4)
          VVS(3) = VV(4)
          VV(5) = VV(1)+ECELDEEP*ECELRDP2
          UU(5) = tan(atan(dexp(-ECELOETA4))*2.0d0)*VV(5)
          VV(6) = ECELIZ
          UU(6) = tan(atan(dexp(-ECELOETA4))*2.0d0)*VV(6)
          CALL DVRRZZ(Zero,FANGFIN,4,6,UU,VV)
         CALL ENDMAT
!-- Outer wheel, sampling 1 with eta between 2.0 and 2.4
         CALL NEWMAT( 'ECalEC41', MATECEL, 1, 40 )
          VV(1) = VV(6)
          UU(1) = UU(6)
          VV(2) = VV(5)
          UU(2) = UU(5)
          VV(3) = ECELIZ+ECELDEEP*ECELRDP2
          UU(3) = tan(atan(dexp(-ECELOETA5))*2.0d0)*VV(3)
          VV(4) = ECELIZ
          UU(4) = tan(atan(dexp(-ECELOETA5))*2.0d0)*VV(4)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
!-- Outer wheel, sampling 1 with eta between 2.4 and 2.5
         CALL NEWMAT( 'ECalEC51', MATECEL, 1, 40 )
          VV(1) = VV(4)
          UU(1) = UU(4)
          VV(2) = VV(3)
          UU(2) = UU(3)
          VV(3) = ECELIZ+ECELDEEP*ECELRDP2
          UU(3) = tan(atan(dexp(-ECELOETA6))*2.0d0)*VV(3)
          UU(3) = UU(3)+ECELGBW/2.0d0
          VV(4) = ECELIZ
          UU(4) = tan(atan(dexp(-ECELOETA6))*2.0d0)*VV(4)
          UU(4) = UU(4)+ECELGBW/2.0d0
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
!-- Outer wheel, sampling 2 with eta between 1.375 and 2.5
         CALL NEWMAT( 'ECalEC12', MATECEL, 1, 40 )
          VVS(4) = VV(3)
          UUS(4) = UU(3)-ECELGBW
          VVS(5) = ECELIZ+ECELDEEP*ECELRDP5
          UUS(5) = tan(atan(dexp(-ECELOETA6))*2.0d0)*VVS(5)
          UUS(5) = UUS(5)-ECELGBW/2.0d0
          VVS(6) = ECELIZ+ECELDEEP*ECELRDP4
          UUS(6) = tan(atan(dexp(-ECELETAS2))*2.0d0)*VVS(6)
          VVS(7) = ECELIZ+ECELDEEP*ECELRDP3
          UUS(7) = tan(atan(dexp(-ECELOETA2))*2.0d0)*VVS(7)
          VVS(8) = ECELIZ+ECELDEEP
          UUS(8) = tan(atan(dexp(-ECELOETA2))*2.0d0)*VVS(8)
          VVS(9) = ECELIZ+ECELDEEP
          UUS(9) = tan(atan(dexp(-ECELETAS3))*2.0d0)*VVS(9)
          CALL DVRRZZ(Zero,FANGFIN,4,9,UUS,VVS)
         CALL ENDMAT
!-- Outer wheel, sampling 3 with eta between 1.5 and 2.5
         CALL NEWMAT( 'ECalEC13', MATECEL, 1, 40 )
          do ii = 1, 4
            VV(ii) = VVS(ii+4)
            UU(ii) = UUS(ii+4)
          enddo
          VV(5) = ECELIZ+ECELDEEP
          UU(5) = tan(atan(dexp(-ECELOETA6))*2.0d0)*VV(5)
          UU(5) = UU(5)-ECELGBW/2.0d0
          CALL DVRRZZ(Zero,FANGFIN,4,5,UU,VV)
         CALL ENDMAT
!-- Inner wheel, sampling 1 with eta between 2.5 and 3.2
         CALL NEWMAT( 'ECalEC61', MATECEL, 1, 40 )
          VV(1) = ECELIZ
          UU(1) = tan(atan(dexp(-ECELIETA1))*2.0d0)*VV(1)
          VV(2) = ECELIZ+ECELDEEP*ECELRDP3
          UU(2) = tan(atan(dexp(-ECELIETA1))*2.0d0)*VV(2)
          VV(3) = ECELIZ+ECELDEEP*ECELRDP5
          UU(3) = tan(atan(dexp(-ECELIETA2))*2.0d0)*VV(3)
          VV(4) = ECELIZ
          UU(4) = tan(atan(dexp(-ECELIETA2))*2.0d0)*VV(4)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
!-- Inner wheel, sampling 2 with eta between 2.5 and 3.2
         CALL NEWMAT( 'ECalEC62', MATECEL, 1, 40 )
          VV(1) = VV(3)
          UU(1) = UU(3)
          VV(3) = ECELIZ+ECELDEEP
          UU(3) = tan(atan(dexp(-ECELIETA1))*2.0d0)*VV(3)
          VV(4) = ECELIZ+ECELDEEP
          UU(4) = tan(atan(dexp(-ECELIETA2))*2.0d0)*VV(4)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
      ENDIF
!-- Third, the end-cap e.m. presampler
      IF( MATECELPS.GE.1 ) THEN
         CALL NEWMAT( 'ECalECPS', MATECELPS, 1, 40 )
          VV(1) = ECELPSIZ
          UU(1) = tan(atan(dexp(-ECELPSETA1))*2.0d0)*VV(1)
          VV(2) = VV(1)
          UU(2) = tan(atan(dexp(-ECELPSETA2))*2.0d0)*VV(2)
          VV(3) = VV(1)+ECELPSDEEP
          UU(3) = UU(2)
          VV(4) = VV(3)
          UU(4) = UU(1)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
      ENDIF
!-- Fourth, the end-cap liq. argon hadronic calorimeter
      IF( MATECHAD1.GE.1 ) THEN
         IF( ABS(DRBTIL1).LT.0.1D0 ) THEN
!-- Old Physics TDR Version
            CALL NEWMAT( 'ECHadCa1', MATECHAD1, 1, 40 )
             UU(1) = RAECHAD
             UU(2) = RAECHAD
             UU(3) = RMECHAD
             UU(4) = RMECHAD
             UU(5) = RIECHAD
             UU(6) = RIECHAD
             VV(1) = -ZIECHAD1
             VV(2) = -ZAECHAD1
             VV(3) = -ZAECHAD1
             VV(4) = -ZMECHAD1
             VV(5) = -ZMECHAD1
             VV(6) = -ZIECHAD1
             CALL DVRRZZ( Zero,FANGFIN,4, 6, UU,VV )
            CALL ENDMAT
         ELSE
!-- New Version
           CALL NEWMAT( 'ECHadCa1', MATECHAD1, 1, 40 )
           CALL DVPIPE( Zero, 3, Zero,ANGFIN,                     &
                        Zero,Zero, -ZMECHAD1, RAECHAD, RIECHAD,   &
                        Zero,Zero, -ZIECHAD1, RAECHAD, RIECHAD )
           CALL ENDMAT
           CALL NEWMAT( 'ECHadCa2', MATECHAD1, 1, 40 )
           CALL DVPIPE( Zero, 3, Zero,ANGFIN,                     &
                        Zero,Zero, -ZAECHAD1, RAECHAD, RMECHAD,   &
                        Zero,Zero, -ZMECHAD1, RAECHAD, RMECHAD )
           CALL ENDMAT
         ENDIF
      ENDIF
      IF( MATECHAD2.GE.1 ) THEN
         IF( ABS(DRBTIL1).LT.0.1D0 ) THEN
!-- Old Physics TDR Version
            CALL NEWMAT( 'ECHadCa2', MATECHAD2, 1, 40 )
             VV(1) = -ZAECHAD2
             VV(2) = -ZIECHAD2
             VV(3) = -ZIECHAD2
             VV(4) = -ZAECHAD2
             CALL DVRRZZ( Zero,FANGFIN,4, 4, UU,VV )
            CALL ENDMAT
         ELSE
!-- New Version
           CALL NEWMAT( 'ECHadCa3', MATECHAD2, 1, 40 )
           CALL DVPIPE( Zero, 3, Zero,ANGFIN,                      &
                        Zero,Zero, -ZAECHAD2, RAECHAD, RMECHAD,    &
                        Zero,Zero, -ZIECHAD2, RAECHAD, RMECHAD )
           CALL ENDMAT
         ENDIF
      ENDIF
!-- Fifth, the end-cap "very forward" calorimeters
      IF( MATECF1.GE.1 ) THEN
        CALL NEWMAT( 'ECElFCal', MATECF1, 1, 40 )
        CALL DVPIPE( Zero, 3, Zero,ANGFIN,                  &
                     Zero,Zero, -ZAECF1, RAECFN, RIECF1,    &
                     Zero,Zero, -ZIECF1, RAECFN, RIECF1 )
        CALL ENDMAT
      ENDIF
      IF( MATECF2.GE.1 ) THEN
        CALL NEWMAT( 'ECHaFCal', MATECF2, 1, 40 )
        CALL DVPIPE( Zero, 3, Zero,ANGFIN,                  &
                     Zero,Zero, -ZAECF2, RAECFN, RIECF2,    &
                     Zero,Zero, -ZIECF2, RAECFN, RIECF2 )
        CALL DVPIPE( Zero, 3, Zero,ANGFIN,                  &
                     Zero,Zero, -ZAECF3, RAECFN, RIECF3,    &
                     Zero,Zero, -ZIECF3, RAECFN, RIECF3 )
        CALL ENDMAT
      ENDIF
      IF( MATECF1.GE.1 ) THEN
        CALL NEWMAT( 'ECPlugFC', MATECF1, 1, 40 )
        CALL DVPIPE( Zero, 3, Zero,ANGFIN,                  &
                     Zero,Zero, -ZAECF4, RAECFN, RIECF4,    &
                     Zero,Zero, -ZIECF4, RAECFN, RIECF4 )
        CALL ENDMAT
      ENDIF
!-- Sixth, the end-cap Cold Vessel
      IF( MATECELC.GE.1 ) THEN
         CALL NEWMAT( 'ECalECCV', MATECELC, 1, 40 )
!*******  VV(1) = ECELCVOZ2       ! temporary (cryostat shape error)
!*******  UU(1) = ECELCVIR1
!*******  VV(2) = ECELCVIZ1
!*******  UU(2) = UU(1)
          VV(1) = ECELCVIZ1
          UU(1) = ECELCVIR2
          VV(2) = VV(1)+0.01d0
          UU(2) = UU(1)+1.0d0
          VV(3) = VV(2)
          UU(3) = ECELCVOR2
          VV(4) = ECELCVIZ2
          UU(4) = UU(3)
          VV(5) = VV(4)
          UU(5) = ECELCVOR3
          VV(6) = ECELCVOZ3
          UU(6) = UU(5)
          VV(7) = VV(6)
          UU(7) = ECELCVOR1
          VV(8) = ECELCVOZ1
          UU(8) = UU(7)
          VV(9) = VV(8)
          UU(9) = tan(atan(dexp(-ECELPSETA1))*2.0d0)*ECELPSIZ
          VV(10) = ECELPSIZ
          UU(10) = UU(9)
          VV(11) = VV(10)
          UU(11) = tan(atan(dexp(-ECELPSETA2))*2.0d0)*ECELPSIZ
          VV(12) = VV(9)
          UU(12) = UU(11)
          VV(13) = ECELCVOZ1
          UU(13) = ECELCVIR2
!*******  VV(14) = VV(1)            ! temporary (cryostat shape error)
!*******  UU(14) = UU(13)
!*******  CALL DVRRZZ(Zero,FANGFIN,4,14,UU,VV)
          CALL DVRRZZ(Zero,FANGFIN,4,13,UU,VV)
         CALL ENDMAT
      ENDIF
!-- Seventh, the plugs
      IF( MATECELPG.GE.1 ) THEN
         CALL NEWMAT( 'ECalECP1', MATECELPG, 1, 40 )
          VV(1) = ECELPG1OZ-ECELPG1DP
          UU(1) = ECELPG1IR
          VV(2) = VV(1)
          UU(2) = ECELPG1OR
          VV(3) = ECELPG1OZ
          UU(3) = UU(2)
          VV(4) = VV(3)
          UU(4) = UU(1)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
         CALL NEWMAT( 'ECalECP2', MATECELPG, 1, 40 )
          VV(1) = ECELPG2IZ
          UU(1) = ECELPG2IR
          VV(2) = VV(1)
          UU(2) = ECELPG2OR
          VV(3) = ECELPG2OZ
          UU(3) = UU(2)
          VV(4) = VV(3)
          UU(4) = UU(1)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
         CALL NEWMAT( 'ECalECP4', MATECELPG, 1, 40 )
          VV(1) = ECELPG1OZ
          UU(1) = ECELPG1IR
          VV(2) = VV(1)
          UU(2) = ECELCVOR3
          VV(3) = VV(2)+ECELPG4DP
          UU(3) = UU(2)
          VV(4) = VV(3)
          UU(4) = UU(1)
          CALL DVRRZZ(Zero,FANGFIN,4,4,UU,VV)
         CALL ENDMAT
      ENDIF
!
      IVMFIN_Z = NVMATI
      CALL SYZMAT(IVMDEB_Z,IVMFIN_Z)
!
690   CONTINUE
!
!
!-- Define solenoid magnet as a new volume
      IF( MATSOLE.GE.1 ) THEN
         CALL NEWMAT( 'Solenoid', MATSOLE, 1, 42 )
         CALL DVPIPE( Zero, 4, Zero,ANGFIN,       &
          Zero, Zero, -ZASOLE, RASOLE, RISOLE,    &
          Zero, Zero,  ZASOLE, RASOLE, RISOLE )
         CALL ENDMAT
      ENDIF
!
!
!-- Copy volumes around Phi
      IVMFIN_F = NVMATI
      CALL SYFMAT(IVMDEB_F,IVMFIN_F)
!
 END SUBROUTINE AmdcLarg
!
!
 SUBROUTINE AmdcInDet
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: IVMDEB_F, IVMFIN_F
 INTEGER :: I, J, ICODE1, ICODE2
 REAL(8) :: ANGFIN, ZCUR
!
#include "AmdcStand/comamu.inc"
!
      ANGFIN  = 45.D0
!
      IVMDEB_F = NVMATI + 1
!
!
!-- Define R_Pixels as a new volume
      IF( MATPIXR.GE.1 ) THEN
         CALL NEWMAT( 'R_Pixels', MATPIXR, 1, 45 )
          DO I=1,NPIXR
          CALL DVPIPE( Zero, I+1, Zero,ANGFIN,                    &
           Zero, Zero, -ZAPIXR(I), RIPIXR(I)+EPPIXR, RIPIXR(I),   &
           Zero, Zero,  ZAPIXR(I), RIPIXR(I)+EPPIXR, RIPIXR(I) )
          ENDDO
         CALL ENDMAT
      ENDIF
!-- Define Z_Pixels as a new volumes
      IF( MATPIXZ.GE.1 ) THEN
         CALL NEWMAT( 'Z-Pixels', MATPIXZ, 1, 45 )
          DO I=1,NPIXZ
          CALL DVPIPE( Zero, 4, Zero,ANGFIN,                     &
           Zero, Zero, -ZIPIXZ(I)-EPPIXZ, RAPIXZ(I), RIPIXZ(I),  &
           Zero, Zero, -ZIPIXZ(I)       , RAPIXZ(I), RIPIXZ(I) )
          ENDDO
         CALL ENDMAT
         CALL NEWMAT( 'Z+Pixels', MATPIXZ, 1, 45 )
          DO I=1,NPIXZ
          CALL DVPIPE( Zero, 4, Zero,ANGFIN,                     &
           Zero, Zero,  ZIPIXZ(I)       , RAPIXZ(I), RIPIXZ(I),  &
           Zero, Zero,  ZIPIXZ(I)+EPPIXZ, RAPIXZ(I), RIPIXZ(I) )
          ENDDO
         CALL ENDMAT
      ENDIF
!-- Define R_SCTs as a new volume
      IF( MATSCTR.GE.1 ) THEN
         CALL NEWMAT( 'R_SCTtra', MATSCTR, 1, 45 )
          DO I=1,NSCTR
          CALL DVPIPE( Zero, 4, Zero,ANGFIN,                    &
           Zero, Zero, -ZASCTR, RISCTR(I)+EPSCTR, RISCTR(I),    &
           Zero, Zero,  ZASCTR, RISCTR(I)+EPSCTR, RISCTR(I) )
          ENDDO
         CALL ENDMAT
      ENDIF
!-- Define Z-SCTs as a new volume
      IF( MATSCTZ.GE.1 ) THEN
         CALL NEWMAT( 'Z-SCTsup', MATSCTZ, 1, 46 )
          DO I=1,NSCTZ
          CALL DVPIPE( Zero, 4, Zero,ANGFIN,                    &
           Zero, Zero, -ZISCTZ(I)-EPSCTZ, RASCTZ, RISCTZ(I),    &
           Zero, Zero, -ZISCTZ(I)       , RASCTZ, RISCTZ(I) )
          ENDDO
         CALL ENDMAT
      ENDIF
      IF( MATSCTYP.GE.1 ) THEN
         CALL NEWMAT( 'Z-SCTtra', MATSCTYP, 1, 46 )
          DO I=1,NSCTZ
           ICODE1 = IISCTZ(I)
710        ICODE2 = ICODE1/10
           J = ICODE1 - ICODE2*10
           IF( J.GE.1 ) THEN
              ZCUR = ZISCTZ(I) + DZSCTYP(J)
              CALL DVPIPE( Zero, 4, Zero,ANGFIN,                   &
               Zero, Zero, -ZCUR-EPSCTYP, RASCTYP(J),RISCTYP(J),   &
               Zero, Zero, -ZCUR        , RASCTYP(J),RISCTYP(J) )
              ICODE1 = ICODE2
              GO TO 710
           ENDIF
          ENDDO
         CALL ENDMAT
      ENDIF
!-- Define Z+SCTs as a new volume
      IF( MATSCTZ.GE.1 ) THEN
         CALL NEWMAT( 'Z+SCTsup', MATSCTZ, 1, 47 )
          DO I=1,NSCTZ
          CALL DVPIPE( Zero, 4, Zero,ANGFIN,                   &
           Zero, Zero,  ZISCTZ(I)       , RASCTZ, RISCTZ(I),   &
           Zero, Zero,  ZISCTZ(I)+EPSCTZ, RASCTZ, RISCTZ(I) )
          ENDDO
         CALL ENDMAT
      ENDIF
      IF( MATSCTYP.GE.1 ) THEN
         CALL NEWMAT( 'Z+SCTtra', MATSCTYP, 1, 47 )
          DO I=1,NSCTZ
           ICODE1 = IISCTZ(I)
720        ICODE2 = ICODE1/10
           J = ICODE1 - ICODE2*10
           IF( J.GE.1 ) THEN
              ZCUR = ZISCTZ(I) + DZSCTYP(J)
              CALL DVPIPE( Zero, 4, Zero,ANGFIN,                   &
               Zero, Zero,  ZCUR        , RASCTYP(J),RISCTYP(J),   &
               Zero, Zero,  ZCUR+EPSCTYP, RASCTYP(J),RISCTYP(J) )
              ICODE1 = ICODE2
              GO TO 720
           ENDIF
          ENDDO
         CALL ENDMAT
      ENDIF
!-- Define Barrel TRTs as a new volume
      IF( MATTRTR.GE.1 ) THEN
         CALL NEWMAT( 'R-TRTtra', MATTRTR, 1, 45 )
         CALL DVPIPE( Zero, 5, Zero,ANGFIN,           &
          Zero, Zero, -ZATRTR, RMTRTR, RITRTR,        &
          Zero, Zero, -ZITRTR, RMTRTR, RITRTR )
         IF( JVETRT == 1 ) THEN
           CALL DVPIPE( Zero, 5, Zero,ANGFIN,         &
            Zero, Zero, -ZATRTR, RATRTR, RMTRTR+EPS,  &
            Zero, Zero, -ZITRTR, RATRTR, RMTRTR+EPS )
         ELSE
           CALL DVPIPE( Zero, 5, Zero,ANGFIN,         &
            Zero, Zero, -ZATRTR, RNTRTR, RMTRTR+EPS,  &
            Zero, Zero, -ZITRTR, RNTRTR, RMTRTR+EPS )
           CALL DVPIPE( Zero, 5, Zero,ANGFIN,         &
            Zero, Zero, -ZATRTR, RATRTR, RNTRTR+EPS,  &
            Zero, Zero, -ZITRTR, RATRTR, RNTRTR+EPS )
         ENDIF
         CALL ENDMAT
         CALL NEWMAT( 'R+TRTtra', MATTRTR, 1, 45 )
         CALL DVPIPE( Zero, 5, Zero,ANGFIN,           &
          Zero, Zero,  ZITRTR, RMTRTR, RITRTR,        &
          Zero, Zero,  ZATRTR, RMTRTR, RITRTR )
         IF( JVETRT == 1 ) THEN
           CALL DVPIPE( Zero, 5, Zero,ANGFIN,         &
            Zero, Zero,  ZITRTR, RATRTR, RMTRTR+EPS,  &
            Zero, Zero,  ZATRTR, RATRTR, RMTRTR+EPS )
         ELSE
           CALL DVPIPE( Zero, 5, Zero,ANGFIN,         &
            Zero, Zero,  ZITRTR, RNTRTR, RMTRTR+EPS,  &
            Zero, Zero,  ZATRTR, RNTRTR, RMTRTR+EPS )
           CALL DVPIPE( Zero, 5, Zero,ANGFIN,         &
            Zero, Zero,  ZITRTR, RATRTR, RNTRTR+EPS,  &
            Zero, Zero,  ZATRTR, RATRTR, RNTRTR+EPS )
         ENDIF
         CALL ENDMAT
      ENDIF
!>> Define End-cap TRTs as a new volume
      IF( MATTRTZ1.GE.1 ) THEN
         CALL NEWMAT( 'Z-TRTtr1', MATTRTZ1, 1, 46 )
          DO I=1,NTRTZ1
          CALL DVPIPE( Zero, 5, Zero,ANGFIN,                   &
           Zero, Zero, -ZITRTZ1(I)-DZTRTZ1, RATRTZ1, RITRTZ1,  &
           Zero, Zero, -ZITRTZ1(I)        , RATRTZ1, RITRTZ1 )
          ENDDO
         CALL ENDMAT
         CALL NEWMAT( 'Z+TRTtr1', MATTRTZ1, 1, 47 )
          DO I=1,NTRTZ1
          CALL DVPIPE( Zero, 5, Zero,ANGFIN,                   &
           Zero, Zero,  ZITRTZ1(I)        , RATRTZ1, RITRTZ1,  &
           Zero, Zero,  ZITRTZ1(I)+DZTRTZ1, RATRTZ1, RITRTZ1 )
          ENDDO
         CALL ENDMAT
      ENDIF
      IF( MATTRTZ2.GE.1 ) THEN
         CALL NEWMAT( 'Z-TRTtr2', MATTRTZ2, 1, 46 )
          DO I=1,NTRTZ2
          CALL DVPIPE( Zero, 5, Zero,ANGFIN,                   &
           Zero, Zero, -ZITRTZ2(I)-DZTRTZ2, RATRTZ2, RITRTZ2,  &
           Zero, Zero, -ZITRTZ2(I)       , RATRTZ2, RITRTZ2 )
          ENDDO
         CALL ENDMAT
         CALL NEWMAT( 'Z+TRTtr2', MATTRTZ2, 1, 47 )
          DO I=1,NTRTZ2
          CALL DVPIPE( Zero, 5, Zero,ANGFIN,                   &
           Zero, Zero,  ZITRTZ2(I)        , RATRTZ2, RITRTZ2,  &
           Zero, Zero,  ZITRTZ2(I)+DZTRTZ2, RATRTZ2, RITRTZ2 )
          ENDDO
         CALL ENDMAT
      ENDIF
      IF( MATTRTZ3.GE.1 ) THEN
         CALL NEWMAT( 'Z-TRTtr3', MATTRTZ3, 1, 46 )
          DO I=1,NTRTZ3
          CALL DVPIPE( Zero, 5, Zero,ANGFIN,                   &
           Zero, Zero, -ZITRTZ3(I)-DZTRTZ3, RATRTZ3, RITRTZ3,  &
           Zero, Zero, -ZITRTZ3(I)       , RATRTZ3, RITRTZ3 )
          ENDDO
         CALL ENDMAT
         CALL NEWMAT( 'Z+TRTtr3', MATTRTZ3, 1, 47 )
          DO I=1,NTRTZ3
          CALL DVPIPE( Zero, 5, Zero,ANGFIN,                   &
           Zero, Zero,  ZITRTZ3(I)        , RATRTZ3, RITRTZ3,  &
           Zero, Zero,  ZITRTZ3(I)+DZTRTZ3, RATRTZ3, RITRTZ3 )
          ENDDO
         CALL ENDMAT
      ENDIF
!
!
!-- Copy volumes around Phi
      IVMFIN_F = NVMATI
      CALL SYFMAT(IVMDEB_F,IVMFIN_F)
!
 END SUBROUTINE AmdcInDet
!
!
!
 SUBROUTINE AmdcShield
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 CHARACTER(4) :: CAR4
 CHARACTER(8) :: CAR8
 INTEGER :: L
!
#include "AmdcStand/comamu.inc"
!
!
!-- Define shielding cylinders as new volumes
      DO L=1,NBSHIE
        WRITE(CAR4,'(I4)') L
        CAR8 = 'Shi-'//CAR4
        CALL NEWMAT( CAR8, MATSHI(L), 0, 43 )
        CALL DVPIPE(Zero, NCSHIE(L), Zero,Z360,         &
          Zero,Zero,-ZISHIE(L), RIOSHI(L),RIISHI(L),    &
          Zero,Zero,-ZASHIE(L), RAOSHI(L),RAISHI(L) )
        CALL ENDMAT
        CAR8 = 'Shi+'//CAR4
        CALL NEWMAT( CAR8, MATSHI(L), 0, 44 )
        CALL DVPIPE(Zero, NCSHIE(L), Zero,Z360,        &
          Zero,Zero, ZISHIE(L), RIOSHI(L),RIISHI(L),   &
          Zero,Zero, ZASHIE(L), RAOSHI(L),RAISHI(L) )
        CALL ENDMAT
      ENDDO
!
END SUBROUTINE AmdcShield
!
