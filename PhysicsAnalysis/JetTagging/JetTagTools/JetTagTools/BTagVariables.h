/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGVARIABLES_H
#define BTAGVARIABLES_H

namespace btagvar {
  static const char* JET_ETA = "jet_eta";
  static const char* JET_ABS_ETA = "jet_abs_eta";
  static const char* JET_PT = "jet_pt";

  static const char* IP2D_PU = "ip2d_pu";
  static const char* IP2D_PB = "ip2d_pb";
  static const char* IP2D_PC = "ip2d_pc";
  static const char* IP2     = "ip2";
  static const char* IP2_C   = "ip2_c";
  static const char* IP2_CU  = "ip2_cu";
  static const char* IP2_NAN     = "ip2_nan";
  static const char* IP2_C_NAN   = "ip2_c_nan";
  static const char* IP2_CU_NAN  = "ip2_cu_nan";

  static const char* IP3D_PU = "ip3d_pu";
  static const char* IP3D_PB = "ip3d_pb";
  static const char* IP3D_PC = "ip3d_pc";
  static const char* IP3     = "ip3";
  static const char* IP3_C   = "ip3_c";
  static const char* IP3_CU  = "ip3_cu";
  static const char* IP3_NAN     = "ip3_nan";
  static const char* IP3_C_NAN   = "ip3_c_nan";
  static const char* IP3_CU_NAN  = "ip3_cu_nan";

  static const char* SV0_EFRC   = "sv0_efc";
  static const char* SV0_NTRKV  = "sv0_ntrkv";
  static const char* SV0_N2T    = "sv0_n2t";
  static const char* SV0_MASS   = "sv0_m";
  static const char* SV0_RADIUS = "sv0_radius";
  static const char* SV0_SIG3D  = "sv0_sig3d";

  static const char* SV1_EFRC  = "sv1_efc";
  static const char* SV1_NTRKV = "sv1_ntrkv";
  static const char* SV1_N2T   = "sv1_n2t";
  static const char* SV1_MASS  = "sv1_m";
  static const char* SV1_DR    = "sv1_dR";
  static const char* SV1_LXY   = "sv1_Lxy";
  static const char* SV1_L3D   = "sv1_L3d";
  static const char* SV1_SIG3D = "sv1_sig3d";
  static const char* SV1_DISTMATLAY = "sv1_distmatlay";
  static const char* SV1_PU    = "sv1_pu";
  static const char* SV1_PB    = "sv1_pb";
  static const char* SV1_PC    = "sv1_pc";
  static const char* SV1       = "sv1";
  static const char* SV1_C     = "sv1_c";
  static const char* SV1_CU    = "sv1_cu";
  static const char* SV1_NAN       = "sv1_nan";
  static const char* SV1_C_NAN     = "sv1_c_nan";
  static const char* SV1_CU_NAN    = "sv1_cu_nan";

  static const char* JF_NVTX   = "jf_nvtx";
  static const char* JF_NVTX1T = "jf_nvtx1t";
  static const char* JF_NTRKV  = "jf_ntrkAtVx";
  static const char* JF_EFRC   = "jf_efc";
  static const char* JF_MASS   = "jf_m";
  static const char* JF_MASS_UNCOR   = "jf_m_uncor";
  static const char* JF_N2TV   = "jf_n2t";
  static const char* JF_DR     = "jf_dR";
  static const char* JF_DR_FLIGHT= "jf_dRFlightDir";
  static const char* JF_DPHI   = "jf_dphi";
  static const char* JF_DETA   = "jf_deta";
  static const char* JF_SIG3D  = "jf_sig3d";

  static const char* JET_WIDTH           = "jet_width";
  static const char* JET_N_TRK3_SIGD0CUT = "jet_n_trk_sigd0cut";
  static const char* JET_TRK3_D0SIG      = "jet_trk3_d0sig";
  static const char* JET_TRK3_Z0SIG      = "jet_trk3_z0sig";
  static const char* JET_SV_SCALED_EFC   = "jet_sv_scaled_efc";
  static const char* JET_JF_SCALED_EFC   = "jet_jf_scaled_efc";

  static const char* JF_NTRK_VTX1         = "nTrk_vtx1";//"jf_ntrk_vtx1";
  static const char* JF_MASS_VTX1         = "mass_first_vtx";//"jf_m_vtx1";
  static const char* JF_E_VTX1            = "e_first_vtx";//"jf_e_vtx1";
  static const char* JF_E_FRAC_VTX1       = "e_frac_vtx1";//"jf_efc_vtx1";
  static const char* JF_L3D_VTX1          = "closestVtx_L3D";//"jf_L3d_vtx1";
  static const char* JF_Lxy_VTX1          = "JF_Lxy1";//"jf_Lxy_vtx1";
  static const char* JF_RAPIDITY_VTX1_MAX = "vtx1_MaxTrkRapidity_jf_path";//"jf_trky_max_vtx1";
  static const char* JF_RAPIDITY_VTX1_AVG = "vtx1_AvgTrkRapidity_jf_path";//"jf_trky_avg_vtx1";
  static const char* JF_RAPIDITY_VTX1_MIN = "vtx1_MinTrkRapidity_jf_path";//"jf_trky_min_vtx1";
  static const char* JF_RAPIDITY_MAX      = "MaxTrkRapidity_jf_path";//"jf_trky_max";
  static const char* JF_RAPIDITY_MIN      = "MinTrkRapidity_jf_path";//"jf_trky_min";
  static const char* JF_RAPIDITY_AVG      = "AvgTrkRapidity_jf_path";//"jf_trky_avg";

  static const char* TRKSUM_NTRK   = "trkSum_ntrk";
  static const char* TRKSUM_SPT    = "trkSum_sPt";
  static const char* TRKSUM_VPT    = "trkSum_vPt";
  static const char* TRKSUM_ABSETA = "trkSum_vAbsEta";

  static const char* SM_MU_PT     = "sm_mu_pt";
  static const char* SM_DR        = "sm_dR";
  static const char* SM_QOVERP    = "sm_qOverPratio";
  static const char* SM_MOMBALSIG = "sm_mombalsignif";
  static const char* SM_SCATNEIGH = "sm_scatneighsignif";
  static const char* SM_PTREL     = "sm_pTrel";
  static const char* SM_MU_D0     = "sm_mu_d0";
  static const char* SM_MU_Z0     = "sm_mu_z0";
  static const char* SM_ID_QOVERP = "sm_ID_qOverP";


}

#endif
