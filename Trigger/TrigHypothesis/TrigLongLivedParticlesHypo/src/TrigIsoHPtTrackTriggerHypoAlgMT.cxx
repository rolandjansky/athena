/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::allFailed;

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
  if(previousDecisionsHandle->size()!=1) {
    ATH_MSG_ERROR("Previous decision handle size isn't 1. It is"<<previousDecisionsHandle->size()<<". This means there is something seriously different the way track desicions are handled and this Isolated High Pt Track Trigger hypo might need re-work");
    return StatusCode::FAILURE;

  }
  const Decision * previousDecision = previousDecisionsHandle->at(0);

  TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);

  ATH_MSG_DEBUG( "Getting Track Handle "<<m_trackKey);
  // get tracks from the key :
  auto trackHandle = SG::makeHandle(m_trackKey, context );

  ATH_CHECK( trackHandle.isValid() );
  ATH_MSG_DEBUG ( "track handle size: " << trackHandle->size() << "..." );

  const xAOD::TrackParticleContainer * AllTracks = trackHandle.get(); //Later this could be updated to run with tracks above a certain threshold 

  // Loop over all tracks above 1GeV and get a track decision associated with it and create the input 
  for ( const xAOD::TrackParticle_v1* track : *AllTracks) {

    if (track->pt()< 1*Gaudi::Units::GeV ) continue;
    
    //The decision tool has been moved inside the Tool, to save time on CPU. (There are too many tracks to make a dicision object. Instead we only make a decision object for events with tracks. 

    auto d = newDecisionIn( decisions , previousDecision, hypoAlgNodeName(), context);
    d->setObjectLink( featureString(), ElementLink<xAOD::TrackParticleContainer>( *AllTracks, track->index() ) );
      
    hypoToolInput.emplace_back( TrigIsoHPtTrackTriggerHypoTool::TrackInfo{ d, track, AllTracks, previousDecisionIDs} );
  }

  //Loop over all hypoToolinputs and get their decisions
  for ( auto & tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );    
  } 
  



  DecisionContainer::iterator it = decisions->begin();
  while(it != decisions->end()) {
    if (allFailed( *it )) {
      it = decisions->erase(it);
    } else {
      ++it;
    }
  }

  




  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );


  
  
  return StatusCode::SUCCESS;
}
