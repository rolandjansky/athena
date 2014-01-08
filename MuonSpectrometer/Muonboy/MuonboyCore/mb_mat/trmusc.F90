!
!> Follow a track with parameters given by VDEB and computes the dead matter crossed
!> between RCHAMI/ZCHAMI and RCHAMA/ZCHAMA
!> \author M.Virchaux
!
 SUBROUTINE TRMUSC(NSENSI)
 USE M_MB_Control
 USE M_MB_General_CONSTANTS
 USE M_MB_MuGeom
 USE M_MB_COMATI
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
!
#include "AmdcStand/comama.inc"
!
      INTEGER, INTENT(IN) :: NSENSI
      LOGICAL :: INSIDE, NAKCHA, NOKTRI, NUKCHA, NUKTRI
      REAL(8) :: VIN(8), VOUT(8), VOUTBI(8)
      REAL(8) :: RRMOSC(KMASC), X0MOSC(KMASC)
      REAL(8) :: ALMOSC(KMASC), BLMOSC(KMASC)
      INTEGER :: JVMOSC(KMASC)
      INTEGER :: JVOPEN(KMUSC)
      REAL(8) :: RROPEN(KMUSC), SSOPEN(KMUSC), SSMUSC(KMUSC)
      INTEGER :: IOKPHI(0:9)
      INTEGER :: NBMOSC, ISENS1,ISENS2, IFLIP, I, IFI1,IFI2,IFI3, ISENS
      INTEGER :: IPAS, IV, IP, IE0, NBE, IC, INOF,INOZ,IGEOM,Itrans
      INTEGER :: IMDT, ICT, JFF, IRPC, IM1,IM2, N, NOPEN, NSENS
      INTEGER :: IMUSC, IOPEN, IMANAT, NBMOSO, NFIX
      REAL(8) :: R2DEB,AZDEB, R, FIFI, RCHAMAP,ZCHAMAP,RCHAMIP,ZCHAMIP
      REAL(8) :: PAPAS, R2IN, R2OUT,AZOUT, XMINI,YMINI,ZMINI
      REAL(8) :: XMAXI,YMAXI,ZMAXI, ZZIN,ZZOUT, PAPABI,R2OUTBI,AZOUTBI
      REAL(8) :: XXBI,YYBI, TZ0,CF0,SF0, TZIN,TZOUT, AA,CAA,SAA
      REAL(8) :: TTIN,TTOUT, ZT0,VTZ0,TT0,VTT0,SS0, DL
      REAL(8) :: ZTIN,ZTOUT, SSIN,SSOUT, DFREE,UFREE, TOL
      REAL(8) :: Sidea,Zidea,Tidea, RDEB,RFIN,EEE,ES2,SCUR,SDIR
      INTEGER          :: nlismat, ICMIN, ICMAX, ICTMIN, ICTMAX
      INTEGER, POINTER :: listmat(:)
      REAL(8), PARAMETER :: BIG=9999999.D0
      REAL(8), PARAMETER :: RMAMA=900.D0, RMAMO=150.D0, RMAMI=0.1D0
      REAL(8), PARAMETER :: TOLZ = 0.D0, TOLS=0.D0
      REAL(8), PARAMETER :: X0LALU=8.90D0, X0LCAR=18.80D0
      REAL(8), PARAMETER :: FACX0_CHAM=1.50D0, FACX0_DEAD=1.00D0
      REAL(8), PARAMETER :: ZnoEndcap=650.d0, ZnoBarrel=1400.d0
      INTEGER, SAVE :: IALUMI = 0, ICARBO = 0
      INTEGER, SAVE :: IPASS = 0
      INTEGER :: intpos
      REAL(8) :: PITOPI
      LOGICAL :: StopPerigee, SortiePerigee
      REAL(8) :: InvPiSect
!
!
      Lstop = .false.
      IPASS = IPASS + 1
!
!
!-- Until just before the end of this subroutine, do not touch
!-- to NBMASC in order not to disturb the tracking in VINVOU
      NBMUSC = 0
      NBMOSC = 0
      NBMESU = 0
      Navant = 0
