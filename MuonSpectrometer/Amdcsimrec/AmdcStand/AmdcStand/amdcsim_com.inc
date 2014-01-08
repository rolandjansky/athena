!
! AmdcStand/amdcsim_com.inc
!
! Author : SaMuSoG  14/02/97
!
#include "AmdcStand/amdcsim_param.inc"
!
      INTEGER       :: I_AMDC_VERSION,IVERSION_AMDC
      CHARACTER(16) :: C_AMDC_NAME
      COMMON/AMDCCOVERS/                                                   & 
     &         I_AMDC_VERSION,IVERSION_AMDC,C_AMDC_NAME
      CHARACTER(3)  ::  CARTYP,CARTEC
      COMMON/AMDCCHACAR/                                                   &
     &         CARTYP(MTYP),CARTEC(MTEC)
!----------------------------
      INTEGER, PARAMETER :: MTYP_F_Z=3000
      REAL(8) :: Pos_Phi_V,Pos_Z_V,Pos_R_V,Pos_S_V
      REAL(8) :: Pos_Alfa_V,Pos_Beta_V,Pos_Gama_V
      REAL(8) :: Pos_Phi,Pos_Z,Pos_R,Pos_S,Pos_Alfa,Pos_Beta,Pos_Gama
      INTEGER :: IGEO_V,ICUT_V,IBAREL_V,JTYP_F_Z,NB_JTYP_F_Z
      INTEGER :: ICSCCH_V
      INTEGER :: IGEO,ICUT,IBAREL
      INTEGER :: ICSCCH
      COMMON/AMDCCHAPOS/                                                   &
     &               Pos_Phi_V (0:MTYP_F_Z),Pos_Z_V   (0:MTYP_F_Z),        &
     &               Pos_R_V   (0:MTYP_F_Z),Pos_S_V   (0:MTYP_F_Z),        &
     &               Pos_Alfa_V(0:MTYP_F_Z),Pos_Beta_V(0:MTYP_F_Z),        &
     &               Pos_Gama_V(0:MTYP_F_Z),                               &
     &               IGEO_V    (0:MTYP_F_Z),ICUT_V    (0:MTYP_F_Z),        &
     &               IBAREL_V  (0:MTYP_F_Z),                               &
     &               ICSCCH_V  (0:MTYP_F_Z),                               &
     &               JTYP_F_Z(MTYP,8,-MZZ:MZZ),NB_JTYP_F_Z
!----------------------------
      INTEGER, PARAMETER :: MTYP_G_O=3000
      REAL(8) :: Geo_dx_V,Geo_dy_V,Geo_dz_V,Geo_Ws_V,Geo_Wl_V
      REAL(8) :: Geo_Le_V,Geo_Ex_V,Geo_D1_V,Geo_D2_V,Geo_D3_V
      REAL(8) :: Geo_dx,Geo_dy,Geo_dz,Geo_Ws,Geo_Wl,Geo_Le,Geo_Ex
      REAL(8) :: Geo_D1,Geo_D2,Geo_D3
      INTEGER ITEC_V,ISTA_V,ISPLIX_V,ISPLIY_V,ISHAPE_V,JTYP_G_O
      INTEGER NB_JTYP_G_O
      INTEGER NOBJ,ITEC,ISTA,ISPLIX,ISPLIY,ISHAPE
      COMMON/AMDCCHAGEO/                                                   & 
     &               Geo_dx_V(0:MTYP_G_O),Geo_dy_V(0:MTYP_G_O),            &
     &               Geo_dz_V(0:MTYP_G_O),                                 &
     &               Geo_Ws_V(0:MTYP_G_O),Geo_Wl_V(0:MTYP_G_O),            &
     &               Geo_Le_V(0:MTYP_G_O),Geo_Ex_V(0:MTYP_G_O),            &
     &               Geo_D1_V(0:MTYP_G_O),Geo_D2_V(0:MTYP_G_O),            &
     &               Geo_D3_V(0:MTYP_G_O),                                 &
     &               NOBJ(MTYP,MGEO),                                      &
     &               ITEC_V  (0:MTYP_G_O),ISTA_V  (0:MTYP_G_O),            &
     &               ISPLIX_V(0:MTYP_G_O),ISPLIY_V(0:MTYP_G_O),            &
     &               ISHAPE_V(0:MTYP_G_O),                                 &
     &               JTYP_G_O(MTYP,MGEO,MOBJ),NB_JTYP_G_O
