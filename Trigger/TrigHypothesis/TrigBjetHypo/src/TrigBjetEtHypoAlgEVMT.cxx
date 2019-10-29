/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetEtHypoAlgEVMT.h"

TrigBjetEtHypoAlgEVMT::TrigBjetEtHypoAlgEVMT( const std::string& name,
					      ISvcLocator* pSvcLocator ) :
  ::TrigBjetEtHypoAlgMT( name,pSvcLocator ) {}


StatusCode TrigBjetEtHypoAlgEVMT::initialize() {

  CHECK( TrigBjetEtHypoAlgMT::initialize() );
  CHECK( m_inputTracksKey.initialize() );

  renounce( m_inputJetsKey );  

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgEVMT::retrieveJets( const EventContext& context,
						ElementLinkVector< xAOD::JetContainer >& jetELs,
						const SG::ReadHandleKey< xAOD::JetContainer >& inputJetsKey,
						const TrigCompositeUtils::DecisionContainer* prevDecisionContainer ) const {

  CHECK( retrieveObjectFromEventView( context,jetELs,inputJetsKey,prevDecisionContainer ) );
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgEVMT::retrieveRoIs( const EventContext& /*context*/,
						const TrigRoiDescriptorCollection*& /*roiContainer*/,
						const SG::ReadHandleKey< TrigRoiDescriptorCollection >& /*inputRoIKey*/ ) const {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgEVMT::setTracksLink( const EventContext& context,
						 const SG::ReadHandleKey< xAOD::TrackParticleContainer >& trackKey,
						 const unsigned int index,
						 const TrigCompositeUtils::DecisionContainer*& inputDecisions,
						 std::vector< TrigCompositeUtils::Decision* >& outputDecisions ) const {

  ElementLink< ViewContainer > viewEL = inputDecisions->at(index)->objectLink< ViewContainer >( TrigCompositeUtils::viewString() );
  ATH_CHECK( viewEL.isValid() );

  SG::ReadHandle< xAOD::TrackParticleContainer > calTrackHandle = ViewHelper::makeHandle( *viewEL,trackKey,context );
  ATH_CHECK( calTrackHandle.isValid() );
  
  ElementLinkVector< xAOD::TrackParticleContainer > tracksELs;
  for ( unsigned int i(0); i<calTrackHandle->size(); i++ ) {
    ElementLink< xAOD::TrackParticleContainer > trackEL( trackKey.key(),i );
    ATH_CHECK( trackEL.isValid() );
    tracksELs.push_back( trackEL );
  }

  outputDecisions.at( index )->addObjectCollectionLinks( m_trackLink.value(),tracksELs);
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgEVMT::setJetLink( const EventContext& context,
					      const SG::ReadHandleKey< xAOD::JetContainer >& jetKey,
					      const unsigned int index,
					      const TrigCompositeUtils::DecisionContainer*& inputDecisions,
					      std::vector< TrigCompositeUtils::Decision* >& outputDecisions ) const {
  
  ElementLink< ViewContainer > viewEL = inputDecisions->at(index)->objectLink< ViewContainer >( TrigCompositeUtils::viewString() );
  ATH_CHECK( viewEL.isValid() );
  
  SG::ReadHandle< xAOD::JetContainer > calJetHandle = ViewHelper::makeHandle( *viewEL, jetKey, context );
  ATH_CHECK( calJetHandle.isValid() );
  
  ElementLink< xAOD::JetContainer > jetEL = ViewHelper::makeLink( *viewEL, calJetHandle, 0 );
  ATH_CHECK( jetEL.isValid() );
  
  outputDecisions.at( index )->setObjectLink( TrigCompositeUtils::featureString(),jetEL);
  return StatusCode::SUCCESS;
}

const TrigCompositeUtils::Decision* TrigBjetEtHypoAlgEVMT::getPreviousDecision( const TrigCompositeUtils::DecisionContainer* decisionContainer,
										unsigned int index ) const {
  return decisionContainer->at( index );
}

StatusCode TrigBjetEtHypoAlgEVMT::attachLinkToDecisions( const EventContext& context,
                                                       const TrigCompositeUtils::DecisionContainer* prevDecisionContainer,
                                                       std::vector< TrigCompositeUtils::Decision* >& newDecisions ) const {
  const unsigned int nDecisions = newDecisions.size();
  
  for ( unsigned int index(0); index<nDecisions; index++ ) {
    CHECK( setJetLink( context,m_inputJetsKey,index,prevDecisionContainer,newDecisions ) );
    CHECK( setTracksLink( context,m_inputTracksKey,index,prevDecisionContainer,newDecisions ) );
  }
  
  ATH_MSG_DEBUG("   ** Added object links to output decision");
  
  return StatusCode::SUCCESS;
}
