/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigMissingETHypoAlgMT.h"

#include "Gaudi/Property.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

using namespace TrigCompositeUtils;
using xAOD::TrigMissingETContainer;

TrigMissingETHypoAlgMT::TrigMissingETHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigMissingETHypoAlgMT::initialize() {

  CHECK( m_hypoTools.retrieve() );
  CHECK( m_metKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigMissingETHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // read in the met container
  auto h_met = SG::makeHandle(m_metKey, context );
  ATH_MSG_DEBUG("Retrieving MET from: " << h_met.key());
  ATH_CHECK(h_met.isValid());
  const TrigMissingETContainer* metContainer = h_met.get();

  ATH_MSG_DEBUG("The MET value is " << round(std::hypot((*metContainer)[0]->ex(), (*metContainer)[0]->ey())/1000.0 ));
  
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
  // (Creating a single output as executing on a single MET value)
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  DecisionContainer* newDecisions = outputHandle.ptr();

  // Make trigger decisions and save to "newDecisions"
  ATH_CHECK(decide(metContainer, newDecisions, prevDecisions, context));

  // Common debug printing
  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  return StatusCode::SUCCESS;
}



StatusCode TrigMissingETHypoAlgMT::decide(const xAOD::TrigMissingETContainer* metContainer,
                         TrigCompositeUtils::DecisionContainer*  newDecisions,
                         const DecisionContainer* oldDecisions,
                         const EventContext& context) const{

  ATH_MSG_DEBUG("Executing decide() of " << name() );
  if (oldDecisions->size() != 1) {
    ATH_MSG_ERROR("TrigMissingETHypoAlgMT requires there to be exactly one previous Decision object, but found " << oldDecisions->size());
    return StatusCode::FAILURE;
  }
  const Decision* previousDecision = oldDecisions->at(0);
  
  if (metContainer->size()==0){
    ATH_MSG_ERROR("There are no TrigEFMissingET objects in the MET container" );
    return StatusCode::FAILURE;
  } else if (metContainer->size()>1) {
    ATH_MSG_ERROR("There is more than one TrigEFMissingET object in the MET container");
    return StatusCode::FAILURE;
  }

  // Create output Decision object, link it to prevDecision & its "feature"
  auto newDecision = TrigCompositeUtils::newDecisionIn(newDecisions, previousDecision, "", context);
  ElementLink<xAOD::TrigMissingETContainer> metElementLink = ElementLink<xAOD::TrigMissingETContainer>(*metContainer, /*index*/ 0);
  newDecision->setObjectLink<xAOD::TrigMissingETContainer>(featureString(), metElementLink);
  
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
      ATH_CHECK(tool->decide(metContainer, pass));
      if (pass) {
        ATH_MSG_DEBUG("Passed " << tool->name() );
    		TrigCompositeUtils::addDecisionID(decisionId, newDecision);
      } else ATH_MSG_DEBUG("Didn't pass " << tool->name() );
    } else ATH_MSG_DEBUG("Didn't pass previous trigger step");
  }
  
  return StatusCode::SUCCESS;
}

