/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering  large dEdx tracks
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
*/
#include "TrigHitDVHypoAlg.h"
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

using xAOD::JetContainer;

TrigHitDVHypoAlg::TrigHitDVHypoAlg( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
   ::HypoBase( name, pSvcLocator ),
   m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool") {}

StatusCode TrigHitDVHypoAlg::initialize()
{
   CHECK( m_hypoTools.retrieve() );
   CHECK( m_jetsKey.initialize() );
   CHECK( m_hitDVTrkKey.initialize() );
   CHECK( m_hitDVSPKey.initialize() );
   CHECK( m_hitDVKey.initialize());
   CHECK( m_lumiDataKey.initialize(!m_isMC) );

   return StatusCode::SUCCESS;
}

StatusCode TrigHitDVHypoAlg::execute( const EventContext& context ) const
{
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
   
   // input objects

   // jets
   auto jetsHandle = SG::makeHandle(m_jetsKey, context );
   ATH_CHECK( jetsHandle.isValid() );
   ATH_MSG_DEBUG( "jet handle size: " << jetsHandle->size() );

   const xAOD::JetContainer* jetsContainer = jetsHandle.get();
   if( jetsContainer == nullptr ) {
      ATH_MSG_ERROR( "ERROR Cannot get jet container" );
      return StatusCode::FAILURE;
   }

   // hitDV objects
   auto hitDVTrkHandle = SG::makeHandle(m_hitDVTrkKey, context );
   ATH_CHECK( hitDVTrkHandle.isValid() );
   ATH_MSG_DEBUG( "hitDVTrk handle size: " << hitDVTrkHandle->size() );

   auto hitDVSPHandle = SG::makeHandle(m_hitDVSPKey, context );
   ATH_CHECK( hitDVSPHandle.isValid() );
   ATH_MSG_DEBUG( "hitDVSP handle size: " << hitDVSPHandle->size() );

   const xAOD::TrigCompositeContainer * hitDVTrksContainer  = hitDVTrkHandle.get();
   const xAOD::TrigCompositeContainer * hitDVSPsContainer   = hitDVSPHandle.get();

   if( hitDVTrksContainer == nullptr ) {
      ATH_MSG_ERROR( "ERROR Cannot get hitDVTrks container" );
      return StatusCode::FAILURE;
   }
   if( hitDVSPsContainer == nullptr ) {
      ATH_MSG_ERROR( "ERROR Cannot get hitDVSPs container" );
      return StatusCode::FAILURE;
   }

   // average mu
   float averageMu = 0;
   if( m_isMC ) {
      if( m_lumiBlockMuTool ) {
	 averageMu = (float)m_lumiBlockMuTool->averageInteractionsPerCrossing();
	 ATH_MSG_DEBUG( "offline averageMu = " << averageMu );
      }
   }
   else {
      SG::ReadCondHandle<LuminosityCondData> lcd (m_lumiDataKey,context);
      averageMu = lcd.cptr()->lbAverageInteractionsPerCrossing();
      ATH_MSG_DEBUG( "online averageMu = " << averageMu );
   }

   // output EDM object
   auto hitDVContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto hitDVContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   hitDVContainer->setStore(hitDVContainerAux.get());

   // create decision
   auto d = newDecisionIn( decisions , previousDecision, hypoAlgNodeName(), context );
   TrigHitDVHypoTool::TrackInfo trkInfo{ d, averageMu, jetsContainer, hitDVTrksContainer, hitDVSPsContainer, hitDVContainer.get(), previousDecisionIDs };

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
      // record hitDV object
      SG::WriteHandle<xAOD::TrigCompositeContainer> hitDVHandle(m_hitDVKey, context);
      ATH_CHECK( hitDVHandle.record( std::move( hitDVContainer ), std::move( hitDVContainerAux ) ) );
      ATH_CHECK( d->setObjectLink( featureString(), ElementLink<xAOD::TrigCompositeContainer>( m_hitDVKey.key(), 0) ) );
   }

   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

   //
   return StatusCode::SUCCESS;
}