!----------------------------
      INTEGER, PARAMETER :: MCUT=400, MOCU=10
      REAL(8) :: Cut_dx,Cut_dy,Cut_Ws,Cut_Wl,Cut_Le,Cut_Ex,Cut_An
      INTEGER :: INOCUT,NOBCUT,IOBCUT
      COMMON/AMDCCHACUT/                                                   &
     &               Cut_dx(MCUT,MOCU),Cut_dy(MCUT,MOCU),                  &
     &               Cut_Ws(MCUT,MOCU),Cut_Wl(MCUT,MOCU),                  &
     &               Cut_Le(MCUT,MOCU),                                    &
     &               Cut_Ex(MCUT,MOCU),Cut_An(MCUT,MOCU),                  &
     &               INOCUT(MTYP,MGEO,4),                                  &
     &               NOBCUT(MCUT),     IOBCUT(MCUT,MOCU)
!----------------------------
      INTEGER, PARAMETER :: MSTA=25
      REAL(8) :: STAX0,STAPP,STARR,STAEE,STAPG,STATT,STAOO
      INTEGER :: NLAZ,NLAS,ISTAMA,IFORMA,IVALIDSTA
      COMMON/AMDCCHASTA/                                                   &
     &               STAX0(MTEC,MSTA),                                     &
     &               STAPP(MTEC,MSTA),   STARR(MTEC,MSTA),                 &
     &               STAEE(MTEC,MSTA),   STAPG(MTEC,MSTA),                 &
     &               STATT(MTEC,MSTA,20),STAOO(MTEC,MSTA,20),              &
     &              ISTAMA(MTEC,MSTA,20),                                  &
     &                NLAZ(MTEC,MSTA),    NLAS(MTEC,MSTA),                 &
     &              IFORMA(MTEC,MSTA),                                     &
     &              IVALIDSTA(MTEC,MSTA)
!----------------------------
      INTEGER, PARAMETER :: Mgang=130, Mstrp=34
      REAL(8) :: XtgcAdd,XtgcStrp
      INTEGER :: NtgcAdd,NtgcGang
      COMMON/AMDCCHASTATGC/                                                &
     &                  XtgcAdd(MSTA,16),XtgcStrp(MSTA,Mstrp,2),           &
     &                  NtgcAdd(MSTA,10),NtgcGang(MSTA,Mgang,3)
!----------------------------
      INTEGER, PARAMETER :: Madjust=3000
      REAL(8) :: SZTtraadjust,SZTrotadjust
      INTEGER :: NBadjust,JTYPFZOLadjust
      INTEGER :: FirstAdjustByType,LastAdjustByType
      COMMON/AMDCCHADJUST/                                                 &
     &                 SZTtraadjust(3,Madjust),SZTrotadjust(3,Madjust),    &
     &                 JTYPFZOLadjust(Madjust),                            &
     &                 FirstAdjustByType(MTYP),LastAdjustByType(MTYP),     &
     &                 NBadjust
!----------------------------
      INTEGER, PARAMETER :: Mdwnlay=3000
      REAL(8) :: SZTtradwnlayShifted,SZTtradwnlayCentred
      REAL(8) :: SZTtradwnlay,SZTrotdwnlay
      INTEGER :: NBdwnlay,JTYPFZOLdwnlay
      COMMON/AMDCCHdwnlay/                                                 &
     &                 SZTtradwnlayCentred(3,Mdwnlay),                     &
     &                 SZTtradwnlayShifted(3,Mdwnlay),                     &
     &                 SZTtradwnlay(3,Mdwnlay),SZTrotdwnlay(3,Mdwnlay),    &
     &                 JTYPFZOLdwnlay(Mdwnlay),NBdwnlay