!
!
      ISENS1 = 1
      ISENS2 = 2
      NSENS  = NSENSI
      IF( NSENS >= 100 ) THEN
        NFIX  = NGEOFIX
        NSENS = MOD( NSENS, 100 )
      ELSE
        NFIX  = 0
      ENDIF
      IF( NSENS >= 10 ) THEN  ! needed for tracks which do not cross calo volume
        StopPerigee = .true.
        NSENS = MOD( NSENS, 10 )
      ELSE
        StopPerigee = .false.
      ENDIF
      SortiePerigee = .false.
      IF( NSENS == 0 ) THEN ! called from perigee (from persint), so dot_product=0 and sign irrelevant
        NSENS  = 1
        IFLIP  = 0
        ISENS2 = 1
      ELSE IF( NSENS == 1 ) THEN
        IFLIP = 0
        IF( DOT_PRODUCT(VDEB(1:2,1),VDEB(4:5,1)) >= 0.D0 ) THEN
          ISENS2 = 1
        ELSE
          ISENS1 = 2
        ENDIF
      ELSEIF( NSENS == 2 ) THEN
        IF( DOT_PRODUCT(VDEB(1:2,1),VDEB(4:5,1)) >= 0.D0 ) THEN
          IFLIP = 2
        ELSE
          IFLIP = 1
        ENDIF
      ELSE
        PRINT 7770,NSENS
7770    FORMAT(/' Routine TRMUSC was called with NSENS =',I6,'  which is meaningless  =====>  STOP !')
        STOP
      ENDIF
!
!-- At first pass, find for energy losses, indexes of material
!-- coresponding to Aluminium and Carbon
      IF( IPASS <= 1 ) THEN
        DO I=1,MAMAMA
          IF( EAMAMA(I) > 0.01D0 ) THEN
            IF( ABS(X0MAMA(I)-X0LALU) < 0.1D0 ) IALUMI = I
            IF( ABS(X0MAMA(I)-X0LCAR) < 0.1D0 ) ICARBO = I
          ENDIF
        ENDDO
      ENDIF
!
!
!-- Find the 2 or 3 Phi sectors closest to the muon
      R2DEB = VDEB(1,1)**2 + VDEB(2,1)**2
      AZDEB = ABS( VDEB(3,1) )
      InvPiSect = 1.d0 / PiSect
      IF( R2DEB < RCYLI2(2) .AND. AZDEB < ZCYLIN(2) ) THEN
        R = 0.5D0 * RCYLIN(2)
        FIFI = ATAN2( VDEB(2,1)+R*VDEB(5,1) , VDEB(1,1)+R*VDEB(4,1) )
        IFI1 = PITOPI( FIFI - 0.99D0*PiSect ) * InvPiSect + 1.D0
        IFI2 = PITOPI( FIFI                 ) * InvPiSect + 1.D0
        IFI3 = PITOPI( FIFI + 0.99D0*PiSect ) * InvPiSect + 1.D0
      ELSE
        FIFI = ATAN2( VDEB(2,1) , VDEB(1,1) )
        IFI1 = PITOPI( FIFI - 0.75D0*PiSect ) * InvPiSect + 1.D0
        IFI2 = PITOPI( FIFI                 ) * InvPiSect + 1.D0
        IFI3 = PITOPI( FIFI + 0.75D0*PiSect ) * InvPiSect + 1.D0
      ENDIF
      IOKPHI(   0) = 1
      IOKPHI( 1:8) = 0
      IOKPHI(   9) = 1
      IOKPHI(IFI1) = 1
      IOKPHI(IFI2) = 1
      IOKPHI(IFI3) = 1
!
!
      RCHAMAP = RCHAMA + CP1000
      ZCHAMAP = ZCHAMA + CP1000
      RCHAMIP = RCHAMI - CP1000
      ZCHAMIP = ZCHAMI - CP1000
!
      IDMASC(1:NBMASC) = 0
      
      VEND = VDEB(:,1)
