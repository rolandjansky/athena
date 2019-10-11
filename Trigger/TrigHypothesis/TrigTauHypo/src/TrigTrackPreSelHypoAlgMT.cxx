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
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new decisions

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigTrackPreSelHypoTool::TrackingInfo> toolInput;

  // loop over previous decisions
  int counter=-1;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    counter++;
    
    //get RoI
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString());
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );
    auto tracksHandle = ViewHelper::makeHandle( *viewEL, m_fastTracksKey, context);
    ATH_CHECK( tracksHandle.isValid() );
    ATH_MSG_DEBUG ( "tracks handle size: " << tracksHandle->size() << "..." );

    if( tracksHandle->size() == 0 ) {
      ATH_MSG_DEBUG("No tracks were found, skipping this view");
      continue;
    }

    // create new decision
    auto d = newDecisionIn( decisions, name() );
    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );

    auto el = ViewHelper::makeLink( *viewEL, tracksHandle, 0 );
    ATH_CHECK( el.isValid() );
    d->setObjectLink( featureString(),  el );
    
    d->setObjectLink( roiString(), roiELInfo.link );
    toolInput.emplace_back( d, roi, tracksHandle.cptr(), previousDecision );

    ATH_MSG_DEBUG( "Added view, roi, tracks, previous decision to new decision " << counter << " for view " << (*viewEL)->name()  );
  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  
  return StatusCode::SUCCESS;
}
