/*
  General-purpose view creation algorithm <bwynne@cern.ch>
  
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EventViewCreatorAlgorithm.h"
#include "AthLinks/ElementLink.h"
#include "AthViews/ViewHelper.h"
#include "AthViews/View.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using namespace TrigCompositeUtils;

EventViewCreatorAlgorithm::EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : InputMakerBase( name, pSvcLocator ) {}

EventViewCreatorAlgorithm::~EventViewCreatorAlgorithm(){}

StatusCode EventViewCreatorAlgorithm::initialize() {
  ATH_MSG_DEBUG("Will produce views=" << m_viewsKey << " roIs=" << m_inViewRoIs );
  ATH_CHECK( m_viewsKey.initialize() );
  ATH_CHECK( m_inViewRoIs.initialize() );
  ATH_CHECK( m_scheduler.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode EventViewCreatorAlgorithm::execute( const EventContext& context ) const {

  // create the output decisions, similar to inputs (copy basic links)
  std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > outputHandles;
  ATH_CHECK (decisionInputToOutput(context, outputHandles));
 
  // make the views
  auto viewsHandle = SG::makeHandle( m_viewsKey ); 
  auto viewVector1 = std::make_unique< ViewContainer >();
  ATH_CHECK( viewsHandle.record(  std::move( viewVector1 ) ) );
  auto viewVector = viewsHandle.ptr();



    // auto viewVector = std::make_unique< ViewContainer >();
  auto contexts = std::vector<EventContext>( );
  unsigned int viewCounter = 0;
  unsigned int conditionsRun = context.getExtension<Atlas::ExtendedEventContext>().conditionsRun();

  //map all RoIs that are stored
  std::vector <ElementLink<TrigRoiDescriptorCollection> > RoIsFromDecision;

  // loop over decisions
  for (auto outputHandle: outputHandles) {
    if( not outputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from output "<< outputHandle.key() << " because handle not valid");
      continue;
    }

    if( outputHandle->size() == 0){ // input filtered out
      ATH_MSG_ERROR( "Got no decisions from output "<< outputHandle.key()<<": handle is valid but container is empty. Is this expected?");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Got output "<< outputHandle.key()<<" with " << outputHandle->size() << " elements" );
    // loop over output decisions in container of outputHandle, follow link to inputDecision
    for ( auto outputDecision : *outputHandle){ 
      ElementLinkVector<DecisionContainer> inputLinks = getLinkToPrevious(outputDecision);
      // loop over input links as predecessors
      for (auto input: inputLinks){
	const Decision* inputDecision = *input;
	// find the RoI
	auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( inputDecision, m_roisLink.value() );
	auto roiEL = roiELInfo.link;
	ATH_CHECK( roiEL.isValid() );
	// check if already found
	auto roiIt=find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL);
	if ( roiIt == RoIsFromDecision.end() ){
	  RoIsFromDecision.push_back(roiEL); // just to keep track of which we have used 
	  const TrigRoiDescriptor* roi = *roiEL;
	  ATH_MSG_DEBUG("Found RoI:" <<*roi<<" FS="<<roi->isFullscan());
	  ATH_MSG_DEBUG( "Positive decisions on RoI, preparing view" );
	  
	  // make the view
	  ATH_MSG_DEBUG( "Making the View" );
	  auto newView = ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough ); //pointer to the view
	  viewVector->push_back( newView );
	  contexts.emplace_back( context );
	  contexts.back().setExtension( Atlas::ExtendedEventContext( viewVector->back(), conditionsRun, roi ) );
	  
	  // link decision to this view
	  outputDecision->setObjectLink( "view", ElementLink< ViewContainer >(m_viewsKey.key(), viewVector->size()-1 ));//adding view to TC
	  ATH_MSG_DEBUG( "Adding new view to new decision; storing view in viewVector component " << viewVector->size()-1 );
	  ATH_CHECK( linkViewToParent( inputDecision, viewVector->back() ) );
	  ATH_CHECK( placeRoIInView( roi, viewVector->back(), contexts.back() ) );	
	}
	else {
	  int iview = roiIt-RoIsFromDecision.begin();
	  outputDecision->setObjectLink( "view", ElementLink< ViewContainer >(m_viewsKey.key(), iview ) ); //adding view to TC
	  ATH_MSG_DEBUG( "Adding already mapped view " << iview << " in ViewVector , to new decision");
	}
      }// loop over previous inputs
    } // loop over decisions   
  }// loop over output keys


  // launch view execution

  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  ATH_CHECK( ViewHelper::ScheduleViews( viewVector,           // Vector containing views
					m_viewNodeName,             // CF node to attach views to
					context,                    // Source context
					m_scheduler.get() ) );
  
  // report number of views, stored already when container was created
  // auto viewsHandle = SG::makeHandle( m_viewsKey );
  // ATH_CHECK( viewsHandle.record(  std::move( viewVector ) ) );
  ATH_MSG_DEBUG( "Store "<< viewsHandle->size() <<" Views");
  
  ATH_CHECK( debugPrintOut(context, outputHandles) );
  return StatusCode::SUCCESS;
}



StatusCode EventViewCreatorAlgorithm::linkViewToParent( const TrigCompositeUtils::Decision* inputDecision, SG::View* newView ) const {
  // see if there is a view linked to the decision object, if so link it to the view that is just made
  TrigCompositeUtils::LinkInfo<ViewContainer> parentViewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(inputDecision, "view" );
  if ( parentViewLinkInfo.isValid() ) {
    ATH_CHECK( parentViewLinkInfo.link.isValid() );
    auto parentView = *parentViewLinkInfo.link;
    newView->linkParent( parentView );
    ATH_MSG_DEBUG( "Parent view linked" );
  } else {
    if ( m_requireParentView ) {
      ATH_MSG_ERROR( "Parent view not linked because it could not be found" );
      ATH_MSG_ERROR( TrigCompositeUtils::dump( inputDecision, [](const xAOD::TrigComposite* tc){ 
								return "TC " + tc->name() + ( tc->hasObjectLink("view") ? " has view " : " has no view " );
							      } ) );
      return StatusCode::FAILURE;
    }
    
  }
  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithm::placeRoIInView( const TrigRoiDescriptor* roi, SG::View* view, const EventContext& context ) const {
  // fill the RoI output collection
  auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >();    
  oneRoIColl->clear( SG::VIEW_ELEMENTS ); //Don't delete the RoIs
  oneRoIColl->push_back( roi );
	
  //store the RoI in the view
  auto handle = SG::makeHandle( m_inViewRoIs, context );
  ATH_CHECK( handle.setProxyDict( view ) );
  ATH_CHECK( handle.record( std::move( oneRoIColl ) ) );
  return StatusCode::SUCCESS;
}