!
!-- Loop on the 2 (or 1, according to NSENS) direction(s) to perform tracking
!--  ISENS = 1  ===>  Away from the vertex  ===>  dE/dx < 0
!--  ISENS = 2  ===>  Toward the vertex     ===>  dE/dx > 0
      DO 310 ISENS=ISENS1,ISENS2
        VIN = VDEB(:,1)
        IF( ISENS == IFLIP ) THEN
          VIN(4) = - VIN(4)
          VIN(5) = - VIN(5)
          VIN(6) = - VIN(6)
          VIN(8) = - VIN(8)
        ENDIF
        R2IN = SQRT( VIN(1)**2 + VIN(2)**2 )
!
!-- Perform traking in one direction
        DFREE = 0.d0
        CALL setSignEloss(ISENS,VIN(4:6))
        SCUR = 0.d0
        SDIR = 1.d0
        IF( ISENS == 2 ) SDIR = -1.d0
        DO 300 IPAS=1,2000
          CALL VINVOU(VIN,VOUT,PAPAS,1,ISENS)
          IF( VOUT(7) < 0.d0 ) THEN
            IF( MBPri >= 1 )  WRITE(*,*) ' MuonboyCore/trmusc track has stopped '         &
                                        ,'with ISENS RCHAMI RCHAMA ',ISENS,RCHAMI,RCHAMA
            SortiePerigee = .true.
            IF( ISENS == 2 ) Lstop = .true.
            EXIT
          ENDIF
          R2OUT = SQRT( VOUT(1)**2 + VOUT(2)**2 )
          AZOUT = ABS( VOUT(3) )
          VEND(1:8) = VOUT(1:8)
!
!-- Compute intersection of track with each plane of each volume of matter
          XMINI = MIN( VIN(1) , VOUT(1) )
          YMINI = MIN( VIN(2) , VOUT(2) )
          ZMINI = MIN( VIN(3) , VOUT(3) )
          XMAXI = MAX( VIN(1) , VOUT(1) )
          YMAXI = MAX( VIN(2) , VOUT(2) )
          ZMAXI = MAX( VIN(3) , VOUT(3) )
          I = 1
          IF( XMAXI > 0.d0 ) I = I + 1
          IF( YMAXI > 0.d0 ) I = I + 2
          IF( ZMINI > Zmat1 ) THEN
            nlismat =  NVPOSMATI(I)
            listmat => IVPOSMATI(:,I)
          ELSE IF( ZMAXI < -Zmat1 ) THEN
            nlismat =  NVNEGMATI(I)
            listmat => IVNEGMATI(:,I)
          ELSE
            nlismat =  NVCENMATI(I)
            listmat => IVCENMATI(:,I)
          ENDIF
!!$          DO 130 IV=1,NVMATI
          DO 130 I=1,nlismat
            IV = listmat(I)
