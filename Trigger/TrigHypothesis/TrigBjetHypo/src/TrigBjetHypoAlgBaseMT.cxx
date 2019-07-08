/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetHypoAlgBaseMT.h"
#include "AthViews/ViewHelper.h"

TrigBjetHypoAlgBaseMT::TrigBjetHypoAlgBaseMT( const std::string& name, 
					      ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetHypoAlgBaseMT::~TrigBjetHypoAlgBaseMT() {}

// ============================================================================== //

StatusCode TrigBjetHypoAlgBaseMT::retrievePreviousDecisionContainer( const EventContext& context,
								     const TrigCompositeUtils::DecisionContainer*& prevDecisionContainer ) const {

  ATH_MSG_DEBUG( "Retrieving Previous Decision" );
  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionContainerHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionContainerHandle.isValid() );

  prevDecisionContainer = prevDecisionContainerHandle.get();
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetHypoAlgBaseMT::retrieveJetsFromEventView( const EventContext& context,
							     ElementLinkVector< xAOD::JetContainer >& jetELs,
							     const SG::ReadHandleKey< xAOD::JetContainer >& inputJetsKey,
							     const TrigCompositeUtils::DecisionContainer* prevDevisionContainer) const {

  ATH_MSG_DEBUG( "Retrieving jets from Event View" );

  for ( const TrigCompositeUtils::Decision* previousDecision: *prevDevisionContainer ) {
    // get View
    ElementLink< ViewContainer > viewEL = previousDecision->objectLink< ViewContainer >( "view" );
    ATH_CHECK( viewEL.isValid() );
    ATH_MSG_DEBUG( "Retrieved View" );
    
    SG::ReadHandle< xAOD::JetContainer > jetsHandle = ViewHelper::makeHandle( *viewEL, inputJetsKey, context );
    ATH_CHECK( jetsHandle.isValid() );
    ATH_MSG_DEBUG ( "EventView " << (*viewEL)->name() << " has JetContainer of size: " << jetsHandle->size() );

    size_t jetCounter = 0;
    for ( auto it = jetsHandle->begin(); it != jetsHandle->end(); ++it, ++jetCounter ) {
      ElementLink< xAOD::JetContainer > jetEL = ViewHelper::makeLink<xAOD::JetContainer>( *viewEL, jetsHandle, jetCounter );
      ATH_CHECK( jetEL.isValid() );
      jetELs.push_back( jetEL );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetHypoAlgBaseMT::retrieveJetsFromStoreGate( const EventContext& context,
							     ElementLinkVector< xAOD::JetContainer >& jetELs,
							     const SG::ReadHandleKey< xAOD::JetContainer >& inputJetsKey,
							     const TrigCompositeUtils::DecisionContainer* /*prevDevisionContainer*/) const {
  ATH_MSG_DEBUG( "Retrieving jets from StoreGate" );

  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( inputJetsKey,context );
  ATH_MSG_DEBUG( "Retrieved jets from : " << inputJetsKey.key() );
  CHECK( jetContainerHandle.isValid() );
  const xAOD::JetContainer *jetCollection = jetContainerHandle.get();

  for (const xAOD::Jet* jet : *jetCollection) {
    ElementLink< xAOD::JetContainer > jetLink = ElementLink< xAOD::JetContainer >(*jetCollection, jet->index());
    jetELs.push_back( jetLink );
  }

  return StatusCode::SUCCESS;
}
