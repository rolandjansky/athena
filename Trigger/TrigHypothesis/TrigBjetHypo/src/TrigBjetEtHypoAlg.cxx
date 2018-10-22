/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlg.h"


TrigBjetEtHypoAlg::TrigBjetEtHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetEtHypoAlg::~TrigBjetEtHypoAlg()
{}

StatusCode TrigBjetEtHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_jetsKey          );
  //  ATH_MSG_DEBUG(  "   " << m_decisionsKey     );

  ATH_MSG_DEBUG( "Initializing Tools" );
  ATH_CHECK( m_hypoTools.retrieve() );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_jetsKey.initialize() );
  //  CHECK( m_decisionsKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlg::execute_r( const EventContext& context ) const {  
  ATH_MSG_INFO ( "Executing " << name() << "..." );

  // In case I need it (not sure). Taken from Jet code
  // Read in previous Decisions made before running this Hypo Alg.
  // The container should have only one such Decision in case we are cutting on 'j' threshold (for L1)
  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionHandle.isValid() );
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = prevDecisionHandle.get();
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");

  // Retrieve Jet Container
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_jetsKey,context );
  ATH_MSG_DEBUG( "Retrieved jets from : " << m_jetsKey.key() );
  CHECK( jetContainerHandle.isValid() );

  const xAOD::JetContainer *jetCollection = jetContainerHandle.get();
  ATH_MSG_DEBUG( "Found " << jetCollection->size()<< " jets."  );
  for ( const xAOD::Jet *jet : * jetCollection ) 
    ATH_MSG_INFO("   -- Jet pt=" << jet->p4().Et() <<" eta="<< jet->eta() << " phi="<< jet->phi() );

  // Prepare Output 
  std::unique_ptr< TrigCompositeUtils::DecisionContainer > decisions = std::make_unique< TrigCompositeUtils::DecisionContainer >();
  std::unique_ptr< TrigCompositeUtils::DecisionAuxContainer > aux = std::make_unique< TrigCompositeUtils::DecisionAuxContainer >();
  decisions->setStore( aux.get() );


  // Taken from Jet Code here 
  const TrigCompositeUtils::Decision *prevDecision = prevDecisionContainer->at(0);
  TrigCompositeUtils::Decision *newDecision = TrigCompositeUtils::newDecisionIn( decisions.get() );

  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs { 
    TrigCompositeUtils::decisionIDs( prevDecision ).begin(),
      TrigCompositeUtils::decisionIDs( prevDecision ).end() 
      };

  // Run decide method of hypo tools
  /*
  for ( const ToolHandle< TrigBjetEtHypoTool >& tool : m_hypoTools ) { 
    bool pass = false;
    CHECK( tool->decide(jetCollection,pass) );
    ATH_MSG_INFO( "Event Passes ? " << (pass?"Y":"N") );
  }
  */
   
  // Decide (Hypo Tool)
  for ( const ToolHandle< TrigBjetEtHypoTool >& tool : m_hypoTools ) {
    const HLT::Identifier  decisionId = tool->getId();
    if ( TrigCompositeUtils::passed( decisionId.numeric() , previousDecisionIDs ) ) {
      bool pass = false;
      CHECK( tool->decide( jetCollection,pass ) );
      if ( pass ) TrigCompositeUtils::addDecisionID( decisionId,newDecision );
    }
  }


  // Save Output
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle =  SG::makeHandle( decisionOutput(), context );
  CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );

  return StatusCode::SUCCESS;
}




