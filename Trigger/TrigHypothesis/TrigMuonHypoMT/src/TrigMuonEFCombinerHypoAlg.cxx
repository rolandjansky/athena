/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFCombinerHypoAlg.h"
#include "AthViews/ViewHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFCombinerHypoAlg::TrigMuonEFCombinerHypoAlg( const std::string& name,
						  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator )
{

} 

TrigMuonEFCombinerHypoAlg::~TrigMuonEFCombinerHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFCombinerHypoAlg::initialize()
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

StatusCode TrigMuonEFCombinerHypoAlg::finalize() 
{   
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  ATH_MSG_INFO( "Finalization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFCombinerHypoAlg::execute( const EventContext& context ) const
{
  ATH_MSG_DEBUG("StatusCode TrigMuonEFCombinerHypoAlg::execute start");

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
  
  std::vector<TrigMuonEFCombinerHypoTool::MuonEFInfo> toolInput;
  size_t counter = 0;  // view counter

  // loop over previous decisions
  for ( const auto previousDecision: *previousDecisionsHandle ) {
     // get RoIs
    auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    auto roiEL = roiInfo.link;
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get muons
    auto muonHandle = ViewHelper::makeHandle( *viewEL, m_muonKey, context );
    ATH_CHECK( muonHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << muonHandle->size() << " ..." );

    // It is posisble that no muons are found, in this case we go to the next decision
    if(muonHandle->size()==0) continue;

    //loop over muons (more than one muon can be found by EF algos)
    for(uint i=0; i<muonHandle->size(); i++){
      auto muonEL = ViewHelper::makeLink( *viewEL, muonHandle, i );
      ATH_CHECK( muonEL.isValid() );

      const xAOD::Muon* muon = *muonEL;

      // create new decisions
      auto newd = newDecisionIn( decisions );
      
      // pussh_back to toolInput
      toolInput.emplace_back( newd, roi, muon, previousDecision );

      newd -> setObjectLink( featureString(), muonEL );
      newd->setObjectLink( viewString(),    viewEL);
      TrigCompositeUtils::linkToPrevious( newd, previousDecision, context );

      ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " pT = " << (*muonEL)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " eta/phi = " << (*muonEL)->eta() << "/" << (*muonEL)->phi());
      ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
      ATH_MSG_DEBUG("Added view, roi, feature, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );
    }
    counter++;
  }

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");

  // to TrigMuonEFCombinerHypoTool
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

  ATH_MSG_DEBUG("StatusCode TrigMuonEFCombinerHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

