/*
  General-purpose view creation algorithm <bwynne@cern.ch>
  
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./TestEventViewCreatorAlgorithm.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"
#include "AthViews/ViewHelper.h"
#include <DecisionHandling/TrigCompositeUtils.h>
#include "DecisionHandling/HLTIdentifier.h"


TestEventViewCreatorAlgorithm::TestEventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ) {
}

StatusCode TestEventViewCreatorAlgorithm::initialize()
{

  CHECK( m_inputDecisionsKey.initialize() );
  renounceArray(m_inputDecisionsKey);
  ATH_MSG_DEBUG("Will consume implicit input data:" );
  for (auto& input: m_inputDecisionsKey){  
    ATH_MSG_DEBUG(" "<<input.key());
  }

  ATH_MSG_DEBUG(" and produce Decisions=" << m_outputDecisionsKey <<" views="<<m_viewsKey<<" roIs="<<m_inViewRoIs );
  CHECK( m_outputDecisionsKey.initialize() );
  CHECK( m_viewsKey.initialize() );
  CHECK( m_inViewRoIs.initialize() );
  CHECK( m_scheduler.retrieve() );


  return StatusCode::SUCCESS;
}

StatusCode TestEventViewCreatorAlgorithm::execute()
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif


  ATH_MSG_DEBUG( "number of implicit ReadHandles is " << m_inputDecisionsKey.size() );

     
    // make the views
  auto viewVector = std::make_unique<std::vector<SG::View*>>();
  auto contexts = std::vector<EventContext>( );

  // prepare output decisions
  auto OutputDecisions = std::make_unique<TrigCompositeUtils::DecisionContainer>();
  auto aux = std::make_unique<TrigCompositeUtils::DecisionAuxContainer>();
  OutputDecisions->setStore( aux.get() );

  unsigned int viewCounter = 0;
  unsigned int conditionsRun = getContext().getExtension<Atlas::ExtendedEventContext>()->conditionsRun();

  const TrigRoiDescriptor* previousRoI = 0;

 // Loop over all input containers, which are of course TrigComposites, and request their features   
  for ( auto inputKey: m_inputDecisionsKey ) {
    auto inputDecisions = SG::makeHandle( inputKey );
    ATH_MSG_DEBUG( "Got DecisionContainer from input "<< inputKey.key()<<" with " << inputDecisions->size() << " elements" );
    ATH_MSG_DEBUG( "Preparing " << inputDecisions.cptr()->size() << " views (one per input decision)" );

    size_t counter = 0;
    for ( auto Idecision: *inputDecisions ) {
   
      //make one TC decision output per input and connect to previous
      auto newd = TrigCompositeUtils::newDecisionIn(OutputDecisions.get());
      // link to previous decision object
      newd->setObjectLink( "previousDecisions", ElementLink<TrigCompositeUtils::DecisionContainer>(inputKey.key(), counter) );

      {
	//copy decisions ID
	TrigCompositeUtils::DecisionIDContainer objDecisions;      
	TrigCompositeUtils::decisionIDs( Idecision, objDecisions );
	for ( const HLT::Identifier& id: objDecisions ) TrigCompositeUtils::addDecisionID( id, newd );
      }	
      
      ATH_MSG_DEBUG( "Positive decisions on RoI, preparing view" );
      // pull RoI descriptor
      CHECK( Idecision->hasObjectLink( m_roisLink ) );    
      auto roiDescriptorEL = Idecision->objectLink<TrigRoiDescriptorCollection>( m_roisLink );
      CHECK( roiDescriptorEL.isValid() );
      
      auto roiDescriptor = *roiDescriptorEL;
      ATH_MSG_DEBUG( "Placing TrigRoiDescriptor " << *roiDescriptor );

      
      if ( previousRoI == roiDescriptor ) continue; 
      // TODO here code supporting the case wnen we have many decisions associated to a single RoI
      previousRoI = roiDescriptor;
      
      auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >();    
      oneRoIColl->clear( SG::VIEW_ELEMENTS ); //Don't delete the RoIs
      oneRoIColl->push_back( roiDescriptor );
      
      newd->setObjectLink( "initialRoI", roiDescriptorEL ); //FPP add this roi to the output
      
      // make the view
      ATH_MSG_DEBUG( "Making the View" );
      auto newview = ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough ); //pointer to the view
      viewVector->push_back( newview );
      contexts.emplace_back( ctx );
      contexts.back().setExtension( Atlas::ExtendedEventContext( viewVector->back(), conditionsRun ) );
      
      // link decision to this view
      newd->setObjectLink( "view", ElementLink<std::vector<SG::View*> >(m_viewsKey.key(), viewVector->size()-1 ));//adding view to TC
      
      // see if there is a view linked to the decision object, if so link it to the view that is just made
      if ( Idecision->hasObjectLink( "view" ) ) {
	auto viewEL = Idecision->objectLink< std::vector<SG::View*> >( "view" );
	CHECK( viewEL.isValid() );
	auto parentView = *viewEL;
	viewVector->back()->linkParent( parentView );
	ATH_MSG_DEBUG( "Parent view linked" );
      }

      //store the RoI in the view
      auto handle = SG::makeHandle( m_inViewRoIs, contexts.back() );
      CHECK( handle.setProxyDict( viewVector->back() ) );
      CHECK( handle.record( std::move( oneRoIColl ) ) );
      
      counter++;
      
    }
  }

  
  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  CHECK( ViewHelper::ScheduleViews( *viewVector,           // Vector containing views
         m_viewNodeName,                                   // CF node to attach views to
         ctx,                                              // Source context
         m_scheduler.get() ) );

  
  // store views

  auto viewsHandle = SG::makeHandle( m_viewsKey );
  CHECK( viewsHandle.record(  std::move( viewVector ) ) );

  //store decisions with all
  auto outputHandle = SG::makeHandle(m_outputDecisionsKey, ctx);
  CHECK( outputHandle.record(std::move(OutputDecisions), std::move(aux) ) );
  ATH_MSG_DEBUG( "Store "<< viewsHandle->size() <<" Views and "<<outputHandle->size()<<" output decisions" ); 

  return StatusCode::SUCCESS;
}

