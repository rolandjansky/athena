!
!> \author SamuSog
!
!--------------------------------------------------------------------
!> Transfert deviations from amdb nominal position (A line)
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert deviations from amdb nominal position (A line)
! for a given chamber to muonboy
! fill    JTYPFZOLadjust with STANAME, JFF, JZZ, JOB
!         SZTtraadjust        A
!         SZTrotadjust        
! ier     <> 0  if too many lines (max = Madjust)
!--------------------------------------------------------------------
 SUBROUTINE stasetalines (STANAME, JFF, JZZ, JOB, A, ier)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: STANAME(3)
 INTEGER     , INTENT(IN)  :: JFF, JZZ, JOB
 REAL(8)     , INTENT(IN)  :: A(6)
 INTEGER     , INTENT(OUT) :: ier
 CHARACTER(3) :: CARIN
 INTEGER      :: JTYP, JTROU3
!
#include "AmdcStand/amdcsim_com.inc"
!
      CARIN(1:1)=STANAME(1)
      CARIN(2:2)=STANAME(2)
      CARIN(3:3)=STANAME(3)
      JTYP = JTROU3(CARIN,MTYP,CARTYP)
!
      CALL setalines(JTYP, JFF, JZZ, JOB, A,ier)
!
 END SUBROUTINE stasetalines
!
!--------------------------------------------------------------------
!> Transfert deviations from amdb nominal position (A line)
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert deviations from amdb nominal position (A line)
! for a given chamber to muonboy
! fill    JTYPFZOLadjust with JTYP, JFF, JZZ, JOB
!         SZTtraadjust        A
!         SZTrotadjust        
! ier     <> 0  if too many lines (max = Madjust)
!--------------------------------------------------------------------
 SUBROUTINE setalines (JTYP, JFF, JZZ, JOB, A, ier)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: JTYP, JFF, JZZ, JOB
 REAL(8), INTENT(IN)  :: A(6)
 INTEGER, INTENT(OUT) :: ier
 INTEGER :: Icode
!
#include "AmdcStand/amdcsim_com.inc"
!
      ier = 0
!--- reset counters to 0
      IF( JTYP == 0 .and. JFF == 0 .and. JZZ == 0 .and. JOB == 0 ) THEN
        NBadjust = 0
        LastAdjustByType(:) = 0
        RETURN
      ENDIF
!---
      NBadjust = NBadjust + 1
      IF( NBadjust > Madjust ) THEN
        ier = 1
      ELSE
        CALL CODE_JTYP_F_Z_O_L(JTYP,JFF,JZZ,JOB,0, Icode)
        JTYPFZOLadjust(NBadjust) = Icode
        SZTtraadjust(1:3,NBadjust) = A(1:3)
        SZTrotadjust(1:3,NBadjust) = A(4:6)
        IF( LastAdjustByType(JTYP) == 0 ) FirstAdjustByType(JTYP) = NBadjust
        LastAdjustByType(JTYP) = NBadjust
      ENDIF
!
 END SUBROUTINE setalines
!
!--------------------------------------------------------------------
!> Transfert deformations (B line) for a given chamber to muonboy
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert deformations (B line) for a given chamber to muonboy
! fill    JTYPFZOLdeform with STANAME, JFF, JZZ, JOB
!         SZTdeform           B
! ier     <> 0  if too many lines (max = Mdeform)
!--------------------------------------------------------------------
 SUBROUTINE stasetblines (STANAME, JFF, JZZ, JOB, B, ier)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: STANAME(3)
 INTEGER     , INTENT(IN)  :: JFF, JZZ, JOB
 REAL(8)     , INTENT(IN)  :: B(11)
 INTEGER     , INTENT(OUT) :: ier
 CHARACTER(3) :: CARIN
 INTEGER      :: JTYP, JTROU3
!
#include "AmdcStand/amdcsim_com.inc"
!
      CARIN(1:1)=STANAME(1)
      CARIN(2:2)=STANAME(2)
      CARIN(3:3)=STANAME(3)
      JTYP = JTROU3(CARIN,MTYP,CARTYP)
!
      CALL setblines(JTYP, JFF, JZZ, JOB, B,ier)
!
 END SUBROUTINE stasetblines
