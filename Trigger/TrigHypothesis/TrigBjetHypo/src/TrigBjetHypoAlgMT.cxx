/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

StatusCode TrigBjetHypoAlgMT::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionHandle.isValid() );
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = prevDecisionHandle.get();
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");
  /*
  // Let's retrieve Jets, just to check we can access the 'Split Jets'
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_jetKey,context );
  CHECK( jetContainerHandle.isValid() );
  const xAOD::JetContainer *jetContainer = jetContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieving " << jetContainer->size() << " Jets from previous step" );
  for ( const xAOD::Jet *jet : *jetContainer )
    ATH_MSG_DEBUG("   ** pt=" << jet->p4().Et()<<" eta="<< jet->eta()<<" phi="<<jet->phi());
  */
  return StatusCode::SUCCESS;

  SG::ReadHandle< xAOD::BTaggingContainer > bTagHandle = SG::makeHandle( m_bTagKey, context );
  SG::ReadHandle< TrigRoiDescriptorCollection > roisHandle = SG::makeHandle( m_roisKey, context );
  
  std::unique_ptr< DecisionContainer > decisions = std::make_unique< DecisionContainer >();
  std::unique_ptr< DecisionAuxContainer > aux = std::make_unique< DecisionAuxContainer >();
  decisions->setStore( aux.get() );

  // prepare decision storage ( we could simplify it )
  // Lidija: should be checked once more
  size_t counter = 0;
  // ---->>>>>>
  xAOD::BTaggingContainer::const_iterator bTagIter = bTagHandle->begin();
  for ( ;  bTagIter != bTagHandle->end(); ++bTagIter, ++counter ) {
    TrigCompositeUtils::Decision *d = newDecisionIn( decisions.get() );
    d->setObjectLink( "feature", ElementLink<xAOD::BTaggingContainer>( m_bTagKey.key(), counter ) );
    d->setObjectLink( "roi", ElementLink<TrigRoiDescriptorCollection>( m_roisKey.key(), counter ) );
  }


  size_t index = 0;
  for ( ; index < decisions->size(); ++index ) {

    const xAOD::BTagging *bTag = bTagHandle->at( index );
    const TrigRoiDescriptor *roiDescriptor = roisHandle->at( index );
    TrigCompositeUtils::Decision *decision = decisions->at( index );


    for ( const ToolHandle< TrigBjetHypoTool >& tool : m_hypoTools ) {
      // interface of the tool needs to be suitable for current system, so no TrigComposite
      // also no support for the multi-electrons yet ( will be additional method )
      if ( tool->decide( bTag, roiDescriptor ) ) {   
	addDecisionID( tool->decisionId(), decision );	  
	ATH_MSG_DEBUG( " + " << tool->name() );
      } else {
	ATH_MSG_DEBUG( " - " << tool->name() );
      }
    }
    
  }

  {
    SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle =  SG::makeHandle( m_decisionsKey, context );
    CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  }

  return StatusCode::SUCCESS;
}




