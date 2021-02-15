/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysStreamerHypo.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;


TrigBphysStreamerHypo::TrigBphysStreamerHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::HypoBase(name, pSvcLocator) {}


StatusCode TrigBphysStreamerHypo::initialize() {
  ATH_MSG_DEBUG( "TrigBphysStreamerHypo::initialize()" );

  ATH_CHECK( m_hypoTools.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode TrigBphysStreamerHypo::execute( const EventContext& context ) const {

  ATH_MSG_DEBUG( "TrigBphysStreamerHypo::execute() starts" );

  ATH_MSG_DEBUG( "decision input key: " << decisionInput().key() );
  auto previousDecisionsHandle = SG::makeHandle(decisionInput(), context);
  CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() << " previous decisions" );

  // create the mutable output DecisionContainer and register it to StoreGate
  SG::WriteHandle<DecisionContainer> outputHandle = TrigCompositeUtils::createAndStore(decisionOutput(), context);
  DecisionContainer* decisions = outputHandle.ptr();

  for (const Decision* previousDecision : *previousDecisionsHandle) {
    Decision* decision = TrigCompositeUtils::newDecisionIn(decisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);

    auto muonLinkInfo = TrigCompositeUtils::findLink<xAOD::MuonContainer>(previousDecision, TrigCompositeUtils::featureString(), true);
    ATH_CHECK( muonLinkInfo.isValid() );
    decision->setObjectLink<xAOD::MuonContainer>(TrigCompositeUtils::featureString(), muonLinkInfo.link);
    TrigCompositeUtils::insertDecisionIDs(previousDecision, decision);

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

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
