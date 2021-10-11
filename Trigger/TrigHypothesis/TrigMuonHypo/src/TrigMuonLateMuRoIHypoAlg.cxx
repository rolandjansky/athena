/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonLateMuRoIHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonLateMuRoIHypoAlg::TrigMuonLateMuRoIHypoAlg( const std::string& name,
						  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator )
{

} 

TrigMuonLateMuRoIHypoAlg::~TrigMuonLateMuRoIHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonLateMuRoIHypoAlg::initialize()
{

  ATH_CHECK(m_hypoTools.retrieve());
  renounce(m_roiKey);
  ATH_CHECK(m_roiKey.initialize());

  return StatusCode::SUCCESS;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonLateMuRoIHypoAlg::execute( const EventContext& context ) const
{
  ATH_MSG_DEBUG("StatusCode TrigMuonLateMuRoIHypoAlg::execute start");

  // common for all hypos
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // end of common


  std::vector<TrigMuonLateMuRoIHypoTool::MuonEFInfo> toolInput;
  // loop over previous decisions
  for (const auto previousDecision: *previousDecisionsHandle ) {

    // get RoIs
    auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    auto roiEL = roiInfo.link;
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;
    
    // get View
    ATH_CHECK( previousDecision->hasObjectLink( viewString()) );
    auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get muons
    auto roiHandle = ViewHelper::makeHandle( *viewEL, m_roiKey, context );
    ATH_CHECK( roiHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << roiHandle->size() << " ..." );

    // It is posisble that no rois are found, in this case we go to the next decision
    if(roiHandle->size()<1) continue;

    for(size_t i=0; i<roiHandle->size();i++){
      auto lateRoIEL = ViewHelper::makeLink( *viewEL, roiHandle, i );
      ATH_CHECK( lateRoIEL.isValid() );
      const TrigRoiDescriptor* lateRoI = *lateRoIEL;
      // create new decisions
      auto newd = newDecisionIn( decisions, hypoAlgNodeName() );
      // push_back to toolInput
      toolInput.emplace_back( newd, roi, lateRoI, previousDecision );
      newd -> setObjectLink( featureString(), lateRoIEL );
      TrigCompositeUtils::linkToPrevious( newd, previousDecision, context );

      ATH_MSG_DEBUG("REGTEST: " << m_roiKey.key() << " eta/phi roi= " << lateRoI->eta() << "/" << lateRoI->phi() );
    }
  }
    

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");

  // to TrigMuonLateMuRoIHypoTool
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

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