!
!--------------------------------------------------------------------
!> Transfert deformations (B line) for a given chamber to muonboy
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert deformations (B line) for a given chamber to muonboy
! fill    JTYPFZOLdeform with JTYP, JFF, JZZ, JOB
!         SZTdeform           B
! ier     <> 0  if too many lines (max = Mdeform)
!--------------------------------------------------------------------
 SUBROUTINE setblines(JTYP, JFF, JZZ, JOB, B, ier)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: JTYP, JFF, JZZ, JOB
 REAL(8), INTENT(IN)  :: B(11)
 INTEGER, INTENT(OUT) :: ier
 INTEGER :: Icode
!
#include "AmdcStand/amdcsim_com.inc"
!
      ier = 0
!--- reset counters to 0
      if( JTYP == 0 .and. JFF == 0 .and. JZZ == 0 .and. JOB == 0 ) then
        NBdeform = 0
        DeformSSZZTTDone = 0
        LastDeformByType(:) = 0
        return
      endif
!---
      NBdeform = NBdeform + 1
      IF( NBdeform > Mdeform ) THEN
        ier = 1
      ELSE
        CALL CODE_JTYP_F_Z_O_L(JTYP,JFF,JZZ,JOB,0, Icode)
        JTYPFZOLdeform(NBdeform) = Icode
        SZTdeform(1:11,NBdeform) = B(1:11)
        IF( LastDeformByType(JTYP) == 0 ) FirstDeformByType(JTYP) = NBdeform
        LastDeformByType(JTYP) = NBdeform
      ENDIF
!
 END SUBROUTINE setblines
!
!--------------------------------------------------------------------
!> Transfert deviations from amdb nominal position unit down to jlay
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert deviations from amdb nominal position unit down to jlay
! for a given chamber to muonboy
! fill    JTYPFZOLdwnlay with JTYP, JFF, JZZ, JOB, JLAY
!         SZTtradwnlay         
!         SZTrotdwnlay        
! ier     <> 0  if too many lines (max = Mdwnlay)
!--------------------------------------------------------------------
 SUBROUTINE stasetilines(STANAME, JFF, JZZ, JOB, JLAY, A, ier)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: STANAME(3)
 INTEGER     , INTENT(IN)  :: JFF, JZZ, JOB, JLAY
 REAL(8)     , INTENT(IN)  :: A(6)
 INTEGER     , INTENT(OUT) :: ier
 CHARACTER(3) :: CARIN
 INTEGER      :: JTYP, JTROU3
!
#include "AmdcStand/amdcsim_com.inc"
!
      CARIN(1:1)=STANAME(1)
      CARIN(2:2)=STANAME(2)
      CARIN(3:3)=STANAME(3)
      JTYP = JTROU3(CARIN,MTYP,CARTYP)
!
      CALL setilines(JTYP, JFF, JZZ, JOB, JLAY, A,ier)
!
 END SUBROUTINE stasetilines
!
!--------------------------------------------------------------------
!> Transfert deviations from amdb nominal position unit down to jlay
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert deviations from amdb nominal position unit down to jlay
! for a given chamber to muonboy
! fill    JTYPFZOLdwnlay with JTYP, JFF, JZZ, JOB, JLAY
!         SZTtradwnlay         
!         SZTrotdwnlay        
! ier     <> 0  if too many lines (max = Mdwnlay)
!--------------------------------------------------------------------
 SUBROUTINE setilines(JTYP, JFF, JZZ, JOB, JLAY, A,ier)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: JTYP, JFF, JZZ, JOB, JLAY
 REAL(8), INTENT(IN)  :: A(6)
 INTEGER, INTENT(OUT) :: ier
 INTEGER :: Icode
!
#include "AmdcStand/amdcsim_com.inc"
!
      ier = 0
!--- reset counters to 0
      IF( JTYP == 0 .and. JFF == 0 .and. JZZ == 0 .and. JOB == 0 .and. JLAY == 0 ) THEN
        NBdwnlay = 0
        RETURN
      ENDIF
!---
      NBdwnlay = NBdwnlay + 1
      IF( NBdwnlay > Mdwnlay ) THEN
        ier = 1
      ELSE
        CALL CODE_JTYP_F_Z_O_L(JTYP,JFF,JZZ,JOB,JLAY, Icode)
        JTYPFZOLdwnlay(NBdwnlay) = Icode
        SZTtradwnlay(1:3,NBdwnlay) = A(1:3)
        SZTrotdwnlay(1:3,NBdwnlay) = A(4:6)
      ENDIF
!
 END SUBROUTINE setilines
!
