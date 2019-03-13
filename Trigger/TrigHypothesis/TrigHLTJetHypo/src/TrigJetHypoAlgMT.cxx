/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigJetHypoAlgMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"


using namespace TrigCompositeUtils;

using xAOD::JetContainer;

TrigJetHypoAlgMT::TrigJetHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigJetHypoAlgMT::~TrigJetHypoAlgMT() {}

StatusCode TrigJetHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}


StatusCode TrigJetHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  
  CHECK( m_hypoTools.retrieve() );
  
  CHECK( m_jetsKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrigJetHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );


  // read in the previous Decisions made before running this hypo Alg.
  // The container should have only one such Decision (for L1) as deding
  // on jets is a one step process.
  auto h_prevDecisions = SG::makeHandle(decisionInput(), context );

  if( not h_prevDecisions.isValid() || h_prevDecisions->size() ==0) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }

  if(h_prevDecisions->size() != 1){
    ATH_MSG_ERROR(" Expected one previous decisions (L1 RoIs not used), found "
                  << h_prevDecisions->size());
    return StatusCode::FAILURE;      
  }

  auto prevDecisions = h_prevDecisions.get();

  // new output decisions                                                      
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto newDecisions = outputHandle.ptr();

  // read in a jets collection, and obtain a bare pointer to it
  auto h_jets = SG::makeHandle(m_jetsKey, context );
  //  SG::ReadHandle<JetContainer> h_jets(m_jetsKey);
  ATH_MSG_DEBUG("Retrieving jets from: " << h_jets.key());
  ATH_CHECK(h_jets.isValid());

  const JetContainer* jets = h_jets.get();

  // Test whether the  hypo tools pass the event, and update the decision
  // containers accordingly.
  // for (const auto& tool: m_hypoTools) {
  //   CHECK(decide(jets, newDecisions, prevDecisions, tool));
  // }

  CHECK(decide(jets, newDecisions, prevDecisions));
 

  // output the decisions for all chains for this event.
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


StatusCode
TrigJetHypoAlgMT::decide(const xAOD::JetContainer* jets,
                         DecisionContainer* nDecisions,
                         const DecisionContainer* oDecisions) const{

 
  auto previousDecision = (*oDecisions)[0];
  auto newdecision = TrigCompositeUtils::newDecisionIn(nDecisions);

  
  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs{
    TrigCompositeUtils::decisionIDs(previousDecision).begin(), 
      TrigCompositeUtils::decisionIDs( previousDecision ).end()
      };

  for (const auto& tool: m_hypoTools) {
    auto decisionId = tool->getId();
    if (TrigCompositeUtils::passed(decisionId.numeric(),
				   previousDecisionIDs)){
      
      bool pass;
      CHECK(tool->decide(jets, pass));
      if (pass) {
	TrigCompositeUtils::addDecisionID(decisionId, newdecision);
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

// StatusCode
// TrigJetHypoAlgMT::decide(const xAOD::JetContainer* jets,
//                          std::unique_ptr<DecisionContainer>& nDecisions,
//                          const DecisionContainer* oDecisions,
//                          const ToolHandle<ITrigJetHypoToolMT>& tool) const{

 
//   auto previousDecision = (*oDecisions)[0];
  
//   const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs{
//     TrigCompositeUtils::decisionIDs(previousDecision).begin(), 
//       TrigCompositeUtils::decisionIDs( previousDecision ).end()
//       };

//   auto decisionId = tool->getId();
//   if (TrigCompositeUtils::passed(decisionId.numeric(),
//                                  previousDecisionIDs)){
//     bool pass;
//     CHECK(tool->decide(jets, pass));
//     if (pass) {
//       // create a new Decision object. This object has been placed in the
//       // nDecisions container.
//       auto decision = TrigCompositeUtils::newDecisionIn(nDecisions);
//       TrigCompositeUtils::addDecisionID(decisionId, decision);
//     }
//     // what if does not pass?
//   }
  
//   return StatusCode::SUCCESS;
// }
