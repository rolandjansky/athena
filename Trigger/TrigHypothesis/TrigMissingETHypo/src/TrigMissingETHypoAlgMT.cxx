/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigMissingETHypoAlgMT.h"

#include "GaudiKernel/Property.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/MonitoredScope.h"


using namespace TrigCompositeUtils;

using xAOD::TrigMissingETContainer;

TrigMissingETHypoAlgMT::TrigMissingETHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigMissingETHypoAlgMT::~TrigMissingETHypoAlgMT() {}

StatusCode TrigMissingETHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}


StatusCode TrigMissingETHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

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
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  ATH_MSG_DEBUG( "Decisions being read from " << decisionInput().key() );
  ATH_MSG_DEBUG( "Running with "<< h_prevDecisions->size() <<" implicit ReadHandles for previous decisions");
  ATH_CHECK(h_prevDecisions.isValid());
  auto prevDecisions = h_prevDecisions.get();

  // Make a new Decisions container which will contain the previous
  // decisions, and the one for this hypo.
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto newDecisions = outputHandle.ptr();


  // Make trigger decisions and save to "newDecisions"
  CHECK(decide(metContainer, newDecisions, prevDecisions));

  ATH_MSG_DEBUG ( "Exit with "<<outputHandle->size() <<" decisions");


  // debug....
  if ( msgLvl(MSG::DEBUG)) {
    TrigCompositeUtils::DecisionIDContainer allPassingIDs;
    for ( auto decisionObject: *outputHandle )  {
      TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
    }
    
    for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
      ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode TrigMissingETHypoAlgMT::decide(const xAOD::TrigMissingETContainer* metContainer,
                         TrigCompositeUtils::DecisionContainer*  nDecisions,
                         const DecisionContainer* oDecisions) const{

  ATH_MSG_DEBUG("Deciding" << name() );
  auto previousDecision = (*oDecisions)[0];
  auto newdecision = TrigCompositeUtils::newDecisionIn(nDecisions);

  
  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs{
    TrigCompositeUtils::decisionIDs(previousDecision).begin(), 
      TrigCompositeUtils::decisionIDs( previousDecision ).end()
      };

  //bool allPassed = true;
  for (const auto& tool: m_hypoTools) {
    auto decisionId = tool->getId();
    if (TrigCompositeUtils::passed(decisionId.numeric(), previousDecisionIDs)){
      ATH_MSG_DEBUG("Passed previous trigger");
      bool pass;
      CHECK(tool->decide(metContainer, pass));
      if (pass) {
        ATH_MSG_DEBUG("Passed HypoTools");
    		TrigCompositeUtils::addDecisionID(decisionId, newdecision);
      } else ATH_MSG_DEBUG("Didn't pass HypoTools");
    } ATH_MSG_DEBUG("Didn't pass previous trigger");
  }
  
  return StatusCode::SUCCESS;
}

