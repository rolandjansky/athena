/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigJetEJsHypoAlg.h"
#include "TrigJetEJsHypoTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

using namespace TrigCompositeUtils;

using xAOD::JetContainer;
using xAOD::TrackParticleContainer;
using xAOD::VertexContainer;


TrigJetEJsHypoAlg::TrigJetEJsHypoAlg( const std::string& name, 
    ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) { }


StatusCode TrigJetEJsHypoAlg::initialize() {

  CHECK( m_hypoTools.retrieve() );
  CHECK( m_tracksKey.initialize() );
  CHECK( m_PV.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TrigJetEJsHypoAlg::execute( const EventContext& context ) const {

  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // get the HLT decision container for the previous decisions (one per jet in the original collection)

  ATH_MSG_DEBUG("Retrieving HLT decision \"" << decisionInput().key() << "\"");
  auto previousDecisionsHandle = SG::makeHandle(decisionInput(), context );
  ATH_CHECK(previousDecisionsHandle.isValid());
  const DecisionContainer* prevDecisions = previousDecisionsHandle.get();

  // read in tracks container
  ATH_MSG_DEBUG("Retrieving tracks from the track container  \"" << m_tracksKey << "\"");
  auto TracksHandle = SG::makeHandle(m_tracksKey, context );
  ATH_CHECK(TracksHandle.isValid());
  const TrackParticleContainer* tracks = TracksHandle.get();

  // read in primary vertex container
  ATH_MSG_DEBUG("Retrieving PV from the vertex container  \"" << m_PV << "\"");
  auto PVHandle = SG::makeHandle(m_PV, context );
  ATH_CHECK(PVHandle.isValid());
  const VertexContainer* vertices = PVHandle.get();
  const xAOD::Vertex_v1 *PV = vertices->at(0);


  std::vector<TrigJetEJsHypoTool::JetInfo> hypoToolInput;
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);
  DecisionContainer* outputDecisions = outputHandle.ptr();

  for (const Decision* previousDecision : *prevDecisions) {
    LinkInfo< xAOD::JetContainer > myFeature = findLink< xAOD::JetContainer >(previousDecision, featureString());
    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
    Decision* newDecision = newDecisionIn(outputDecisions, previousDecision, hypoAlgNodeName(), context);
    newDecision->setObjectLink<xAOD::JetContainer>(featureString(), myFeature.link);
    hypoToolInput.emplace_back(TrigJetEJsHypoTool::JetInfo{newDecision, *(myFeature.link), tracks, PV, previousDecisionIDs});
  }//end loop on previous decisions
  

  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG("Now computing decision for " << tool->name());
    CHECK(tool->decide(hypoToolInput));
  }//end loop on hypoTools


  DecisionContainer::iterator it = outputDecisions->begin();
  while(it != outputDecisions->end()) {
    if (allFailed( *it )) {
      it = outputDecisions->erase(it);
    } else {
      ++it;
    }
  }//end loop on output decisios
  
  return StatusCode::SUCCESS;

}//end execute
