!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetDisplacementFromAMDCJobN( TypChar1,Jff,Jzz,Job,                                 &
                                         TranslatS,TranslatZ,TranslatT, RotatS,RotatZ,RotatT )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz 
 REAL(8),      INTENT(OUT) :: TranslatS,TranslatZ,TranslatT
 REAL(8),      INTENT(OUT) :: RotatS,RotatZ,RotatT
 INTEGER :: Job
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
    Job = 0
    CALL GetDisplacementFromAMDC( TypChar1,Jff,Jzz,Job,                                 &
                                  TranslatS,TranslatZ,TranslatT, RotatS,RotatZ,RotatT )
!
 END SUBROUTINE GetDisplacementFromAMDCJobN
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetDisplacementFromAMDC( TypChar1,Jff,Jzz,Job,                                 &
                                     TranslatS,TranslatZ,TranslatT, RotatS,RotatZ,RotatT )
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 REAL(8),      INTENT(OUT) :: TranslatS,TranslatZ,TranslatT
 REAL(8),      INTENT(OUT) :: RotatS,RotatZ,RotatT
 CHARACTER(3) :: Car3
 INTEGER      :: Jtyp, J, Ideb, Ifin
 INTEGER      :: JTROU3, FindObject
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    TranslatS = 0.D0
    TranslatZ = 0.D0
    TranslatT = 0.D0
    RotatS    = 0.D0
    RotatZ    = 0.D0
    RotatT    = 0.D0
!
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp < 1 ) RETURN
!
    Ifin = LastAdjustByType(Jtyp)
    IF( Ifin <= 0 ) RETURN
    Ideb = FirstAdjustByType(Jtyp)
    J = FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, JTYPFZOLadjust)
    IF( J <= 0 ) RETURN
    TranslatS = SZTtraadjust(1,J)
    TranslatZ = SZTtraadjust(2,J)
    TranslatT = SZTtraadjust(3,J)
    RotatS    = SZTrotadjust(1,J)
    RotatZ    = SZTrotadjust(2,J)
    RotatT    = SZTrotadjust(3,J)
!
 END SUBROUTINE GetDisplacementFromAMDC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetDisplacementFromAMDCJadjust( Jadjust,Jtyp,Jff,Jzz,Jobj,                            &
                                            TranslatS,TranslatZ,TranslatT, RotatS,RotatZ,RotatT )
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jadjust
 INTEGER, INTENT(OUT) :: Jtyp,Jff,Jzz,Jobj
 REAL(8), INTENT(OUT) :: TranslatS,TranslatZ,TranslatT
 REAL(8), INTENT(OUT) :: RotatS,RotatZ,RotatT
 INTEGER :: Jlay
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
    Jtyp = 0
    Jff  = 0
    Jzz  = 0
    Jobj = 0
    TranslatS = 0.D0
    TranslatZ = 0.D0
    TranslatT = 0.D0
    RotatS    = 0.D0
    RotatZ    = 0.D0
    RotatT    = 0.D0
    if(Jadjust< 1 )        RETURN
    if(Jadjust> NBadjust ) RETURN
!
    CALL DECO_JTYP_F_Z_O_L(JTYPFZOLadjust(Jadjust), Jtyp,Jff,Jzz,Jobj,Jlay)
    TranslatS = SZTtraadjust(1,Jadjust)
    TranslatZ = SZTtraadjust(2,Jadjust)
    TranslatT = SZTtraadjust(3,Jadjust)
    RotatS    = SZTrotadjust(1,Jadjust)
    RotatZ    = SZTrotadjust(2,Jadjust)
    RotatT    = SZTrotadjust(3,Jadjust)
