/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthLinks/ElementLink.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigMufastHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMufastHypoAlg::TrigMufastHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
   ::HypoBase( name, pSvcLocator )
{}


TrigMufastHypoAlg::~TrigMufastHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK(m_hypoTools.retrieve());

  renounce(m_muFastKey);
  ATH_CHECK(m_muFastKey.initialize());
 
  ATH_MSG_INFO( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::finalize() 
{   
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  ATH_MSG_INFO( "Finalization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::execute( const EventContext& context ) const
{

  // common for all Hypos, to move in the base class
  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute start");
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

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
    //    auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink< ViewContainer >( viewString() );
    ATH_CHECK( viewEL.isValid() );

    //// get info of that view
    auto muFastHandle = ViewHelper::makeHandle( *viewEL, m_muFastKey, context );
    ATH_CHECK( muFastHandle.isValid() );
    ATH_MSG_DEBUG ( "Muinfo handle size: " << muFastHandle->size() << "..." );

    auto muonEL = ViewHelper::makeLink( *viewEL, muFastHandle, 0 );
    ATH_CHECK( muonEL.isValid() );
    const xAOD::L2StandAloneMuon* muon = *muonEL;

    // create new decision
    auto newd = newDecisionIn( decisions );

    // push_back to toolInput
    toolInput.emplace_back( newd, roi, muon, previousDecision );
    
    newd->setObjectLink( featureString(), muonEL );
    // This attaches the same ROI with a different name ("InitialRoI" -> "RoI").
    // If the ROI will never change, please re-configure your InputMaker to use the "InitialRoI" link
    newd->setObjectLink( roiString(),     roiEL );
    TrigCompositeUtils::linkToPrevious( newd, previousDecision, context );
    
    ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " pT = " << (*muonEL)->pt() << " GeV");
    ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " eta/phi = " << (*muonEL)->eta() << "/" << (*muonEL)->phi());
    ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
    ATH_MSG_DEBUG("Added view, roi, feature, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );
    counter++;
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

  ATH_CHECK(printDebugInformation(outputHandle));

  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

