/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigStreamerHypoAlgMT.h"

#include "Gaudi/Property.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

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
  
  // read in the previous Decisions made before running this hypo Alg.
  auto h_prevDecisions = SG::makeHandle(decisionInput(), context );
  if( not h_prevDecisions.isValid() ) {//implicit
    ATH_MSG_ERROR( "No implicit RH for previous decisions "<<  decisionInput().key()<<". Check the input decisions key?" );
    return StatusCode::FAILURE;      
  }
  ATH_MSG_DEBUG( "Decisions being read from " << decisionInput().key() );
  ATH_MSG_DEBUG( "Running with "<< h_prevDecisions->size() <<" implicit ReadHandles for previous decisions");
  auto prevDecisions = h_prevDecisions.get();

  // Make a new Decisions container which will contain the new Decision object created by this hypo.
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  DecisionContainer* newDecisions = outputHandle.ptr();

  // -----------------------------------------------------------------------
  if (prevDecisions->size() != 1) {
    ATH_MSG_ERROR("TrigStreamerHypoAlgMT requires there to be exactly one previous Decision object, but found " << prevDecisions->size());
    return StatusCode::FAILURE;
  }
  const Decision* previousDecision = prevDecisions->at(0);
  
  // Create output Decision object, link it to prevDecision & its "feature"
  auto newDecision = TrigCompositeUtils::newDecisionIn(newDecisions, previousDecision, "", context);
  
  // Get set of active chains
  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs{
    TrigCompositeUtils::decisionIDs(previousDecision).begin(), 
    TrigCompositeUtils::decisionIDs(previousDecision).end()
  };

  //bool allPassed = true;
  for (const auto& tool: m_hypoTools) {
    auto decisionId = tool->getId();
    ATH_MSG_DEBUG( "About to decide for " << tool->name() );
    if (TrigCompositeUtils::passed(decisionId.numeric(), previousDecisionIDs)){
      ATH_MSG_DEBUG("Passed previous trigger step");
      bool pass;
      ATH_CHECK(tool->decide(pass)); //this is unnecessary as the hypoTools are streamers, but lets go
      if (pass) {
        ATH_MSG_DEBUG("Passed " << tool->name() );
    		TrigCompositeUtils::addDecisionID(decisionId, newDecision);
      } else ATH_MSG_DEBUG("Didn't pass " << tool->name() );
    } else ATH_MSG_DEBUG("Didn't pass previous trigger step");
  }
  // -----------------------------------------------------------------------

  // Common debug printing
  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  return StatusCode::SUCCESS;
}

