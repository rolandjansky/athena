
      SUBROUTINE USEPROTOS
      IMPLICIT NONE

#include "ProtosLHEF_i/Protos.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"

!     Local

      INTEGER NJET,CHVB,CHL

!     Dummy

      INTEGER i0,j,id_f1,id_f2,id_VB,id_q,id_l,id_b1,id_b2,nn1
      INTEGER IDB1,IDB2,id_L1,id_L2,CHL1,CHL2
      INTEGER ID_HQ,id_Qt,num_Qt,id_b

C ------------------------------------------------------------------

!     Clear arrays

      NUP=0
      IDPRUP=0
      XWGTUP=0
      SCALUP=0
      AQEDUP=0
      AQCDUP=0
      DO i=1,MAXNUP
        IDUP(i)=0
        ISTUP(i)=0
        MOTHUP(1,i)=0
        MOTHUP(2,i)=0
        ICOLUP(1,i)=0
        ICOLUP(2,i)=0
        SPINUP(I)=9d0
        PUP(1,i)=0d0    
        PUP(2,i)=0d0
        PUP(3,i)=0d0
        PUP(4,i)=0d0
        PUP(5,i)=0d0
        VTIMUP(i)=0d0
      END DO

C -------------------------------------------------------------------

      IDPRUP=LPRUP(NPRUP)
      AQEDUP=-1
      AQCDUP=-1

C -------------------------------------------------------------------
C -------------------------------------------------------------------

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


C -------------------------------------------------------------------------
C -------------------------------------------------------------------------

!     ------------------------------------------------------
!     Read tj (3), tbj (4), tb (5), At (12), t (14), Ht (16)
!     ------------------------------------------------------

      include 'ProtosLHEF_i/tj.inc'

!     -----------------------------
!     Read tW (6), tWb (7), Zt (10)
!     -----------------------------

      include 'ProtosLHEF_i/tW.inc'

!     --------------------
!     Read tt (8), Zqt (9)
!     --------------------

      include 'ProtosLHEF_i/tt.inc'

!     ---------------------------------
!     Read Aqt (11), gqt (13), Hqt (15)
!     ---------------------------------

      include 'ProtosLHEF_i/tfcn.inc'

!     --------------
!     Read llqt (17)
!     --------------

      include 'ProtosLHEF_i/tt4f.inc'

!     ---------------------------------------------------------------------------
!     Read E+E- (31), EN (32), NN (33), Z'-> E+E- (34), W'-> EN (35), Z'-> NN(36)  
!     ---------------------------------------------------------------------------

      include 'ProtosLHEF_i/2HL.inc'

!     --------------------------
!     Read lN (37), W'-> lN (38)
!     --------------------------

      include 'ProtosLHEF_i/1HL.inc'

!     --------------------------------------------------
!     Read D++ D-- (41), D++D- / D+ D-- (42), D+ D- (43)
!     --------------------------------------------------

      include 'ProtosLHEF_i/DD4l.inc'

!     ------------
!     Read TT (51)
!     ------------

      include 'ProtosLHEF_i/double_heavy_T.inc'

!     ------------
!     Read BB (52)
!     ------------

      include 'ProtosLHEF_i/BB.inc'

!     ------------
!     Read XX (53)
!     ------------

      include 'ProtosLHEF_i/XX.inc'

!     ------------
!     Read YY (54)
!     ------------

      include 'ProtosLHEF_i/YY.inc'

!     --------------------------------
!     Read Tj (61), Tbj (62), Ttj (67)
!     --------------------------------

      include 'ProtosLHEF_i/heavy_Tj.inc'

!     ----------------------
!     Read Bj (63), Bbj (64)
!     ----------------------

      include 'ProtosLHEF_i/Bj.inc'

!     ----------------------
!     Read Yj (65), Ybj (66)
!     ----------------------

      include 'ProtosLHEF_i/Yj.inc'

!     -------------
!     Read Xtj (69)
!     -------------

      include 'ProtosLHEF_i/Xtj.inc'


10001 FORMAT ('Unknown process IHRD = ',I2)
10002 FORMAT ('File end: NOT Doing Rewinding... ',I2)

      END

