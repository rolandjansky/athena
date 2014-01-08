!
! AmdcStand/amdcsim_stfunc.inc
!
! Author : M.Virchaux  14/02/97
!
!- Statement functions for CHAPOS
      Pos_Phi   (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &Pos_Phi_V (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Pos_Z     (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &Pos_Z_V   (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Pos_R     (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &Pos_R_V   (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Pos_S     (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &Pos_S_V   (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Pos_Alfa  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &Pos_Alfa_V(JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Pos_Beta  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &Pos_Beta_V(JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Pos_Gama  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &Pos_Gama_V(JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      IGEO      (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &IGEO_V    (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      ICUT      (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &ICUT_V    (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      IBAREL    (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &IBAREL_V  (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      ICSCCH    (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &ICSCCH_V  (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
!
!- Statement functions for CHAGEO
      Geo_dx  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_dx_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_dy  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_dy_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_dz  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_dz_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_Ws  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_Ws_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_Wl  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_Wl_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_Le  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_Le_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_Ex  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_Ex_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_D1  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_D1_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_D2  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_D2_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      Geo_D3  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &Geo_D3_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      ITEC    (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &ITEC_V  (JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      ISTA    (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &ISTA_V  (JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      ISPLIX  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &ISPLIX_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      ISPLIY  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &ISPLIY_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
      ISHAPE  (         IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =         &
     &ISHAPE_V(JTYP_G_O(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
!
!- Statement functions for HARDSOFT
      IentryHardSoft     (IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =       &
     &IentryHardSoft_V(JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
!
!- Statement functions for XTOMO
      IentryXtomo (IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG ) =              &
     &IentryXtomo_V  (JTYP_F_Z(IAMDCSaMuSoG,JAMDCSaMuSoG,KAMDCSaMuSoG))
