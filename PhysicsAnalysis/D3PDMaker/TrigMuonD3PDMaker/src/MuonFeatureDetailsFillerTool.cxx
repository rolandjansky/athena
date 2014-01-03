/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeatureDetailsFillerTool.cxx 310395 2010-07-14 05:38:11Z kanno $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "MuonFeatureDetailsFillerTool.h"

namespace D3PD {

   MuonFeatureDetailsFillerTool::MuonFeatureDetailsFillerTool( const std::string& type,
                                                               const std::string& name,
                                                              const IInterface* parent )
      : BlockFillerTool< MuonFeatureDetails >( type, name, parent ) {
      //declareProperty( "WriteRecoInfo", m_writeRecoInfo = false );
      //declareProperty( "WriteHitsInfo", m_writeHitsInfo = false );

     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode MuonFeatureDetailsFillerTool::book() {

      CHECK( addVariable( "id",                     m_id ) );

      CHECK( addVariable( "te_id",                  m_te_id ) );
      CHECK( addVariable( "error",                  m_error ) );


      CHECK( addVariable( "lvl1_id",                m_lvl1_id ) );
      CHECK( addVariable( "lumi_block",             m_lumi_block ) );
      CHECK( addVariable( "muondetmask",            m_muondetmask ) );
      CHECK( addVariable( "RoIID",                  m_RoIID ) );
      CHECK( addVariable( "RoISystem",              m_RoISystem ) );
      CHECK( addVariable( "RoISubsystem",           m_RoISubsystem ) );
      CHECK( addVariable( "RoISector",              m_RoISector ) );
      CHECK( addVariable( "RoINumber",              m_RoINumber ) );
      CHECK( addVariable( "RoIThreshold",           m_RoIThreshold ) );
      CHECK( addVariable( "RoIEta",                 m_RoIEta ) );
      CHECK( addVariable( "RoIPhi",                 m_RoIPhi ) );

      CHECK( addVariable( "rpc_pad_error",          m_rpc_pad_error ) );
      CHECK( addVariable( "tgc_rdo_error",          m_tgc_rdo_error ) );

      CHECK( addVariable( "rpc1_x",                 m_rpc1_x ) );
      CHECK( addVariable( "rpc1_y",                 m_rpc1_y ) );
      CHECK( addVariable( "rpc1_z",                 m_rpc1_z ) );

      CHECK( addVariable( "rpc2_x",                 m_rpc2_x ) );
      CHECK( addVariable( "rpc2_y",                 m_rpc2_y ) );
      CHECK( addVariable( "rpc2_z",                 m_rpc2_z ) );

      CHECK( addVariable( "rpc3_x",                 m_rpc3_x ) );
      CHECK( addVariable( "rpc3_y",                 m_rpc3_y ) );
      CHECK( addVariable( "rpc3_z",                 m_rpc3_z ) );

      CHECK( addVariable( "tgc_Mid1_eta",           m_tgc_Mid1_eta ) );
      CHECK( addVariable( "tgc_Mid1_phi",           m_tgc_Mid1_phi ) );
      CHECK( addVariable( "tgc_Mid1_r",             m_tgc_Mid1_r ) );
      CHECK( addVariable( "tgc_Mid1_z",             m_tgc_Mid1_z ) );

      CHECK( addVariable( "tgc_Mid2_eta",           m_tgc_Mid2_eta ) );
      CHECK( addVariable( "tgc_Mid2_phi",           m_tgc_Mid2_phi ) );
      CHECK( addVariable( "tgc_Mid2_r",             m_tgc_Mid2_r ) );
      CHECK( addVariable( "tgc_Mid2_z",             m_tgc_Mid2_z ) );

      CHECK( addVariable( "tgc_Mid_rho_chi2",       m_tgc_Mid_rho_chi2 ) );
      CHECK( addVariable( "tgc_Mid_rho_N",          m_tgc_Mid_rho_N ) );
      CHECK( addVariable( "tgc_Mid_phi_chi2",       m_tgc_Mid_phi_chi2 ) );
      CHECK( addVariable( "tgc_Mid_phi_N",          m_tgc_Mid_phi_N ) );

      CHECK( addVariable( "tgc_Inn_eta",            m_tgc_Inn_eta ) );
      CHECK( addVariable( "tgc_Inn_phi",            m_tgc_Inn_phi ) );
      CHECK( addVariable( "tgc_Inn_r",              m_tgc_Inn_r ) );
      CHECK( addVariable( "tgc_Inn_z",              m_tgc_Inn_z ) );
      CHECK( addVariable( "tgc_Inn_rho_std",        m_tgc_Inn_rho_std ) );
      CHECK( addVariable( "tgc_Inn_rho_N",          m_tgc_Inn_rho_N ) );
      CHECK( addVariable( "tgc_Inn_phi_std",        m_tgc_Inn_phi_std ) );
      CHECK( addVariable( "tgc_Inn_phi_N",          m_tgc_Inn_phi_N ) );

      CHECK( addVariable( "tgc_PT",                 m_tgc_PT ) );

      CHECK( addVariable( "mdt_Inner_slope",        m_mdt_Inner_slope ) );
      CHECK( addVariable( "mdt_Inner_intercept",    m_mdt_Inner_intercept ) );
      CHECK( addVariable( "mdt_Inner_Z",            m_mdt_Inner_Z ) );
      CHECK( addVariable( "mdt_Inner_R",            m_mdt_Inner_R ) );
      CHECK( addVariable( "mdt_Inner_fit_chi",      m_mdt_Inner_fit_chi ) );
      CHECK( addVariable( "mdt_Middle_slope",       m_mdt_Middle_slope ) );
      CHECK( addVariable( "mdt_Middle_intercept",   m_mdt_Middle_intercept ) );
      CHECK( addVariable( "mdt_Middle_Z",           m_mdt_Middle_Z ) );
      CHECK( addVariable( "mdt_Middle_R",           m_mdt_Middle_R ) );
      CHECK( addVariable( "mdt_Middle_fit_chi",     m_mdt_Middle_fit_chi ) );
      CHECK( addVariable( "mdt_Outer_slope",        m_mdt_Outer_slope ) );
      CHECK( addVariable( "mdt_Outer_intercept",    m_mdt_Outer_intercept ) );
      CHECK( addVariable( "mdt_Outer_Z",            m_mdt_Outer_Z ) );
      CHECK( addVariable( "mdt_Outer_R",            m_mdt_Outer_R ) );
      CHECK( addVariable( "mdt_Outer_fit_chi",      m_mdt_Outer_fit_chi ) );

      CHECK( addVariable( "Address",                m_Address ) );
      CHECK( addVariable( "Sagitta",                m_Sagitta ) );
      CHECK( addVariable( "Radius",                 m_Radius ) );
      CHECK( addVariable( "Slope",                  m_Slope ) );
      CHECK( addVariable( "Intercept",              m_Intercept ) );
      CHECK( addVariable( "Alpha",                  m_Alpha ) );
      CHECK( addVariable( "Beta",                   m_Beta ) );
      CHECK( addVariable( "DeltaR",                 m_DeltaR ) );
      CHECK( addVariable( "Speed_over_c",           m_Speed_over_c ) );
      CHECK( addVariable( "Eta",                    m_Eta ) );
      CHECK( addVariable( "PhiMap",                 m_PhiMap ) );
      CHECK( addVariable( "Phi",                    m_Phi ) );
      CHECK( addVariable( "PhiDir",                 m_PhiDir ) );
      CHECK( addVariable( "Pt",                     m_Pt ) );
      CHECK( addVariable( "Charge",                 m_Charge ) );

      CHECK( addVariable( "type",                   m_type ) );
      CHECK( addVariable( "t_x_center",             m_t_x_center ) );
      CHECK( addVariable( "t_y_center",             m_t_y_center ) );
      CHECK( addVariable( "t_radius",               m_t_radius ) );
      CHECK( addVariable( "t_ap",                   m_t_ap ) );
      CHECK( addVariable( "t_aphi",                 m_t_aphi ) );
      CHECK( addVariable( "t_phi",                  m_t_phi ) );
      CHECK( addVariable( "extrInnerEta",           m_extrInnerEta ) );
      CHECK( addVariable( "extrInnerPhi",           m_extrInnerPhi ) );

      CHECK( addVariable( "tr_chamber_type_1",      m_tr_chamber_type_1 ) );
      CHECK( addVariable( "tr_chamber_type_2",      m_tr_chamber_type_2 ) );
      CHECK( addVariable( "tr_pos",                 m_tr_pos ) );
      CHECK( addVariable( "tr_aw",                  m_tr_aw ) );
      CHECK( addVariable( "tr_bw",                  m_tr_bw ) );
      CHECK( addVariable( "tr_zetaMin",             m_tr_zetaMin ) );
      CHECK( addVariable( "tr_zetaMax",             m_tr_zetaMax ) );
      CHECK( addVariable( "tr_radMin",              m_tr_radMin ) );
      CHECK( addVariable( "tr_radMax",              m_tr_radMax ) );
      CHECK( addVariable( "tr_etaMin",              m_tr_etaMin ) );
      CHECK( addVariable( "tr_etaMax",              m_tr_etaMax ) );
      CHECK( addVariable( "tr_st_phi",              m_tr_st_phi ) );
      CHECK( addVariable( "tr_st_roads",            m_tr_st_roads ) );

      CHECK( addVariable( "ov_chamber_type_1",      m_ov_chamber_type_1 ) );
      CHECK( addVariable( "ov_chamber_type_2",      m_ov_chamber_type_2 ) );
      CHECK( addVariable( "ov_pos",                 m_ov_pos ) );
      CHECK( addVariable( "ov_aw",                  m_ov_aw ) );
      CHECK( addVariable( "ov_bw",                  m_ov_bw ) );
      CHECK( addVariable( "ov_zetaMin",             m_ov_zetaMin ) );
      CHECK( addVariable( "ov_zetaMax",             m_ov_zetaMax ) );
      CHECK( addVariable( "ov_radMin",              m_ov_radMin ) );
      CHECK( addVariable( "ov_radMax",              m_ov_radMax ) );
      CHECK( addVariable( "ov_etaMin",              m_ov_etaMin ) );
      CHECK( addVariable( "ov_etaMax",              m_ov_etaMax ) );
      CHECK( addVariable( "ov_st_phi",              m_ov_st_phi ) );
      CHECK( addVariable( "ov_st_roads",            m_ov_st_roads ) );

      CHECK( addVariable( "extension0",             m_extension0 ) );
      CHECK( addVariable( "extension1",             m_extension1 ) );
      CHECK( addVariable( "extension2",             m_extension2 ) );
      CHECK( addVariable( "extension3",             m_extension3 ) );
      CHECK( addVariable( "extension4",             m_extension4 ) );
      CHECK( addVariable( "extension5",             m_extension5 ) );
      CHECK( addVariable( "extension6",             m_extension6 ) );
      CHECK( addVariable( "extension7",             m_extension7 ) );
      CHECK( addVariable( "extension8",             m_extension8 ) );
      CHECK( addVariable( "extension9",             m_extension9 ) );

      CHECK( addVariable( "lvl1_emulation",         m_lvl1_emulation ) );

      CHECK( addVariable( "rob_id",                 m_rob_id ) );
      CHECK( addVariable( "csm_id",                 m_csm_id ) );
      CHECK( addVariable( "csm_size",               m_csm_size ) );
      CHECK( addVariable( "csm_error",              m_csm_error ) );
      CHECK( addVariable( "removed_rob_id",         m_removed_rob_id ) );
      CHECK( addVariable( "removed_csm_id",         m_removed_csm_id ) );

      CHECK( addVariable( "pad_hit_onlineId",       m_pad_hit_onlineId ) );
      CHECK( addVariable( "pad_hit_code",           m_pad_hit_code ) );
      CHECK( addVariable( "pad_hit_x",              m_pad_hit_x ) );
      CHECK( addVariable( "pad_hit_y",              m_pad_hit_y ) );
      CHECK( addVariable( "pad_hit_z",              m_pad_hit_z ) );
      CHECK( addVariable( "pad_hit_r",              m_pad_hit_r ) );
      CHECK( addVariable( "pad_hit_p",              m_pad_hit_p ) );

      CHECK( addVariable( "tgc_Inn_rho_hit_phi",    m_tgc_Inn_rho_hit_phi ) );
      CHECK( addVariable( "tgc_Inn_rho_hit_r",      m_tgc_Inn_rho_hit_r ) );
      CHECK( addVariable( "tgc_Inn_rho_hit_z",      m_tgc_Inn_rho_hit_z ) );
      CHECK( addVariable( "tgc_Inn_rho_hit_width",  m_tgc_Inn_rho_hit_width ) );
      CHECK( addVariable( "tgc_Inn_rho_hit_in_seg", m_tgc_Inn_rho_hit_in_seg ) );
      CHECK( addVariable( "tgc_Inn_phi_hit_phi",    m_tgc_Inn_phi_hit_phi ) );
      CHECK( addVariable( "tgc_Inn_phi_hit_r",      m_tgc_Inn_phi_hit_r ) );
      CHECK( addVariable( "tgc_Inn_phi_hit_z",      m_tgc_Inn_phi_hit_z ) );
      CHECK( addVariable( "tgc_Inn_phi_hit_width",  m_tgc_Inn_phi_hit_width ) );
      CHECK( addVariable( "tgc_Inn_phi_hit_in_seg", m_tgc_Inn_phi_hit_in_seg ) );
      CHECK( addVariable( "tgc_Mid_rho_hit_phi",    m_tgc_Mid_rho_hit_phi ) );
      CHECK( addVariable( "tgc_Mid_rho_hit_r",      m_tgc_Mid_rho_hit_r ) );
      CHECK( addVariable( "tgc_Mid_rho_hit_z",      m_tgc_Mid_rho_hit_z ) );
      CHECK( addVariable( "tgc_Mid_rho_hit_width",  m_tgc_Mid_rho_hit_width ) );
      CHECK( addVariable( "tgc_Mid_rho_hit_in_seg", m_tgc_Mid_rho_hit_in_seg ) );
      CHECK( addVariable( "tgc_Mid_phi_hit_phi",    m_tgc_Mid_phi_hit_phi ) );
      CHECK( addVariable( "tgc_Mid_phi_hit_r",      m_tgc_Mid_phi_hit_r ) );
      CHECK( addVariable( "tgc_Mid_phi_hit_z",      m_tgc_Mid_phi_hit_z ) );
      CHECK( addVariable( "tgc_Mid_phi_hit_width",  m_tgc_Mid_phi_hit_width ) );
      CHECK( addVariable( "tgc_Mid_phi_hit_in_seg", m_tgc_Mid_phi_hit_in_seg ) );

      CHECK( addVariable( "mdt_onlineId",           m_mdt_onlineId ) );
      CHECK( addVariable( "mdt_offlineId",          m_mdt_offlineId ) );
      CHECK( addVariable( "mdt_tube_r",             m_mdt_tube_r ) );
      CHECK( addVariable( "mdt_tube_z",             m_mdt_tube_z ) );
      CHECK( addVariable( "mdt_tube_residual",      m_mdt_tube_residual ) );
      CHECK( addVariable( "mdt_tube_time",          m_mdt_tube_time ) );
      CHECK( addVariable( "mdt_tube_space",         m_mdt_tube_space ) );
      CHECK( addVariable( "mdt_tube_sigma",         m_mdt_tube_sigma ) );

      return StatusCode::SUCCESS;
   }

   StatusCode MuonFeatureDetailsFillerTool::fill( const MuonFeatureDetails& muon ) {

      *m_id    = muon.id();
      *m_te_id = muon.te_id();
      *m_error = muon.error();

      *m_lvl1_id      = muon.lvl1_id();
      *m_lumi_block   = muon.lumi_block();
      *m_muondetmask  = muon.muondetmask(); 
      *m_RoIID        = muon.roi_id();
      *m_RoISystem    = muon.roi_system();
      *m_RoISubsystem = muon.roi_subsystem(); 
      *m_RoISector    = muon.roi_sector();
      *m_RoINumber    = muon.roi_number();
      *m_RoIThreshold = muon.roi_threshold();
      *m_RoIEta       = muon.roi_eta();
      *m_RoIPhi       = muon.roi_phi();  

      *m_rpc_pad_error = muon.rpc_pad_error();
      *m_tgc_rdo_error = muon.tgc_rdo_error();

      *m_rpc1_x = muon.rpc1_x();
      *m_rpc1_y = muon.rpc1_y();
      *m_rpc1_z = muon.rpc1_z();

      *m_rpc2_x = muon.rpc2_x();
      *m_rpc2_y = muon.rpc2_y();
      *m_rpc2_z = muon.rpc2_z();

      *m_rpc3_x = muon.rpc3_x();
      *m_rpc3_y = muon.rpc3_y();
      *m_rpc3_z = muon.rpc3_z(); 

      *m_tgc_Mid1_eta = muon.tgc_Mid1_eta();
      *m_tgc_Mid1_phi = muon.tgc_Mid1_phi();
      *m_tgc_Mid1_r   = muon.tgc_Mid1_r();
      *m_tgc_Mid1_z   = muon.tgc_Mid1_z();

      *m_tgc_Mid2_eta = muon.tgc_Mid2_eta();
      *m_tgc_Mid2_phi = muon.tgc_Mid2_phi();
      *m_tgc_Mid2_r   = muon.tgc_Mid2_r();
      *m_tgc_Mid2_z   = muon.tgc_Mid2_z();

      *m_tgc_Mid_rho_chi2 = muon.tgc_Mid_rho_chi2();
      *m_tgc_Mid_rho_N    = muon.tgc_Mid_rho_N();
      *m_tgc_Mid_phi_chi2 = muon.tgc_Mid_phi_chi2();
      *m_tgc_Mid_phi_N    = muon.tgc_Mid_phi_N();

      *m_tgc_Inn_eta     = muon.tgc_Inn_eta();
      *m_tgc_Inn_phi     = muon.tgc_Inn_phi();
      *m_tgc_Inn_r       = muon.tgc_Inn_r();
      *m_tgc_Inn_z       = muon.tgc_Inn_z();
      *m_tgc_Inn_rho_std = muon.tgc_Inn_rho_std();
      *m_tgc_Inn_rho_N   = muon.tgc_Inn_rho_N();
      *m_tgc_Inn_phi_std = muon.tgc_Inn_phi_std();
      *m_tgc_Inn_phi_N   = muon.tgc_Inn_phi_N();

      *m_tgc_PT = muon.tgc_PT();

      *m_mdt_Inner_slope      = muon.mdt_Inner_slope();
      *m_mdt_Inner_intercept  = muon.mdt_Inner_intercept();
      *m_mdt_Inner_Z          = muon.mdt_Inner_Z();
      *m_mdt_Inner_R          = muon.mdt_Inner_R();
      *m_mdt_Inner_fit_chi    = muon.mdt_Inner_fit_chi();
      *m_mdt_Middle_slope     = muon.mdt_Middle_slope();
      *m_mdt_Middle_intercept = muon.mdt_Middle_intercept();
      *m_mdt_Middle_Z         = muon.mdt_Middle_Z();
      *m_mdt_Middle_R         = muon.mdt_Middle_R();
      *m_mdt_Middle_fit_chi   = muon.mdt_Middle_fit_chi();
      *m_mdt_Outer_slope      = muon.mdt_Outer_slope();
      *m_mdt_Outer_intercept  = muon.mdt_Outer_intercept();
      *m_mdt_Outer_Z          = muon.mdt_Outer_Z();
      *m_mdt_Outer_R          = muon.mdt_Outer_R();
      *m_mdt_Outer_fit_chi    = muon.mdt_Outer_fit_chi();

      *m_Address      = muon.Address();  
      *m_Sagitta      = muon.Sagitta();
      *m_Radius       = muon.Radius();
      *m_Slope        = muon.Slope();
      *m_Intercept    = muon.Intercept();
      *m_Alpha        = muon.Alpha();
      *m_Beta         = muon.Beta();
      *m_DeltaR       = muon.DeltaR();
      *m_Speed_over_c = muon.Speed_over_c();
      *m_Eta          = muon.Eta();
      *m_PhiMap       = muon.PhiMap();
      *m_Phi          = muon.Phi();
      *m_PhiDir       = muon.PhiDir();
      *m_Pt           = muon.Pt();
      *m_Charge       = muon.Charge();

      *m_type         = muon.type();
      *m_t_x_center   = muon.t_x_center();
      *m_t_y_center   = muon.t_y_center();
      *m_t_radius     = muon.t_radius();
      *m_t_ap         = muon.t_ap();
      *m_t_aphi       = muon.t_aphi();
      *m_t_phi        = muon.t_phi();
      *m_extrInnerEta = muon.extrInnerEta();
      *m_extrInnerPhi = muon.extrInnerPhi();

      *m_tr_chamber_type_1 = muon.chamber_type_1()[0];
      *m_tr_chamber_type_2 = muon.chamber_type_2()[0];
      *m_tr_pos            = muon.pos()[0];
      *m_tr_aw             = muon.aw()[0];
      *m_tr_bw             = muon.bw()[0];
      *m_tr_zetaMin        = muon.zetaMin()[0];
      *m_tr_zetaMax        = muon.zetaMax()[0];
      *m_tr_radMin         = muon.radMin()[0];
      *m_tr_radMax         = muon.radMax()[0];
      *m_tr_etaMin         = muon.etaMin()[0];
      *m_tr_etaMax         = muon.etaMax()[0];
      *m_tr_st_phi         = muon.st_phi()[0];
      *m_tr_st_roads       = muon.st_roads()[0];

      *m_ov_chamber_type_1 = muon.chamber_type_1()[1];
      *m_ov_chamber_type_2 = muon.chamber_type_2()[1];
      *m_ov_pos            = muon.pos()[1];
      *m_ov_aw             = muon.aw()[1];
      *m_ov_bw             = muon.bw()[1];
      *m_ov_zetaMin        = muon.zetaMin()[1];
      *m_ov_zetaMax        = muon.zetaMax()[1];
      *m_ov_radMin         = muon.radMin()[1];
      *m_ov_radMax         = muon.radMax()[1];
      *m_ov_etaMin         = muon.etaMin()[1];
      *m_ov_etaMax         = muon.etaMax()[1];
      *m_ov_st_phi         = muon.st_phi()[1];
      *m_ov_st_roads       = muon.st_roads()[1];

      *m_extension0 = muon.extension0();
      *m_extension1 = muon.extension1();
      *m_extension2 = muon.extension2();
      *m_extension3 = muon.extension3();
      *m_extension4 = muon.extension4();
      *m_extension5 = muon.extension5();
      *m_extension6 = muon.extension6();
      *m_extension7 = muon.extension7();
      *m_extension8 = muon.extension8();
      *m_extension9 = muon.extension9();

      *m_lvl1_emulation = muon.lvl1_emulation();

      *m_rob_id         = muon.rob_id();
      *m_csm_id         = muon.csm_id();
      *m_csm_size       = muon.csm_size();
      *m_csm_error      = muon.csm_error();
      *m_removed_rob_id = muon.removed_rob_id();
      *m_removed_csm_id = muon.removed_csm_id();

      *m_pad_hit_onlineId = muon.pad_hit_onlineId();
      *m_pad_hit_code     = muon.pad_hit_code();
      *m_pad_hit_x        = muon.pad_hit_x();
      *m_pad_hit_y        = muon.pad_hit_y();
      *m_pad_hit_z        = muon.pad_hit_z();
      *m_pad_hit_r        = muon.pad_hit_r();
      *m_pad_hit_p        = muon.pad_hit_p();

      *m_tgc_Inn_rho_hit_phi    = muon.tgc_Inn_rho_hit_phi();
      *m_tgc_Inn_rho_hit_r      = muon.tgc_Inn_rho_hit_r();
      *m_tgc_Inn_rho_hit_z      = muon.tgc_Inn_rho_hit_z();
      *m_tgc_Inn_rho_hit_width  = muon.tgc_Inn_rho_hit_width();
      *m_tgc_Inn_rho_hit_in_seg = muon.tgc_Inn_rho_hit_in_seg();
      *m_tgc_Inn_phi_hit_phi    = muon.tgc_Inn_phi_hit_phi();
      *m_tgc_Inn_phi_hit_r      = muon.tgc_Inn_phi_hit_r();
      *m_tgc_Inn_phi_hit_z      = muon.tgc_Inn_phi_hit_z();
      *m_tgc_Inn_phi_hit_width  = muon.tgc_Inn_phi_hit_width();
      *m_tgc_Inn_phi_hit_in_seg = muon.tgc_Inn_phi_hit_in_seg();
      *m_tgc_Mid_rho_hit_phi    = muon.tgc_Mid_rho_hit_phi();
      *m_tgc_Mid_rho_hit_r      = muon.tgc_Mid_rho_hit_r();
      *m_tgc_Mid_rho_hit_z      = muon.tgc_Mid_rho_hit_z();
      *m_tgc_Mid_rho_hit_width  = muon.tgc_Mid_rho_hit_width();
      *m_tgc_Mid_rho_hit_in_seg = muon.tgc_Mid_rho_hit_in_seg();
      *m_tgc_Mid_phi_hit_phi    = muon.tgc_Mid_phi_hit_phi();
      *m_tgc_Mid_phi_hit_r      = muon.tgc_Mid_phi_hit_r();
      *m_tgc_Mid_phi_hit_z      = muon.tgc_Mid_phi_hit_z();
      *m_tgc_Mid_phi_hit_width  = muon.tgc_Mid_phi_hit_width();
      *m_tgc_Mid_phi_hit_in_seg = muon.tgc_Mid_phi_hit_in_seg();

      *m_mdt_onlineId      = muon.mdt_onlineId();
      *m_mdt_offlineId     = muon.mdt_offlineId();
      *m_mdt_tube_r        = muon.mdt_tube_r();
      *m_mdt_tube_z        = muon.mdt_tube_z();
      *m_mdt_tube_residual = muon.mdt_tube_residual();
      *m_mdt_tube_time     = muon.mdt_tube_time();
      *m_mdt_tube_space    = muon.mdt_tube_space();
      *m_mdt_tube_sigma    = muon.mdt_tube_sigma();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
