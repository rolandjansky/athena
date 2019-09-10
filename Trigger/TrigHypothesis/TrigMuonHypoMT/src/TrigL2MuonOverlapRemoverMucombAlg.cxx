/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthLinks/ElementLink.h" 

#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h" 
#include "xAODTrigger/TrigCompositeContainer.h"

#include "TrigL2MuonOverlapRemoverMucombAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonOverlapRemoverMucombAlg::TrigL2MuonOverlapRemoverMucombAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
   ::HypoBase( name, pSvcLocator )
{} 

TrigL2MuonOverlapRemoverMucombAlg::~TrigL2MuonOverlapRemoverMucombAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverMucombAlg::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() << "..." );
  ATH_CHECK(m_OverlapRemoverTools.retrieve());

  renounce(m_OverlapRemoverKey);
  ATH_CHECK(m_OverlapRemoverKey.initialize());

  ATH_MSG_DEBUG( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverMucombAlg::execute(const EventContext& context) const
{
   ATH_MSG_DEBUG("StatusCode TrigL2MuonOverlapRemoverMucomb::execute start"); 
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context);
   ATH_MSG_DEBUG("Runnung with " << previousDecisionsHandle->size() << " implicit ReadHandles for previous decision");

   // new output decisions
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
   auto decisions = outputHandle.ptr();
   // end of common   


   std::vector<TrigL2MuonOverlapRemoverTool::L2CBMuonOverlapInfo> toolInput;
   // loop over previous decisions
   size_t counter = 0; 
   
   for ( const auto previousDecision: *previousDecisionsHandle ) {
     //get RecRoI
     auto RecRoIInfo = TrigCompositeUtils::findLink< DataVector< LVL1::RecMuonRoI > >( previousDecision, initialRecRoIString() );
     auto RecRoIEL = RecRoIInfo.link;
     ATH_CHECK( RecRoIEL.isValid() );
     const LVL1::RecMuonRoI* RecRoI = *RecRoIEL;

     // get View
     ATH_CHECK( previousDecision->hasObjectLink( viewString()) );
     auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
     ATH_CHECK( viewEL.isValid() );
     
     // get info
     auto L2MuonOverlapRemoverHandle = ViewHelper::makeHandle( *viewEL, m_OverlapRemoverKey, context );
     ATH_CHECK( L2MuonOverlapRemoverHandle.isValid() );
     ATH_MSG_DEBUG( "Muinfo handle size: " << L2MuonOverlapRemoverHandle->size() << "...");
     
     auto overlapEL = ViewHelper::makeLink( *viewEL, L2MuonOverlapRemoverHandle, 0 );
     ATH_CHECK( overlapEL.isValid() );
     const xAOD::L2CombinedMuon* overlap = *overlapEL;

     // create new decisions
     auto newd = newDecisionIn( decisions );
     
     if ( decisionIDs( previousDecision ).size() != 0 ) { 
        toolInput.emplace_back( TrigL2MuonOverlapRemoverTool::L2CBMuonOverlapInfo{ newd, RecRoI, overlap, previousDecision} );
     }     

     // set objectLink
     newd->setObjectLink( featureString(), overlapEL );
     TrigCompositeUtils::linkToPrevious( newd, previousDecision, context);
     
     // DEBUG
     auto muFastInfo = (*overlapEL)->muSATrack(); 
     ATH_MSG_DEBUG("REGTEST: muSATrack pt in " << m_OverlapRemoverKey.key() << " = " << muFastInfo->pt() << " GeV");
     ATH_MSG_DEBUG("REGTEST: muSATrack eta/phi in " << m_OverlapRemoverKey.key() << " = " << muFastInfo->eta() << "/" << muFastInfo->phi());
     ATH_MSG_DEBUG("REGTEST: L2MuonCBTrack pt in " << m_OverlapRemoverKey.key() << " = " << (*overlapEL)->pt() << " GeV");
     ATH_MSG_DEBUG("REGTEST: L2MuonCBTrack eta/phi in " << m_OverlapRemoverKey.key() << " = " << (*overlapEL)->eta() << "/" << (*overlapEL)->phi());
     ATH_MSG_DEBUG("Added view, features, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );
     ATH_MSG_DEBUG("Number of positive previous hypo Decision::" << decisionIDs( previousDecision ).size() );
     
     counter++;
   }

   ATH_MSG_DEBUG("Found " << toolInput.size() << " input to tools");

   // to TrigL2MuonOverlapRemoverTool
   StatusCode sc = StatusCode::SUCCESS;
   for ( auto& tool: m_OverlapRemoverTools ) {
     ATH_MSG_DEBUG("Go to " << tool );
     sc = tool->decide(toolInput);
     if (!sc.isSuccess()) {
       ATH_MSG_ERROR("MuonHypoTool is failed");
       return StatusCode::FAILURE;
     }
   } // End of tool algorithms */

   ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

   ATH_MSG_DEBUG("StatusCode TrigL2MuonOverlapRemoverMucombAlg::execute success");
   return StatusCode::SUCCESS;

}
