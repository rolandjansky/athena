/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuisoHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuisoHypoAlg::TrigMuisoHypoAlg( const std::string& name,
			            ISvcLocator* pSvcLocator ) :
   ::HypoBase( name, pSvcLocator )
{}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuisoHypoAlg::initialize()
{
  ATH_CHECK(m_hypoTools.retrieve());

  renounce( m_muIsoKey );
  ATH_CHECK( m_muIsoKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TrigMuisoHypoAlg::execute( const EventContext& context) const
{
  // common for all Hypos, to move in the base class
  ATH_MSG_DEBUG("StatusCode TrigMuisoHypoAlg::execute start");
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // end of common
 
  std::vector<TrigMuisoHypoTool::MuisoInfo> toolInput; 
  size_t counter = 0; // view counter
  for ( const auto previousDecision: *previousDecisionsHandle ) {
    // get View
    auto viewEL = previousDecision->objectLink< ViewContainer >( viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get info of that view
    auto muIsoHandle = ViewHelper::makeHandle(*viewEL, m_muIsoKey, context);
    ATH_CHECK( muIsoHandle.isValid() );
    ATH_MSG_DEBUG ( "Muinfo handle size: " << muIsoHandle->size() << "..." );

    auto muonEL = ViewHelper::makeLink(*viewEL, muIsoHandle, 0);
    ATH_CHECK( muonEL.isValid() );
    const xAOD::L2IsoMuon* muon = *muonEL;

    // create new decision
    auto newd = newDecisionIn( decisions, hypoAlgNodeName() );

    // push_back to toolInput
    toolInput.emplace_back( newd, muon, previousDecision );
    
    newd->setObjectLink( featureString(), muonEL );
    TrigCompositeUtils::linkToPrevious( newd, previousDecision, context );
    
    ATH_MSG_DEBUG("REGTEST: " << m_muIsoKey.key() << " pT = " << (*muonEL)->pt() << " GeV");
    ATH_MSG_DEBUG("REGTEST: " << m_muIsoKey.key() << " eta/phi = " << (*muonEL)->eta() << "/" << (*muonEL)->phi());
    ATH_MSG_DEBUG("Added view, muIsoInfo, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );
    counter++;
  }

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");

  // to TrigMuisoHypoTool
  StatusCode sc = StatusCode::SUCCESS;
  for ( auto& tool: m_hypoTools ) {
    ATH_MSG_DEBUG("Go to " << tool );
    sc = tool->decide(toolInput);
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("MuonHypoTool is failed");
      return StatusCode::FAILURE;
    }
  } // End of tool algorithms */

  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  ATH_MSG_DEBUG("StatusCode TrigMuisoHypoAlg::execute success");
  return StatusCode::SUCCESS;
}


