
      SUBROUTINE USEPROTOS
      IMPLICIT NONE

#include "Protos_i/Protos.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"

!     Standard PYTHIA commonblocks.

      INTEGER KCHG
      REAL*8 PMAS,PARF,VCKM
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)

!     Local

      INTEGER NJET,CHVB,CHL
      REAL*8 Pz,mf
      REAL*8 mtau,mb,mt,MW,MZ,MH,mL,MZp,MWp,mQ

!     Dummy

      INTEGER i0,i,j,id_f1,id_f2,id_VB,id_q,id_l,id_b1,id_b2,nn1
      INTEGER IDB1,IDB2,id_L1,id_L2,CHL1,CHL2
      INTEGER ID_HQ,id_Qt,num_Qt,id_b

!     ----------------------------------------------------

!     Clear arrays

      DO i=1,MAXNUP
        MOTHUP(1,i)=0
        MOTHUP(2,i)=0
        ICOLUP(1,i)=0
        ICOLUP(2,i)=0
        SPINUP(I)=9d0
        PUP(1,i)=0d0
        PUP(2,i)=0d0
        PUP(5,i)=0d0
        VTIMUP(i)=0d0
      ENDDO

      IDPRUP=LPRUP(NPRUP)

      mtau=PMAS(15,1)
      mb=PMAS(5,1)
      mt=PMAS(6,1)
      MW=PMAS(24,1)
      MZ=PMAS(23,1)
      MH=PMAS(25,1)
      mL=PMAS(17,1)
      MZp=PMAS(32,1)
      MWp=PMAS(34,1)
      mQ=PMAS(8,1)

!     ====================
!     Select process group
!     ====================

!     One top quark (plus jets, a photon or a Higgs)

      IF ( ((IHRD .GE. 3) .AND. (IHRD .LE. 5)) .OR. (IHRD .EQ. 12)
     &  .OR. (IHRD .EQ. 14) .OR. (IHRD .EQ. 16))
     &  GOTO 1003

!     One top quark and a W/Z boson (plus jets)

      IF ((IHRD .EQ. 6) .OR. (IHRD .EQ. 7) .OR. (IHRD .EQ. 10))
     &  GOTO 1006

!     A top quark pair decaying both to W/Z

      IF ((IHRD .EQ. 8) .OR. (IHRD .EQ. 9) .OR. (IHRD .EQ. 18))
     &   GOTO 1008

!     A top quark pair decaying to W/Z and a photon/gluon/Higgs

      IF ((IHRD .EQ. 11) .OR. (IHRD .EQ. 13) .OR. (IHRD .EQ. 15))
     &  GOTO 1011

!     A top quark pair decaying to W/Z and 3-body

      IF (IHRD .EQ. 17) GOTO 1017

!     Two heavy leptons

      IF ((IHRD .GE. 31) .AND. (IHRD .LE. 36))
     &  GOTO 1031

!     One heavy lepton

      IF ((IHRD .GE. 37) .AND. (IHRD .LE. 38))
     &  GOTO 1037

!     Two scalar triplets decaying into four leptons

      IF ((IHRD .GE. 41) .AND. (IHRD .LE. 43))
     &  GOTO 1041

!     T Tbar

      IF (IHRD .EQ. 51) GOTO 1051

!     B Bbar

      IF (IHRD .EQ. 52) GOTO 1052

!     X Xbar

      IF (IHRD .EQ. 53) GOTO 1053

!     Y Ybar

      IF (IHRD .EQ. 54) GOTO 1054

!     One T quark plus j, bj, t j

      IF ((IHRD .EQ. 61) .OR. (IHRD .EQ. 62) .OR. (IHRD .EQ. 67))
     &  GOTO 1061

!     One B quark plus j, bj

      IF ((IHRD .EQ. 63) .OR. (IHRD .EQ. 64)) GOTO 1063

!     One Y quark plus j, bj

      IF ((IHRD .EQ. 65) .OR. (IHRD .EQ. 66)) GOTO 1065

 !    Xtj

      IF (IHRD .EQ. 69) GOTO 1069

!     None of the above

      PRINT 10001,IHRD
      STOP

!     ------------------------------------------------------
!     Read tj (3), tbj (4), tb (5), At (12), t (14), Ht (16)
!     ------------------------------------------------------

      include 'Protos_i/tj.inc'

!     -----------------------------
!     Read tW (6), tWb (7), Zt (10)
!     -----------------------------

      include 'Protos_i/tW.inc'

!     --------------------
!     Read tt (8), Zqt (9)
!     --------------------

      include 'Protos_i/tt.inc'

!     ---------------------------------
!     Read Aqt (11), gqt (13), Hqt (15)
!     ---------------------------------

      include 'Protos_i/tfcn.inc'

!     --------------
!     Read llqt (17)
!     --------------

      include 'Protos_i/tt4f.inc'

!     ---------------------------------------------------------------------------
!     Read E+E- (31), EN (32), NN (33), Z'-> E+E- (34), W'-> EN (35), Z'-> NN(36)
!     ---------------------------------------------------------------------------

      include 'Protos_i/2HL.inc'

!     --------------------------
!     Read lN (37), W'-> lN (38)
!     --------------------------

      include 'Protos_i/1HL.inc'

!     --------------------------------------------------
!     Read D++ D-- (41), D++D- / D+ D-- (42), D+ D- (43)
!     --------------------------------------------------

      include 'Protos_i/DD4l.inc'

!     ------------
!     Read TT (51)
!     ------------

      include 'Protos_i/double_heavy_T.inc'

!     ------------
!     Read BB (52)
!     ------------

      include 'Protos_i/BB.inc'

!     ------------
!     Read XX (53)
!     ------------

      include 'Protos_i/XX.inc'

!     ------------
!     Read YY (54)
!     ------------

      include 'Protos_i/YY.inc'

!     --------------------------------
!     Read Tj (61), Tbj (62), Ttj (67)
!     --------------------------------

      include 'Protos_i/heavy_Tj.inc'

!     ----------------------
!     Read Bj (63), Bbj (64)
!     ----------------------

      include 'Protos_i/Bj.inc'

!     ----------------------
!     Read Yj (65), Ybj (66)
!     ----------------------

      include 'Protos_i/Yj.inc'

!     -------------
!     Read Xtj (69)
!     -------------

      include 'Protos_i/Xtj.inc'


10001 FORMAT ('Unknown process IHRD = ',I2)
10002 FORMAT ('File end: NOT Doing Rewinding... ',I2)

      END

