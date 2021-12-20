/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo algorithm for IDCalib stream trigger
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> 
*/
#include "IDCalibHypoAlg.h"
#include "AthViews/ViewHelper.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoringKernel/Monitored.h"

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

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

IDCalibHypoAlg::IDCalibHypoAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
   ::HypoBase( name, pSvcLocator ) {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode IDCalibHypoAlg::initialize()
{
   CHECK( m_hypoTools.retrieve() );

   CHECK( m_tracksKey.initialize() );

   if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

   ATH_MSG_INFO( "initialization completed" );

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode IDCalibHypoAlg::execute( const EventContext& context ) const
{
   // previous decisions
   ATH_MSG_DEBUG( "Retrieving previous decision: \"" << decisionInput().key() << "\"" );
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
   ATH_CHECK( previousDecisionsHandle.isValid() );

   ATH_MSG_DEBUG( "Running with " << previousDecisionsHandle->size() << " previous decisions" ); 
   if( previousDecisionsHandle->size()!=1 ) {
      ATH_MSG_ERROR( "Previous decision handle size is not 1. It is" << previousDecisionsHandle->size() );
      return StatusCode::FAILURE;
   }
   const Decision * previousDecision = previousDecisionsHandle->at(0);
   
   TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
   TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
   ATH_MSG_DEBUG( "IDs of active legs:" );
   for(auto decisionID: previousDecisionIDs) { ATH_MSG_DEBUG( "    " << decisionID ); }

   // new output decisions
   ATH_MSG_DEBUG( "Creating new output decision handle" ); 
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
   auto outputDecisions = outputHandle.ptr();

   // get tracks from the key
   ATH_MSG_DEBUG( "Getting Track Handle " << m_tracksKey );
   auto trackHandle = SG::makeHandle(m_tracksKey, context );
   ATH_CHECK( trackHandle.isValid() );
   ATH_MSG_DEBUG ( "Track handle size: " << trackHandle->size() << "..." );
   const xAOD::TrackParticleContainer * trackContainer = trackHandle.get();

   // monitoring
   std::vector<float> mnt_pt;
   auto mon_pt = Monitored::Collection("pt",  mnt_pt);
   auto monIDCalib = Monitored::Group(m_monTool, mon_pt);

   // track loop
   unsigned int itrk_passed = 0;
   std::vector<IDCalibHypoTool::IDCalibHypoInfo> IDCalibHypoInputs;
   for ( const xAOD::TrackParticle* track : *trackContainer ) {
      
      if (track->pt()< 1*Gaudi::Units::GeV ) continue;
      itrk_passed++;

      mnt_pt.push_back(track->pt()/Gaudi::Units::GeV);

      Decision* newDecision = TrigCompositeUtils::newDecisionIn( outputDecisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);

      ElementLink<xAOD::TrackParticleContainer> trackEL = ElementLink<xAOD::TrackParticleContainer>(*trackContainer, track->index());
      ATH_CHECK( trackEL.isValid() );

      ATH_CHECK( newDecision->setObjectLink<xAOD::TrackParticleContainer>(TrigCompositeUtils::featureString(), trackEL) );

      IDCalibHypoTool::IDCalibHypoInfo hypoInfo{ newDecision, track, trackContainer, previousDecisionIDs };
      IDCalibHypoInputs.push_back( hypoInfo );
   }
   ATH_MSG_DEBUG( "nr tracks = " << trackContainer->size() << " -> selected nr = " << itrk_passed );

   // Loop over all hypoToolinputs and get their decisions
   for ( auto & tool: m_hypoTools ) {
      ATH_MSG_DEBUG( "+++++ Now computing decision for " << tool->name() );
      ATH_CHECK( tool->decide( IDCalibHypoInputs ) );    
   } 
   
   DecisionContainer::iterator it = outputDecisions->begin();
   while(it != outputDecisions->end()) {
      if ( allFailed( *it ) ) {
	 ATH_MSG_DEBUG( "all failed, erasing for outputDecision: " << *it );
	 it = outputDecisions->erase(it);
      } else {
	 ATH_MSG_DEBUG( "keeping outputDecision: " << *it );
	 ++it;
      }
   }

   //
   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

   //
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

