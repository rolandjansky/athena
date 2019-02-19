/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///
/// General-purpose view creation algorithm <bwynne@cern.ch>
///

#include "EventViewCreatorAlgorithmWithJets.h"
#include "AthLinks/ElementLink.h"
#include "AthViews/ViewHelper.h"
#include "AthViews/View.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"

using namespace TrigCompositeUtils;

EventViewCreatorAlgorithmWithJets::EventViewCreatorAlgorithmWithJets( const std::string& name, ISvcLocator* pSvcLocator )
  : EventViewCreatorAlgorithm( name, pSvcLocator ) {}

EventViewCreatorAlgorithmWithJets::~EventViewCreatorAlgorithmWithJets() {}

StatusCode EventViewCreatorAlgorithmWithJets::initialize() {
  EventViewCreatorAlgorithm::initialize();

  ATH_CHECK( m_inViewJets.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithmWithJets::execute( const EventContext& context ) const {
  // create the output decisions, similar to inputs (copy basic links)
  std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > outputHandles;
  ATH_CHECK (decisionInputToOutput(context, outputHandles));

  // make the views
  auto viewsHandle = SG::makeHandle( m_viewsKey ); 
  auto viewVector1 = std::make_unique< ViewContainer >();
  ATH_CHECK( viewsHandle.record(  std::move( viewVector1 ) ) );
  auto viewVector = viewsHandle.ptr();

  //  auto viewVector = std::make_unique< ViewContainer >();
  auto contexts = std::vector<EventContext>( );
  unsigned int viewCounter = 0;
  unsigned int conditionsRun = context.getExtension<Atlas::ExtendedEventContext>().conditionsRun();

  //map all RoIs that are stored
  std::vector <ElementLink<TrigRoiDescriptorCollection> > RoIsFromDecision;


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
        // Retrieve jets ...
        ATH_MSG_DEBUG( "Checking there are jets linked to decision object" );
        TrigCompositeUtils::LinkInfo< xAOD::JetContainer > jetELInfo = TrigCompositeUtils::findLink< xAOD::JetContainer >( inputDecision,m_jetsLink );
        ATH_CHECK( jetELInfo.isValid() );
        const xAOD::Jet *jet = *jetELInfo.link;
        ATH_MSG_DEBUG( "Placing xAOD::JetContainer " );
        ATH_MSG_DEBUG( "   -- pt="<< jet->p4().Et() <<" eta="<< jet->eta() << " phi="<< jet->phi() );

        
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
          outputDecision->setObjectLink( "jets", jetELInfo.link );
          ATH_MSG_DEBUG( "Adding new view to new decision; storing view in viewVector component " << viewVector->size()-1 );
          ATH_CHECK( linkViewToParent( inputDecision, viewVector->back() ) );
          ATH_CHECK( placeRoIInView( roi, viewVector->back(), contexts.back() ) );
          ATH_CHECK( placeJetInView( jet, viewVector->back(), contexts.back() ) );
        }
        else {
          int iview = roiIt-RoIsFromDecision.begin();
          outputDecision->setObjectLink( "view", ElementLink< ViewContainer >(m_viewsKey.key(), iview ) ); //adding view to TC
          outputDecision->setObjectLink( "jets", jetELInfo.link );
          ATH_MSG_DEBUG( "Adding already mapped view " << iview << " in ViewVector , to new decision");
        }
      }// loop over previous inputs
    } // loop over decisions   
  }// loop over output keys

  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  ATH_CHECK( ViewHelper::ScheduleViews( viewVector,           // Vector containing views
             m_viewNodeName,             // CF node to attach views to
             context,                    // Source context
             m_scheduler.get() ) );
  
  // store views
  // auto viewsHandle = SG::makeHandle( m_viewsKey );
  // ATH_CHECK( viewsHandle.record(  std::move( viewVector ) ) );
  ATH_MSG_DEBUG( "Store "<< viewsHandle->size() <<" Views");

  ATH_CHECK( debugPrintOut(context, outputHandles) );
  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithmWithJets::placeJetInView( const xAOD::Jet* theObject, SG::View* view, const EventContext& context ) const {
  // fill the Jet output collection  
  ATH_MSG_DEBUG( "Adding Jet To View : " << m_inViewJets.key() );
  auto oneObjectCollection = std::make_unique< ConstDataVector< xAOD::JetContainer > >();
  oneObjectCollection->clear( SG::VIEW_ELEMENTS ); 
  oneObjectCollection->push_back( theObject );

  //store in the view 
  auto handle = SG::makeHandle( m_inViewJets,context );
  ATH_CHECK( handle.setProxyDict( view ) );
  ATH_CHECK( handle.record( std::move( oneObjectCollection ) ) ); 
  return StatusCode::SUCCESS;
}

