/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetHypoAlgBase.h"

TrigBjetHypoAlgBase::TrigBjetHypoAlgBase( const std::string& name, 
					      ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetHypoAlgBase::~TrigBjetHypoAlgBase() {}

// ============================================================================== //

StatusCode TrigBjetHypoAlgBase::retrievePreviousDecisionContainer( const EventContext& context,
								     const TrigCompositeUtils::DecisionContainer*& prevDecisionContainer ) const {

  ATH_MSG_DEBUG( "Retrieving Previous Decision" );
  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionContainerHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionContainerHandle.isValid() );

  prevDecisionContainer = prevDecisionContainerHandle.get();
  return StatusCode::SUCCESS;
}

