/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigmuCombHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigmuCombHypoAlg::TrigmuCombHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator )
{} 


StatusCode TrigmuCombHypoAlg::initialize()
{
  ATH_CHECK(m_hypoTools.retrieve());

  renounce(m_muCombKey);
  ATH_CHECK(m_muCombKey.initialize());

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoAlg::execute(const EventContext& context) const
{
  // common for all Hypos, to move in the base class
  ATH_MSG_DEBUG("StatusCode TrigmuCombHypoAlg::execute start");
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context);
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // end of common

  std::vector<TrigmuCombHypoTool::CombinedMuonInfo> toolInput;
  // loop over previous decisions
  size_t counter = 0; 

  for ( const auto previousDecision: *previousDecisionsHandle )  {
    // get L2MuonSA Feature
    TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> linkInfo = 
       TrigCompositeUtils::findLink<xAOD::L2StandAloneMuonContainer>(previousDecision, featureString());
    ElementLink<xAOD::L2StandAloneMuonContainer> muFastLink = linkInfo.link;
    ATH_CHECK( muFastLink.isValid() );        
    const xAOD::L2StandAloneMuon* muFast = *muFastLink;

    // get View
    ATH_CHECK( previousDecision->hasObjectLink(viewString()) );
    auto viewEL = previousDecision->objectLink<ViewContainer>(viewString());
    ATH_CHECK( viewEL.isValid() );

    // get info
    auto muCombHandle = ViewHelper::makeHandle( *viewEL, m_muCombKey, context );
    ATH_CHECK( muCombHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << muCombHandle->size() << "...");

    // loop over muCombs (more than one muon can be found by L2 inside-out algos)
    for(uint i=0; i<muCombHandle->size(); i++){
      auto muCombEL = ViewHelper::makeLink( *viewEL, muCombHandle, i );
      ATH_CHECK( muCombEL.isValid() );
      const xAOD::L2CombinedMuon* muComb = *muCombEL;

      // create new decisions
      auto newd = newDecisionIn( decisions );

      toolInput.emplace_back( TrigmuCombHypoTool::CombinedMuonInfo{ newd, muComb, muFast, previousDecision} );

      // set objectLink
      newd->setObjectLink( featureString(), muCombEL );
      TrigCompositeUtils::linkToPrevious( newd, previousDecision, context);

      // DEBUG
      ATH_MSG_DEBUG("REGTEST: muCBTrack pt in " << m_muCombKey.key() << " = " << (*muCombEL)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: muCBTrack eta/phi in " << m_muCombKey.key() << " = " << (*muCombEL)->eta() << "/" << (*muCombEL)->phi());
      ATH_MSG_DEBUG("Added view, features, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );

      counter++;
    }
  }
  for ( auto & tool: m_hypoTools ) {
    ATH_MSG_DEBUG("Go to " << tool);
    ATH_CHECK( tool->decide( toolInput ) );
  }

  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  ATH_MSG_DEBUG("StatusCode TrigmuCombHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

