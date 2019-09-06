/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEFInvMassHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFInvMassHypoAlg::TrigMuonEFInvMassHypoAlg( const std::string& name,
						  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator )
{

} 

TrigMuonEFInvMassHypoAlg::~TrigMuonEFInvMassHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFInvMassHypoAlg::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name() << "..." );
  ATH_CHECK(m_hypoTools.retrieve());

  renounce(m_muonKey);
  ATH_CHECK(m_muonKey.initialize());

  ATH_MSG_DEBUG( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFInvMassHypoAlg::execute( const EventContext& context ) const
{
  ATH_MSG_DEBUG("StatusCode TrigMuonEFInvMassHypoAlg::execute start");

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

  //vector of muon container and decisions
  std::vector<std::pair<const xAOD::MuonContainer*, const TrigCompositeUtils::Decision*> > vecMuDec;
  std::pair<const xAOD::MuonContainer*, const TrigCompositeUtils::Decision*> muonDec;

  std::vector<TrigMuonEFInvMassHypoTool::MuonEFInfo> toolInput;
  // loop over previous decisions
  for (const auto previousDecision: *previousDecisionsHandle ) {

    // get View
    ATH_CHECK( previousDecision->hasObjectLink( viewString()) );
    auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get muons
    auto muonHandle = ViewHelper::makeHandle( *viewEL, m_muonKey, context );
    ATH_CHECK( muonHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << muonHandle->size() << " ..." );

    // It is posisble that no muons are found, in this case we go to the next decision
    if(muonHandle->size()<2) continue;
    muonDec.first = muonHandle.ptr();
    muonDec.second = previousDecision;
    vecMuDec.push_back(muonDec);
  }

    //send pairs of muons to the hypo tool
  for(size_t i=0; i<vecMuDec.size();i++){

    auto muonCont1 = (vecMuDec.at(i)).first;
    auto dec1 = (vecMuDec.at(i)).second;

    for(size_t j=i; j<vecMuDec.size(); j++){
      auto muonCont2 = (vecMuDec.at(j)).first;
      auto dec2 = (vecMuDec.at(j)).second;

      for(auto mu1 : *muonCont1){
	for(auto mu2 : *muonCont2){

	  if(mu1==mu2) continue;

	  // create new decisions
	  auto newd = newDecisionIn( decisions );

	  // push_back to toolInput
	  toolInput.emplace_back( newd, mu1, mu2, dec1, dec2);
	  const ElementLink<xAOD::MuonContainer> muonEL1 = ElementLink<xAOD::MuonContainer>( *muonCont1, mu1->index() );
	  const ElementLink<xAOD::MuonContainer> muonEL2 = ElementLink<xAOD::MuonContainer>( *muonCont2, mu2->index() );
	  newd -> setObjectLink( featureString(), muonEL1 );
	  newd -> setObjectLink( featureString(), muonEL2 );
	  TrigCompositeUtils::linkToPrevious( newd, dec1, context );
	  TrigCompositeUtils::linkToPrevious( newd, dec2, context );

	  ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " pT mu1 = " << mu1->pt() << " GeV"<< " pT mu2 = " << mu2->pt() << " GeV");
	  ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " eta/phi mu1= " << mu1->eta() << "/" << mu1->phi() << " eta/phi mu2= " << mu2->eta() << "/" << mu2->phi());
	}
      }
    }
  }
    

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");

  // to TrigMuonEFInvMassHypoTool
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

  ATH_MSG_DEBUG("StatusCode TrigMuonEFInvMassHypoAlg::execute success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

