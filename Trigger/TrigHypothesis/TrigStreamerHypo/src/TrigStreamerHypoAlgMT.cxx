/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigStreamerHypoAlgMT.h"

#include "Gaudi/Property.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;

TrigStreamerHypoAlgMT::TrigStreamerHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigStreamerHypoAlgMT::initialize() {

  CHECK( m_hypoTools.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TrigStreamerHypoAlgMT::execute( const EventContext& context ) const {  

  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto h_prevDecisions = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( h_prevDecisions.isValid() );
  ATH_MSG_DEBUG( "Running with "<< h_prevDecisions->size() <<" previous decisions");

  // Make a new Decisions container which will contain the new Decision object created by this hypo.
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  DecisionContainer* newDecisions = outputHandle.ptr();

  // Struct to pass info on to the HypoTools
  std::vector< ITrigStreamerHypoToolMT::HypoInfo > hypoInfo;

  for (const Decision* previousDecision : *h_prevDecisions ) {
    // Create output Decision object, link it to prevDecision.
    Decision* newDecision = newDecisionIn(newDecisions, previousDecision, hypoAlgNodeName(), context);

    // Obligatory link to feature. Use a dummy link here (a link to self)
    ElementLink<DecisionContainer> dummyLink(*newDecisions, newDecisions->size()-1, context);
    newDecision->setObjectLink(featureString(), dummyLink);

    hypoInfo.emplace_back(newDecision, previousDecision);
  }

  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG( "About to decide for " << tool->name() );
    ATH_CHECK(tool->decide(hypoInfo)); // The StreamerHypoTool only contains logical-flow checks
  }

  // Common debug printing
  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  return StatusCode::SUCCESS;
}

