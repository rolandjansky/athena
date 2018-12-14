/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetHypoAlgMT.h"

using namespace TrigCompositeUtils;

TrigBjetHypoAlgMT::TrigBjetHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetHypoAlgMT::~TrigBjetHypoAlgMT()
{}

StatusCode TrigBjetHypoAlgMT::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_MSG_DEBUG( "Initializing Tools" );
  ATH_CHECK( m_hypoTools.retrieve() );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_bTagKey.initialize() );
  CHECK( m_roisKey.initialize() );
  CHECK( m_jetKey.initialize()  );

  CHECK( m_decisionsKey.initialize() );

  ATH_MSG_INFO("Initializing TrigBjetHypoAlgMT");

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_roisKey          );
  ATH_MSG_DEBUG(  "   " << m_bTagKey          );
  ATH_MSG_DEBUG(  "   " << m_jetKey           );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetHypoAlgMT::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionHandle.isValid() );
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = prevDecisionHandle.get();
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");

  return StatusCode::SUCCESS;
}




