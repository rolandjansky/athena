!
!> Convert TGC radial coordinates to cartesian coordinates
!> \author SaMuSoG
!
 SUBROUTINE TGCCV
 IMPLICIT NONE
 INTEGER :: jtyp, jgeo, job, ktyp_g_o, JGET_JTYP_G_O 
 REAL(8) :: rk, wk, ang
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
!-- Change the stations definitions
      do jtyp=1,mtyp
        if( cartyp(jtyp)(1:1) /= 'T' ) cycle
        do jgeo=1,mgeo
          if( nobj(jtyp,jgeo) <= 0 )   cycle
          do job=1,nobj(jtyp,jgeo)
            if( cartec(itec(jtyp,jgeo,job)) /= 'TGC' ) cycle
            if( ishape(jtyp,jgeo,job) /= 0 )           cycle
            rk =Geo_dy(jtyp,jgeo,job)
            wk =Geo_Le(jtyp,jgeo,job)
            ang=Geo_dx(jtyp,jgeo,job)
            ktyp_g_o = JGET_JTYP_G_O(jtyp,jgeo,job)
            ishape_V(ktyp_g_o)=3
            Geo_d2_V(ktyp_g_o)=ang
            Geo_dx_V(ktyp_g_o)=-sin(ang*pis180)*(rk+wk/2.)
            Geo_dy_V(ktyp_g_o)= cos(ang*pis180)*(rk+wk/2.)-wk/2.
          enddo
        enddo
      enddo
!
      PRINT 1000
1000  FORMAT(/' ===> During reading of AMDC_simrec, the TGC radial ',   &
              'coordinates have been converted '                        &
             /' ===> to cartesian coordinates in routine TGCCV')
 END SUBROUTINE TGCCV
!
