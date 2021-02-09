/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering high pt isolated tracks 
  * author Ismet Siral <ismet.siral@cern.ch> - University of Oregon
*/
#include "TrigIsoHPtTrackTriggerHypoAlgMT.h"
#include "AthViews/ViewHelper.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "GaudiKernel/SystemOfUnits.h"

using TrigCompositeUtils::createAndStore; 
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;

TrigIsoHPtTrackTriggerHypoAlgMT::TrigIsoHPtTrackTriggerHypoAlgMT( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigIsoHPtTrackTriggerHypoAlgMT::initialize() {
  CHECK( m_hypoTools.retrieve() );
  CHECK( m_trackKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TrigIsoHPtTrackTriggerHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );

  

  ATH_MSG_DEBUG( "Creating new output decision handle"); 
  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // prepare imput for tools
  std::vector<TrigIsoHPtTrackTriggerHypoTool::TrackInfo> hypoToolInput;

  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions"); 
  for ( auto previousDecision: *previousDecisionsHandle ) {

    ATH_MSG_DEBUG( "Getting Track Handle "<<m_trackKey);
    // get tracks from the key :
    auto trackHandle = SG::makeHandle(m_trackKey, context );

    ATH_CHECK( trackHandle.isValid() );
    ATH_MSG_DEBUG ( "track handle size: " << trackHandle->size() << "..." );
    if(trackHandle->size()==0) continue;

    const xAOD::TrackParticleContainer * AllTracks = trackHandle.get(); //Later this could be updated to run with tracks above a certain threshold 

    // Loop over all tracks and get a track decision associated with it and create the input 
    for ( const xAOD::TrackParticle_v1* track : *AllTracks) {

      //Although it's unconventional to have a track pT decision in the Algorithm part, this line should save a lot of CPU time. We require at least 50 GeV track pT to have decision on that track.
      if (track->pt()< 50*Gaudi::Units::GeV ) continue;

      auto d = newDecisionIn( decisions , previousDecision, hypoAlgNodeName(), context);
      d->setObjectLink( featureString(), ElementLink<xAOD::TrackParticleContainer>( *AllTracks, track->index() ) );
      
      hypoToolInput.emplace_back( TrigIsoHPtTrackTriggerHypoTool::TrackInfo{ d, track, AllTracks } );
    }
  }
  //Loop over all hypoToolinputs and get their decisions
  for ( auto & tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );    
  } 

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
