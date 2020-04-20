/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BTagHighLevelAugmenterAlg.h"

#include <cmath>
#include <cstddef>
#include <regex>

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/WriteDecorHandle.h"

namespace Analysis {
  BTagHighLevelAugmenterAlg::BTagHighLevelAugmenterAlg( const std::string& name, ISvcLocator* loc )
    : AthAlgorithm(name, loc),
    m_flipTagConfig("STANDARD"),
    m_acc_jet_track_links("BTagTrackToJetAssociation")
  {
    declareProperty("FlipTagConfig", m_flipTagConfig);
    declareProperty("BTagTrackToJetAssociatorName", m_acc_jet_track_links);
  }

  StatusCode BTagHighLevelAugmenterAlg::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << "... " );
    
    // Initialize Container keys
    ATH_MSG_DEBUG( "Inizializing containers:"        );
    ATH_MSG_DEBUG( "    ** " << m_BTagCollectionName  );

    ATH_CHECK( m_BTagCollectionName.initialize() );
    ATH_CHECK( m_bTagJetDecorLinkName.initialize() );

    FlipTagConfig flipTagConf = FlavorTagDiscriminants::flipTagConfigFromString(m_flipTagConfig);
    m_aug = std::make_unique<BTagJetAugmenter>(m_acc_jet_track_links, flipTagConf);

    m_dec_pt_uncalib = m_BTagCollectionName.key() + "." + m_aug->get_pt_uncalib_key();
    m_dec_eta_uncalib = m_BTagCollectionName.key() + "." + m_aug->get_eta_uncalib_key();
    m_dec_abs_eta_uncalib = m_BTagCollectionName.key() + "." + m_aug->get_abs_eta_uncalib_key();

    m_dec_ip2d_nTrks = m_BTagCollectionName.key() + "." + m_aug->get_ip2d_nTrks_key();
    m_dec_ip2d_isDefaults = m_BTagCollectionName.key() + "." + m_aug->get_ip2d_isDefaults_key();
    m_dec_ip2d_cu = m_BTagCollectionName.key() + "." + m_aug->get_ip2d_cu_key();
    m_dec_ip2d_bu = m_BTagCollectionName.key() + "." + m_aug->get_ip2d_bu_key();
    m_dec_ip2d_bc = m_BTagCollectionName.key() + "." + m_aug->get_ip2d_bc_key();

    m_dec_ip3d_nTrks = m_BTagCollectionName.key() + "." + m_aug->get_ip3d_nTrks_key();
    m_dec_ip3d_isDefaults = m_BTagCollectionName.key() + "." + m_aug->get_ip3d_isDefaults_key();
    m_dec_ip3d_cu = m_BTagCollectionName.key() + "." + m_aug->get_ip3d_cu_key();
    m_dec_ip3d_bu = m_BTagCollectionName.key() + "." + m_aug->get_ip2d_bu_key();
    m_dec_ip3d_bc = m_BTagCollectionName.key() + "." + m_aug->get_ip2d_bc_key();
    
    m_dec_jf_isDefaults = m_BTagCollectionName.key() + "." + m_aug->get_jf_isDefaults_key();
    m_dec_jf_deltaR = m_BTagCollectionName.key() + "." + m_aug->get_jf_deltaR_key();

    m_dec_sv1_isDefaults = m_BTagCollectionName.key() + "." + m_aug->get_sv1_isDefaults_key();
    
    m_dec_secondaryVtx_isDefaults = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_isDefaults_key();
    m_dec_secondaryVtx_nTrks = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_nTrks_key();
    m_dec_secondaryVtx_m = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_m_key();
    m_dec_secondaryVtx_E = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_E_key();
    m_dec_secondaryVtx_EFrac = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_EFrac_key();
    m_dec_secondaryVtx_L3d = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_L3d_key();
    m_dec_secondaryVtx_Lxy = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_Lxy_key();
    m_dec_secondaryVtx_min_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_min_trk_flightDirRelEta_key();
    m_dec_secondaryVtx_max_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_max_trk_flightDirRelEta_key();
    m_dec_secondaryVtx_avg_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + m_aug->get_secondaryVtx_avg_trk_flightDirRelEta_key();
    m_dec_min_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + m_aug->get_min_trk_flightDirRelEta_key();
    m_dec_max_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + m_aug->get_max_trk_flightDirRelEta_key();
    m_dec_avg_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + m_aug->get_avg_trk_flightDirRelEta_key();
    
    m_dec_smt_isDefaults = m_BTagCollectionName.key() + "." + m_aug->get_smt_isDefaults_key();

    m_dec_rnnip_isDefaults = m_BTagCollectionName.key() + "." + m_aug->get_rnnip_isDefaults_key();

    CHECK( m_dec_pt_uncalib.initialize() );
    CHECK( m_dec_eta_uncalib.initialize() );
    CHECK( m_dec_abs_eta_uncalib.initialize() );

    CHECK( m_dec_ip2d_nTrks.initialize() );
    CHECK( m_dec_ip2d_isDefaults.initialize() );
    CHECK( m_dec_ip2d_cu.initialize() );
    CHECK( m_dec_ip2d_bu.initialize() );
    CHECK( m_dec_ip2d_bc.initialize() );

    CHECK( m_dec_ip3d_nTrks.initialize() );
    CHECK( m_dec_ip3d_isDefaults.initialize() );
    CHECK( m_dec_ip3d_cu.initialize() );
    CHECK( m_dec_ip3d_bu.initialize() );
    CHECK( m_dec_ip3d_bc.initialize() );

    CHECK( m_dec_jf_isDefaults.initialize() );
    CHECK( m_dec_jf_deltaR.initialize() );

    CHECK( m_dec_sv1_isDefaults.initialize() );

    CHECK( m_dec_secondaryVtx_isDefaults.initialize() );
    CHECK( m_dec_secondaryVtx_nTrks.initialize() );
    CHECK( m_dec_secondaryVtx_m.initialize() );
    CHECK( m_dec_secondaryVtx_E.initialize() );
    CHECK( m_dec_secondaryVtx_EFrac.initialize() );
    CHECK( m_dec_secondaryVtx_L3d.initialize() );
    CHECK( m_dec_secondaryVtx_Lxy.initialize() );
    CHECK( m_dec_secondaryVtx_min_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_secondaryVtx_max_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_secondaryVtx_avg_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_min_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_max_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_avg_trk_flightDirRelEta.initialize() );

    CHECK( m_dec_smt_isDefaults.initialize() );

    CHECK( m_dec_rnnip_isDefaults.initialize() );

    return StatusCode::SUCCESS;
  }

  StatusCode BTagHighLevelAugmenterAlg::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << "... " );
  
    SG::ReadHandle< xAOD::BTaggingContainer > h_bTagContainer( m_BTagCollectionName);
    CHECK( h_bTagContainer.isValid() );
    const xAOD::BTaggingContainer* btags = h_bTagContainer.get();
    ATH_MSG_DEBUG( "Retrieved " << btags->size() << " input btagging..." );

    SG::AuxElement::Accessor< ElementLink< xAOD::JetContainer > > jetAcc("jetLink");
    for (const xAOD::BTagging * btag : *btags) {
      const xAOD::Jet* jet_ptr = *jetAcc(*btag);
      m_aug->augment(*jet_ptr);
    }
    
    return StatusCode::SUCCESS;
  }

 


}