!!$!-code removed (muonboy-07-79-20) because of potential problem with meaning of IFI from XML matter
!!$            IFI = JMATI(IV)     / KMAJFF
!!$            IFI = JMATI(IV) - IFI*KMAJFF
!!$!-bug for XML matter - all XML matter put to IFI=1 - have to treat differently IFI=1
!!$            IF( IFI /= 1 ) then
!!$              IF( IOKPHI(IFI) < 1 ) then
!!$                CYCLE
!!$              endif
!!$            endif
            IF( XMINI > XMAMATI(IV) )  CYCLE
            IF( XMAXI < XMIMATI(IV) )  CYCLE
            IF( YMINI > YMAMATI(IV) )  CYCLE
            IF( YMAXI < YMIMATI(IV) )  CYCLE
            IF( ZMINI > ZMAMATI(IV) )  CYCLE
            IF( ZMAXI < ZMIMATI(IV) )  CYCLE
            DO IP=NPMAT0(IV),NPMAT1(IV)
              ZZIN  = DOT_PRODUCT( VIN(1:3),  VZMATI(1:3,IP) ) - VZMATI(4,IP)
              ZZOUT = DOT_PRODUCT( VOUT(1:3), VZMATI(1:3,IP) ) - VZMATI(4,IP)
              IF( ZZIN*ZZOUT > 0.D0 )                           CYCLE
              PAPABI = PAPAS * (-ZZIN)/(ZZOUT-ZZIN)
              IF( ABS(PAPABI) > 0.00001d0 ) THEN
                CALL ELEMHC(VIN,1,PAPABI, VOUTBI)
              ELSE
                VOUTBI(1:3) = VIN(1:3) + PAPABI*VIN(4:6)
                VOUTBI(4:8) = VIN(4:8)
              ENDIF
              R2OUTBI = SQRT( VOUTBI(1)**2 + VOUTBI(2)**2 )
              AZOUTBI = ABS( VOUTBI(3) )
              IF( R2OUTBI > RCHAMAP .OR.  AZOUTBI > ZCHAMAP )   CYCLE
              IF( R2OUTBI < RCHAMIP .AND. AZOUTBI < ZCHAMIP )   CYCLE
              XXBI = DOT_PRODUCT( VOUTBI(1:3), VXMATI(1:3,IP) ) - VXMATI(4,IP)
              YYBI = DOT_PRODUCT( VOUTBI(1:3), VYMATI(1:3,IP) ) - VYMATI(4,IP)
              IE0 = NEMAT0(IP)
              NBE = NEMAT1(IP) - IE0 + 1
              IF( INSIDE(XXBI,YYBI,NBE,XXMATI(IE0),YYMATI(IE0)) ) THEN
                NBMUSC = MIN( KMUSC , NBMUSC+1 )
                JVMUSC(NBMUSC) = JMATI(IV)
                SSMUSC(NBMUSC) = SCUR + SDIR*PAPABI
                RRMUSC(NBMUSC) = getRMUSC( VOUTBI )
              ENDIF
            ENDDO
130       ENDDO
!
!-- Skip the following if tracking region does not overlap the MDT/RPC/TGC region
          IF( RCHAMA < RCYLIN(3)+2.D0 ) then
            IF( ZCHAMA < ZCYLIN(3)+2.D0 ) GO TO 290
          endif
          DFREE = DFREE - PAPAS
          IF( DFREE > 0.d0 ) GO TO 290
          UFREE = BIG
          TOL = MAX( PAPAS, 10.d0 )
!
          ICMIN  = 1
          ICMAX  = NCOCSC
          ICTMIN = 1
          ICTMAX = NTRITO
          IF( ABS(ZMINI) < ZnoEndcap .AND. ABS(ZMAXI) < ZnoEndcap ) THEN
            ICMAX  = NCOUBA
            ICTMAX = NTRIBA
          ELSE IF( ABS(ZMINI) > ZnoBarrel .AND. ABS(ZMAXI) > ZnoBarrel ) THEN
            ICMIN  = NCOUBA + 1
            ICTMIN = NTRIBA + 1
          ENDIF
