/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGVARIABLES_H
#define BTAGVARIABLES_H

namespace btagvar {
  static const char* const JET_ETA = "jet_eta";
  static const char* const JET_ABS_ETA = "jet_abs_eta";
  static const char* const JET_PT = "jet_pt";

  static const char* const IP2D_PU = "ip2d_pu";
  static const char* const IP2D_PB = "ip2d_pb";
  static const char* const IP2D_PC = "ip2d_pc";
  static const char* const IP2     = "ip2";
  static const char* const IP2_C   = "ip2_c";
  static const char* const IP2_CU  = "ip2_cu";
  static const char* const IP2_NAN     = "ip2_nan";
  static const char* const IP2_C_NAN   = "ip2_c_nan";
  static const char* const IP2_CU_NAN  = "ip2_cu_nan";

  static const char* const IP3D_PU = "ip3d_pu";
  static const char* const IP3D_PB = "ip3d_pb";
  static const char* const IP3D_PC = "ip3d_pc";
  static const char* const IP3     = "ip3";
  static const char* const IP3_C   = "ip3_c";
  static const char* const IP3_CU  = "ip3_cu";
  static const char* const IP3_NAN     = "ip3_nan";
  static const char* const IP3_C_NAN   = "ip3_c_nan";
  static const char* const IP3_CU_NAN  = "ip3_cu_nan";

  static const char* const SV0_EFRC   = "sv0_efc";
  static const char* const SV0_NTRKV  = "sv0_ntrkv";
  static const char* const SV0_N2T    = "sv0_n2t";
  static const char* const SV0_MASS   = "sv0_m";
  static const char* const SV0_RADIUS = "sv0_radius";
  static const char* const SV0_SIG3D  = "sv0_sig3d";

  static const char* const SV1_EFRC  = "sv1_efc";
  static const char* const SV1_NTRKV = "sv1_ntrkv";
  static const char* const SV1_N2T   = "sv1_n2t";
  static const char* const SV1_MASS  = "sv1_m";
  static const char* const SV1_DR    = "sv1_dR";
  static const char* const SV1_LXY   = "sv1_Lxy";
  static const char* const SV1_L3D   = "sv1_L3d";
  static const char* const SV1_SIG3D = "sv1_sig3d";
  static const char* const SV1_DISTMATLAY = "sv1_distmatlay";
  static const char* const SV1_PU    = "sv1_pu";
  static const char* const SV1_PB    = "sv1_pb";
  static const char* const SV1_PC    = "sv1_pc";
  static const char* const SV1       = "sv1";
  static const char* const SV1_C     = "sv1_c";
  static const char* const SV1_CU    = "sv1_cu";
  static const char* const SV1_NAN       = "sv1_nan";
  static const char* const SV1_C_NAN     = "sv1_c_nan";
  static const char* const SV1_CU_NAN    = "sv1_cu_nan";

  static const char* const JF_NVTX   = "jf_nvtx";
  static const char* const JF_NVTX1T = "jf_nvtx1t";
  static const char* const JF_NTRKV  = "jf_ntrkAtVx";
  static const char* const JF_EFRC   = "jf_efc";
  static const char* const JF_MASS   = "jf_m";
  static const char* const JF_MASS_UNCOR   = "jf_m_uncor";
  static const char* const JF_N2TV   = "jf_n2t";
  static const char* const JF_DR     = "jf_dR";
  static const char* const JF_DR_FLIGHT= "jf_dRFlightDir";
  static const char* const JF_DPHI   = "jf_dphi";
  static const char* const JF_DETA   = "jf_deta";
  static const char* const JF_SIG3D  = "jf_sig3d";

  static const char* const TRKSUM_NTRK   = "trkSum_ntrk";
  static const char* const TRKSUM_SPT    = "trkSum_sPt";
  static const char* const TRKSUM_VPT    = "trkSum_vPt";
  static const char* const TRKSUM_ABSETA = "trkSum_vAbsEta";

  static const char* const SM_MU_PT     = "sm_mu_pt";
  static const char* const SM_DR        = "sm_dR";
  static const char* const SM_QOVERP    = "sm_qOverPratio";
  static const char* const SM_MOMBALSIG = "sm_mombalsignif";
  static const char* const SM_SCATNEIGH = "sm_scatneighsignif";
  static const char* const SM_PTREL     = "sm_pTrel";
  static const char* const SM_MU_D0     = "sm_mu_d0";
  static const char* const SM_MU_Z0     = "sm_mu_z0";
  static const char* const SM_ID_QOVERP = "sm_ID_qOverP";


}

#endif
