/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_HIGHLEVEL_AUGMENTER_ALG_HH
#define BTAG_HIGHLEVEL_AUGMENTER_ALG_HH

// ATLAS things
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODJet/Jet.h"
#include "xAODBTagging/BTagging.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadDecorHandle.h"

#include "FlavorTagDiscriminants/BTagJetAugmenter.h"

#include "FlavorTagDiscriminants/FlipTagEnums.h"
typedef FlavorTagDiscriminants::FlipTagConfig FlipTagConfig;

namespace Analysis {
  class BTagHighLevelAugmenterAlg: public AthAlgorithm {
  public:
    BTagHighLevelAugmenterAlg(const std::string& name,
                          ISvcLocator* pSvcLocator );

    StatusCode initialize() override final;
    StatusCode execute() override final;

   
  private:
    // Input Containers
    SG::ReadHandleKey<xAOD::BTaggingContainer > m_BTagCollectionName {this, "BTaggingCollectionName", "", "Input BTagging container"};
    SG::ReadDecorHandleKey<xAOD::BTaggingContainer> m_bTagJetDecorLinkName {this, "JetLinkName", "", "Element Link from BTagging to Jet container"};
    std::string m_flipTagConfig;
    std::string m_acc_jet_track_links;
    std::unique_ptr<BTagJetAugmenter> m_aug;


    typedef SG::AuxElement AE;
    
    // Decorator keys will be modified at run-time to conform to the correct container name
    // For the run-time update to work, the decoration key name properties must start with a period (".")

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_pt_uncalib {this, "pt_uncalib", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_eta_uncalib {this, "eta_uncalib", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_abs_eta_uncalib {this, "abs_eta_uncalib", "", ""};

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip2d_nTrks {this, "ip2d_nTrks", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip2d_isDefaults {this, "ip2d_isDefaults", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip2d_cu {this, "ip2d_cu", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip2d_bu {this, "ip2d_bu", "", "" };
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip2d_bc {this, "ip2d_bc", "", ""};

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip3d_nTrks {this, "ip3d_nTrks", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip3d_isDefaults {this, "ip3d_isDefaults", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip3d_cu {this, "ip3d_cu", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip3d_bu {this, "ip3d_bu", "", "" };
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_ip3d_bc {this, "ip3d_bc", "", ""};

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_jf_isDefaults {this, "jf_isDefaults", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_jf_deltaR {this, "jf_deltaR", "", ""};

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_sv1_isDefaults {this, "sv1_isDefaults", "", ""};

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_isDefaults {this, "secondaryVtx_isDefaults", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_nTrks {this, "secondaryVtx_nTrks", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_m {this, "secondaryVtx_m", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_E {this, "secondaryVtx_E", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_EFrac {this, "secondaryVtx_EFrac", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_L3d {this, "secondaryVtx_L3d", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_Lxy {this, "secondaryVtx_Lxy", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_min_trk_flightDirRelEta {this, "secondaryVtx_min_trk_flightDirRelEta", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_max_trk_flightDirRelEta {this, "secondaryVtx_max_trk_flightDirRelEta", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_secondaryVtx_avg_trk_flightDirRelEta {this, "secondaryVtx_avg_trk_flightDirRelEta", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_min_trk_flightDirRelEta {this, "min_trk_flightDirRelEta", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_max_trk_flightDirRelEta {this, "max_trk_flightDirRelEta", "", ""};
    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_avg_trk_flightDirRelEta {this, "avg_trk_flightDirRelEta", "", ""};

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_smt_isDefaults {this, "smt_isDefaults", "", ""};

    SG::WriteDecorHandleKey< xAOD::BTaggingContainer > m_dec_rnnip_isDefaults {this, "rnnip_isDefaults", "", ""};
  };
}

#endif
