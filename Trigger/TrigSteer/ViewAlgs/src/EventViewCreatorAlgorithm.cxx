/*
  General-purpose view creation algorithm <bwynne@cern.ch>
  
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./EventViewCreatorAlgorithm.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"
#include "AthViews/ViewHelper.h"


EventViewCreatorAlgorithm::EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ) {
}

StatusCode EventViewCreatorAlgorithm::initialize()
{
  CHECK( m_inputDecisionsKey.initialize() );
  CHECK( m_viewsKey.initialize() );

  renounce( m_inViewRoIs );
  CHECK( m_inViewRoIs.initialize() );
  renounce( m_inViewDecisions );
  CHECK( m_inViewDecisions.initialize() );

  //  CHECK( m_viewAlgorithmsPool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithm::execute()
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  auto inputDecisions = SG::makeHandle( m_inputDecisionsKey );

  auto viewVector = std::make_unique<std::vector<SG::View*>>();
  auto contexts = std::vector<EventContext>( );

  unsigned int viewCounter = 0;
  unsigned int conditionsRun = getContext().getExtension<Atlas::ExtendedEventContext>()->conditionsRun();

  const TrigRoiDescriptor* previousRoI = 0;

  ATH_MSG_DEBUG( "Preparing " << inputDecisions.cptr()->size() << " views" );


  for ( auto decision: *inputDecisions ) {
    if ( TrigCompositeUtils::allFailed( decision ) ) {
      ATH_MSG_DEBUG( "Skipping, as all decisions are negative" );
      continue;
    } 
    ATH_MSG_DEBUG( "Positive decisions on RoI, peparing view" );
    // pull RoI descriptor
    CHECK( decision->hasObjectLink( m_roisLink ) );    
    auto roiDescriptorEL = decision->objectLink<TrigRoiDescriptorCollection>( m_roisLink );
    CHECK( roiDescriptorEL.isValid() );

    auto roiDescriptor = *roiDescriptorEL;
    ATH_MSG_DEBUG( "Placing TrigRoiDescriptor " << *roiDescriptor );

    if ( previousRoI == roiDescriptor ) continue; 
    // TODO here code supporting the case wnen we have many decisions associated to a single RoI
    previousRoI = roiDescriptor;

    auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >();    
    oneRoIColl->clear( SG::VIEW_ELEMENTS ); //Don't delete the RoIs
    oneRoIColl->push_back( roiDescriptor );

    // make the view
    viewVector->push_back( ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough ) );
    contexts.emplace_back( ctx );
    contexts.back().setExtension( Atlas::ExtendedEventContext( viewVector->back(), conditionsRun ) );
    
    // see if there is a view linked to the decision object, if so link it to the view that is just made
    if ( decision->hasObjectLink( "view" ) ) {
      auto viewEL = decision->objectLink< std::vector<SG::View*> >( "view" );
      CHECK( viewEL.isValid() );
      auto parentView = *viewEL;
      viewVector->back()->linkParent( parentView );
      ATH_MSG_DEBUG( "Parent view linked" );
    }

    auto handle = SG::makeHandle( m_inViewRoIs, contexts.back() );
    CHECK( handle.setProxyDict( viewVector->back() ) );
    CHECK( handle.record( std::move( oneRoIColl ) ) );

    // TODO recording decisions related to the current view

  }
  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  // Run the views
  CHECK( ViewHelper::runInViews( contexts,				// Vector containing views
				 m_viewAlgorithmNames,			// Algorithms to run in each view
				 serviceLocator()->service( m_algPoolName ) ) );	// Service to retrieve algorithms by name
  
  // store views
  auto viewsHandle = SG::makeHandle( m_viewsKey );
  CHECK( viewsHandle.record(  std::move( viewVector ) ) );
    

  return StatusCode::SUCCESS;
}

