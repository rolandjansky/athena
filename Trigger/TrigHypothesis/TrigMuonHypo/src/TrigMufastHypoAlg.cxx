/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "AthLinks/ElementLink.h"
#include "TrigMufastHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMufastHypoAlg::TrigMufastHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
   ::HypoBase( name, pSvcLocator )
{}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::initialize()
{
  ATH_CHECK(m_hypoTools.retrieve());

  renounce(m_muFastKey);
  ATH_CHECK(m_muFastKey.initialize());

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::execute( const EventContext& context ) const
{

  // common for all Hypos, to move in the base class
  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute start");
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // end of common
 

  std::vector<TrigMufastHypoTool::MuonClusterInfo> toolInput;
  // loop over previous decisions
  size_t counter=0;
  for ( const auto previousDecision: *previousDecisionsHandle ) {
    //get RoI
    auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    auto roiEL = roiInfo.link;
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink< ViewContainer >( viewString() );
    ATH_CHECK( viewEL.isValid() );

    //// get info of that view (MuonL2SAInfo)
    auto muFastHandle = ViewHelper::makeHandle( *viewEL, m_muFastKey, context );
    ATH_CHECK( muFastHandle.isValid() );
    ATH_MSG_DEBUG ( "Muinfo handle size: " << muFastHandle->size() << "..." );

    //loop over muFast (more than one muon can be found by L2 multimu-in-pad algo)
    for(uint i=0; i < muFastHandle->size(); i++){
      auto muonEL = ViewHelper::makeLink( *viewEL, muFastHandle, i );
      ATH_CHECK( muonEL.isValid() );
      const xAOD::L2StandAloneMuon* muon = *muonEL;

      // create new decision
      auto newd = newDecisionIn( decisions, hypoAlgNodeName() );

      // push_back to toolInput
      toolInput.emplace_back( newd, roi, muon, previousDecision );

      newd->setObjectLink( featureString(), muonEL );
      TrigCompositeUtils::linkToPrevious( newd, previousDecision, context );

      ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " pT = " << (*muonEL)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " eta/phi = " << (*muonEL)->eta() << "/" << (*muonEL)->phi());
      ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
      ATH_MSG_DEBUG("Added view, roi, feature, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );
      counter++;
    }
  }

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");


  // to TrigMufastHypoTool
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

  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

