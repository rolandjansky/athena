/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iomanip>

#include "TrigMuonEvent/MuonFeatureDetails.h"

//#include "GaudiKernel/MsgStream.h"

MuonFeatureDetails::MuonFeatureDetails() {
  m_id = NULLID; 
  m_te_id = 0x0;
  m_lvl1_id = 0x0;
  m_lumi_block = 0x0;
  m_muondetmask = 0x0;
  m_RoIID = 0x0; 
  m_RoISystem = 0x0;
  m_RoISubsystem = 0x0; 
  m_RoISector = 0x0;
  m_RoINumber = 0x0;
  m_RoIThreshold = 0x0;
  m_RoIEta = 0.;
  m_RoIPhi = 0.;
  m_extension_capacity = -1;
  m_max_rob_capacity = -1;
  m_max_csm_capacity = -1;
  m_max_lvl1_emu_capacity = -1;
  m_max_rpc_hits_capacity = -1;
  m_max_tgc_hits_capacity = -1;
  m_max_mdt_hits_capacity = -1;
  
  init_variables();
  init_vectors();
}

MuonFeatureDetails::MuonFeatureDetails(AlgoId id, 
  uint32_t te_id, uint32_t lvl1_id, uint32_t lumi_block, uint32_t muondetmask,
  uint32_t RoIID, uint32_t RoISystem, uint32_t RoISubsystem, uint32_t RoISector,
  uint32_t RoINumber, uint32_t RoIThreshold, float RoIEta, float RoIPhi,
  int extension_capacity,
  int max_rob_capacity,
  int max_csm_capacity,
  int max_lvl1_emu_capacity,
  int max_rpc_hits_capacity,
  int max_tgc_hits_capacity,
  int max_mdt_hits_capacity)  :
  
  m_id(id), m_te_id(te_id), m_lvl1_id(lvl1_id), m_lumi_block(lumi_block),
  m_muondetmask(muondetmask), m_RoIID(RoIID), m_RoISystem(RoISystem),
  m_RoISubsystem(RoISubsystem), m_RoISector(RoISector), m_RoINumber(RoINumber),
  m_RoIThreshold(RoIThreshold), m_RoIEta(RoIEta), m_RoIPhi(RoIPhi) 
{
  m_extension_capacity = extension_capacity;
  m_max_rob_capacity = max_rob_capacity;
  m_max_csm_capacity = max_csm_capacity;
  m_max_lvl1_emu_capacity = max_lvl1_emu_capacity;
  m_max_rpc_hits_capacity = max_rpc_hits_capacity;
  m_max_tgc_hits_capacity = max_tgc_hits_capacity;
  m_max_mdt_hits_capacity = max_mdt_hits_capacity;
  
  init_variables();
  init_vectors();
}

