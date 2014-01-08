!
! AmdcStand/coindextec.inc
!
! Author : SaMuSoG  20/06/02
!
!
!- IndexXXX(...) = iii  (i.e. 111 or 112 or 121 or 232 or ...)
!-                 |||
!-                 |||     rank order, in the station (Jtyp_geo),
!-                 |||     of the object (Jtyp_geo_obj) of technology XXX
!-                 ||+---> along T (or R)
!-                 |+----> along Z
!-                 +-----> along S (or Phi)
!
      INTEGER :: IndexMDT,IndexRPC,IndexTGC,IndexCSC
      COMMON/AMDCCOindexTEC/                                               &
     &                   IndexMDT(0:MTYP_G_O), IndexRPC(0:MTYP_G_O),       &
     &                   IndexTGC(0:MTYP_F_Z), IndexCSC(0:MTYP_G_O)
!
