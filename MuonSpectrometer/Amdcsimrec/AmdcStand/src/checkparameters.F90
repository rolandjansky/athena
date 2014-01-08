!
!> \author SaMuSoG  27/06/00
!
 SUBROUTINE CheckParameters(MTYP_F_Z_i,MTYP_i,         &
                            MTYP_G_O_i,MGEO_i,MOBJ_i,  &
                            MCUT_i,MOCU_i,             &
                            MTEC_i,MSTA_i)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: MTYP_F_Z_i,MTYP_i
 INTEGER, INTENT(IN) :: MTYP_G_O_i,MGEO_i,MOBJ_i
 INTEGER, INTENT(IN) :: MCUT_i,MOCU_i
 INTEGER, INTENT(IN) :: MTEC_i,MSTA_i
!
#include "AmdcStand/amdcsim_com.inc"
!
    if (           MTYP_F_Z_i .NE. MTYP_F_Z) then
       print 1010, MTYP_F_Z_i  ,   MTYP_F_Z
       stop
    elseif (       MTYP_i .NE. MTYP    ) then
       print 1020, MTYP_i  ,   MTYP
       stop
    elseif (       MTYP_G_O_i .NE. MTYP_G_O) then
       print 1040, MTYP_G_O_i  ,   MTYP_G_O
       stop
    elseif (       MGEO_i .NE. MGEO    ) then
       print 1050, MGEO_i  ,   MGEO
       stop
    elseif (       MOBJ_i .NE. MOBJ    ) then
       print 1060, MOBJ_i  ,   MOBJ
       stop
    elseif (       MCUT_i .NE. MCUT    ) then
       print 1070, MCUT_i  ,   MCUT
       stop
    elseif (       MOCU_i .NE. MOCU    ) then
       print 1080, MOCU_i  ,   MOCU
       stop
    elseif (       MTEC_i .NE. MTEC    ) then
       print 1090, MTEC_i  ,   MTEC
       stop
    elseif (       MSTA_i .NE. MSTA    ) then
       print 1100, MSTA_i  ,   MSTA
       stop
    endif
!
1010  format('CheckParameters: Pb with MTYP_F_Z ',2I10)
1020  format('CheckParameters: Pb with MTYP     ',2I10)
1040  format('CheckParameters: Pb with MTYP_G_O ',2I10)
1050  format('CheckParameters: Pb with MGEO     ',2I10)
1060  format('CheckParameters: Pb with MOBJ     ',2I10)
1070  format('CheckParameters: Pb with MCUT     ',2I10)
1080  format('CheckParameters: Pb with MOCU     ',2I10)
1090  format('CheckParameters: Pb with MTEC     ',2I10)
1100  format('CheckParameters: Pb with MSTA     ',2I10)
!
 END SUBROUTINE CheckParameters
!
