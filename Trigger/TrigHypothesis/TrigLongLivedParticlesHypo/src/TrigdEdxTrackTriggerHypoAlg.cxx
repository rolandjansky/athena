/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering  large dEdx tracks
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
*/
#include "TrigdEdxTrackTriggerHypoAlg.h"
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

TrigdEdxTrackTriggerHypoAlg::TrigdEdxTrackTriggerHypoAlg( const std::string& name, 
							  ISvcLocator* pSvcLocator ) : 
   ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigdEdxTrackTriggerHypoAlg::initialize()
{
   CHECK( m_hypoTools.retrieve() );
   CHECK( m_dEdxTrkKey.initialize() );
   CHECK( m_dEdxHitKey.initialize() );
   CHECK( m_HPtdEdxTrkKey.initialize());
   return StatusCode::SUCCESS;
}

StatusCode TrigdEdxTrackTriggerHypoAlg::execute( const EventContext& context ) const
{
   const std::string header = "UTT: ";

   ATH_MSG_DEBUG( header << name() << "..." );
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
   ATH_CHECK( previousDecisionsHandle.isValid() );
   
   // new output decisions
   ATH_MSG_DEBUG( header << "Creating new output decision handle" ); 
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
   auto decisions = outputHandle.ptr();

   // previous decisions
   ATH_MSG_DEBUG( header << "Running with " << previousDecisionsHandle->size() << " previous decisions" ); 
   if(previousDecisionsHandle->size()!=1) {
      ATH_MSG_ERROR( header << "Previous decision handle size isn't 1. It is" << previousDecisionsHandle->size() );
      return StatusCode::FAILURE;
   }
   const Decision * previousDecision = previousDecisionsHandle->at(0);

   TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
   TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
   
   // input dEdx objects
   auto dEdxTrkHandle = SG::makeHandle(m_dEdxTrkKey, context );
   ATH_CHECK( dEdxTrkHandle.isValid() );
   ATH_MSG_DEBUG( header << "dEdxTrk handle size: " << dEdxTrkHandle->size() );

   auto dEdxHitHandle = SG::makeHandle(m_dEdxHitKey, context );
   ATH_CHECK( dEdxHitHandle.isValid() );
   ATH_MSG_DEBUG( header << "dEdxHit handle size: " << dEdxHitHandle->size() );

   const xAOD::TrigCompositeContainer * dEdxTrksContainer = dEdxTrkHandle.get();
   const xAOD::TrigCompositeContainer * dEdxHitsContainer = dEdxHitHandle.get();

   if( dEdxTrksContainer == nullptr ) {
      ATH_MSG_ERROR( header << "ERROR Cannot get dEdxTrks container");
      return StatusCode::FAILURE;
   }
   if( dEdxHitsContainer == nullptr ) {
      ATH_MSG_ERROR( header << "ERROR Cannot get dEdxHits container");
      return StatusCode::FAILURE;
   }

   // output EDM object
   auto HPtdEdxTrkContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto HPtdEdxTrkContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   HPtdEdxTrkContainer->setStore(HPtdEdxTrkContainerAux.get());

   // create decision
   auto d = newDecisionIn( decisions , previousDecision, hypoAlgNodeName(), context );
   TrigdEdxTrackTriggerHypoTool::TrackInfo trkInfo{ d, dEdxTrksContainer, dEdxHitsContainer, *HPtdEdxTrkContainer, previousDecisionIDs };

   // Loop over all hypoToolinputs and get their decisions
   for ( auto & tool: m_hypoTools ) {
      ATH_CHECK( tool->decide( trkInfo ) );    
   } 
   
   DecisionContainer::iterator it = decisions->begin();
   bool isPassed = false;
   while(it != decisions->end()) {
      if (allFailed( *it )) {
	 it = decisions->erase(it);
      } else {
	 isPassed = true;
	 ++it;
      }
   }

   // record link to EDM object if it is passed
   if( isPassed ) {
      SG::WriteHandle<xAOD::TrigCompositeContainer> HPtdEdxTrkHandle(m_HPtdEdxTrkKey, context);
      ATH_CHECK( HPtdEdxTrkHandle.record( std::move( HPtdEdxTrkContainer ), std::move( HPtdEdxTrkContainerAux ) ) );
      ATH_CHECK( d->setObjectLink( featureString(), ElementLink<xAOD::TrigCompositeContainer>( m_HPtdEdxTrkKey.key(), 0) ) );
   }

   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

   //
   return StatusCode::SUCCESS;
}
