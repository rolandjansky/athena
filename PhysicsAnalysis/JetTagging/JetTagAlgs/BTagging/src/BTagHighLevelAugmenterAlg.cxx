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

    ATH_CHECK( m_JetCollectionName.initialize() );
    ATH_CHECK( m_BTagCollectionName.initialize() );
    ATH_CHECK( m_bTagJetDecorLinkName.initialize() );

    FlipTagConfig flipTagConf = FlavorTagDiscriminants::flipTagConfigFromString(m_flipTagConfig);
    m_aug = std::make_unique<BTagJetAugmenter>(m_acc_jet_track_links, flipTagConf);

    // create and initialize write handles
    for (const std::string& key: m_aug->getDecoratorKeys()) {
      std::string full_key = m_BTagCollectionName.key() + "." + key;
      ATH_MSG_DEBUG("Adding " << full_key);
      m_write_handles.emplace_back(
        std::make_unique<SG::WriteDecorHandleKey<xAOD::BTaggingContainer>>(
          this, key, full_key, ""));
      ATH_MSG_DEBUG("Initializing " << full_key);
      ATH_CHECK(m_write_handles.back()->initialize());
    }
    return StatusCode::SUCCESS;
  }

  StatusCode BTagHighLevelAugmenterAlg::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << "... " );
  
    EventContext ctx = Gaudi::Hive::currentContext();

    //retrieve the Jet container
    SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName, ctx);
    if (!h_JetCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    SG::ReadHandle< xAOD::BTaggingContainer > h_bTagContainer( m_BTagCollectionName, ctx);
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
