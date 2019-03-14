/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEFMSonlyHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFMSonlyHypoAlg::TrigMuonEFMSonlyHypoAlg( const std::string& name,
						  ISvcLocator* pSvcLocator ) :
//  ::AthReentrantAlgorithm( name, pSvcLocator )
  ::HypoBase( name, pSvcLocator )
{

} 

TrigMuonEFMSonlyHypoAlg::~TrigMuonEFMSonlyHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFMSonlyHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK(m_hypoTools.retrieve());

  renounce(m_muonKey);
  ATH_CHECK(m_muonKey.initialize());

  ATH_MSG_INFO( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFMSonlyHypoAlg::finalize() 
{   
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  ATH_MSG_INFO( "Finalization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFMSonlyHypoAlg::execute( const EventContext& context ) const
{
  ATH_MSG_DEBUG("StatusCode TrigMuonEFMSonlyHypoAlg::execute start");

  // common for all hypos, to move in the base class
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if ( not previousDecisionsHandle.isValid() ) { // implict
     ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
     return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // end of common
  
  std::vector<TrigMuonEFMSonlyHypoTool::MuonEFInfo> toolInput;
  size_t counter = 0;  // view counter

  // loop over previous decisions
  for (const auto previousDecision: *previousDecisionsHandle ) {
     // get RoIs
    auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, "initialRoI"  );
    auto roiEL = roiInfo.link;
    //auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink<ViewContainer>( "view" );
    ATH_CHECK( viewEL.isValid() );

    // get muons
    auto muonHandle = ViewHelper::makeHandle( *viewEL, m_muonKey, context );
    ATH_CHECK( muonHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << muonHandle->size() << " ..." );

    // It is posisble that no muons are found, in this case we go to the next decision
    if(muonHandle->size()==0) continue;

    // this code only gets muon 0. The EF algorithms can potentially make more than 1 muon, so may need to revisit this
    auto muonEL = ViewHelper::makeLink( *viewEL, muonHandle, 0 );
    ATH_CHECK( muonEL.isValid() );

    const xAOD::Muon* muon = *muonEL;

    // create new decisions
    auto newd = newDecisionIn( decisions );

    // pussh_back to toolInput
    toolInput.emplace_back( newd, roi, muon, previousDecision );

    newd -> setObjectLink( "feature", muonEL );
    newd -> setObjectLink( "roi",     roiEL  );
    newd -> setObjectLink( "view",    viewEL );
    TrigCompositeUtils::linkToPrevious( newd, previousDecision );

    ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " pT = " << (*muonEL)->pt() << " GeV");
    ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " eta/phi = " << (*muonEL)->eta() << "/" << (*muonEL)->phi());
    ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
    ATH_MSG_DEBUG("Added view, roi, feature, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );

    counter++;
  }

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");

  // to TrigMuonEFMSonlyHypoTool
  StatusCode sc = StatusCode::SUCCESS;
  for ( auto& tool: m_hypoTools ) {
    ATH_MSG_DEBUG("Go to " << tool );
    sc = tool->decide(toolInput);
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("MuonHypoTool is failed");
      return StatusCode::FAILURE;
    }
  } // End of tool algorithms */	

  { // make output handle and debug, in the base class
    ATH_MSG_DEBUG ( "Exit with " << outputHandle->size() << " decisions");
    TrigCompositeUtils::DecisionIDContainer allPassingIDs;
    if ( outputHandle.isValid() ) {
      for ( auto decisionObject: *outputHandle )  {
	TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
      }
      for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
	ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
      }
      
    } else ATH_MSG_WARNING( "Output decisions are NOT valid with key : " << decisionOutput().key() );
  }

  ATH_MSG_DEBUG("StatusCode TrigMuonEFMSonlyHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

