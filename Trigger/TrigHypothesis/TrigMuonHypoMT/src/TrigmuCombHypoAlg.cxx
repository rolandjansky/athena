/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthLinks/ElementLink.h" 

#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h" 

#include "TrigmuCombHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigmuCombHypoAlg::TrigmuCombHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator )
{} 

TrigmuCombHypoAlg::~TrigmuCombHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK(m_hypoTools.retrieve());

  renounce(m_muCombKey);
  ATH_CHECK(m_muCombKey.initialize());

  ATH_MSG_INFO( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoAlg::finalize() 
{   
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  ATH_MSG_INFO( "Finalization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoAlg::execute(const EventContext& context) const
{
  // common for all Hypos, to move in the base class
  ATH_MSG_DEBUG("StatusCode TrigmuCombHypoAlg::execute start");
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context);
  if ( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit ReadHandles for previous decisions " << decisionInput().key() << ": is this expected?");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Running with " << previousDecisionsHandle->size() << " implicit ReadHandles for previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // end of common

  std::vector<TrigmuCombHypoTool::CombinedMuonInfo> toolInput;
  // loop over previous decisions
  size_t counter = 0; 

  for ( auto previousDecision: *previousDecisionsHandle )  {
    // get L2MuonSA Feature
    TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> linkInfo = 
       TrigCompositeUtils::findLink<xAOD::L2StandAloneMuonContainer>(previousDecision, "feature");
    ElementLink<xAOD::L2StandAloneMuonContainer> muFastLink = linkInfo.link;
    ATH_CHECK( muFastLink.isValid() );        
    const xAOD::L2StandAloneMuon* muFast = *muFastLink;

    // get View
    ATH_CHECK( previousDecision->hasObjectLink("view") );
    auto viewEL = previousDecision->objectLink<ViewContainer>("view");
    ATH_CHECK( viewEL.isValid() );

    // get info
    auto muCombHandle = ViewHelper::makeHandle( *viewEL, m_muCombKey, context );
    ATH_CHECK( muCombHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << muCombHandle->size() << "...");

    auto muCombEL = ViewHelper::makeLink( *viewEL, muCombHandle, 0 );
    ATH_CHECK( muCombEL.isValid() );
    const xAOD::L2CombinedMuon* muComb = *muCombEL;
    
    // create new decisions
    auto newd = newDecisionIn( decisions );

    toolInput.emplace_back( TrigmuCombHypoTool::CombinedMuonInfo{ newd, muComb, muFast, previousDecision} );

    // set objectLink
    newd->setObjectLink( "feature", muCombEL );
    TrigCompositeUtils::linkToPrevious( newd, decisionInput().key(), counter);

    // DEBUG
    auto muFastInfo = (*muCombEL)->muSATrack(); 
    ATH_MSG_DEBUG("REGTEST: muSATrack pt in " << m_muCombKey.key() << " = " << muFastInfo->pt() << " GeV");
    ATH_MSG_DEBUG("REGTEST: muSATrack eta/phi in " << m_muCombKey.key() << " = " << muFastInfo->eta() << "/" << muFastInfo->phi());
    ATH_MSG_DEBUG("REGTEST: muCBTrack pt in " << m_muCombKey.key() << " = " << (*muCombEL)->pt() << " GeV");
    ATH_MSG_DEBUG("REGTEST: muCBTrack eta/phi in " << m_muCombKey.key() << " = " << (*muCombEL)->eta() << "/" << (*muCombEL)->phi());
    ATH_MSG_DEBUG("Added view, features, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );

    counter++;
  }
  for ( auto & tool: m_hypoTools ) {
    ATH_MSG_DEBUG("Go to " << tool);
    ATH_CHECK( tool->decide( toolInput ) );
  }
  {// debug printout
    ATH_MSG_DEBUG( "Exit with " << outputHandle->size() << " decisions");
    TrigCompositeUtils::DecisionIDContainer allPassingIDs;
    if ( outputHandle.isValid() ) {
      for ( auto decisionObject: *outputHandle ) {
        TrigCompositeUtils::decisionIDs ( decisionObject, allPassingIDs );
      }
      for ( TrigCompositeUtils::DecisionID id: allPassingIDs ) {
        ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
      }
    }
  }

  ATH_MSG_DEBUG("StatusCode TrigmuCombHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