!
!-- Compute intersection of track with each MDT/CSC multilayer which gives a
!-- radius of multiple scattering and, by default, a radius of measurement
          DO 210 INOF=1,8
            IF( IOKPHI(INOF) < 1 )                                 CYCLE
            DO 200 IC=ICMIN,ICMAX
              DO INOZ=1,NZM
                IGEOM = IGEOMA(IC,INOF,INOZ)
                IF( IGEOM < 1 )                                    CYCLE
                TZ0 = TCECHAV(IGEOM)
                CF0 = COSCHAV(IGEOM)
                SF0 = SINCHAV(IGEOM)
                IF( IC <= NCOUBA ) THEN
                  TZIN  = CF0*VIN(1)  + SF0*VIN(2)
                  TZOUT = CF0*VOUT(1) + SF0*VOUT(2)
                ELSE IF( IC <= NCOUCH ) THEN
                  TZIN  = VIN(3)
                  TZOUT = VOUT(3)
                ELSE
                  AA = GCECHAV(IGEOM)
                  CAA = COS(AA)
                  SAA = SIN(AA)
                  TTIN  = CF0*VIN(1)  + SF0*VIN(2)
                  TTOUT = CF0*VOUT(1) + SF0*VOUT(2)
                  TZIN  = CAA*VIN(3)  + SAA*TTIN
                  TZOUT = CAA*VOUT(3) + SAA*TTOUT
                ENDIF
                IF( (TZOUT-TZIN)*(TZ0-TZOUT) >= 0.d0 ) UFREE = MIN( UFREE , ABS(TZ0-TZOUT) )                
                IF( (TZ0-TZIN)*(TZ0-TZOUT) >= 0.D0 )               CYCLE
                SSIN  = -SF0*VIN(1)  + CF0*VIN(2)
                SSOUT = -SF0*VOUT(1) + CF0*VOUT(2)
                IF( IC <= NCOUBA ) THEN
                  ZTIN  = VIN(3)
                  ZTOUT = VOUT(3)
                ELSE IF( IC <= NCOUCH ) THEN
                  ZTIN  = CF0*VIN(1)  + SF0*VIN(2)
                  ZTOUT = CF0*VOUT(1) + SF0*VOUT(2)
                ELSE
                  ZTIN  = - SAA*VIN(3)  + CAA*TTIN
                  ZTOUT = - SAA*VOUT(3) + CAA*TTOUT
                ENDIF
                IF(   NUKCHA(IC,INOZ,IGEOM,ZTIN ,SSIN ,TOL,TOL) ) THEN
                  IF( NUKCHA(IC,INOZ,IGEOM,ZTOUT,SSOUT,TOL,TOL) )  CYCLE
                ENDIF
                PAPABI = PAPAS * (TZ0-TZIN) / (TZOUT-TZIN)
                CALL ELEMHC(VIN,1,PAPABI, VOUTBI)
                IF( IC <= NCOUBA ) THEN
                  ZT0  = VOUTBI(3)
                  VTZ0 = CF0*VOUTBI(4) + SF0*VOUTBI(5)
                ELSE IF( IC <= NCOUCH ) THEN
                  ZT0  = CF0*VOUTBI(1) + SF0*VOUTBI(2)
                  VTZ0 = VOUTBI(6)
                ELSE
                  TT0  = CF0*VOUTBI(1) + SF0*VOUTBI(2)
                  ZT0  = - SAA*VOUTBI(3) + CAA*TT0
                  VTT0 = CF0*VOUTBI(4) + SF0*VOUTBI(5)
                  VTZ0 =   CAA*VOUTBI(6) + SAA*VTT0
                ENDIF
                IF( intpos( IGEOM, LGEOFIX, NFIX ) <= 0 ) THEN
                  SS0 = - SF0*VOUTBI(1) + CF0*VOUTBI(2)
                  Itrans = ItransCHA(IGEOM)
                  CALL Pos_Ideal_from_True( Itrans, SS0,ZT0,TZ0, Sidea,Zidea,Tidea )
                  IF(NAKCHA(IC,INOZ,IGEOM,Zidea,Sidea,Tidea,TOLZ,TOLS)) CYCLE
                ENDIF
                IMDT = ISTMDTV(IGEOM)
                NBMOSC = MIN( KMASC , NBMOSC+1 )
                RRMOSC(NBMOSC) = getRMUSC( VOUTBI )
                X0MOSC(NBMOSC) = FACX0_CHAM * RADTUB(IMDT) / ABS(VTZ0)
                JVMOSC(NBMOSC) = 0
!-- An MDT multilayer is essentially made of aluminium
                DL = X0MOSC(NBMOSC) * X0LALU
                IF( IALUMI > 0 ) THEN
                  ALMOSC(NBMOSC) = EXP( - EAMAMA(IALUMI) * DL* 1.D-6 )
                  BLMOSC(NBMOSC) = EBMAMA(IALUMI)/EAMAMA(IALUMI) * 1.D3 * (ALMOSC(NBMOSC) - 1.D0)
                ELSE
                  ALMOSC(NBMOSC) = 1.D0
                  BLMOSC(NBMOSC) = 0.D0
                ENDIF
                NBMESU = MIN( KMESU , NBMESU+1 )
                RRMESU(NBMESU) = RRMOSC(NBMOSC)
                DDMESU(NBMESU) = ACUTUB(IMDT)
                FFMESU(NBMESU) = FCECHAV(IGEOM)
                ICMESU(NBMESU) = IC
                EXIT  ! allow to cross only one station per value of IC
              ENDDO
