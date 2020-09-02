/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_HIGHLEVEL_AUGMENTER_ALG_HH
#define BTAG_HIGHLEVEL_AUGMENTER_ALG_HH

// ATLAS things
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODJet/JetContainer.h"
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
    SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
    SG::ReadHandleKey<xAOD::BTaggingContainer > m_BTagCollectionName {this, "BTaggingCollectionName", "", "Input BTagging container"};
    SG::ReadDecorHandleKey<xAOD::BTaggingContainer> m_bTagJetDecorLinkName {this, "JetLinkName", "", "Element Link from BTagging to Jet container"};
    std::string m_flipTagConfig;
    std::string m_acc_jet_track_links;
    std::unique_ptr<BTagJetAugmenter> m_aug;


    typedef SG::AuxElement AE;

    // Decorator keys will be created at run-time to conform to the
    // correct container name.
    std::vector<
      std::unique_ptr<
        SG::WriteDecorHandleKey<xAOD::BTaggingContainer>
        >
      > m_write_handles;
  };
}
#endif
