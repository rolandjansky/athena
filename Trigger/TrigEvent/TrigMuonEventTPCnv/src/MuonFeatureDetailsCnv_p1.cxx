/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p1.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p1.h"

void MuonFeatureDetailsCnv_p1::persToTrans(const MuonFeatureDetails_p1* persObj,
                                                 MuonFeatureDetails* transObj,
                                                 MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p1::persToTrans called " << endreq;

    transObj->m_id           = persObj->m_id;
    transObj->m_te_id        = persObj->m_te_id;
    transObj->m_error        = persObj->m_error;

    transObj->m_lvl1_id      = persObj->m_lvl1_id;
    transObj->m_lumi_block   = persObj->m_lumi_block;
    transObj->m_muondetmask  = persObj->m_muondetmask; 
    transObj->m_RoIID        = persObj->m_RoIID;
    transObj->m_RoISystem    = persObj->m_RoISystem;
    transObj->m_RoISubsystem = persObj->m_RoISubsystem; 
    transObj->m_RoISector    = persObj->m_RoISector;
    transObj->m_RoINumber    = persObj->m_RoINumber;
    transObj->m_RoIThreshold = persObj->m_RoIThreshold;

    transObj->m_RoIEta = persObj->m_RoIEta;
    transObj->m_RoIPhi = persObj->m_RoIPhi;

    transObj->m_extension_capacity = -1;
    transObj->m_max_rob_capacity = -1;
    transObj->m_max_csm_capacity = -1;
    transObj->m_max_lvl1_emu_capacity = -1;
    transObj->m_max_rpc_hits_capacity = -1;
    transObj->m_max_tgc_hits_capacity = -1;
    transObj->m_max_mdt_hits_capacity = -1;
    
    transObj->m_rpc_pad_error = persObj->m_rpc_pad_error;
    transObj->m_tgc_rdo_error = persObj->m_tgc_rdo_error;
    
    transObj->m_rpc1_x = persObj->m_rpc1_x;
    transObj->m_rpc1_y = persObj->m_rpc1_y;
    transObj->m_rpc1_z = persObj->m_rpc1_z;
     
    transObj->m_rpc2_x = persObj->m_rpc2_x;
    transObj->m_rpc2_y = persObj->m_rpc2_y;
    transObj->m_rpc2_z = persObj->m_rpc2_z;
    
    transObj->m_rpc3_x = persObj->m_rpc3_x;
    transObj->m_rpc3_y = persObj->m_rpc3_y;
    transObj->m_rpc3_z = persObj->m_rpc3_z; 
    
    transObj->m_tgc_Mid1_eta      = persObj->m_tgc_Mid1_eta;
    transObj->m_tgc_Mid1_phi      = persObj->m_tgc_Mid1_phi;
    transObj->m_tgc_Mid1_r        = persObj->m_tgc_Mid1_r;
    transObj->m_tgc_Mid1_z        = persObj->m_tgc_Mid1_z;
    
    transObj->m_tgc_Mid2_eta      = persObj->m_tgc_Mid2_eta;
    transObj->m_tgc_Mid2_phi      = persObj->m_tgc_Mid2_phi;
    transObj->m_tgc_Mid2_r        = persObj->m_tgc_Mid2_r;
    transObj->m_tgc_Mid2_z        = persObj->m_tgc_Mid2_z;
    
    transObj->m_tgc_Mid_rho_chi2  = persObj->m_tgc_Mid_rho_chi2;
    transObj->m_tgc_Mid_rho_N     = persObj->m_tgc_Mid_rho_N;
    transObj->m_tgc_Mid_phi_chi2  = persObj->m_tgc_Mid_phi_chi2;
    transObj->m_tgc_Mid_phi_N     = persObj->m_tgc_Mid_phi_N;
    
    transObj->m_tgc_Inn_eta       = persObj->m_tgc_Inn_eta;
    transObj->m_tgc_Inn_phi       = persObj->m_tgc_Inn_phi;
    transObj->m_tgc_Inn_r         = persObj->m_tgc_Inn_r;
    transObj->m_tgc_Inn_z         = persObj->m_tgc_Inn_z;
    transObj->m_tgc_Inn_rho_std   = persObj->m_tgc_Inn_rho_std;
    transObj->m_tgc_Inn_rho_N     = persObj->m_tgc_Inn_rho_N;
    transObj->m_tgc_Inn_phi_std   = persObj->m_tgc_Inn_phi_std;
    transObj->m_tgc_Inn_phi_N     = persObj->m_tgc_Inn_phi_N;
    
    transObj->m_tgc_PT = persObj->m_tgc_PT;
    
    transObj->m_type           = persObj->m_type;
    transObj->m_t_x_center     = persObj->m_t_x_center;
    transObj->m_t_y_center     = persObj->m_t_y_center;
    transObj->m_t_radius       = persObj->m_t_radius;
    transObj->m_t_ap           = persObj->m_t_ap;
    transObj->m_t_aphi         = persObj->m_t_aphi;
    transObj->m_t_phi          = persObj->m_t_phi;
    transObj->m_extrInnerEta   = persObj->m_extrInnerEta;
    transObj->m_extrInnerPhi   = persObj->m_extrInnerPhi;
    
    transObj->m_chamber_type_1 = persObj->m_chamber_type_1;
    transObj->m_chamber_type_2 = persObj->m_chamber_type_2;
    transObj->m_pos            = persObj->m_pos;
    transObj->m_aw             = persObj->m_aw;
    transObj->m_bw             = persObj->m_bw;
    transObj->m_zetaMin        = persObj->m_zetaMin;
    transObj->m_zetaMax        = persObj->m_zetaMax;
    transObj->m_radMin         = persObj->m_radMin;
    transObj->m_radMax         = persObj->m_radMax;
    transObj->m_etaMin         = persObj->m_etaMin;
    transObj->m_etaMax         = persObj->m_etaMax;
    transObj->m_st_phi         = persObj->m_st_phi;
    transObj->m_st_roads       = persObj->m_st_roads;
    
    transObj->m_mdt_Inner_slope      = persObj->m_mdt_Inner_slope;
    transObj->m_mdt_Inner_intercept  = persObj->m_mdt_Inner_intercept;
    transObj->m_mdt_Inner_Z          = persObj->m_mdt_Inner_Z;
    transObj->m_mdt_Inner_R          = persObj->m_mdt_Inner_R;
    transObj->m_mdt_Inner_fit_chi    = persObj->m_mdt_Inner_fit_chi;
    transObj->m_mdt_Middle_slope     = persObj->m_mdt_Middle_slope;
    transObj->m_mdt_Middle_intercept = persObj->m_mdt_Middle_intercept;
    transObj->m_mdt_Middle_Z         = persObj->m_mdt_Middle_Z;
    transObj->m_mdt_Middle_R         = persObj->m_mdt_Middle_R;
    transObj->m_mdt_Middle_fit_chi   = persObj->m_mdt_Middle_fit_chi;
    transObj->m_mdt_Outer_slope      = persObj->m_mdt_Outer_slope;
    transObj->m_mdt_Outer_intercept  = persObj->m_mdt_Outer_intercept;
    transObj->m_mdt_Outer_Z          = persObj->m_mdt_Outer_Z;
    transObj->m_mdt_Outer_R          = persObj->m_mdt_Outer_R;
    transObj->m_mdt_Outer_fit_chi    = persObj->m_mdt_Outer_fit_chi;
    
    transObj->m_Address      = persObj->m_Address;
    transObj->m_Sagitta      = persObj->m_Sagitta;
    transObj->m_Radius       = persObj->m_Radius;
    transObj->m_Slope        = persObj->m_Slope;
    transObj->m_Intercept    = persObj->m_Intercept;
    transObj->m_Alpha        = persObj->m_Alpha;
    transObj->m_Beta         = persObj->m_Beta;
    transObj->m_DeltaR       = persObj->m_DeltaR;
    transObj->m_Speed_over_c = persObj->m_Speed_over_c;
    transObj->m_Eta          = persObj->m_Eta;
    transObj->m_PhiMap       = persObj->m_PhiMap;
    transObj->m_Phi          = persObj->m_Phi;
    transObj->m_PhiDir       = persObj->m_PhiDir;
    transObj->m_Pt           = persObj->m_Pt;
    transObj->m_Charge       = persObj->m_Charge;
    
    transObj->m_extension0   = persObj->m_extension0;
    transObj->m_extension1   = persObj->m_extension1;
    transObj->m_extension2   = persObj->m_extension2;
    transObj->m_extension3   = persObj->m_extension3;
    transObj->m_extension4   = persObj->m_extension4;
    transObj->m_extension5   = persObj->m_extension5;
    transObj->m_extension6   = persObj->m_extension6;
    transObj->m_extension7   = persObj->m_extension7;
    transObj->m_extension8   = persObj->m_extension8;
    transObj->m_extension9   = persObj->m_extension9;
    
    transObj->m_lvl1_emulation = persObj->m_lvl1_emulation;
    
    transObj->m_rob_id         = persObj->m_rob_id;
    transObj->m_csm_id         = persObj->m_csm_id;
    transObj->m_csm_size       = persObj->m_csm_size;
    transObj->m_csm_error      = persObj->m_csm_error;
    transObj->m_removed_rob_id = persObj->m_removed_rob_id;
    transObj->m_removed_csm_id = persObj->m_removed_csm_id;
    
    transObj->m_pad_hit_onlineId = persObj->m_pad_hit_onlineId;
    transObj->m_pad_hit_code     = persObj->m_pad_hit_code;
    transObj->m_pad_hit_x        = persObj->m_pad_hit_x;
    transObj->m_pad_hit_y        = persObj->m_pad_hit_y;
    transObj->m_pad_hit_z        = persObj->m_pad_hit_z;
    transObj->m_pad_hit_r        = persObj->m_pad_hit_r;
    transObj->m_pad_hit_p        = persObj->m_pad_hit_p;
    
    transObj->m_tgc_Inn_rho_hit_phi    = persObj->m_tgc_Inn_rho_hit_phi;
    transObj->m_tgc_Inn_rho_hit_r      = persObj->m_tgc_Inn_rho_hit_r;
    transObj->m_tgc_Inn_rho_hit_z      = persObj->m_tgc_Inn_rho_hit_z;
    transObj->m_tgc_Inn_rho_hit_width  = persObj->m_tgc_Inn_rho_hit_width;
    transObj->m_tgc_Inn_rho_hit_in_seg = persObj->m_tgc_Inn_rho_hit_in_seg;
    transObj->m_tgc_Inn_phi_hit_phi    = persObj->m_tgc_Inn_phi_hit_phi;
    transObj->m_tgc_Inn_phi_hit_r      = persObj->m_tgc_Inn_phi_hit_r;
    transObj->m_tgc_Inn_phi_hit_z      = persObj->m_tgc_Inn_phi_hit_z;
    transObj->m_tgc_Inn_phi_hit_width  = persObj->m_tgc_Inn_phi_hit_width;
    transObj->m_tgc_Inn_phi_hit_in_seg = persObj->m_tgc_Inn_phi_hit_in_seg;
    transObj->m_tgc_Mid_rho_hit_phi    = persObj->m_tgc_Mid_rho_hit_phi;
    transObj->m_tgc_Mid_rho_hit_r      = persObj->m_tgc_Mid_rho_hit_r;
    transObj->m_tgc_Mid_rho_hit_z      = persObj->m_tgc_Mid_rho_hit_z;
    transObj->m_tgc_Mid_rho_hit_width  = persObj->m_tgc_Mid_rho_hit_width;
    transObj->m_tgc_Mid_rho_hit_in_seg = persObj->m_tgc_Mid_rho_hit_in_seg;
    transObj->m_tgc_Mid_phi_hit_phi    = persObj->m_tgc_Mid_phi_hit_phi;
    transObj->m_tgc_Mid_phi_hit_r      = persObj->m_tgc_Mid_phi_hit_r;
    transObj->m_tgc_Mid_phi_hit_z      = persObj->m_tgc_Mid_phi_hit_z;
    transObj->m_tgc_Mid_phi_hit_width  = persObj->m_tgc_Mid_phi_hit_width;
    transObj->m_tgc_Mid_phi_hit_in_seg = persObj->m_tgc_Mid_phi_hit_in_seg;
    
    transObj->m_mdt_onlineId      = persObj->m_mdt_onlineId;
    transObj->m_mdt_offlineId     = persObj->m_mdt_offlineId;
    transObj->m_mdt_tube_r        = persObj->m_mdt_tube_r;
    transObj->m_mdt_tube_z        = persObj->m_mdt_tube_z;
    transObj->m_mdt_tube_residual = persObj->m_mdt_tube_residual;
    transObj->m_mdt_tube_time     = persObj->m_mdt_tube_time;
    transObj->m_mdt_tube_space    = persObj->m_mdt_tube_space;
    transObj->m_mdt_tube_sigma    = persObj->m_mdt_tube_sigma;
}


