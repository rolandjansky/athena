/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering  large dEdx tracks
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> 
*/
#include "TrigDisappearingTrackTriggerHypoAlg.h"
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

TrigDisappearingTrackTriggerHypoAlg::TrigDisappearingTrackTriggerHypoAlg( const std::string& name, 
									  ISvcLocator* pSvcLocator ) : 
   ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigDisappearingTrackTriggerHypoAlg::initialize()
{
   CHECK( m_hypoTools.retrieve() );
   CHECK( m_DisTrkCandKey.initialize() );
   CHECK( m_DisTrkBDTSelKey.initialize());
   return StatusCode::SUCCESS;
}

StatusCode TrigDisappearingTrackTriggerHypoAlg::execute( const EventContext& context ) const
{
   ATH_MSG_DEBUG( name() << "..." );
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
   ATH_CHECK( previousDecisionsHandle.isValid() );
   
   // new output decisions
   ATH_MSG_DEBUG( "Creating new output decision handle" ); 
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
   auto decisions = outputHandle.ptr();

   // previous decisions
   ATH_MSG_DEBUG( "Running with " << previousDecisionsHandle->size() << " previous decisions" ); 
   if(previousDecisionsHandle->size()!=1) {
      ATH_MSG_ERROR( "Previous decision handle size isn't 1. It is" << previousDecisionsHandle->size() );
      return StatusCode::FAILURE;
   }
   const Decision * previousDecision = previousDecisionsHandle->at(0);

   TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
   TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
   
   // input Disappearing objects
   auto DisTrkCandHandle = SG::makeHandle(m_DisTrkCandKey, context );
   ATH_CHECK( DisTrkCandHandle.isValid() );
   ATH_MSG_DEBUG( "DisTrkCand handle size: " << DisTrkCandHandle->size() );

   const xAOD::TrigCompositeContainer * DisTrkCandContainer = DisTrkCandHandle.get();

   if( DisTrkCandContainer == nullptr ) {
      ATH_MSG_ERROR( "ERROR Cannot get DisappearingTrks container" );
      return StatusCode::FAILURE;
   }

   // output EDM object
   auto DisTrkBDTSelContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto DisTrkBDTSelContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   DisTrkBDTSelContainer->setStore(DisTrkBDTSelContainerAux.get());

   // create decision
   auto d = newDecisionIn( decisions , previousDecision, hypoAlgNodeName(), context );
   TrigDisappearingTrackTriggerHypoTool::TrackInfo trkInfo{ d, DisTrkCandContainer, *DisTrkBDTSelContainer, previousDecisionIDs };

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
      SG::WriteHandle<xAOD::TrigCompositeContainer> DisTrkBDTSelHandle(m_DisTrkBDTSelKey, context);
      ATH_CHECK( DisTrkBDTSelHandle.record( std::move( DisTrkBDTSelContainer ), std::move( DisTrkBDTSelContainerAux ) ) );
      ATH_CHECK( d->setObjectLink( featureString(), ElementLink<xAOD::TrigCompositeContainer>( m_DisTrkBDTSelKey.key(), 0) ) );
   }

   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

   //
   return StatusCode::SUCCESS;
}