200         ENDDO
210       ENDDO
!
!-- Compute intersection of track with each RPC/TGC chamber
!-- which gives a radius of multiple scattering
          DO ICT=ICTMIN,ICTMAX
!-25-09-07  CALL IDBTRI(ICT, JTYP,JFF,JZZ,JOB) ! trop long, remplace par calcul direct de JFF
            JFF = IBITS(JDBTRI(ICT),16,8)
            IF( IOKPHI(JFF) < 1 )                CYCLE
            TZ0 = TCETRI(ICT)
            CF0 = COSTRI(ICT)
            SF0 = SINTRI(ICT)
            IF( ICT <= NTRIBA ) THEN
              TZIN  = CF0*VIN(1)  + SF0*VIN(2)
              TZOUT = CF0*VOUT(1) + SF0*VOUT(2)
            ELSE
              TZIN  = VIN(3)
              TZOUT = VOUT(3)
            ENDIF
            IF( (TZOUT-TZIN)*(TZ0-TZOUT) >= 0.d0 ) UFREE = MIN( UFREE , ABS(TZ0-TZOUT) )
            IF( (TZ0-TZIN)*(TZ0-TZOUT) >= 0.D0 )       CYCLE
            SSIN  = -SF0*VIN(1)  + CF0*VIN(2)
            SSOUT = -SF0*VOUT(1) + CF0*VOUT(2)
            IF( ICT <= NTRIBA ) THEN
              ZTIN  = VIN(3)
              ZTOUT = VOUT(3)
            ELSE
              ZTIN  = CF0*VIN(1)  + SF0*VIN(2)
              ZTOUT = CF0*VOUT(1) + SF0*VOUT(2)
            ENDIF
            IF(   NUKTRI(ICT,ZTIN ,SSIN ,TOL,TOL) ) THEN
              IF( NUKTRI(ICT,ZTOUT,SSOUT,TOL,TOL) )  CYCLE
            ENDIF
            PAPABI = PAPAS * (TZ0-TZIN) / (TZOUT-TZIN)
            CALL ELEMHC(VIN,1,PAPABI, VOUTBI)
            IF(ICT <= NTRIBA) THEN
              ZT0  = VOUTBI(3)
              VTZ0 = CF0*VOUTBI(4) + SF0*VOUTBI(5)
            ELSE
              ZT0  = CF0*VOUTBI(1) + SF0*VOUTBI(2)
              VTZ0 = VOUTBI(6)
            ENDIF
            SS0 = - SF0*VOUTBI(1) + CF0*VOUTBI(2)
            IF( NOKTRI(ICT,ZT0,SS0,TOLZ,TOLS) )          CYCLE
            IRPC = IRPTGC(ICT)
            NBMOSC = MIN( KMASC , NBMOSC+1 )
            RRMOSC(NBMOSC) = getRMUSC( VOUTBI )
            X0MOSC(NBMOSC) = FACX0_CHAM * RABAND(IRPC) / ABS(VTZ0)
            JVMOSC(NBMOSC) = 0
!-- An RPC/TGC chamber is essentially made of carbon
            DL = X0MOSC(NBMOSC) * X0LCAR
            IF( ICARBO > 0 ) THEN
              ALMOSC(NBMOSC) = EXP( - EAMAMA(ICARBO) * DL* 1.D-6 )
              BLMOSC(NBMOSC) = EBMAMA(ICARBO)/EAMAMA(ICARBO) * 1.D3 * (ALMOSC(NBMOSC) - 1.D0)
            ELSE
              ALMOSC(NBMOSC) = 1.D0
              BLMOSC(NBMOSC) = 0.D0
            ENDIF
          ENDDO
!
          DFREE = UFREE
