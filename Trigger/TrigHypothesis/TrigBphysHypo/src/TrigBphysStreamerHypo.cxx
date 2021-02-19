/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysStreamerHypo.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;


TrigBphysStreamerHypo::TrigBphysStreamerHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::HypoBase(name, pSvcLocator) {}


StatusCode TrigBphysStreamerHypo::initialize() {
  ATH_MSG_DEBUG( "TrigBphysStreamerHypo::initialize()" );

  if (m_triggerLevel != "L2" && m_triggerLevel != "EF") {
    ATH_MSG_ERROR( "triggerLevel should be L2 or EF, but " << m_triggerLevel << " provided" );
    return StatusCode::FAILURE;
  }

  // convert chain names into hashes
  if (!m_triggerList.empty()) {
    ATH_MSG_DEBUG( "The ID will be copied from the previous decision if it matches with some trigger from the list below:" );
    for (size_t i = 0; i < m_triggerList.size(); ++i) {
      const auto id = HLT::Identifier(m_triggerList[i]);
      m_triggerIDs.insert(id.numeric());
      ATH_MSG_DEBUG( i << " " << m_triggerList[i] );
    }
  }
  else {
    ATH_MSG_DEBUG( "The triggerList is empty, all decision IDs will be copied from the previous decisions" );
  }

  ATH_CHECK( m_hypoTools.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode TrigBphysStreamerHypo::execute( const EventContext& context ) const {

  ATH_MSG_DEBUG( "TrigBphysStreamerHypo::execute() starts" );

  ATH_MSG_DEBUG( "decision input key: " << decisionInput().key() );
  auto previousDecisionsHandle = SG::makeHandle(decisionInput(), context);
  CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with " << previousDecisionsHandle->size() << " previous decisions" );

  // create the mutable output DecisionContainer and register it to StoreGate
  SG::WriteHandle<DecisionContainer> outputHandle = TrigCompositeUtils::createAndStore(decisionOutput(), context);
  DecisionContainer* decisions = outputHandle.ptr();

  for (const Decision* previousDecision : *previousDecisionsHandle) {
    Decision* decision = TrigCompositeUtils::newDecisionIn(decisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);

    if (!m_triggerList.empty()) {
      TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
      for (const auto& previousDecisionID : previousDecisionIDs) {
        // we should use the HLT identifier which corresponds to a whole chain
        HLT::Identifier id = TrigCompositeUtils::getIDFromLeg(HLT::Identifier(previousDecisionID));
        if (m_triggerIDs.find(id.numeric()) != m_triggerIDs.end()) {
          TrigCompositeUtils::addDecisionID(previousDecisionID, decision);
        }
      }
    }
    else {
      TrigCompositeUtils::insertDecisionIDs(previousDecision, decision);
    }

    if (m_triggerLevel == "L2") {
      auto muonLinkInfo = TrigCompositeUtils::findLink<xAOD::L2StandAloneMuonContainer>(previousDecision, TrigCompositeUtils::featureString(), true);
      ATH_CHECK( muonLinkInfo.isValid() );
      decision->setObjectLink<xAOD::L2StandAloneMuonContainer>(TrigCompositeUtils::featureString(), muonLinkInfo.link);
    }
    else if (m_triggerLevel == "EF") {
      auto muonLinkInfo = TrigCompositeUtils::findLink<xAOD::MuonContainer>(previousDecision, TrigCompositeUtils::featureString(), true);
      ATH_CHECK( muonLinkInfo.isValid() );
      decision->setObjectLink<xAOD::MuonContainer>(TrigCompositeUtils::featureString(), muonLinkInfo.link);

      if (msgLvl(MSG::DEBUG)) {
        const xAOD::Muon* muon = *(muonLinkInfo.link);
        if (muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) {
          const ElementLink<xAOD::TrackParticleContainer> trackEL = muon->inDetTrackParticleLink();
          ATH_CHECK( trackEL.isValid() );
          const xAOD::TrackParticle* track = *trackEL;
          ATH_MSG_DEBUG( " -- muon pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
          ATH_MSG_DEBUG( " Allowed decisions:" );
          DecisionIDContainer IDs;
          TrigCompositeUtils::decisionIDs(decision, IDs);
          for (const TrigCompositeUtils::DecisionID& id : IDs) {
            ATH_MSG_DEBUG( " +++ " << HLT::Identifier(id) );
          }
        }
      }
    }

  }

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