void MuonFeatureDetailsCnv_p1::transToPers(const MuonFeatureDetails* transObj,
                                                 MuonFeatureDetails_p1* persObj,
                                                 MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p1::transToPers called " << endreq;
   
    persObj->m_id           = transObj->m_id;
    persObj->m_te_id        = transObj->m_te_id;
    persObj->m_error        = transObj->m_error;

    persObj->m_lvl1_id      = transObj->m_lvl1_id;
    persObj->m_lumi_block   = transObj->m_lumi_block;
    persObj->m_muondetmask  = transObj->m_muondetmask;
    persObj->m_RoIID        = transObj->m_RoIID;
    persObj->m_RoISystem    = transObj->m_RoISystem;
    persObj->m_RoISubsystem = transObj->m_RoISubsystem;
    persObj->m_RoISector    = transObj->m_RoISector;
    persObj->m_RoINumber    = transObj->m_RoINumber;
    persObj->m_RoIThreshold = transObj->m_RoIThreshold;

    persObj->m_RoIEta = transObj->m_RoIEta;
    persObj->m_RoIPhi = transObj->m_RoIPhi;

    persObj->m_rpc_pad_error = transObj->m_rpc_pad_error;
    persObj->m_tgc_rdo_error = transObj->m_tgc_rdo_error;
    
    persObj->m_rpc1_x = transObj->m_rpc1_x;
    persObj->m_rpc1_y = transObj->m_rpc1_y;
    persObj->m_rpc1_z = transObj->m_rpc1_z;
     
    persObj->m_rpc2_x = transObj->m_rpc2_x;
    persObj->m_rpc2_y = transObj->m_rpc2_y;
    persObj->m_rpc2_z = transObj->m_rpc2_z;
    
    persObj->m_rpc3_x = transObj->m_rpc3_x;
    persObj->m_rpc3_y = transObj->m_rpc3_y;
    persObj->m_rpc3_z = transObj->m_rpc3_z; 
    
    persObj->m_tgc_Mid1_eta      = transObj->m_tgc_Mid1_eta;
    persObj->m_tgc_Mid1_phi      = transObj->m_tgc_Mid1_phi;
    persObj->m_tgc_Mid1_r        = transObj->m_tgc_Mid1_r;
    persObj->m_tgc_Mid1_z        = transObj->m_tgc_Mid1_z;
    
    persObj->m_tgc_Mid2_eta      = transObj->m_tgc_Mid2_eta;
    persObj->m_tgc_Mid2_phi      = transObj->m_tgc_Mid2_phi;
    persObj->m_tgc_Mid2_r        = transObj->m_tgc_Mid2_r;
    persObj->m_tgc_Mid2_z        = transObj->m_tgc_Mid2_z;
    
    persObj->m_tgc_Mid_rho_chi2  = transObj->m_tgc_Mid_rho_chi2;
    persObj->m_tgc_Mid_rho_N     = transObj->m_tgc_Mid_rho_N;
    persObj->m_tgc_Mid_phi_chi2  = transObj->m_tgc_Mid_phi_chi2;
    persObj->m_tgc_Mid_phi_N     = transObj->m_tgc_Mid_phi_N;
    
    persObj->m_tgc_Inn_eta       = transObj->m_tgc_Inn_eta;
    persObj->m_tgc_Inn_phi       = transObj->m_tgc_Inn_phi;
    persObj->m_tgc_Inn_r         = transObj->m_tgc_Inn_r;
    persObj->m_tgc_Inn_z         = transObj->m_tgc_Inn_z;
    persObj->m_tgc_Inn_rho_std   = transObj->m_tgc_Inn_rho_std;
    persObj->m_tgc_Inn_rho_N     = transObj->m_tgc_Inn_rho_N;
    persObj->m_tgc_Inn_phi_std   = transObj->m_tgc_Inn_phi_std;
    persObj->m_tgc_Inn_phi_N     = transObj->m_tgc_Inn_phi_N;
    
    persObj->m_tgc_PT = transObj->m_tgc_PT;
    
    persObj->m_type           = transObj->m_type;
    persObj->m_t_x_center     = transObj->m_t_x_center;
    persObj->m_t_y_center     = transObj->m_t_y_center;
    persObj->m_t_radius       = transObj->m_t_radius;
    persObj->m_t_ap           = transObj->m_t_ap;
    persObj->m_t_aphi         = transObj->m_t_aphi;
    persObj->m_t_phi          = transObj->m_t_phi;
    persObj->m_extrInnerEta   = transObj->m_extrInnerEta;
    persObj->m_extrInnerPhi   = transObj->m_extrInnerPhi;
    
    persObj->m_chamber_type_1 = transObj->m_chamber_type_1;
    persObj->m_chamber_type_2 = transObj->m_chamber_type_2;
    persObj->m_pos            = transObj->m_pos;
    persObj->m_aw             = transObj->m_aw;
    persObj->m_bw             = transObj->m_bw;
    persObj->m_zetaMin        = transObj->m_zetaMin;
    persObj->m_zetaMax        = transObj->m_zetaMax;
    persObj->m_radMin         = transObj->m_radMin;
    persObj->m_radMax         = transObj->m_radMax;
    persObj->m_etaMin         = transObj->m_etaMin;
    persObj->m_etaMax         = transObj->m_etaMax;
    persObj->m_st_phi         = transObj->m_st_phi;
    persObj->m_st_roads       = transObj->m_st_roads;
    
    persObj->m_mdt_Inner_slope      = transObj->m_mdt_Inner_slope;
    persObj->m_mdt_Inner_intercept  = transObj->m_mdt_Inner_intercept;
    persObj->m_mdt_Inner_Z          = transObj->m_mdt_Inner_Z;
    persObj->m_mdt_Inner_R          = transObj->m_mdt_Inner_R;
    persObj->m_mdt_Inner_fit_chi    = transObj->m_mdt_Inner_fit_chi;
    persObj->m_mdt_Middle_slope     = transObj->m_mdt_Middle_slope;
    persObj->m_mdt_Middle_intercept = transObj->m_mdt_Middle_intercept;
    persObj->m_mdt_Middle_Z         = transObj->m_mdt_Middle_Z;
    persObj->m_mdt_Middle_R         = transObj->m_mdt_Middle_R;
    persObj->m_mdt_Middle_fit_chi   = transObj->m_mdt_Middle_fit_chi;
    persObj->m_mdt_Outer_slope      = transObj->m_mdt_Outer_slope;
    persObj->m_mdt_Outer_intercept  = transObj->m_mdt_Outer_intercept;
    persObj->m_mdt_Outer_Z          = transObj->m_mdt_Outer_Z;
    persObj->m_mdt_Outer_R          = transObj->m_mdt_Outer_R;
    persObj->m_mdt_Outer_fit_chi    = transObj->m_mdt_Outer_fit_chi;
    
    persObj->m_Address      = transObj->m_Address;
    persObj->m_Sagitta      = transObj->m_Sagitta;
    persObj->m_Radius       = transObj->m_Radius;
    persObj->m_Slope        = transObj->m_Slope;
    persObj->m_Intercept    = transObj->m_Intercept;
    persObj->m_Alpha        = transObj->m_Alpha;
    persObj->m_Beta         = transObj->m_Beta;
    persObj->m_DeltaR       = transObj->m_DeltaR;
    persObj->m_Speed_over_c = transObj->m_Speed_over_c;
    persObj->m_Eta          = transObj->m_Eta;
    persObj->m_PhiMap       = transObj->m_PhiMap;
    persObj->m_Phi          = transObj->m_Phi;
    persObj->m_PhiDir       = transObj->m_PhiDir;
    persObj->m_Pt           = transObj->m_Pt;
    persObj->m_Charge       = transObj->m_Charge;
    
    persObj->m_extension0   = transObj->m_extension0;
    persObj->m_extension1   = transObj->m_extension1;
    persObj->m_extension2   = transObj->m_extension2;
    persObj->m_extension3   = transObj->m_extension3;
    persObj->m_extension4   = transObj->m_extension4;
    persObj->m_extension5   = transObj->m_extension5;
    persObj->m_extension6   = transObj->m_extension6;
    persObj->m_extension7   = transObj->m_extension7;
    persObj->m_extension8   = transObj->m_extension8;
    persObj->m_extension9   = transObj->m_extension9;
    
    persObj->m_lvl1_emulation = transObj->m_lvl1_emulation;
    
    persObj->m_rob_id         = transObj->m_rob_id;
    persObj->m_csm_id         = transObj->m_csm_id;
    persObj->m_csm_size       = transObj->m_csm_size;
    persObj->m_csm_error      = transObj->m_csm_error;
    persObj->m_removed_rob_id = transObj->m_removed_rob_id;
    persObj->m_removed_csm_id = transObj->m_removed_csm_id;
    
    persObj->m_pad_hit_onlineId = transObj->m_pad_hit_onlineId;
    persObj->m_pad_hit_code     = transObj->m_pad_hit_code;
    persObj->m_pad_hit_x        = transObj->m_pad_hit_x;
    persObj->m_pad_hit_y        = transObj->m_pad_hit_y;
    persObj->m_pad_hit_z        = transObj->m_pad_hit_z;
    persObj->m_pad_hit_r        = transObj->m_pad_hit_r;
    persObj->m_pad_hit_p        = transObj->m_pad_hit_p;
    
    persObj->m_tgc_Inn_rho_hit_phi    = transObj->m_tgc_Inn_rho_hit_phi;
    persObj->m_tgc_Inn_rho_hit_r      = transObj->m_tgc_Inn_rho_hit_r;
    persObj->m_tgc_Inn_rho_hit_z      = transObj->m_tgc_Inn_rho_hit_z;
    persObj->m_tgc_Inn_rho_hit_width  = transObj->m_tgc_Inn_rho_hit_width;
    persObj->m_tgc_Inn_rho_hit_in_seg = transObj->m_tgc_Inn_rho_hit_in_seg;
    persObj->m_tgc_Inn_phi_hit_phi    = transObj->m_tgc_Inn_phi_hit_phi;
    persObj->m_tgc_Inn_phi_hit_r      = transObj->m_tgc_Inn_phi_hit_r;
    persObj->m_tgc_Inn_phi_hit_z      = transObj->m_tgc_Inn_phi_hit_z;
    persObj->m_tgc_Inn_phi_hit_width  = transObj->m_tgc_Inn_phi_hit_width;
    persObj->m_tgc_Inn_phi_hit_in_seg = transObj->m_tgc_Inn_phi_hit_in_seg;
    persObj->m_tgc_Mid_rho_hit_phi    = transObj->m_tgc_Mid_rho_hit_phi;
    persObj->m_tgc_Mid_rho_hit_r      = transObj->m_tgc_Mid_rho_hit_r;
    persObj->m_tgc_Mid_rho_hit_z      = transObj->m_tgc_Mid_rho_hit_z;
    persObj->m_tgc_Mid_rho_hit_width  = transObj->m_tgc_Mid_rho_hit_width;
    persObj->m_tgc_Mid_rho_hit_in_seg = transObj->m_tgc_Mid_rho_hit_in_seg;
    persObj->m_tgc_Mid_phi_hit_phi    = transObj->m_tgc_Mid_phi_hit_phi;
    persObj->m_tgc_Mid_phi_hit_r      = transObj->m_tgc_Mid_phi_hit_r;
    persObj->m_tgc_Mid_phi_hit_z      = transObj->m_tgc_Mid_phi_hit_z;
    persObj->m_tgc_Mid_phi_hit_width  = transObj->m_tgc_Mid_phi_hit_width;
    persObj->m_tgc_Mid_phi_hit_in_seg = transObj->m_tgc_Mid_phi_hit_in_seg;
    
    persObj->m_mdt_onlineId      = transObj->m_mdt_onlineId;
    persObj->m_mdt_offlineId     = transObj->m_mdt_offlineId;
    persObj->m_mdt_tube_r        = transObj->m_mdt_tube_r;
    persObj->m_mdt_tube_z        = transObj->m_mdt_tube_z;
    persObj->m_mdt_tube_residual = transObj->m_mdt_tube_residual;
    persObj->m_mdt_tube_time     = transObj->m_mdt_tube_time;
    persObj->m_mdt_tube_space    = transObj->m_mdt_tube_space;
    persObj->m_mdt_tube_sigma    = transObj->m_mdt_tube_sigma;
}