!
 END SUBROUTINE GetDisplacementFromAMDCJadjust
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetDeformationFromAMDCJobN(TypChar1,Jff,Jzz,                 &
                                       bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz
 REAL(8),      INTENT(OUT) :: bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en
 INTEGER :: Job
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
    Job = 0
    CALL GetDeformationFromAMDC(TypChar1,Jff,Jzz,Job,              &
                                bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en)
!      
 END SUBROUTINE GetDeformationFromAMDCJobN
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetDeformationFromAMDC(TypChar1,Jff,Jzz,Job,              &
                                   bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 REAL(8),      INTENT(OUT) :: bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en
 CHARACTER(3) :: Car3
 INTEGER :: Jtyp, J, Ideb,Ifin
 INTEGER :: JTROU3, FindObject
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
    bz = 0.d0
    bp = 0.d0
    bn = 0.d0
    sp = 0.d0
    sn = 0.d0
    tw = 0.d0
    pg = 0.d0
    tr = 0.d0
    eg = 0.d0
    ep = 0.d0
    en = 0.d0
!
    Car3(1:1)=TypChar1(1)
    Car3(2:2)=TypChar1(2)
    Car3(3:3)=TypChar1(3)
    Jtyp = JTROU3(Car3,MTYP,CARTYP)
    IF( Jtyp < 1 )  RETURN
!
    Ifin = LastDeformByType(Jtyp)
    IF( Ifin <= 0 ) RETURN
    Ideb = FirstDeformByType(Jtyp)
    J = FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, JTYPFZOLdeform)
    IF( J <= 0 ) RETURN
    bz = SZTdeform( 1,J)
    bp = SZTdeform( 2,J)
    bn = SZTdeform( 3,J)
    sp = SZTdeform( 4,J)
    sn = SZTdeform( 5,J)
    tw = SZTdeform( 6,J)
    pg = SZTdeform( 7,J)
    tr = SZTdeform( 8,J)
    eg = SZTdeform( 9,J)
    ep = SZTdeform(10,J)
    en = SZTdeform(11,J)
!
 END SUBROUTINE GetDeformationFromAMDC
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetDeformationFromAMDCJdeform( Jdeform,Jtyp,Jff,Jzz,Jobj,        &
                                           bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jdeform
 INTEGER, INTENT(OUT) :: Jtyp,Jff,Jzz,Jobj
 REAL(8), INTENT(OUT) :: bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en
 INTEGER :: Jlay
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    Jtyp = 0
    Jff  = 0
    Jzz  = 0
    Jobj = 0
    bz = 0.d0
    bp = 0.d0
    bn = 0.d0
    sp = 0.d0
    sn = 0.d0
    tw = 0.d0
    pg = 0.d0
    tr = 0.d0
    eg = 0.d0
    ep = 0.d0
    en = 0.d0
    if (Jdeform< 1 )        RETURN
    if (Jdeform> NBdeform ) RETURN
!
    CALL DECO_JTYP_F_Z_O_L(JTYPFZOLdeform(Jdeform), Jtyp,Jff,Jzz,Jobj,Jlay)
    bz = SZTdeform( 1,Jdeform)
    bp = SZTdeform( 2,Jdeform)
    bn = SZTdeform( 3,Jdeform)
    sp = SZTdeform( 4,Jdeform)
    sn = SZTdeform( 5,Jdeform)
    tw = SZTdeform( 6,Jdeform)
    pg = SZTdeform( 7,Jdeform)
    tr = SZTdeform( 8,Jdeform)
    eg = SZTdeform( 9,Jdeform)
    ep = SZTdeform(10,Jdeform)
    en = SZTdeform(11,Jdeform)
!
 END SUBROUTINE GetDeformationFromAMDCJdeform
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE GetInternalAlignmentFromAMDCJdwnlay( Jdwnlay,Jtyp,Jff,Jzz,Jobj,Jlay,                       &
                                                 TranslatS,TranslatZ,TranslatT, RotatS,RotatZ,RotatT )
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jdwnlay
 INTEGER, INTENT(OUT) :: Jtyp,Jff,Jzz,Jobj
 INTEGER, INTENT(OUT) :: Jlay
 REAL(8), INTENT(OUT) :: TranslatS,TranslatZ,TranslatT
 REAL(8), INTENT(OUT) :: RotatS,RotatZ,RotatT
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    Jtyp = 0
    Jff  = 0
    Jzz  = 0
    Jobj = 0
    Jlay = 0
    TranslatS = 0.D0
    TranslatZ = 0.D0
    TranslatT = 0.D0
    RotatS    = 0.D0
    RotatZ    = 0.D0
    RotatT    = 0.D0
    if(Jdwnlay< 1 )        RETURN
    if(Jdwnlay> NBdwnlay ) RETURN
!
    CALL DECO_JTYP_F_Z_O_L(JTYPFZOLdwnlay(Jdwnlay), Jtyp,Jff,Jzz,Jobj,Jlay)
    TranslatS = SZTtradwnlay(1,Jdwnlay)
    TranslatZ = SZTtradwnlay(2,Jdwnlay)
    TranslatT = SZTtradwnlay(3,Jdwnlay)
    RotatS    = SZTrotdwnlay(1,Jdwnlay)
    RotatZ    = SZTrotdwnlay(2,Jdwnlay)
    RotatT    = SZTrotdwnlay(3,Jdwnlay)
!
 END SUBROUTINE GetInternalAlignmentFromAMDCJdwnlay
!
