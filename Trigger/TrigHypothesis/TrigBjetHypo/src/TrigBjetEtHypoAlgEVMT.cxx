/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetEtHypoAlgEVMT.h"

TrigBjetEtHypoAlgEVMT::TrigBjetEtHypoAlgEVMT( const std::string& name,
					      ISvcLocator* pSvcLocator ) :
  ::TrigBjetEtHypoAlgMT( name,pSvcLocator ) {}

TrigBjetEtHypoAlgEVMT::~TrigBjetEtHypoAlgEVMT() {}

StatusCode TrigBjetEtHypoAlgEVMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  CHECK( TrigBjetEtHypoAlgMT::initialize() );
  renounce( m_inputJetsKey );  

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgEVMT::retrieveJets( const EventContext& context,
						ElementLinkVector< xAOD::JetContainer >& jetELs,
						const SG::ReadHandleKey< xAOD::JetContainer >& inputJetsKey,
						const TrigCompositeUtils::DecisionContainer* prevDecisionContainer ) const {

  CHECK( retrieveJetsFromEventView( context,jetELs,inputJetsKey,prevDecisionContainer ) );
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgEVMT::retrieveRoIs( const EventContext& /*context*/,
						const TrigRoiDescriptorCollection*& /*roiContainer*/,
						const SG::ReadHandleKey< TrigRoiDescriptorCollection >& /*inputRoIKey*/ ) const {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgEVMT::setJetLink( const EventContext& context,
					      const SG::ReadHandleKey< xAOD::JetContainer >& jetKey,
					      const unsigned int index,
					      const TrigCompositeUtils::DecisionContainer*& inputDecisions,
					      std::vector< TrigCompositeUtils::Decision* >& outputDecisions ) const {
  
  ElementLink< ViewContainer > viewEL = inputDecisions->at(index)->objectLink< ViewContainer >( "view" );
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

