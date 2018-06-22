/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

StatusCode TrigJetHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  
  CHECK( m_hypoTools.retrieve() );
  
  CHECK( m_jetsKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}

  /*
OLD
    ITC -> get 1 RoI/TC
    loop over views
       make one TC per view
       get RoIs of view - from RoI input
       get clusters of view -from View input
       map the roi to a decision - from input decisions
       create new decision with one cluster, one roi, one view

NEW
    loop over ITC
       get RoI and view of TC
       get cluster of that view
       create new decision with one cluster, one roi, one view

   */

StatusCode TrigJetHypoAlgMT::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );


  // read in the previous Decisions made before running this hypo Alg.
  // The container should have only one such Decision (for L1) as deding
  // on jets is a one step process.
  auto h_prevDecisions = SG::makeHandle(decisionInput(), context );
  ATH_CHECK(h_prevDecisions.isValid());
  

  if(h_prevDecisions->size() != 1){
    ATH_MSG_ERROR(" Expected one previous decisions (L1 RoIs not used), found "
                  << h_prevDecisions->size());
    return StatusCode::FAILURE;      
  }

  auto prevDecisions = h_prevDecisions.get();

  // Make a new Decisions container which will contain the previous
  // decisions, and the one for this hypo.
  auto newDecisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  newDecisions->setStore(aux.get());

  // read in a jets collection, and obtain a bare pointer to it
  SG::ReadHandle<JetContainer> h_jets(m_jetsKey);
  ATH_MSG_DEBUG("Retrieving jets from: " << h_jets.key());
  ATH_CHECK(h_jets.isValid());

  const JetContainer* jets = h_jets.get();

  // Pass the jet collection and the decision objects to each hypo tool.
  // There is a tool for each chain. The tol will decide whether the
  // chain passes, and update the various decision objects accordingly.
  // The updates will be seen by this Algorithm.
  for (const auto& tool: m_hypoTools) {
    CHECK(tool->decide(jets, newDecisions, prevDecisions));
  }
 
  // output the decisions for all chains for this event.
  auto outputHandle = SG::makeHandle(decisionOutput(), context);
  CHECK( outputHandle.record( std::move( newDecisions ), std::move( aux ) ) );
  ATH_MSG_DEBUG ( "Exit with "<<outputHandle->size() <<" decisions");


  // debug....
  TrigCompositeUtils::DecisionIDContainer allPassingIDs;
  for ( auto decisionObject: *outputHandle )  {
    TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
  }

  for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
    ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
  }


  return StatusCode::SUCCESS;
}