!----------------------------
      INTEGER, PARAMETER :: Mdeform=3000
      REAL(8) :: SZTdeform
      REAL(8) :: SZTDeformSS,SZTDeformZZ,SZTDeformTT
      INTEGER :: NBdeform,JTYPFZOLdeform
      INTEGER :: FirstDeformByType,LastDeformByType
      INTEGER :: DeformSSZZTTDone 
      COMMON/AMDCCHADEFOR/                                                 &
     &                 SZTdeform(11,Mdeform) ,                             &
     &                 SZTDeformSS(4,Mdeform),                             &
     &                 SZTDeformZZ(4,Mdeform),                             &
     &                 SZTDeformTT(4,Mdeform),                             &
     &                 JTYPFZOLdeform(Mdeform),                            &
     &                 FirstDeformByType(MTYP),LastDeformByType(MTYP),     &
     &                 NBdeform,DeformSSZZTTDone
!----------------------------
      INTEGER, PARAMETER :: MdeadChan=4000
      INTEGER :: NBdisaChan,NBdeadChan
      INTEGER :: JTYPFZOLdeadChan,JCHANdeadChan
      COMMON/AMDCCHADEADCHA/                                               &
     &                   JTYPFZOLdeadChan(MdeadChan),                      &
     &                   JCHANdeadChan(5,MdeadChan),                       &
     &                   NBdisaChan,NBdeadChan
!----------------------------
      INTEGER, PARAMETER :: MaxAMDC=800000
      INTEGER      :: LenAMDC
      CHARACTER(1) :: CarAMDC
      COMMON/AMDCCHAAMDC/                                                  &
     &                   LenAMDC,CarAMDC(MaxAMDC)
!----------------------------
      INTEGER, PARAMETER :: MaxAGDD=780000
      INTEGER      :: LenAGDD
      CHARACTER(1) :: CarAGDD
      COMMON/AMDCCHAAGDD/                                                  &
     &                   LenAGDD,CarAGDD(MaxAGDD)
!----------------------------
      INTEGER      :: IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG
      Character(7) :: HARDNAME
      Character(3) :: SOFTNAME    
      Integer      :: SOFTOCTANT 
      Integer      :: SOFTIZ 
      Integer      :: HARDENTRIES 
      Integer      :: IentryHardSoft  
      Integer      :: IentryHardSoft_V 
      COMMON/AMDCHARDSOFT/                                                 &
     & HARDNAME   (MTYP_F_Z),                                              &
     & SOFTNAME   (MTYP_F_Z),                                              &
     & SOFTOCTANT (MTYP_F_Z),                                              &
     & SOFTIZ     (MTYP_F_Z),                                              &
     & IentryHardSoft_V(0:MTYP_F_Z),                                       &
     & HARDENTRIES
