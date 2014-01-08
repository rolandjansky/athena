!
!> Perform deformation
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE getDeformation(Itra, S,Z,T, dS,dZ,dT)
 USE M_MB_TransGeo
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Itra
 REAL(8), INTENT(IN)  ::  S,  Z,  T
 REAL(8), INTENT(OUT) :: dS, dZ, dT
 REAL(8), SAVE :: chfb_bz, chfb_bp, chfb_bn
 REAL(8), SAVE :: chfb_sp, chfb_sn
 REAL(8), SAVE :: chfb_tw, chfb_pg, chfb_tr
 REAL(8), SAVE :: chfb_eg, chfb_ep, chfb_en
 REAL(8), SAVE :: S0A, S0B
 REAL(8), SAVE :: SLA, SLB
 REAL(8), SAVE :: Z0 , ZL
 REAL(8), SAVE :: T0 , TL
 INTEGER, SAVE :: ISBA
!--------------------------------------------------------------------
!
! Ideal_from_True : ZT -> ZT-dZT and dS = 0
! True_from_Ideal : ZT -> ZT+dZT and dS = 0
!
   IF( LDeform(Itra) ) THEN
!
     IF( Itra /= lastTrans ) THEN
       S0A  = TransGeoSS(1,Itra)
       S0B  = TransGeoSS(2,Itra)
       SLA  = TransGeoSS(3,Itra)
       SLB  = TransGeoSS(4,Itra)
       Z0   = TransGeoZZ(1,Itra)
       ZL   = TransGeoZZ(2,Itra)
       T0   = TransGeoTT(1,Itra)
       TL   = TransGeoTT(2,Itra)
       ISBA = TransGeoBA(Itra)  
!--  CHFB format
       chfb_bz = TransGeoDeform( 1,Itra)
       chfb_bp = TransGeoDeform( 2,Itra)
       chfb_bn = TransGeoDeform( 3,Itra)
       chfb_sp = TransGeoDeform( 4,Itra)
       chfb_sn = TransGeoDeform( 5,Itra)
       chfb_tw = TransGeoDeform( 6,Itra)
       chfb_pg = TransGeoDeform( 7,Itra)
       chfb_tr = TransGeoDeform( 8,Itra)
       chfb_eg = TransGeoDeform( 9,Itra)
       chfb_ep = TransGeoDeform(10,Itra)
       chfb_en = TransGeoDeform(11,Itra)
!--
       lastTrans = Itra
     ENDIF
!
     CALL getDeformationCHFB(chfb_bz, chfb_bp, chfb_bn, &
                             chfb_sp, chfb_sn,          &
                             chfb_tw, chfb_pg, chfb_tr, &
                             chfb_eg, chfb_ep, chfb_en, &
                             S0A    ,    S0B ,          &
                             SLA    ,    SLB ,          &
                             Z0     ,    ZL  ,          &
                             T0     ,    TL  ,          &
                             ISBA   ,                   &
                             S,Z,T, dS,dZ,dT)
!
!
   ELSE
     dS = 0.d0
     dZ = 0.d0
     dT = 0.d0
   ENDIF
!
 END SUBROUTINE getDeformation
!