!
290       continue
          IF( R2OUT > RCHAMAP )    EXIT
          IF( AZOUT > ZCHAMAP )    EXIT
          IF( R2OUT < RCHAMIP ) THEN
            IF( AZOUT < ZCHAMIP )  EXIT
          ENDIF
          IF( StopPerigee ) THEN
            IF( ISENS == 2 ) THEN
              IF( R2OUT > R2IN ) THEN
                SortiePerigee = .true.
                EXIT
              ENDIF
            ENDIF
          ENDIF
          VIN(1:8) = VOUT(1:8)
          R2IN     = R2OUT
          SCUR     = SCUR + SDIR*PAPAS
300     ENDDO
!
310   ENDDO
!
!
!-- Re-order intersections with each "matter-plane" in increasing radius
      CALL CROISS(SSMUSC,NBMUSC,JVOPEN)
      CALL REORDO(SSMUSC,JVOPEN,NBMUSC)
      CALL REORDO(RRMUSC,JVOPEN,NBMUSC)
      CALL IEORDO(JVMUSC,JVOPEN,NBMUSC)
!
!-- Keep only matter for which there exists both entrance and exit
!--   There is an ambiguity because we do not know which are entrance and exit,
!--   which can give problems when one is missed. The reversed ordering in case of
!--   perigee is just a fix as usually in that case, if we miss something, it is the
!--   last exit because the perigee is inside the matter.
!
      JVOPEN(1:NBMUSC) = 0
      IF( SortiePerigee ) THEN
        DO IM1=NBMUSC,1,-1
          IF( JVOPEN(IM1) > 0 )              CYCLE
          DO IM2=IM1-1,1,-1
            IF( JVMUSC(IM2) /= JVMUSC(IM1) ) CYCLE
            JVOPEN(IM1) = 1
            JVOPEN(IM2) = 1
            EXIT
          ENDDO
        ENDDO
      ELSE
        DO IM1=1,NBMUSC
          IF( JVOPEN(IM1) > 0 )              CYCLE
          DO IM2=IM1+1,NBMUSC
            IF( JVMUSC(IM2) /= JVMUSC(IM1) ) CYCLE
            JVOPEN(IM1) = 1
            JVOPEN(IM2) = 1
            EXIT
          ENDDO
        ENDDO
      ENDIF
      N = 0
      DO IM1=1,NBMUSC
        IF( JVOPEN(IM1) < 1 ) THEN
          IF( MBPri >= 3 ) THEN
            IF( SortiePerigee ) THEN
              WRITE(*,500) '  ------>  '
            ELSE
              WRITE(*,500) ' WARNING : '
            ENDIF
          ENDIF
          CYCLE
        ENDIF
        N = N + 1
        JVMUSC(N) = JVMUSC(IM1)
        SSMUSC(N) = SSMUSC(IM1)
        RRMUSC(N) = RRMUSC(IM1)
      ENDDO
      NBMUSC = N
!
500   FORMAT(' Muonboy/trmusc',A11,'matter without entrance or exit')
!
!
!
!-- Compute center point of each volume of matter traversed
!-- and add it to the radii of multiple scattering
      NOPEN = 0
      DO 600 IMUSC=1,NBMUSC
!
        IF( NOPEN > 0 ) THEN
!-- One creates one (or two) radius(i) of m.s. and e.loss
          RDEB = RROPEN(NOPEN)
          RFIN = RRMUSC(IMUSC)
          EEE  = SSMUSC(IMUSC) - SSOPEN(NOPEN)
          IF( EEE >= RMAMI ) THEN
            IMANAT = JVOPEN(NOPEN) / (KMAJFF*KMAJZZ*KMANAM)
            IF( EEE <= RMAMO ) THEN
              NBMOSC = MIN( KMASC , NBMOSC+1 )
              RRMOSC(NBMOSC) = (RDEB+RFIN) *0.5d0
              X0MOSC(NBMOSC) = FACX0_DEAD * EEE / X0MAMA(IMANAT)
              JVMOSC(NBMOSC) = JVOPEN(NOPEN)
