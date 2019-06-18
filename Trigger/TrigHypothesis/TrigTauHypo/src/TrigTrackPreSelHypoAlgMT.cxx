/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigTrackPreSelHypoAlgMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigTrackPreSelHypoAlgMT::TrigTrackPreSelHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigTrackPreSelHypoAlgMT::~TrigTrackPreSelHypoAlgMT() {}

StatusCode TrigTrackPreSelHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_fastTracksKey.initialize() );
  renounce( m_fastTracksKey );// tau candidates are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigTrackPreSelHypoAlgMT::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigTrackPreSelHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  // new decisions

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigTrackPreSelHypoTool::TrackingInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    //get RoI
    auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, "initialRoI");
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    auto viewELInfo = TrigCompositeUtils::findLink< ViewContainer >( previousDecision, "view" );
    ATH_CHECK( viewELInfo.isValid() );
    auto tracksHandle = ViewHelper::makeHandle( *(viewELInfo.link), m_fastTracksKey, context);
    ATH_CHECK( tracksHandle.isValid() );
    ATH_MSG_DEBUG ( "tracks handle size: " << tracksHandle->size() << "..." );

    // create new decision
    auto d = newDecisionIn( decisions, name() );

    toolInput.emplace_back( d, roi, tracksHandle.cptr(), previousDecision );

     {
       auto el = ViewHelper::makeLink( *(viewELInfo.link), tracksHandle, 0 );
      ATH_CHECK( el.isValid() );
      d->setObjectLink( "feature",  el );
    }
     d->setObjectLink( "roi", roiELInfo.link );

     TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
     ATH_MSG_DEBUG( "Added view, roi, tracks, previous decision to new decision " << counter << " for view " << (*viewELInfo.link)->name()  );
     counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  
  return StatusCode::SUCCESS;
}