MuonFeatureDetails::MuonFeatureDetails(const MuonFeatureDetails& source) {
  m_extension_capacity = source.extension_capacity();
  m_max_rob_capacity = source.max_rob_capacity();
  m_max_csm_capacity = source.max_csm_capacity();
  m_max_lvl1_emu_capacity = source.max_lvl1_emu_capacity();
  m_max_rpc_hits_capacity = source.max_rpc_hits_capacity();
  m_max_tgc_hits_capacity = source.max_tgc_hits_capacity();
  m_max_mdt_hits_capacity = source.max_mdt_hits_capacity();
  
  copy_variables(*this,source);
  
  m_chamber_type_1 = source.chamber_type_1();
  m_chamber_type_2 = source.chamber_type_2();
  m_pos            = source.pos();
  m_aw             = source.aw();
  m_bw             = source.bw();
  m_zetaMin        = source.zetaMin();
  m_zetaMax        = source.zetaMax();
  m_radMin         = source.radMin();
  m_radMax         = source.radMax();
  m_etaMin         = source.etaMin();
  m_etaMax         = source.etaMax();
  m_st_phi         = source.st_phi();
  m_st_roads       = source.st_roads();
  
  copy_vector(m_extension0, source.extension0(), m_extension_capacity);
  copy_vector(m_extension1, source.extension1(), m_extension_capacity);
  copy_vector(m_extension2, source.extension2(), m_extension_capacity);
  copy_vector(m_extension3, source.extension3(), m_extension_capacity);
  copy_vector(m_extension4, source.extension4(), m_extension_capacity);
  copy_vector(m_extension5, source.extension5(), m_extension_capacity);
  copy_vector(m_extension6, source.extension6(), m_extension_capacity);
  copy_vector(m_extension7, source.extension7(), m_extension_capacity);
  copy_vector(m_extension8, source.extension8(), m_extension_capacity);
  copy_vector(m_extension9, source.extension9(), m_extension_capacity);
    
  copy_vector(m_lvl1_emulation, source.lvl1_emulation(), m_max_lvl1_emu_capacity);
    
  copy_vector(m_rob_id, source.rob_id(), m_max_rob_capacity);
  copy_vector(m_csm_id, source.csm_id(), m_max_csm_capacity);
  copy_vector(m_csm_size, source.csm_size(), m_max_csm_capacity);
  copy_vector(m_csm_error, source.csm_error(), m_max_csm_capacity);
  copy_vector(m_removed_rob_id, source.removed_rob_id(), m_max_rob_capacity);
  copy_vector(m_removed_csm_id, source.removed_csm_id(), m_max_csm_capacity);
    
  copy_vector(m_pad_hit_onlineId, source.pad_hit_onlineId(), m_max_rpc_hits_capacity);
  copy_vector(m_pad_hit_code, source.pad_hit_code(), m_max_rpc_hits_capacity);
  copy_vector(m_pad_hit_x, source.pad_hit_x(), m_max_rpc_hits_capacity);
  copy_vector(m_pad_hit_y, source.pad_hit_y(), m_max_rpc_hits_capacity);
  copy_vector(m_pad_hit_z, source.pad_hit_z(), m_max_rpc_hits_capacity);
  copy_vector(m_pad_hit_r, source.pad_hit_r(), m_max_rpc_hits_capacity);
  copy_vector(m_pad_hit_p, source.pad_hit_p(), m_max_rpc_hits_capacity);
    
  copy_vector(m_tgc_Inn_rho_hit_phi, source.tgc_Inn_rho_hit_phi(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_rho_hit_r, source.tgc_Inn_rho_hit_r(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_rho_hit_z, source.tgc_Inn_rho_hit_z(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_rho_hit_width, source.tgc_Inn_rho_hit_width(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_rho_hit_in_seg, source.tgc_Inn_rho_hit_in_seg(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_phi_hit_phi, source.tgc_Inn_phi_hit_phi(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_phi_hit_r, source.tgc_Inn_phi_hit_r(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_phi_hit_z, source.tgc_Inn_phi_hit_z(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_phi_hit_width, source.tgc_Inn_phi_hit_width(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Inn_phi_hit_in_seg, source.tgc_Inn_phi_hit_in_seg(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_rho_hit_phi, source.tgc_Mid_rho_hit_phi(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_rho_hit_r, source.tgc_Mid_rho_hit_r(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_rho_hit_z, source.tgc_Mid_rho_hit_z(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_rho_hit_width, source.tgc_Mid_rho_hit_width(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_rho_hit_in_seg, source.tgc_Mid_rho_hit_in_seg(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_phi_hit_phi, source.tgc_Mid_phi_hit_phi(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_phi_hit_r, source.tgc_Mid_phi_hit_r(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_phi_hit_z, source.tgc_Mid_phi_hit_z(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_phi_hit_width, source.tgc_Mid_phi_hit_width(), m_max_tgc_hits_capacity);
  copy_vector(m_tgc_Mid_phi_hit_in_seg, source.tgc_Mid_phi_hit_in_seg(), m_max_tgc_hits_capacity);
    
  copy_vector(m_mdt_onlineId, source.mdt_onlineId(), m_max_mdt_hits_capacity);
  copy_vector(m_mdt_offlineId, source.mdt_offlineId(), m_max_mdt_hits_capacity);
  copy_vector(m_mdt_tube_r, source.mdt_tube_r(), m_max_mdt_hits_capacity);
  copy_vector(m_mdt_tube_z, source.mdt_tube_z(), m_max_mdt_hits_capacity);
  copy_vector(m_mdt_tube_residual, source.mdt_tube_residual(), m_max_mdt_hits_capacity);
  copy_vector(m_mdt_tube_time, source.mdt_tube_time(), m_max_mdt_hits_capacity);
  copy_vector(m_mdt_tube_space, source.mdt_tube_space(), m_max_mdt_hits_capacity);
  copy_vector(m_mdt_tube_sigma, source.mdt_tube_sigma(), m_max_mdt_hits_capacity);
}

MuonFeatureDetails& 
MuonFeatureDetails::operator=(const MuonFeatureDetails& source){

  int ext  = (m_extension_capacity != source.extension_capacity()) ?
                                      source.extension_capacity() : -1;
  int rob  = (m_max_rob_capacity   != source.max_rob_capacity()) ?
                                      source.max_rob_capacity() : -1;
  int csm  = (m_max_csm_capacity   != source.max_csm_capacity()) ?
                                      source.max_csm_capacity() : -1;
  int lvl1 = (m_max_lvl1_emu_capacity != source.max_lvl1_emu_capacity()) ?
                                      source.max_lvl1_emu_capacity() : -1;
  int rpc  = (m_max_rpc_hits_capacity != source.max_rpc_hits_capacity()) ?
                                      source.max_rpc_hits_capacity() : -1;
  int tgc  = (m_max_tgc_hits_capacity != source.max_tgc_hits_capacity()) ?
                                      source.max_tgc_hits_capacity() : -1;
  int mdt  = (m_max_mdt_hits_capacity != source.max_mdt_hits_capacity()) ?
                                      source.max_mdt_hits_capacity() : -1;
  
  m_extension_capacity = source.extension_capacity();
  m_max_rob_capacity = source.max_rob_capacity();
  m_max_csm_capacity = source.max_csm_capacity();
  m_max_lvl1_emu_capacity = source.max_lvl1_emu_capacity();
  m_max_rpc_hits_capacity = source.max_rpc_hits_capacity();
  m_max_tgc_hits_capacity = source.max_tgc_hits_capacity();
  m_max_mdt_hits_capacity = source.max_mdt_hits_capacity();
  
  copy_variables(*this,source);

  m_chamber_type_1 = source.chamber_type_1();
  m_chamber_type_2 = source.chamber_type_2();
  m_pos            = source.pos();
  m_aw             = source.aw();
  m_bw             = source.bw();
  m_zetaMin        = source.zetaMin();
  m_zetaMax        = source.zetaMax();
  m_radMin         = source.radMin();
  m_radMax         = source.radMax();
  m_etaMin         = source.etaMin();
  m_etaMax         = source.etaMax();
  m_st_phi         = source.st_phi();
  m_st_roads       = source.st_roads();
  
  copy_vector(m_extension0, source.extension0(), ext);
  copy_vector(m_extension1, source.extension1(), ext);
  copy_vector(m_extension2, source.extension2(), ext);
  copy_vector(m_extension3, source.extension3(), ext);
  copy_vector(m_extension4, source.extension4(), ext);
  copy_vector(m_extension5, source.extension5(), ext);
  copy_vector(m_extension6, source.extension6(), ext);
  copy_vector(m_extension7, source.extension7(), ext);
  copy_vector(m_extension8, source.extension8(), ext);
  copy_vector(m_extension9, source.extension9(), ext);
    
  copy_vector(m_lvl1_emulation, source.lvl1_emulation(), lvl1);
    
  copy_vector(m_rob_id, source.rob_id(), rob);
  copy_vector(m_csm_id, source.csm_id(), csm);
  copy_vector(m_csm_size, source.csm_size(), csm);
  copy_vector(m_csm_error, source.csm_error(), csm);
  copy_vector(m_removed_rob_id, source.removed_rob_id(), rob);
  copy_vector(m_removed_csm_id, source.removed_csm_id(), csm);
    
  copy_vector(m_pad_hit_onlineId, source.pad_hit_onlineId(), rpc);
  copy_vector(m_pad_hit_code, source.pad_hit_code(), rpc);
  copy_vector(m_pad_hit_x, source.pad_hit_x(), rpc);
  copy_vector(m_pad_hit_y, source.pad_hit_y(), rpc);
  copy_vector(m_pad_hit_z, source.pad_hit_z(), rpc);
  copy_vector(m_pad_hit_r, source.pad_hit_r(), rpc);
  copy_vector(m_pad_hit_p, source.pad_hit_p(), rpc);
    
  copy_vector(m_tgc_Inn_rho_hit_phi, source.tgc_Inn_rho_hit_phi(), tgc);
  copy_vector(m_tgc_Inn_rho_hit_r, source.tgc_Inn_rho_hit_r(), tgc);
  copy_vector(m_tgc_Inn_rho_hit_z, source.tgc_Inn_rho_hit_z(), tgc);
  copy_vector(m_tgc_Inn_rho_hit_width, source.tgc_Inn_rho_hit_width(), tgc);
  copy_vector(m_tgc_Inn_rho_hit_in_seg, source.tgc_Inn_rho_hit_in_seg(), tgc);
  copy_vector(m_tgc_Inn_phi_hit_phi, source.tgc_Inn_phi_hit_phi(), tgc);
  copy_vector(m_tgc_Inn_phi_hit_r, source.tgc_Inn_phi_hit_r(), tgc);
  copy_vector(m_tgc_Inn_phi_hit_z, source.tgc_Inn_phi_hit_z(), tgc);
  copy_vector(m_tgc_Inn_phi_hit_width, source.tgc_Inn_phi_hit_width(), tgc);
  copy_vector(m_tgc_Inn_phi_hit_in_seg, source.tgc_Inn_phi_hit_in_seg(), tgc);
  copy_vector(m_tgc_Mid_rho_hit_phi, source.tgc_Mid_rho_hit_phi(), tgc);
  copy_vector(m_tgc_Mid_rho_hit_r, source.tgc_Mid_rho_hit_r(), tgc);
  copy_vector(m_tgc_Mid_rho_hit_z, source.tgc_Mid_rho_hit_z(), tgc);
  copy_vector(m_tgc_Mid_rho_hit_width, source.tgc_Mid_rho_hit_width(), tgc);
  copy_vector(m_tgc_Mid_rho_hit_in_seg, source.tgc_Mid_rho_hit_in_seg(), tgc);
  copy_vector(m_tgc_Mid_phi_hit_phi, source.tgc_Mid_phi_hit_phi(), tgc);
  copy_vector(m_tgc_Mid_phi_hit_r, source.tgc_Mid_phi_hit_r(), tgc);
  copy_vector(m_tgc_Mid_phi_hit_z, source.tgc_Mid_phi_hit_z(), tgc);
  copy_vector(m_tgc_Mid_phi_hit_width, source.tgc_Mid_phi_hit_width(), tgc);
  copy_vector(m_tgc_Mid_phi_hit_in_seg, source.tgc_Mid_phi_hit_in_seg(), tgc);
    
  copy_vector(m_mdt_onlineId, source.mdt_onlineId(), mdt);
  copy_vector(m_mdt_offlineId, source.mdt_offlineId(), mdt);
  copy_vector(m_mdt_tube_r, source.mdt_tube_r(), mdt);
  copy_vector(m_mdt_tube_z, source.mdt_tube_z(), mdt);
  copy_vector(m_mdt_tube_residual, source.mdt_tube_residual(), mdt);
  copy_vector(m_mdt_tube_time, source.mdt_tube_time(), mdt);
  copy_vector(m_mdt_tube_space, source.mdt_tube_space(), mdt);
  copy_vector(m_mdt_tube_sigma, source.mdt_tube_sigma(), mdt);
  
  return *this;
}

void MuonFeatureDetails::init_variables() {

  m_error = 0x0;
  
  m_rpc_pad_error = 0x0;
  m_tgc_rdo_error = 0x0;
    
  m_rpc1_x = 99999.;
  m_rpc1_y = 99999.;
  m_rpc1_z = 99999.;
     
  m_rpc2_x = 99999.;
  m_rpc2_y = 99999.;
  m_rpc2_z = 99999.;
    
  m_rpc3_x = 99999.;
  m_rpc3_y = 99999.;
  m_rpc3_z = 99999.; 
    
  m_tgc_Mid1_eta = 99999.;
  m_tgc_Mid1_phi = 99999.;
  m_tgc_Mid1_r   = 99999.;
  m_tgc_Mid1_z   = 99999.;
    
  m_tgc_Mid2_eta = 99999.;
  m_tgc_Mid2_phi = 99999.;
  m_tgc_Mid2_r   = 99999.;
  m_tgc_Mid2_z   = 99999.;
    
  m_tgc_Mid_rho_chi2 = -99999.;
  m_tgc_Mid_rho_N    = 0;
  m_tgc_Mid_phi_chi2 = -99999.;
  m_tgc_Mid_phi_N    = 0;
    
  m_tgc_Inn_eta = 99999.;
  m_tgc_Inn_phi = 99999.;
  m_tgc_Inn_r   = 99999.;
  m_tgc_Inn_z   = 99999.;
  m_tgc_Inn_rho_std = 0.;
  m_tgc_Inn_rho_N   = 0;
  m_tgc_Inn_phi_std = 0.;
  m_tgc_Inn_phi_N   = 0;
    
  m_tgc_PT = 99999.;
  
  m_type         = 99999;
  m_t_x_center   = 99999.;
  m_t_y_center   = 99999.;
  m_t_radius     = 99999.;
  m_t_ap         = 99999.;
  m_t_aphi       = 99999.;
  m_t_phi        = 99999.;
  m_extrInnerEta = 99999.;
  m_extrInnerPhi = 99999.;  
  
  
  m_mdt_Inner_slope      = 99999.;
  m_mdt_Inner_intercept  = 99999.;
  m_mdt_Inner_Z          = 99999.;
  m_mdt_Inner_R          = 99999.;
  m_mdt_Inner_fit_chi    = -99999.;
  m_mdt_Middle_slope     = 99999.;
  m_mdt_Middle_intercept = 99999.;
  m_mdt_Middle_Z         = 99999.;
  m_mdt_Middle_R         = 99999.; 
  m_mdt_Middle_fit_chi   = -99999.;
  m_mdt_Outer_slope      = 99999.;
  m_mdt_Outer_intercept  = 99999.;
  m_mdt_Outer_Z          = 99999.;
  m_mdt_Outer_R          = 99999.;
  m_mdt_Outer_fit_chi    = -99999.;
    
  m_Address      = 99999;  
  m_Sagitta      = 99999.;
  m_Radius       = -99999.;
  m_Slope        = 99999.;
  m_Intercept    = 99999.;
  m_Alpha        = 99999.;
  m_Beta         = 99999.;
  m_DeltaR       = 99999.;
  m_Speed_over_c = 99999.;
  m_Eta          = 99999.;
  m_PhiMap       = 99999.;
  m_Phi          = 99999.;
  m_PhiDir       = 99999.;
  m_Pt           = 99999.;
  m_Charge       = 99999.;
}

void MuonFeatureDetails::init_vectors() {

  std::vector< std::vector<int  > > vec_int  (2, std::vector<int  >(4,99999));
  std::vector< std::vector<float> > vec_float(2, std::vector<float>(4,99999.));
  
  m_chamber_type_1 = vec_int;
  m_chamber_type_2 = vec_int;
  m_pos            = vec_int;
  m_aw             = vec_float;
  m_bw             = vec_float;
  m_zetaMin        = vec_float;
  m_zetaMax        = vec_float;
  m_radMin         = vec_float;
  m_radMax         = vec_float;
  m_etaMin         = vec_float;
  m_etaMax         = vec_float;
  m_st_phi         = vec_float;
  m_st_roads       = vec_float;
  
  if(m_extension_capacity>0) {
    m_extension0.reserve( (unsigned int)m_extension_capacity);
    m_extension1.reserve( (unsigned int)m_extension_capacity);
    m_extension2.reserve( (unsigned int)m_extension_capacity);
    m_extension3.reserve( (unsigned int)m_extension_capacity);
    m_extension4.reserve( (unsigned int)m_extension_capacity);
    m_extension5.reserve( (unsigned int)m_extension_capacity);
    m_extension6.reserve( (unsigned int)m_extension_capacity);
    m_extension7.reserve( (unsigned int)m_extension_capacity);
    m_extension8.reserve( (unsigned int)m_extension_capacity);
    m_extension9.reserve( (unsigned int)m_extension_capacity);
  }
    
  if(m_max_rob_capacity>0) {
    m_rob_id.reserve( (unsigned int)m_max_rob_capacity);
    m_removed_rob_id.reserve( (unsigned int)m_max_rob_capacity);
  }
    
  if(m_max_csm_capacity>0) {
    m_csm_id.reserve( (unsigned int)m_max_csm_capacity);
    m_csm_size.reserve( (unsigned int)m_max_csm_capacity);
    m_csm_error.reserve( (unsigned int)m_max_csm_capacity);
    m_removed_csm_id.reserve( (unsigned int)m_max_csm_capacity);
  }
    
  if(m_max_lvl1_emu_capacity>0) {
    m_lvl1_emulation.reserve( (unsigned int)m_max_lvl1_emu_capacity);
  }
    
  if(m_max_rpc_hits_capacity>0) {
    m_pad_hit_onlineId.reserve( (unsigned int)m_max_rpc_hits_capacity);
    m_pad_hit_code.reserve( (unsigned int)m_max_rpc_hits_capacity);
    m_pad_hit_x.reserve( (unsigned int)m_max_rpc_hits_capacity);
    m_pad_hit_y.reserve( (unsigned int)m_max_rpc_hits_capacity);
    m_pad_hit_z.reserve( (unsigned int)m_max_rpc_hits_capacity);
    m_pad_hit_r.reserve( (unsigned int)m_max_rpc_hits_capacity);
    m_pad_hit_p.reserve( (unsigned int)m_max_rpc_hits_capacity);
  }
    
  if(m_max_tgc_hits_capacity>0) {
    m_tgc_Inn_rho_hit_phi.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_rho_hit_r.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_rho_hit_z.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_rho_hit_width.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_rho_hit_in_seg.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_phi_hit_phi.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_phi_hit_r.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_phi_hit_z.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_phi_hit_width.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Inn_phi_hit_in_seg.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_rho_hit_phi.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_rho_hit_r.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_rho_hit_z.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_rho_hit_width.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_rho_hit_in_seg.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_phi_hit_phi.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_phi_hit_r.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_phi_hit_z.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_phi_hit_width.reserve( (unsigned int)m_max_tgc_hits_capacity);
    m_tgc_Mid_phi_hit_in_seg.reserve( (unsigned int)m_max_tgc_hits_capacity);
  }
    
  if(m_max_mdt_hits_capacity>0) {
    m_mdt_onlineId.reserve( (unsigned int)m_max_mdt_hits_capacity);
    m_mdt_offlineId.reserve( (unsigned int)m_max_mdt_hits_capacity);
    m_mdt_tube_r.reserve( (unsigned int)m_max_mdt_hits_capacity);
    m_mdt_tube_z.reserve( (unsigned int)m_max_mdt_hits_capacity);
    m_mdt_tube_residual.reserve( (unsigned int)m_max_mdt_hits_capacity);
    m_mdt_tube_time.reserve( (unsigned int)m_max_mdt_hits_capacity);
    m_mdt_tube_space.reserve( (unsigned int)m_max_mdt_hits_capacity);
    m_mdt_tube_sigma.reserve( (unsigned int)m_max_mdt_hits_capacity);
  }
}

void MuonFeatureDetails::copy_variables(MuonFeatureDetails& destination,
                                        const MuonFeatureDetails& source) {
    
  destination.m_id    = source.id();
  destination.m_te_id = source.te_id();
  destination.m_error = source.error();

  destination.m_lvl1_id      = source.lvl1_id();
  destination.m_lumi_block   = source.lumi_block();
  destination.m_muondetmask  = source.muondetmask(); 
  destination.m_RoIID        = source.roi_id();
  destination.m_RoISystem    = source.roi_system();
  destination.m_RoISubsystem = source.roi_subsystem(); 
  destination.m_RoISector    = source.roi_sector();
  destination.m_RoINumber    = source.roi_number();
  destination.m_RoIThreshold = source.roi_threshold();
  destination.m_RoIEta       = source.roi_eta();
  destination.m_RoIPhi       = source.roi_phi();  
  
  destination.m_rpc_pad_error = source.rpc_pad_error();
  destination.m_tgc_rdo_error = source.tgc_rdo_error();
    
  destination.m_rpc1_x = source.rpc1_x();
  destination.m_rpc1_y = source.rpc1_y();
  destination.m_rpc1_z = source.rpc1_z();
     
  destination.m_rpc2_x = source.rpc2_x();
  destination.m_rpc2_y = source.rpc2_y();
  destination.m_rpc2_z = source.rpc2_z();
    
  destination.m_rpc3_x = source.rpc3_x();
  destination.m_rpc3_y = source.rpc3_y(); 
  destination.m_rpc3_z = source.rpc3_z(); 
    
  destination.m_tgc_Mid1_eta = source.tgc_Mid1_eta();
  destination.m_tgc_Mid1_phi = source.tgc_Mid1_phi();
  destination.m_tgc_Mid1_r   = source.tgc_Mid1_r();
  destination.m_tgc_Mid1_z   = source.tgc_Mid1_z();
    
  destination.m_tgc_Mid2_eta = source.tgc_Mid2_eta();
  destination.m_tgc_Mid2_phi = source.tgc_Mid2_phi();
  destination.m_tgc_Mid2_r   = source.tgc_Mid2_r();
  destination.m_tgc_Mid2_z   = source.tgc_Mid2_z();
    
  destination.m_tgc_Mid_rho_chi2 = source.tgc_Mid_rho_chi2();
  destination.m_tgc_Mid_rho_N    = source.tgc_Mid_rho_N();
  destination.m_tgc_Mid_phi_chi2 = source.tgc_Mid_phi_chi2();
  destination.m_tgc_Mid_phi_N    = source.tgc_Mid_phi_N();
    
  destination.m_tgc_Inn_eta = source.tgc_Inn_eta();
  destination.m_tgc_Inn_phi = source.tgc_Inn_phi();
  destination.m_tgc_Inn_r   = source.tgc_Inn_r();
  destination.m_tgc_Inn_z   = source.tgc_Inn_z();
  destination.m_tgc_Inn_rho_std = source.tgc_Inn_rho_std();
  destination.m_tgc_Inn_rho_N   = source.tgc_Inn_rho_N();
  destination.m_tgc_Inn_phi_std = source.tgc_Inn_phi_std();
  destination.m_tgc_Inn_phi_N   = source.tgc_Inn_phi_N();
    
  destination.m_tgc_PT = source.tgc_PT();
  
  destination.m_type         = source.type();
  destination.m_t_x_center   = source.t_x_center();
  destination.m_t_y_center   = source.t_y_center();
  destination.m_t_radius     = source.t_radius();
  destination.m_t_ap         = source.t_ap();
  destination.m_t_aphi       = source.t_aphi();
  destination.m_t_phi        = source.t_phi();
  destination.m_extrInnerEta = source.extrInnerEta();
  destination.m_extrInnerPhi = source.extrInnerPhi();
      
  destination.m_mdt_Inner_slope      = source.mdt_Inner_slope();
  destination.m_mdt_Inner_intercept  = source.mdt_Inner_intercept();
  destination.m_mdt_Inner_Z          = source.mdt_Inner_Z();
  destination.m_mdt_Inner_R          = source.mdt_Inner_R();
  destination.m_mdt_Inner_fit_chi    = source.mdt_Inner_fit_chi();
  destination.m_mdt_Middle_slope     = source.mdt_Middle_slope();
  destination.m_mdt_Middle_intercept = source.mdt_Middle_intercept();
  destination.m_mdt_Middle_Z         = source.mdt_Middle_Z();
  destination.m_mdt_Middle_R         = source.mdt_Middle_R(); 
  destination.m_mdt_Middle_fit_chi   = source.mdt_Middle_fit_chi();
  destination.m_mdt_Outer_slope      = source.mdt_Outer_slope();
  destination.m_mdt_Outer_intercept  = source.mdt_Outer_intercept();
  destination.m_mdt_Outer_Z          = source.mdt_Outer_Z();
  destination.m_mdt_Outer_R          = source.mdt_Outer_R();
  destination.m_mdt_Outer_fit_chi    = source.mdt_Outer_fit_chi();
    
  destination.m_Address      = source.Address();
  destination.m_Sagitta      = source.Sagitta();
  destination.m_Radius       = source.Radius();
  destination.m_Slope        = source.Slope();
  destination.m_Intercept    = source.Intercept();
  destination.m_Alpha        = source.Alpha();
  destination.m_Beta         = source.Beta();
  destination.m_DeltaR       = source.DeltaR();
  destination.m_Speed_over_c = source.Speed_over_c();
  destination.m_Eta          = source.Eta();
  destination.m_PhiMap       = source.PhiMap();
  destination.m_Phi          = source.Phi();
  destination.m_PhiDir       = source.PhiDir();
  destination.m_Pt           = source.Pt();
  destination.m_Charge       = source.Charge();
}

template <class T>
void MuonFeatureDetails::copy_vector(std::vector<T>& dest,
                                     const std::vector<T>& source,
			             int resize) {
  if (resize>=0) {
    std::vector<T> v;
    if (resize>0) v.reserve(resize);
    dest.swap(v);
  }
  dest = source;

}

 void MuonFeatureDetails::setError(uint32_t err)        {m_error = err;}
 void MuonFeatureDetails::setRpcPadError(uint32_t err)  {m_rpc_pad_error = err;}
 void MuonFeatureDetails::setTgcPadError(uint32_t err)  {m_tgc_rdo_error = err;}
 void MuonFeatureDetails::setTgcPt(float tgc_pt)        {m_tgc_PT = tgc_pt;}
 void MuonFeatureDetails::setAddress(int address)       {m_Address = address;}
 void MuonFeatureDetails::setSagitta(float sagitta)     {m_Sagitta = sagitta;}
 void MuonFeatureDetails::setRadius(float radius)       {m_Radius = radius;}
 void MuonFeatureDetails::setSlope(float slope)         {m_Slope = slope;}
 void MuonFeatureDetails::setIntercept(float intercept) {m_Intercept = intercept;}
 void MuonFeatureDetails::setAlpha(float alpha)         {m_Alpha = alpha;}
 void MuonFeatureDetails::setBeta(float beta)           {m_Beta = beta;}
 void MuonFeatureDetails::setDeltaR(float deltaR)       {m_DeltaR = deltaR;}
 void MuonFeatureDetails::setSpeed_over_c(float speed)  {m_Speed_over_c = speed;}
 void MuonFeatureDetails::setEta(float eta)             {m_Eta = eta;}
 void MuonFeatureDetails::setPhiMap(float phimap)       {m_PhiMap = phimap;}
 void MuonFeatureDetails::setPhi(float phi)             {m_Phi = phi;}
 void MuonFeatureDetails::setPhiDir(float dir_phi)      {m_PhiDir = dir_phi;}
 void MuonFeatureDetails::setPt(float pt)               {m_Pt = pt;}
 void MuonFeatureDetails::setCharge(float charge)       {m_Charge = charge;}
     
 void MuonFeatureDetails::setRpc1(float x, float y, float z) {
  m_rpc1_x = x;
  m_rpc1_y = y;
  m_rpc1_z = z;
}

 void MuonFeatureDetails::setRpc2(float x, float y, float z) {
  m_rpc2_x = x;
  m_rpc2_y = y;
  m_rpc2_z = z;
}

 void MuonFeatureDetails::setRpc3(float x, float y, float z) {
  m_rpc3_x = x;
  m_rpc3_y = y;
  m_rpc3_z = z; 
}

 void MuonFeatureDetails::setTgcMid1(float Mid1_eta, float Mid1_phi, 
                                           float Mid1_r,   float Mid1_z) {
  m_tgc_Mid1_eta = Mid1_eta;
  m_tgc_Mid1_phi = Mid1_phi;
  m_tgc_Mid1_r   = Mid1_r;
  m_tgc_Mid1_z   = Mid1_z;
}

 void MuonFeatureDetails::setTgcMid2(float Mid2_eta, float Mid2_phi, 
                                           float Mid2_r,   float Mid2_z) {
  m_tgc_Mid2_eta = Mid2_eta;
  m_tgc_Mid2_phi = Mid2_phi;
  m_tgc_Mid2_r   = Mid2_r;
  m_tgc_Mid2_z   = Mid2_z;
}

 void MuonFeatureDetails::setTgcMidF(float rho_chi2, long rho_n,     
                                           float phi_chi2, long phi_n) {
  m_tgc_Mid_rho_chi2 = rho_chi2;
  m_tgc_Mid_rho_N    = rho_n;
  m_tgc_Mid_phi_chi2 = phi_chi2;
  m_tgc_Mid_phi_N    = phi_n;
}

 void MuonFeatureDetails::setTgcInn (float Inn_eta,  float Inn_phi,  
                                           float Inn_r,    float Inn_z) {
  m_tgc_Inn_eta = Inn_eta;
  m_tgc_Inn_phi = Inn_phi;
  m_tgc_Inn_r   = Inn_r;
  m_tgc_Inn_z   = Inn_z;
}

 void MuonFeatureDetails::setTgcInnF(float rho_std,  long rho_n,     
                                           float phi_std,  long phi_n) {
  m_tgc_Inn_rho_std = rho_std;
  m_tgc_Inn_rho_N   = rho_n;
  m_tgc_Inn_phi_std = phi_std;
  m_tgc_Inn_phi_N   = phi_n;
}



 void MuonFeatureDetails::setType(int t)                {m_type = t;}
 void MuonFeatureDetails::setT_x_center(float x)        {m_t_x_center = x;}
 void MuonFeatureDetails::setT_y_center(float y)        {m_t_y_center = y;}
 void MuonFeatureDetails::setT_radius(float r)          {m_t_radius = r;}
 void MuonFeatureDetails::setT_ap(float ap)             {m_t_ap = ap;}
 void MuonFeatureDetails::setT_aphi(float aphi)         {m_t_aphi = aphi;}
 void MuonFeatureDetails::setT_phi(float phi)           {m_t_phi = phi;}
 void MuonFeatureDetails::setExtrInnerEta(float extEta) {m_extrInnerEta=extEta;}
 void MuonFeatureDetails::setExtrInnerPhi(float extPhi) {m_extrInnerPhi=extPhi;}
    
 void MuonFeatureDetails::setChamber_type_1(int tr, int st, int ty) {
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_chamber_type_1[tr][st] = ty;
 }
 
 void MuonFeatureDetails::setChamber_type_1(const std::vector< std::vector<int> >& v)
 {
   m_chamber_type_1 = v;
 }

 void MuonFeatureDetails::setChamber_type_2(int tr, int st, int ty){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_chamber_type_2[tr][st] = ty; 
 }
 
 void MuonFeatureDetails::setChamber_type_2(const std::vector< std::vector<int> >& v)
 {
   m_chamber_type_2 = v;
 }

 void MuonFeatureDetails::setPos(int tr, int st, int pos){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_pos[tr][st] = pos; 
 }
 
 void MuonFeatureDetails::setPos(const std::vector< std::vector<int> >& v)
 {
   m_pos = v;
 }

 void MuonFeatureDetails::setAw(int tr, int st, float aw){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_aw[tr][st] = aw;
 }
 
 void MuonFeatureDetails::setAw(const std::vector< std::vector<float> >& v)
 {
   m_aw = v;
 }

 void MuonFeatureDetails::setBw(int tr, int st, float bw){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_bw[tr][st] = bw;
 }
 
 void MuonFeatureDetails::setBw(const std::vector< std::vector<float> >& v)
 {
   m_bw = v;
 }

 void MuonFeatureDetails::setZetaMin(int tr, int st, float min){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_zetaMin[tr][st] = min;
 }
  
 void MuonFeatureDetails::setZetaMin(const std::vector< std::vector<float> >& v)
 {
   m_zetaMin = v;
 }

 void MuonFeatureDetails::setZetaMax(int tr, int st, float max){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_zetaMax[tr][st] = max;
 }
 
 void MuonFeatureDetails::setZetaMax(const std::vector< std::vector<float> >& v)
 {
   m_zetaMax = v;
 }

 void MuonFeatureDetails::setRadMin (int tr, int st, float min){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_radMin[tr][st] = min;
 }
 
 void MuonFeatureDetails::setRadMin(const std::vector< std::vector<float> >& v)
 {
   m_radMin = v;
 }

 void MuonFeatureDetails::setRadMax (int tr, int st, float max){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_radMax[tr][st] = max;
 }
 
 void MuonFeatureDetails::setRadMax(const std::vector< std::vector<float> >& v)
 {
   m_radMax = v;
 }

 void MuonFeatureDetails::setEtaMin (int tr, int st, float min){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_etaMin[tr][st] = min;
 }
 
 void MuonFeatureDetails::setEtaMin(const std::vector< std::vector<float> >& v)
 {
   m_etaMin = v;
 }

 void MuonFeatureDetails::setEtaMax (int tr, int st, float max){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_etaMax[tr][st] = max;
 }
 
 void MuonFeatureDetails::setEtaMax(const std::vector< std::vector<float> >& v)
 {
   m_etaMax = v;
 }

 void MuonFeatureDetails::setSt_phi (int tr, int st, float phi){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_st_phi[tr][st] = phi;
 }
 
 void MuonFeatureDetails::setSt_phi(const std::vector< std::vector<float> >& v)
 {
   m_st_phi = v;
 }

 void MuonFeatureDetails::setSt_roads(int tr, int st, float road){
   if (tr<0 || tr>1) return;
   if (st<0 || st>3) return;
   m_st_roads[tr][st] = road;
 }

 void MuonFeatureDetails::setSt_roads(const std::vector< std::vector<float> >& v)
 {
   m_st_roads = v;
 }

 void MuonFeatureDetails::setEtaPivot(uint32_t ly0, uint32_t ly1) {
  if(m_max_lvl1_emu_capacity==0) return;
  if(m_lvl1_emulation.size() < 2) m_lvl1_emulation.resize(2);
  uint32_t myarray [] = { ly0,ly1 };
  m_lvl1_emulation.insert(m_lvl1_emulation.begin(),myarray,myarray+2);
}

 void MuonFeatureDetails::setEtaLow (uint32_t ly00, uint32_t ly01,
                                           uint32_t ly10, uint32_t ly11) {
  if(m_max_lvl1_emu_capacity==0) return;
  if(m_lvl1_emulation.size() < 6) m_lvl1_emulation.resize(6);
  uint32_t myarray [] = { ly00,ly01,ly10,ly11 };
  m_lvl1_emulation.insert(m_lvl1_emulation.begin()+2,myarray,myarray+4);
}

 void MuonFeatureDetails::setEtaHigh(uint32_t ly00, uint32_t ly01, 
                                           uint32_t ly10, uint32_t ly11) {
  if(m_max_lvl1_emu_capacity==0) return;
  if(m_lvl1_emulation.size() < 10) m_lvl1_emulation.resize(10);
  uint32_t myarray [] = { ly00,ly01,ly10,ly11 };
  m_lvl1_emulation.insert(m_lvl1_emulation.begin()+6,myarray,myarray+4);

}

 void MuonFeatureDetails::setPhiPivot(uint32_t ly0, uint32_t ly1) {
  if(m_max_lvl1_emu_capacity==0) return;
  if(m_lvl1_emulation.size() < 12) m_lvl1_emulation.resize(12);
  uint32_t myarray [] = { ly0,ly1 };
  m_lvl1_emulation.insert(m_lvl1_emulation.begin(),myarray,myarray+2);
}

 void MuonFeatureDetails::setPhiLow (uint32_t ly00, uint32_t ly01, 
                                           uint32_t ly10, uint32_t ly11) {
  if(m_max_lvl1_emu_capacity==0) return;
  if(m_lvl1_emulation.size() < 16) m_lvl1_emulation.resize(16);
  uint32_t myarray [] = { ly00,ly01,ly10,ly11 };
  m_lvl1_emulation.insert(m_lvl1_emulation.begin()+12,myarray,myarray+4);
}

 void MuonFeatureDetails::setPhiHigh(uint32_t ly00, uint32_t ly01, 
                                           uint32_t ly10, uint32_t ly11) {
  if(m_max_lvl1_emu_capacity==0) return;
  if(m_lvl1_emulation.size() < 20) m_lvl1_emulation.resize(20);
  uint32_t myarray [] = { ly00,ly01,ly10,ly11 };
  m_lvl1_emulation.insert(m_lvl1_emulation.begin()+16,myarray,myarray+4);
}
    
 void MuonFeatureDetails::setLvl1Emulation (const std::vector<uint32_t>& v)
 {
   m_lvl1_emulation = v;
 }

 void MuonFeatureDetails::setSP1(float slope,float intercept,float z,float r,float chisquare){
  m_mdt_Inner_slope = slope;
  m_mdt_Inner_intercept = intercept;
  m_mdt_Inner_Z = z;
  m_mdt_Inner_R = r;
  m_mdt_Inner_fit_chi = chisquare;
}

 void MuonFeatureDetails::setSP2(float slope,float intercept,float z,float r,float chisquare){
  m_mdt_Middle_slope = slope;
  m_mdt_Middle_intercept = intercept;
  m_mdt_Middle_Z = z;
  m_mdt_Middle_R = r;
  m_mdt_Middle_fit_chi = chisquare;
}

 void MuonFeatureDetails::setSP3(float slope,float intercept,float z,float r,float chisquare){
  m_mdt_Outer_slope = slope;
  m_mdt_Outer_intercept = intercept;
  m_mdt_Outer_Z = z;
  m_mdt_Outer_R = r;
  m_mdt_Outer_fit_chi = chisquare;
}

 void MuonFeatureDetails::setPadHit(uint32_t onlineId, uint32_t code, 
                               float x, float y, float z, float r, float p) {
  if (m_max_rpc_hits_capacity >= 0)
    if (m_pad_hit_onlineId.size() == (unsigned int)m_max_rpc_hits_capacity) return;

  m_pad_hit_onlineId.push_back(onlineId);
  m_pad_hit_code.push_back(code);
  m_pad_hit_x.push_back(x);
  m_pad_hit_y.push_back(y);
  m_pad_hit_z.push_back(z);
  m_pad_hit_r.push_back(r);
  m_pad_hit_p.push_back(p);
}

 void MuonFeatureDetails::setTgcMidRhoHit(float phi,float r,float z,
                                                float width, uint32_t in_seg) {
  if(m_max_tgc_hits_capacity>=0)
    if(m_tgc_Mid_rho_hit_phi.size() == (unsigned int)m_max_tgc_hits_capacity) return;
    
  m_tgc_Mid_rho_hit_phi.push_back(phi);
  m_tgc_Mid_rho_hit_r.push_back(r);
  m_tgc_Mid_rho_hit_z.push_back(z);
  m_tgc_Mid_rho_hit_width.push_back(width);
  m_tgc_Mid_rho_hit_in_seg.push_back(in_seg);
}

 void MuonFeatureDetails::setTgcMidPhiHit(float phi,float r,float z,
                                                float width, uint32_t in_seg) {
  if(m_max_tgc_hits_capacity>=0)
    if(m_tgc_Mid_rho_hit_phi.size() == (unsigned int)m_max_tgc_hits_capacity) return;

  m_tgc_Mid_phi_hit_phi.push_back(phi);
  m_tgc_Mid_phi_hit_r.push_back(r);
  m_tgc_Mid_phi_hit_z.push_back(z);
  m_tgc_Mid_phi_hit_width.push_back(width);
  m_tgc_Mid_phi_hit_in_seg.push_back(in_seg);
}

 void MuonFeatureDetails::setTgcInnRhoHit(float phi,float r,float z,
                                                float width, uint32_t in_seg) {
  if(m_max_tgc_hits_capacity>=0)
    if(m_tgc_Inn_rho_hit_phi.size() == (unsigned int)m_max_tgc_hits_capacity) return;

  m_tgc_Inn_rho_hit_phi.push_back(phi);
  m_tgc_Inn_rho_hit_r.push_back(r);
  m_tgc_Inn_rho_hit_z.push_back(z);
  m_tgc_Inn_rho_hit_width.push_back(width);
  m_tgc_Inn_rho_hit_in_seg.push_back(in_seg);
}    

 void MuonFeatureDetails::setTgcInnPhiHit(float phi,float r,float z,
                                                float width, uint32_t in_seg) {
  if(m_max_tgc_hits_capacity>=0)
    if(m_tgc_Inn_phi_hit_phi.size() == (unsigned int)m_max_tgc_hits_capacity) return;
  
  m_tgc_Inn_phi_hit_phi.push_back(phi);
  m_tgc_Inn_phi_hit_r.push_back(r);
  m_tgc_Inn_phi_hit_z.push_back(z);
  m_tgc_Inn_phi_hit_width.push_back(width);
  m_tgc_Inn_phi_hit_in_seg.push_back(in_seg); 
}


 void MuonFeatureDetails::setMdtHit(uint32_t onlineId, uint32_t offlineId,
    float r, float z, float residual, float time, float space, float sigma) {
  if(m_max_mdt_hits_capacity>=0)
    if(m_mdt_onlineId.size() == (unsigned int)m_max_mdt_hits_capacity) return;

  m_mdt_onlineId.push_back(onlineId);
  m_mdt_offlineId.push_back(offlineId);
  m_mdt_tube_r.push_back(r);
  m_mdt_tube_z.push_back(z);
  m_mdt_tube_residual.push_back(residual);
  m_mdt_tube_time.push_back(time);
  m_mdt_tube_space.push_back(space);
  m_mdt_tube_sigma.push_back(sigma);
}

 void MuonFeatureDetails::setRequestedRob(uint32_t rob) {
  if (m_max_rob_capacity >= 0)
    if(m_rob_id.size() == (unsigned int)m_max_rob_capacity) return;
  
  m_rob_id.push_back(rob);
}

 void MuonFeatureDetails::setRequestedRob(const std::vector<uint32_t>& v)
 {
   m_rob_id = v;
 }

 void MuonFeatureDetails::setConvertedCsm(uint32_t csm, uint32_t sz) {
  if (m_max_csm_capacity >= 0)
    if(m_csm_id.size() == (unsigned int)m_max_csm_capacity) return;
  
  m_csm_id.push_back(csm);
  m_csm_size.push_back(sz);
}

 void MuonFeatureDetails::setConvertedCsm(const std::vector<uint32_t>& v)
 {
   m_csm_id = v;
 }

 void MuonFeatureDetails::setConvertedCsmSize(const std::vector<uint32_t>& v)
 {
   m_csm_size = v;
 }

 void MuonFeatureDetails::setCsmError(uint32_t err) {
  if (m_max_csm_capacity >= 0)
    if(m_csm_error.size() == (unsigned int)m_max_csm_capacity) return;
  
  m_csm_error.push_back(err);
}


 void MuonFeatureDetails::setCsmError(const std::vector<uint32_t>& v)
 {
   m_csm_error = v;
 }

 void MuonFeatureDetails::setRemovedRob(uint32_t rob) {
  if (m_max_rob_capacity >= 0)
    if(m_removed_rob_id.size() == (unsigned int)m_max_rob_capacity) return;
  
  m_removed_rob_id.push_back(rob);
}
 
 void MuonFeatureDetails::setRemovedRob(const std::vector<uint32_t>& v)
 {
   m_removed_rob_id = v;
 }

 void MuonFeatureDetails::setRemovedCsm(uint32_t csm) {
  if (m_max_csm_capacity >= 0)
    if(m_removed_csm_id.size() == (unsigned int)m_max_csm_capacity) return;
  
  m_removed_csm_id.push_back(csm);
}

 void MuonFeatureDetails::setRemovedCsm(const std::vector<uint32_t>& v)
 {
   m_removed_csm_id = v;
 }

 void MuonFeatureDetails::setExtension0(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension0.size() == (unsigned int)m_extension_capacity) return;
  m_extension0.push_back(ext);
}

 void MuonFeatureDetails::setExtension0(const std::vector<uint32_t>& v)
 {
   m_extension0 = v;
 }

 void MuonFeatureDetails::setExtension1(uint32_t ext) {  
  if (m_extension_capacity >= 0)
    if(m_extension1.size() == (unsigned int)m_extension_capacity) return;
  m_extension1.push_back(ext);
}

 void MuonFeatureDetails::setExtension1(const std::vector<uint32_t>& v)
 {
   m_extension1 = v;
 }

 void MuonFeatureDetails::setExtension2(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension2.size() == (unsigned int)m_extension_capacity) return;
  m_extension2.push_back(ext);
}

 void MuonFeatureDetails::setExtension2(const std::vector<uint32_t>& v)
 {
   m_extension2 = v;
 }

 void MuonFeatureDetails::setExtension3(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension3.size() == (unsigned int)m_extension_capacity) return;
  m_extension3.push_back(ext);
}

 void MuonFeatureDetails::setExtension3(const std::vector<uint32_t>& v)
 {
   m_extension3 = v;
 }

 void MuonFeatureDetails::setExtension4(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension4.size() == (unsigned int)m_extension_capacity) return;
  m_extension4.push_back(ext);
}

 void MuonFeatureDetails::setExtension4(const std::vector<uint32_t>& v)
 {
   m_extension4 = v;
 }

 void MuonFeatureDetails::setExtension5(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension5.size() == (unsigned int)m_extension_capacity) return;
  m_extension5.push_back(ext);
}

 void MuonFeatureDetails::setExtension5(const std::vector<uint32_t>& v)
 {
   m_extension5 = v;
 }

 void MuonFeatureDetails::setExtension6(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension6.size() == (unsigned int)m_extension_capacity) return;
  m_extension6.push_back(ext);
}

 void MuonFeatureDetails::setExtension6(const std::vector<uint32_t>& v)
 {
   m_extension6 = v;
 }

 void MuonFeatureDetails::setExtension7(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension7.size() == (unsigned int)m_extension_capacity) return;
  m_extension7.push_back(ext);
}

 void MuonFeatureDetails::setExtension7(const std::vector<uint32_t>& v)
 {
   m_extension7 = v;
 }

 void MuonFeatureDetails::setExtension8(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension8.size() == (unsigned int)m_extension_capacity) return;
  m_extension8.push_back(ext);
}

 void MuonFeatureDetails::setExtension8(const std::vector<uint32_t>& v)
 {
   m_extension8 = v;
 }

 void MuonFeatureDetails::setExtension9(uint32_t ext) {
  if (m_extension_capacity >= 0)
    if(m_extension9.size() == (unsigned int)m_extension_capacity) return;
  m_extension9.push_back(ext);
}

 void MuonFeatureDetails::setExtension9(const std::vector<uint32_t>& v)
 {
   m_extension9 = v;
 }

bool MuonFeatureDetails::hasSameTrackReco(const MuonFeatureDetails& d) const {
  bool result = true;
  result &= (m_Address == d.Address() );  
  result &= (fabs (m_Sagitta - d.Sagitta())           < 0.01);
  result &= (fabs (m_Radius - d.Radius())             < 0.01);
  result &= (fabs (m_Slope - d.Slope())               < 0.01);
  result &= (fabs (m_Intercept - d.Intercept())       < 0.01);
  result &= (fabs (m_Alpha - d.Alpha())               < 0.01);
  result &= (fabs (m_Beta - d.Beta())                 < 0.01);
  result &= (fabs (m_DeltaR - d.DeltaR())             < 0.01);
  result &= (fabs (m_Speed_over_c - d.Speed_over_c()) < 0.01);
  result &= (fabs (m_Phi - d.Phi())                   < 0.01);
  result &= (fabs (m_PhiDir - d.PhiDir())             < 0.01);
  return result;
}

bool MuonFeatureDetails::operator==(const MuonFeatureDetails& d) const {
  bool result = hasSameTrackReco(d);
  result &= (fabs (m_Eta - d.Eta())       < 0.01);
  result &= (fabs (m_Phi - d.Phi())       < 0.01);
  result &= (fabs (m_Pt - d.Pt())         < 0.01);
  result &= (fabs (m_Charge - d.Charge()) < 0.01);
  return result;
}

bool MuonFeatureDetails::hasSameRoIOf(const MuonFeatureDetails& d) const {
  bool result = true;
  result &= (m_lvl1_id                == d.lvl1_id() );
  result &= (m_lumi_block             == d.lumi_block() );
  result &= (m_RoIID                  == d.roi_id() );
  result &= (m_RoISystem              == d.roi_system() );
  result &= (m_RoISubsystem           == d.roi_subsystem()); 
  result &= (m_RoISector              == d.roi_sector() );
  result &= (m_RoINumber              == d.roi_number() );
  result &= (m_RoIThreshold           == d.roi_threshold() );
  return result;
}

bool MuonFeatureDetails::isEqual(const MuonFeatureDetails& d) const {
  return (m_id == d.id() ) & this->hasSameContentOf(d);
}

bool MuonFeatureDetails::hasSameContentOf(const MuonFeatureDetails& d) const {
  bool result = true;
  
  result &= (m_te_id                  == d.te_id() );
  result &= (m_error                  == d.error() );


  result &= (m_lvl1_id                == d.lvl1_id() );
  result &= (m_lumi_block             == d.lumi_block() );
  result &= (m_muondetmask            == d.muondetmask() ); 
  result &= (m_RoIID                  == d.roi_id() );
  result &= (m_RoISystem              == d.roi_system() );
  result &= (m_RoISubsystem           == d.roi_subsystem()); 
  result &= (m_RoISector              == d.roi_sector() );
  result &= (m_RoINumber              == d.roi_number() );
  result &= (m_RoIThreshold           == d.roi_threshold() );
  result &= (m_RoIEta                 == d.roi_eta() );
  result &= (m_RoIPhi                 == d.roi_phi() );  
    
  result &= (m_rpc_pad_error          == d.rpc_pad_error() );
  result &= (m_tgc_rdo_error          == d.tgc_rdo_error() );
   
  result &= (m_rpc1_x                 == d.rpc1_x() );
  result &= (m_rpc1_y                 == d.rpc1_y() );
  result &= (m_rpc1_z                 == d.rpc1_z() );
     
  result &= (m_rpc2_x                 == d.rpc2_x() );
  result &= (m_rpc2_y                 == d.rpc2_y() );
  result &= (m_rpc2_z                 == d.rpc2_z() );
     
  result &= (m_rpc3_x                 == d.rpc3_x() );
  result &= (m_rpc3_y                 == d.rpc3_y() );
  result &= (m_rpc3_z                 == d.rpc3_z() ); 
    
  result &= (m_tgc_Mid1_eta           == d.tgc_Mid1_eta() );
  result &= (m_tgc_Mid1_phi           == d.tgc_Mid1_phi() );
  result &= (m_tgc_Mid1_r             == d.tgc_Mid1_r() );
  result &= (m_tgc_Mid1_z             == d.tgc_Mid1_z() );
    
  result &= (m_tgc_Mid2_eta           == d.tgc_Mid2_eta() );
  result &= (m_tgc_Mid2_phi           == d.tgc_Mid2_phi() );
  result &= (m_tgc_Mid2_r             == d.tgc_Mid2_r() );
  result &= (m_tgc_Mid2_z             == d.tgc_Mid2_z() );
    
  result &= (m_tgc_Mid_rho_chi2       == d.tgc_Mid_rho_chi2() );
  result &= (m_tgc_Mid_rho_N          == d.tgc_Mid_rho_N() );
  result &= (m_tgc_Mid_phi_chi2       == d.tgc_Mid_phi_chi2() );
  result &= (m_tgc_Mid_phi_N          == d.tgc_Mid_phi_N() );
    
  result &= (m_tgc_Inn_eta            == d.tgc_Inn_eta() );
  result &= (m_tgc_Inn_phi            == d.tgc_Inn_phi() );
  result &= (m_tgc_Inn_r              == d.tgc_Inn_r() );
  result &= (m_tgc_Inn_z              == d.tgc_Inn_z() );
  result &= (m_tgc_Inn_rho_std        == d.tgc_Inn_rho_std() );
  result &= (m_tgc_Inn_rho_N          == d.tgc_Inn_rho_N() );
  result &= (m_tgc_Inn_phi_std        == d.tgc_Inn_phi_std () );
  result &= (m_tgc_Inn_phi_N          == d.tgc_Inn_phi_N() );
    
  result &= (m_tgc_PT                 == d.tgc_PT() );
  
  result &= (m_type                   == d.type() );
  result &= (m_t_x_center             == d.t_x_center() );
  result &= (m_t_y_center             == d.t_y_center() );
  result &= (m_t_radius               == d.t_radius() );
  result &= (m_t_ap                   == d.t_ap() );
  result &= (m_t_aphi                 == d.t_aphi() );
  result &= (m_t_phi                  == d.t_phi() );
  result &= (m_extrInnerEta           == d.extrInnerEta() );
  result &= (m_extrInnerPhi           == d.extrInnerPhi() );
   
  result &= (m_chamber_type_1         == d.chamber_type_1() );
  result &= (m_chamber_type_2         == d.chamber_type_2() );
  result &= (m_pos                    == d.pos() );
  result &= (m_aw                     == d.aw() );
  result &= (m_bw                     == d.bw() );
  result &= (m_zetaMin                == d.zetaMin() );
  result &= (m_zetaMax                == d.zetaMax() );
  result &= (m_radMin                 == d.radMin() );
  result &= (m_radMax                 == d.radMax() );
  result &= (m_etaMin                 == d.etaMin() );
  result &= (m_etaMax                 == d.etaMax() );
  result &= (m_st_phi                 == d.st_phi() );
  result &= (m_st_roads               == d.st_roads() );
  
    
  result &= (m_mdt_Inner_slope        == d.mdt_Inner_slope() );
  result &= (m_mdt_Inner_intercept    == d.mdt_Inner_intercept() );
  result &= (m_mdt_Inner_Z            == d.mdt_Inner_Z() );
  result &= (m_mdt_Inner_R            == d.mdt_Inner_R() );
  result &= (m_mdt_Inner_fit_chi      == d.mdt_Inner_fit_chi() );
  result &= (m_mdt_Middle_slope       == d.mdt_Middle_slope() );
  result &= (m_mdt_Middle_intercept   == d.mdt_Middle_intercept() );
  result &= (m_mdt_Middle_Z           == d.mdt_Middle_Z() );
  result &= (m_mdt_Middle_R           == d.mdt_Middle_R() );
  result &= (m_mdt_Middle_fit_chi     == d.mdt_Middle_fit_chi() );
  result &= (m_mdt_Outer_slope        == d.mdt_Outer_slope() );
  result &= (m_mdt_Outer_intercept    == d.mdt_Outer_intercept() );
  result &= (m_mdt_Outer_Z            == d.mdt_Outer_Z() );
  result &= (m_mdt_Outer_R            == d.mdt_Outer_R() );
  result &= (m_mdt_Outer_fit_chi      == d.mdt_Outer_fit_chi() );
    
  result &= (m_Address                == d.Address() );  
  result &= (m_Sagitta                == d.Sagitta() );
  result &= (m_Radius                 == d.Radius() );
  result &= (m_Slope                  == d.Slope() );
  result &= (m_Intercept              == d.Intercept() );
  result &= (m_Alpha                  == d.Alpha() );
  result &= (m_Beta                   == d.Beta() );
  result &= (m_DeltaR                 == d.DeltaR() );
  result &= (m_Speed_over_c           == d.Speed_over_c() );
  result &= (m_Eta                    == d.Eta() );
  result &= (m_PhiMap                 == d.PhiMap() );
  result &= (m_Phi                    == d.Phi() );
  result &= (m_PhiDir                 == d.PhiDir() );
  result &= (m_Pt                     == d.Pt() );
  result &= (m_Charge                 == d.Charge() );
    
  result &= (m_extension0             == d.extension0() );
  result &= (m_extension1             == d.extension1() );
  result &= (m_extension2             == d.extension2() );
  result &= (m_extension3             == d.extension3() );
  result &= (m_extension4             == d.extension4() );
  result &= (m_extension5             == d.extension5() );
  result &= (m_extension6             == d.extension6() );
  result &= (m_extension7             == d.extension7() );
  result &= (m_extension8             == d.extension8() );
  result &= (m_extension9             == d.extension9() );
    
  result &= (m_lvl1_emulation         == d.lvl1_emulation() );
  
  result &= (m_rob_id                 == d.rob_id() );
  result &= (m_csm_id                 == d.csm_id() );
  result &= (m_csm_size               == d.csm_size() );
  result &= (m_csm_error              == d.csm_error() );
  result &= (m_removed_rob_id         == d.removed_rob_id() );
  result &= (m_removed_csm_id         == d.removed_csm_id() );
    
  result &= (m_pad_hit_onlineId       == d.pad_hit_onlineId() );
  result &= (m_pad_hit_code           == d.pad_hit_code() );
  result &= (m_pad_hit_x              == d.pad_hit_x() );
  result &= (m_pad_hit_y              == d.pad_hit_y() );
  result &= (m_pad_hit_z              == d.pad_hit_z() );
  result &= (m_pad_hit_r              == d.pad_hit_r() );
  result &= (m_pad_hit_p              == d.pad_hit_p() );
    
  result &= (m_tgc_Inn_rho_hit_phi    == d.tgc_Inn_rho_hit_phi() );
  result &= (m_tgc_Inn_rho_hit_r      == d.tgc_Inn_rho_hit_r() );
  result &= (m_tgc_Inn_rho_hit_z      == d.tgc_Inn_rho_hit_z() );
  result &= (m_tgc_Inn_rho_hit_width  == d.tgc_Inn_rho_hit_width() );
  result &= (m_tgc_Inn_rho_hit_in_seg == d.tgc_Inn_rho_hit_in_seg() );
  result &= (m_tgc_Inn_phi_hit_phi    == d.tgc_Inn_phi_hit_phi() );
  result &= (m_tgc_Inn_phi_hit_r      == d.tgc_Inn_phi_hit_r() );
  result &= (m_tgc_Inn_phi_hit_z      == d.tgc_Inn_phi_hit_z() );
  result &= (m_tgc_Inn_phi_hit_width  == d.tgc_Inn_phi_hit_width() );
  result &= (m_tgc_Inn_phi_hit_in_seg == d.tgc_Inn_phi_hit_in_seg() );
  result &= (m_tgc_Mid_rho_hit_phi    == d.tgc_Mid_rho_hit_phi() );
  result &= (m_tgc_Mid_rho_hit_r      == d.tgc_Mid_rho_hit_r() );
  result &= (m_tgc_Mid_rho_hit_z      == d.tgc_Mid_rho_hit_z() );
  result &= (m_tgc_Mid_rho_hit_width  == d.tgc_Mid_rho_hit_width() );
  result &= (m_tgc_Mid_rho_hit_in_seg == d.tgc_Mid_rho_hit_in_seg() );
  result &= (m_tgc_Mid_phi_hit_phi    == d.tgc_Mid_phi_hit_phi() );
  result &= (m_tgc_Mid_phi_hit_r      == d.tgc_Mid_phi_hit_r() );
  result &= (m_tgc_Mid_phi_hit_z      == d.tgc_Mid_phi_hit_z() );
  result &= (m_tgc_Mid_phi_hit_width  == d.tgc_Mid_phi_hit_width() );
  result &= (m_tgc_Mid_phi_hit_in_seg == d.tgc_Mid_phi_hit_in_seg() );
    
  result &= (m_mdt_onlineId           == d.mdt_onlineId() );
  result &= (m_mdt_offlineId          == d.mdt_offlineId() );
  result &= (m_mdt_tube_r             == d.mdt_tube_r() );
  result &= (m_mdt_tube_z             == d.mdt_tube_z() );
  result &= (m_mdt_tube_residual      == d.mdt_tube_residual() );
  result &= (m_mdt_tube_time          == d.mdt_tube_time() );
  result &= (m_mdt_tube_space         == d.mdt_tube_space() );
  result &= (m_mdt_tube_sigma         == d.mdt_tube_sigma() );
  
  return result;
}

std::string str_bit(const uint32_t w, const unsigned int n) {

  int nbit = (n<=32)? n : 32;
  std::stringstream ss;
  
  for(int i=nbit-1;i>=0;--i) ss << ( ((w>>i)&0x1)? 1 : 0 );
  //ss << std::endl;
  
  return ss.str();
}

std::string MuonFeatureDetails::str() const {

  const MuonFeatureDetails& d = *this;
  
  std::string id;
  switch (d.id()) {
    
    case 0: id = "900 GEV Instance"; break;
    case 1: id = "MUON Instance"; break;
    case 2: id = "HALO Instance"; break;
    case 3: id = "COSMIC Instance"; break;
    default: id = "UNKNOWN Instance";
  }
    
  std::stringstream ss;

  ss << std::endl;
  ss << "  *********************************************************" << std::endl;
  ss << "  *             Dump of the muFast ESD content            *" << std::endl;
  ss << "  *********************************************************" << std::endl;
  ss << "  Instance          : " << id << std::endl;
  ss << "  TE id             : " << d.te_id() << std::endl; 
  ss << "  Error code        : 0x" << std::hex << d.error() << std::dec << std::endl; 
  ss << "  LVL1 ID           : " << d.lvl1_id() << std::endl;
  ss << "  Muon DetectorMask : " << /*str_bit(d.muondetmask(),10)*/d.muondetmask()  << std::endl;
  ss << "  RoI ID            : " << d.roi_id() << std::endl; 
  ss << "  RoI System        : " << d.roi_system() << std::endl;
  ss << "  RoI Subsystem     : " << d.roi_subsystem() << std::endl;
  ss << "  RoI Sector        : " << d.roi_sector() << std::endl;
  ss << "  RoI Number        : " << d.roi_number() << std::endl;
  ss << "  RoI Threshold     : " << d.roi_threshold() << std::endl;
  ss << "  RoI Eta position  : " << d.roi_eta() << std::endl;
  ss << "  RoI Phi position  : " << d.roi_phi() << std::endl;
  
  ss << "  ********* Level-2 Data Access ********" << std::endl;
  ss << "  Requested ROBs    :";
  for (unsigned int i=0; i<d.rob_id().size();++i) 
    ss << " 0x" << std::hex << d.rob_id()[i] << std::dec;
  ss << std::endl;
  
  ss << "  Removed ROBs      :";
  for (unsigned int i=0; i<d.removed_rob_id().size();++i) 
    ss << " 0x" << std::hex << d.removed_rob_id()[i] << std::dec;
  ss << std::endl;
  
  ss << "  RPC PAD access err: 0x" << std::hex << d.rpc_pad_error() << std::dec << std::endl;
  ss << "  TGC RDO access err: 0x" << std::hex << d.tgc_rdo_error() << std::dec << std::endl;
 
  ss << "  Requested CSMs    :";
  for (unsigned int i=0; i<d.csm_id().size(); ++i)
    ss << " " << d.csm_id()[i] << "(" << d.csm_size()[i] << ")";
  ss << std::endl;
  
  ss << "  Removed CSMs      :";
  for (unsigned int i=0; i<d.removed_csm_id().size(); ++i)
    ss << " " << d.removed_csm_id()[i];
  ss << std::endl;
  
  ss << "  CSMs access err   :";
  for (unsigned int i=0; i<d.csm_error().size(); ++i)
    ss << " 0x" << std::hex << d.csm_error()[i] << std::dec;
  ss << std::endl;
  
  ss << "  ******* RPC hit reconstruction *******" << std::endl;
  ss << "  RPC point 1       : "
     << "x = "   << std::setw(9) << std::setprecision(3) << d.rpc1_x()
     << ", y = " << std::setw(9) << std::setprecision(3) << d.rpc1_y()
     << ", z = " << std::setw(9) << std::setprecision(3) << d.rpc1_z() << std::endl;
  ss << "  RPC point 2       : "
     << "x = "   << std::setw(9) << std::setprecision(3) << d.rpc2_x()
     << ", y = " << std::setw(9) << std::setprecision(3) << d.rpc2_y()
     << ", z = " << std::setw(9) << std::setprecision(3) << d.rpc2_z() << std::endl;
  ss << "  RPC point 3       : "
     << "x = "   << std::setw(9) << std::setprecision(3) << d.rpc3_x()
     << ", y = " << std::setw(9) << std::setprecision(3) << d.rpc3_y()
     << ", z = " << std::setw(9) << std::setprecision(3) << d.rpc3_z() << std::endl;
  
  if(d.lvl1_emulation().size()) {
    ss << "  eta_pivot_lay0    : " << str_bit(d.eta_pivot_lay0(),32) << std::endl;
    ss << "  eta_pivot_lay1    : " << str_bit(d.eta_pivot_lay1(),32) << std::endl;
    ss << "  eta_low_0_lay0    : " << str_bit(d.eta_low_0_lay0(),32) << std::endl;
    ss << "  eta_low_1_lay0    : " << str_bit(d.eta_low_1_lay0(),32) << std::endl;
    ss << "  eta_low_0_lay1    : " << str_bit(d.eta_low_0_lay1(),32) << std::endl;
    ss << "  eta_low_1_lay1    : " << str_bit(d.eta_low_1_lay1(),32) << std::endl;
    ss << "  eta_high_0_lay0   : " << str_bit(d.eta_high_0_lay0(),32) << std::endl;
    ss << "  eta_high_1_lay0   : " << str_bit(d.eta_high_1_lay0(),32) << std::endl;
    ss << "  eta_high_0_lay1   : " << str_bit(d.eta_high_0_lay1(),32) << std::endl;
    ss << "  eta_high_1_lay1   : " << str_bit(d.eta_high_1_lay1(),32) << std::endl;
    ss << "  phi_pivot_lay0    : " << str_bit(d.phi_pivot_lay0(),32) << std::endl;
    ss << "  phi_pivot_lay1    : " << str_bit(d.phi_pivot_lay1(),32) << std::endl;
    ss << "  phi_low_0_lay0    : " << str_bit(d.phi_low_0_lay0(),32) << std::endl;
    ss << "  phi_low_1_lay0    : " << str_bit(d.phi_low_1_lay0(),32) << std::endl;
    ss << "  phi_low_0_lay1    : " << str_bit(d.phi_low_0_lay1(),32) << std::endl;
    ss << "  phi_low_1_lay1    : " << str_bit(d.phi_low_1_lay1(),32) << std::endl;
    ss << "  phi_high_0_lay0   : " << str_bit(d.phi_high_0_lay0(),32) << std::endl;
    ss << "  phi_high_1_lay0   : " << str_bit(d.phi_high_1_lay0(),32) << std::endl;
    ss << "  phi_high_0_lay1   : " << str_bit(d.phi_high_0_lay1(),32) << std::endl;
    ss << "  phi_high_1_lay1   : " << str_bit(d.phi_high_1_lay1(),32) << std::endl;
  }
  
  for (unsigned int i=0;i<d.pad_hit_onlineId().size();++i)
    ss << "  RPC hit           : "
       << "onlineId=0x" << std::hex << d.pad_hit_onlineId()[i] << std::dec
       << ", code=" << d.pad_hit_code()[i]
       << ", x=" << std::setw(9) << std::setprecision(3) << d.pad_hit_x()[i]
       << ", y=" << std::setw(9) << std::setprecision(3) << d.pad_hit_y()[i]
       << ", z=" << std::setw(9) << std::setprecision(3) << d.pad_hit_z()[i]
       << ", r=" << std::setw(10) << std::setprecision(3) << d.pad_hit_r()[i]
       << ", p=" << std::setw(6) << std::setprecision(3) << d.pad_hit_p()[i]
       << std::endl;
       
  ss << "  ******* TGC hit reconstruction *******" << std::endl;
  ss << "  TGC Mid1 point    :"
     << " eta="  << std::setw(9) << std::setprecision(4) << d.tgc_Mid1_eta()
     << ", phi=" << std::setw(9) << std::setprecision(4) << d.tgc_Mid1_phi()
     << ", r="   << std::setw(10) << std::setprecision(2) << d.tgc_Mid1_r()
     << ", z="   << std::setw(10) << std::setprecision(2) << d.tgc_Mid1_z() << std::endl;
  ss << "  TGC Mid2  point   :"
     << " eta="  << std::setw(9) << std::setprecision(4) << d.tgc_Mid2_eta()
     << ", phi=" << std::setw(9) << std::setprecision(4) << d.tgc_Mid2_phi()
     << ", r="   << std::setw(10) << std::setprecision(2) << d.tgc_Mid2_r()
     << ", z="   << std::setw(10) << std::setprecision(2) << d.tgc_Mid2_z() << std::endl;
  ss << "  TGC Mid fit       :"
     << " r_chi2="  << std::setw(9) << std::setprecision(1) << d.tgc_Mid_rho_chi2()
     << ", r_num="  << std::setw(4) << d.tgc_Mid_rho_N()
     << ", p_chi2=" << std::setw(9) << std::setprecision(1) << d.tgc_Mid_phi_chi2()
     << ", p_num="  << std::setw(4) << d.tgc_Mid_phi_N() << std::endl;
  ss << "  TGC Inner point   :"
     << " eta="  << std::setw(9) << std::setprecision(4) << d.tgc_Inn_eta()
     << ", phi=" << std::setw(9) << std::setprecision(4) << d.tgc_Inn_phi()
     << ", r="   << std::setw(10) << std::setprecision(2) << d.tgc_Inn_r()
     << ", z="   << std::setw(10) << std::setprecision(2) << d.tgc_Inn_z() << std::endl;
  ss << "  TGC Inner fit     :"
     << " r_std="  << std::setw(9) << std::setprecision(1) << d.tgc_Inn_rho_std()
     << ", r_num=" << std::setw(4) << d.tgc_Inn_rho_N()
     << ", p_std=" << std::setw(9) << std::setprecision(1) << d.tgc_Inn_phi_std()
     << ", p_num=" << std::setw(4) << d.tgc_Inn_phi_N() << std::endl;

  ss << "  TGC PT estimate   : " << d.tgc_PT() << std::endl;
  
  for (unsigned int i=0;i<d.tgc_Mid_rho_hit_phi().size();++i)
    ss << "  TGC Mid rho hit   :"
     << " phi=" << std::setw(9) << std::setprecision(4) << d.tgc_Mid_rho_hit_phi()[i]
     << ", r="  << std::setw(10) << std::setprecision(2) << d.tgc_Mid_rho_hit_r()[i]
     << ", z="  << std::setw(10) << std::setprecision(2) << d.tgc_Mid_rho_hit_z()[i]
     << ", width=" << std::setw(4) << std::setprecision(2) << d.tgc_Mid_rho_hit_width()[i] 
     << ", in road=" << d.tgc_Mid_rho_hit_in_seg()[i] << std::endl;
  
  for (unsigned int i=0;i<d.tgc_Mid_phi_hit_phi().size();++i)
    ss << "  TGC Mid phi hit   :"
     << " phi=" << std::setw(9) << std::setprecision(4) << d.tgc_Mid_phi_hit_phi()[i]
     << ", r="  << std::setw(10) << std::setprecision(2) << d.tgc_Mid_phi_hit_r()[i]
     << ", z="  << std::setw(10) << std::setprecision(2) << d.tgc_Mid_phi_hit_z()[i]
     << ", width=" << std::setw(4) << std::setprecision(2) << d.tgc_Mid_phi_hit_width()[i] 
     << ", in road=" << d.tgc_Mid_phi_hit_in_seg()[i] << std::endl;

  for (unsigned int i=0;i<d.tgc_Inn_rho_hit_phi().size();++i)
    ss << "  TGC Inn rho hit   :"
     << " phi=" << std::setw(9) << std::setprecision(4) << d.tgc_Inn_rho_hit_phi()[i]
     << ", r="  << std::setw(10) << std::setprecision(2) << d.tgc_Inn_rho_hit_r()[i]
     << ", z="  << std::setw(10) << std::setprecision(2) << d.tgc_Inn_rho_hit_z()[i]
     << ", width=" << std::setw(4) << std::setprecision(2) << d.tgc_Inn_rho_hit_width()[i] 
     << ", in road=" << d.tgc_Inn_rho_hit_in_seg()[i] << std::endl;

  for (unsigned int i=0;i<d.tgc_Inn_phi_hit_phi().size();++i)
    ss << "  TGC Inn rho hit   :"
     << " phi=" << std::setw(9) << std::setprecision(4) << d.tgc_Inn_phi_hit_phi()[i]
     << ", r="  << std::setw(10) << std::setprecision(2) << d.tgc_Inn_phi_hit_r()[i]
     << ", z="  << std::setw(10) << std::setprecision(2) << d.tgc_Inn_phi_hit_z()[i]
     << ", width=" << std::setw(4) << std::setprecision(2) << d.tgc_Inn_phi_hit_width()[i] 
     << ", in road=" << d.tgc_Inn_phi_hit_in_seg()[i] << std::endl;

  ss << "  ******** Track reconstruction ********" << std::endl;  
  ss << "  Track model       : type=" << d.type() 
     << ", x="   << std::setw(9)  << std::setprecision(3) << d.t_x_center()
     << ", y="   << std::setw(9)  << std::setprecision(3) << d.t_y_center()
     << ", rad=" << std::setw(10) << std::setprecision(3) << d.t_radius()
     << std::endl;
  ss << "  Phi position      :"
     << " ap="    << std::setw(9) << std::setprecision(3) << d.t_ap() 
     << ", aphi=" << std::setw(9) << std::setprecision(3) << d.t_aphi()
     << ", phi="  << std::setw(9) << std::setprecision(3) << d.t_phi()
     << std::endl;
  ss << "  Extrapolation     :"
     << " InnerEta=" << std::setw(9) << std::setprecision(3) << d.extrInnerEta()
     << " InnerPhi=" << std::setw(9) << std::setprecision(3) << d.extrInnerPhi()
     << std::endl;
  
  ss << "  ************* Muon Roads *************" << std::endl;
  ss << "  Inner trigger     :"
     << " ty1=" << (d.chamber_type_1())[0][0] 
     << ", ty2=" <<(d.chamber_type_2())[0][0]
     << ", pos=" <<(d.pos())[0][0]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[0][0]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[0][0]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[0][0]
     << std::endl;
  ss << "  Inner trigger     :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[0][0] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[0][0]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[0][0]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[0][0]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[0][0]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[0][0]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[0][0]
     << std::endl;
  ss << "  Middle trigger    :"
     << " ty1=" << (d.chamber_type_1())[0][1] 
     << ", ty2=" <<(d.chamber_type_2())[0][1]
     << ", pos=" <<(d.pos())[0][1]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[0][1]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[0][1]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[0][1]
     << std::endl;
  ss << "  Middle trigger    :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[0][1] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[0][1]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[0][1]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[0][1]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[0][1]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[0][1]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[0][1]
     << std::endl;
  ss << "  Outer trigger     :"
     << " ty1=" << (d.chamber_type_1())[0][2] 
     << ", ty2=" <<(d.chamber_type_2())[0][2]
     << ", pos=" <<(d.pos())[0][2]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[0][2]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[0][2]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[0][2]
     << std::endl;
  ss << "  Outer trigger     :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[0][2] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[0][2]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[0][2]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[0][2]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[0][2]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[0][2]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[0][2]
     << std::endl;
  ss << "  Ext. trigger      :"
     << " ty1=" << (d.chamber_type_1())[0][3] 
     << ", ty2=" <<(d.chamber_type_2())[0][3]
     << ", pos=" <<(d.pos())[0][3]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[0][3]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[0][3]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[0][3]
     << std::endl;
  ss << "  Ext. trigger      :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[0][3] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[0][3]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[0][3]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[0][3]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[0][3]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[0][3]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[0][3]
     << std::endl;

  ss << "  Inner overlap     :"
     << " ty1=" << (d.chamber_type_1())[1][0] 
     << ", ty2=" <<(d.chamber_type_2())[1][0]
     << ", pos=" <<(d.pos())[1][0]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[1][0]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[1][0]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[1][0]
     << std::endl;
  ss << "  Inner overlap     :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[1][0] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[1][0]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[1][0]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[1][0]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[1][0]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[1][0]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[1][0]
     << std::endl;
  ss << "  Middle overlap    :"
     << " ty1=" << (d.chamber_type_1())[1][1] 
     << ", ty2=" <<(d.chamber_type_2())[1][1]
     << ", pos=" <<(d.pos())[1][1]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[1][1]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[1][1]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[1][1]
     << std::endl;
  ss << "  Middle overlap    :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[1][1] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[1][1]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[1][1]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[1][1]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[1][1]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[1][1]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[1][1]
     << std::endl;
  ss << "  Outer overlap     :"
     << " ty1=" << (d.chamber_type_1())[1][2] 
     << ", ty2=" <<(d.chamber_type_2())[1][2]
     << ", pos=" <<(d.pos())[1][2]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[1][2]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[1][2]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[1][2]
     << std::endl;
  ss << "  Outer overlap     :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[1][2] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[1][2]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[1][2]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[1][2]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[1][2]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[1][2]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[1][2]
     << std::endl;
  ss << "  Ext. overlap      :"
     << " ty1=" << (d.chamber_type_1())[1][3] 
     << ", ty2=" <<(d.chamber_type_2())[1][3]
     << ", pos=" <<(d.pos())[1][3]
     << ", aw=" << std::setw(9) << std::setprecision(3) << (d.aw())[1][3]
     << ", bw=" << std::setw(9) << std::setprecision(3) << (d.bw())[1][3]
     << ", phi=" << std::setw(8) << std::setprecision(3) << (d.st_phi())[1][3]
     << std::endl;
  ss << "  Ext. overlap      :"
     << " Zmin="  << std::setw(9) << std::setprecision(3) << (d.zetaMin())[1][3] 
     << ", Zmax=" << std::setw(9) << std::setprecision(3) << (d.zetaMax())[1][3]
     << ", Rmin=" << std::setw(9) << std::setprecision(3) << (d.radMin())[1][3]
     << ", Rmax=" << std::setw(9) << std::setprecision(3) << (d.radMax())[1][3]
     << ", Emin=" << std::setw(7) << std::setprecision(3) << (d.etaMin())[1][3]
     << ", Emax=" << std::setw(7) << std::setprecision(3) << (d.etaMax())[1][3]
     << ", road=" << std::setw(7) << std::setprecision(3) << (d.st_roads())[1][3]
     << std::endl;



  ss << "  ******* MDT hit reconstruction *******" << std::endl;
  ss << "  MDT Innermost fit :"
     << " slope=" << std::setw(9) << std::setprecision(3) << d.mdt_Inner_slope()
     << ", interc.=" << std::setw(9) << std::setprecision(3) << d.mdt_Inner_intercept()
     << ", z=" << std::setw(9) << std::setprecision(3) << d.mdt_Inner_Z()
     << ", r=" << std::setw(9) << std::setprecision(3) << d.mdt_Inner_R()
     << ", chi2=" << std::setw(9) << std::setprecision(2) << d.mdt_Inner_fit_chi() << std::endl;
  
  ss << "  MDT Middle fit    :"
     << " slope=" << std::setw(9) << std::setprecision(3) << d.mdt_Middle_slope()
     << ", interc.=" << std::setw(9) << std::setprecision(3) << d.mdt_Middle_intercept()
     << ", z=" << std::setw(9) << std::setprecision(3) << d.mdt_Middle_Z()
     << ", r=" << std::setw(9) << std::setprecision(3) << d.mdt_Middle_R()
     << ", chi2=" << std::setw(9) << std::setprecision(2) << d.mdt_Middle_fit_chi() << std::endl;

  ss << "  MDT Outermost fit :"
     << " slope=" << std::setw(9) << std::setprecision(3) << d.mdt_Outer_slope()
     << ", interc.=" << std::setw(9) << std::setprecision(3) << d.mdt_Outer_intercept()
     << ", z=" << std::setw(9) << std::setprecision(3) << d.mdt_Outer_Z()
     << ", r=" << std::setw(9) << std::setprecision(3) << d.mdt_Outer_R()
     << ", chi2=" << std::setw(9) << std::setprecision(2) << d.mdt_Outer_fit_chi() << std::endl;

  for (unsigned int i=0;i<d.mdt_onlineId().size();++i)
    ss << "  MDT hit            :"
       << " onlineId=0x" << std::hex << d.mdt_onlineId()[i] << std::dec
       << ", offlineId=0x" << std::hex << d.mdt_offlineId()[i] << std::dec
       << ", r=" << std::setw(9) << std::setprecision(2) << d.mdt_tube_r()[i]
       << ", z=" << std::setw(9) << std::setprecision(2) << d.mdt_tube_z()[i]
       << ", res=" << std::setw(9) << std::setprecision(2) << d.mdt_tube_residual()[i]
       << ", time=" << std::setw(5) << d.mdt_tube_time()[i]
       << ", space=" << d.mdt_tube_space()[i]
       << ", sigma=" << d.mdt_tube_sigma()[i] << std::endl;
     
     
  ss << "  ********** Extended Variable *********" << std::endl;
  ss << "  Extension 0       :";
  for (unsigned int i=0;i<d.extension0().size();++i)
    ss << " " << d.extension0()[i];    
  ss << std::endl;
  ss << "  Extension 1       :";
  for (unsigned int i=0;i<d.extension1().size();++i)
    ss << " " << d.extension1()[i];    
  ss << std::endl;
  ss << "  Extension 2       :";
  for (unsigned int i=0;i<d.extension2().size();++i)
    ss << " " << d.extension2()[i];    
  ss << std::endl;
  ss << "  Extension 3       :";
  for (unsigned int i=0;i<d.extension3().size();++i)
    ss << " " << d.extension3()[i];    
  ss << std::endl;
  ss << "  Extension 4       :";
  for (unsigned int i=0;i<d.extension4().size();++i)
    ss << " " << d.extension4()[i];    
  ss << std::endl;
  ss << "  Extension 5       :";
  for (unsigned int i=0;i<d.extension5().size();++i)
    ss << " " << d.extension5()[i];    
  ss << std::endl;
  ss << "  Extension 6       :";
  for (unsigned int i=0;i<d.extension6().size();++i)
    ss << " " << d.extension6()[i];    
  ss << std::endl;
  ss << "  Extension 7       :";
  for (unsigned int i=0;i<d.extension7().size();++i)
    ss << " " << d.extension7()[i];    
  ss << std::endl;
  ss << "  Extension 8       :";
  for (unsigned int i=0;i<d.extension8().size();++i)
    ss << " " << d.extension8()[i];    
  ss << std::endl;
  ss << "  Extension 9       :";
  for (unsigned int i=0;i<d.extension9().size();++i)
    ss << " " << d.extension9()[i];    
  ss << std::endl;
  
  
  ss << "  ********** Feature Extracted *********" << std::endl;  
  ss << "  Address           : " << d.Address() << std::endl;
  ss << "  Sagitta           : " << d.Sagitta() << std::endl; 
  ss << "  Radius            : " << d.Radius() << std::endl; 
  ss << "  Slope             : " << d.Slope() << std::endl;
  ss << "  Intercept         : " << d.Intercept() << std::endl;
  ss << "  Alpha             : " << d.Alpha() << std::endl; 
  ss << "  Beta              : " << d.Beta() << std::endl;
  ss << "  DeltaR            : " << d.DeltaR() << std::endl;
  ss << "  Speed over c      : " << d.Speed_over_c() << std::endl;
  ss << "  Eta map           : " << d.Eta() << std::endl;
  ss << "  Phi map           : " << d.PhiMap() << std::endl;
  ss << "  Phi               : " << d.Phi() << std::endl;
  ss << "  Phi direction     : " << d.PhiDir() << std::endl;
  ss << "  PT                : " << d.Pt() << std::endl;
  ss << "  Charge            : " << d.Charge() << std::endl;

  return ss.str();
}


MsgStream& operator<< ( MsgStream& m, const MuonFeatureDetails& d ) {
  m << d.str();
  return m;
}

std::ostream& operator<< (std::ostream& s, const MuonFeatureDetails& d ) {
  s << d.str();
  return s;
}