!-- A :  MeV/TeV/cm ---> GeV/GeV/cm  =========>   A  --->  A/1.D6
!-- B :    MeV/cm   --->   GeV/cm    =========>   B  --->  B/1.D3
!--                                  =========>  B/A ---> B/A*1.D3
              IF( EAMAMA(IMANAT) > 0.01D0 ) THEN
                ALMOSC(NBMOSC) = EXP( - EAMAMA(IMANAT)*EEE * 1.D-6 )
                BLMOSC(NBMOSC) = EBMAMA(IMANAT)/EAMAMA(IMANAT) * 1.D3 * (ALMOSC(NBMOSC) - 1.D0)
              ELSE
                ALMOSC(NBMOSC) = 1.D0
                BLMOSC(NBMOSC) = 0.D0
              ENDIF
            ELSEIF( EEE <= RMAMA ) THEN
              ES2 = 0.50D0 * EEE
              NBMOSO = MIN( KMASC , NBMOSC+1 )
              RRMOSC(NBMOSO) = 0.7887D0*RDEB + 0.2113D0*RFIN
              X0MOSC(NBMOSO) = FACX0_DEAD * ES2 / X0MAMA(IMANAT)
              JVMOSC(NBMOSO) = JVOPEN(NOPEN)
              IF( EAMAMA(IMANAT) > 0.01D0 ) THEN
                ALMOSC(NBMOSO) = EXP( - EAMAMA(IMANAT)*ES2 * 1.D-6 )
                BLMOSC(NBMOSO) = EBMAMA(IMANAT)/EAMAMA(IMANAT) * 1.D3 * (ALMOSC(NBMOSO) - 1.D0)
              ELSE
                ALMOSC(NBMOSO) = 1.D0
                BLMOSC(NBMOSO) = 0.D0
              ENDIF
              NBMOSC = MIN( KMASC , NBMOSO+1 )
              RRMOSC(NBMOSC) = 0.2113D0*RDEB + 0.7887D0*RFIN
              X0MOSC(NBMOSC) = X0MOSC(NBMOSO)
              JVMOSC(NBMOSC) = JVMOSC(NBMOSO)
              ALMOSC(NBMOSC) = ALMOSC(NBMOSO)
              BLMOSC(NBMOSC) = BLMOSC(NBMOSO)
            ENDIF
          ENDIF
        ENDIF
!
        N = 0
        DO IOPEN=1,NOPEN
          IF( JVMUSC(IMUSC) == JVOPEN(IOPEN) ) CYCLE
          N = N + 1
          JVOPEN(N) = JVOPEN(IOPEN)
          SSOPEN(N) = SSOPEN(IOPEN)
          RROPEN(N) = RROPEN(IOPEN)
        ENDDO
!
        IF( N == NOPEN ) THEN
          NOPEN = NOPEN + 1
          JVOPEN(NOPEN) = JVMUSC(IMUSC)
          SSOPEN(NOPEN) = SSMUSC(IMUSC)
          RROPEN(NOPEN) = RRMUSC(IMUSC)
        ELSE
          NOPEN = N
        ENDIF
!
600   ENDDO
!
!
!
!-- Now, replace all the centers of m.s. and e.loss
!-- (NBMOSC --> NBMASC) and, by default, apply in tracking
!-- energy losses but no multiple scattering (not yet)
      RRMASC(1:NBMOSC) = RRMOSC(1:NBMOSC)
      X0MASC(1:NBMOSC) = X0MOSC(1:NBMOSC)
      ALMASC(1:NBMOSC) = ALMOSC(1:NBMOSC)
      BLMASC(1:NBMOSC) = BLMOSC(1:NBMOSC)
      JVMASC(1:NBMOSC) = JVMOSC(1:NBMOSC)
      LLMASC(1:NBMOSC) = 1
      S1MASC(1:NBMOSC) = 0.D0
      S2MASC(1:NBMOSC) = 0.D0
      NBMASC = NBMOSC
!
 END SUBROUTINE TRMUSC
!