!----------------------------
      INTEGER, PARAMETER :: XTOMOMAX=3000
      Character(7) :: XtomoChberName   
      Character(3) :: XtomoSite      
      Integer      :: XtomoSiteId    
      Integer      :: XtomoTime      
      Integer      :: XtomoPassed    
      Character(1) :: XtomoSide      
      Integer      :: XtomoNberTube1 
      Integer      :: XtomoNberTube2 
      Integer      :: XtomoNberML    
      Integer      :: XtomoNberLayer 
      REAL(8)      :: XtomoML1NYtub 
      REAL(8)      :: XtomoML1NZtub 
      REAL(8)      :: XtomoML1NDely 
      REAL(8)      :: XtomoML1NDelz    
      REAL(8)      :: XtomoML1NDela    
      REAL(8)      :: XtomoML1NYpit    
      REAL(8)      :: XtomoML1NZpit 
      REAL(8)      :: XtomoML1PYtub 
      REAL(8)      :: XtomoML1PZtub 
      REAL(8)      :: XtomoML1PDely    
      REAL(8)      :: XtomoML1PDelz    
      REAL(8)      :: XtomoML1PDela    
      REAL(8)      :: XtomoML1PYpit 
      REAL(8)      :: XtomoML1PZpit 
      REAL(8)      :: XtomoML2NYtub 
      REAL(8)      :: XtomoML2NZtub    
      REAL(8)      :: XtomoML2NDely    
      REAL(8)      :: XtomoML2NDelz    
      REAL(8)      :: XtomoML2NDela 
      REAL(8)      :: XtomoML2NYpit 
      REAL(8)      :: XtomoML2NZpit 
      REAL(8)      :: XtomoML2PYtub    
      REAL(8)      :: XtomoML2PZtub    
      REAL(8)      :: XtomoML2PDely 
      REAL(8)      :: XtomoML2PDelz 
      REAL(8)      :: XtomoML2PDela 
      REAL(8)      :: XtomoML2PYpit 
      REAL(8)      :: XtomoML2PZpit 
      Integer      :: IentryXtomo 
      Integer      :: IentryXtomo_V 
      Integer      :: XTOMOENTRIES 
      COMMON/AMDCXTOMO/                                                    &
     & XtomoChberName (XTOMOMAX) ,                                         &
     & XtomoSite      (XTOMOMAX) ,                                         &
     & XtomoSiteId    (XTOMOMAX) ,                                         &
     & XtomoTime      (XTOMOMAX) ,                                         &
     & XtomoPassed    (XTOMOMAX) ,                                         &
     & XtomoSide      (XTOMOMAX) ,                                         &
     & XtomoNberTube1 (XTOMOMAX) ,                                         &
     & XtomoNberTube2 (XTOMOMAX) ,                                         &
     & XtomoNberML    (XTOMOMAX) ,                                         &
     & XtomoNberLayer (XTOMOMAX) ,                                         &
     & XtomoML1NYtub (XTOMOMAX) ,                                         &
     & XtomoML1NZtub (XTOMOMAX) ,                                         &
     & XtomoML1NDely (XTOMOMAX) ,                                         &
     & XtomoML1NDelz (XTOMOMAX) ,                                         & 
     & XtomoML1NDela (XTOMOMAX) ,                                         & 
     & XtomoML1NYpit (XTOMOMAX) ,                                         & 
     & XtomoML1NZpit (XTOMOMAX) ,                                         & 
     & XtomoML1PYtub (XTOMOMAX) ,                                         &
     & XtomoML1PZtub (XTOMOMAX) ,                                         &
     & XtomoML1PDely (XTOMOMAX) ,                                         &
     & XtomoML1PDelz (XTOMOMAX) ,                                         & 
     & XtomoML1PDela (XTOMOMAX) ,                                         & 
     & XtomoML1PYpit (XTOMOMAX) ,                                         & 
     & XtomoML1PZpit (XTOMOMAX) ,                                         & 
     & XtomoML2NYtub (XTOMOMAX) ,                                         &
     & XtomoML2NZtub (XTOMOMAX) ,                                         &
     & XtomoML2NDely (XTOMOMAX) ,                                         &
     & XtomoML2NDelz (XTOMOMAX) ,                                         & 
     & XtomoML2NDela (XTOMOMAX) ,                                         & 
     & XtomoML2NYpit (XTOMOMAX) ,                                         & 
     & XtomoML2NZpit (XTOMOMAX) ,                                         & 
     & XtomoML2PYtub (XTOMOMAX) ,                                         &
     & XtomoML2PZtub (XTOMOMAX) ,                                         &
     & XtomoML2PDely (XTOMOMAX) ,                                         &
     & XtomoML2PDelz (XTOMOMAX) ,                                         & 
     & XtomoML2PDela (XTOMOMAX) ,                                         & 
     & XtomoML2PYpit (XTOMOMAX) ,                                         & 
     & XtomoML2PZpit (XTOMOMAX) ,                                         &
     & IentryXtomo_V  (XTOMOMAX) ,                                         &
     & XTOMOENTRIES
!
