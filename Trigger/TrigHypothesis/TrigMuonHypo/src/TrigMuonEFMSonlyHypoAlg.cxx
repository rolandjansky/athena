/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonHypo/TrigMuonEFMSonlyHypoAlg.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFMSonlyHypoAlg::TrigMuonEFMSonlyHypoAlg( const std::string& name,
						  ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
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

  ATH_CHECK(m_viewsKey.initialize());

  renounce(m_muonKey);
  ATH_CHECK(m_muonKey.initialize());

  renounce(m_roiKey);
  ATH_CHECK(m_roiKey.initialize());

  ATH_CHECK(m_decisionsKey.initialize());

  ATH_CHECK(m_previousDecisionsKey.initialize());
 
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

StatusCode TrigMuonEFMSonlyHypoAlg::execute_r( const EventContext& context ) const
{

  ATH_MSG_DEBUG("StatusCode TrigMuonEFMSonlyHypoAlg::execute_r start");

  auto viewsHandle = SG::makeHandle( m_viewsKey, context );
  std::map<const TrigRoiDescriptor*, const TrigCompositeUtils::Decision* > roiToDecision;
  auto previousDecisionsHandle = SG::makeHandle( m_previousDecisionsKey, context );
  for ( auto previousDecision: *previousDecisionsHandle ) { 
    auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    if (!roiEL.isValid()) {
      ATH_MSG_ERROR("ReadHandle for std::vector<SG::View*> key:" << m_viewsKey.key() << " is failed");
      return StatusCode::FAILURE;
    }
    const TrigRoiDescriptor* roi = *roiEL;
    roiToDecision.insert( std::make_pair( roi, previousDecision ) );
  }
  ATH_MSG_DEBUG("REGTEST: RoI to decisions map size: " << roiToDecision.size() );

  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore(aux.get());
  size_t counter = 0;	// view counter
  std::vector<TrigMuonEFMSonlyHypoTool::MuonEFInfo> toolInput;
  for (auto view: *viewsHandle) {
    auto d = newDecisionIn( decisions.get() );
    // retrieve Muons
    auto muonHandle = SG::makeHandle( m_muonKey, context );
    if (muonHandle.setProxyDict(view).isFailure()) {
      ATH_MSG_ERROR("ReadHandle for xAOD::MuonContainer key:" << m_muonKey.key() << " is failed");
      return StatusCode::FAILURE;
    } 
    const xAOD::MuonContainer* muons = muonHandle.cptr();
    // retrieve RoIs
    auto roiHandle = SG::makeHandle( m_roiKey, context );
    if (roiHandle.setProxyDict(view).isFailure()) {
      ATH_MSG_ERROR("ReadHandle for TrigRoiDescriptor key:" << m_roiKey.key() << " is failed");
      return StatusCode::FAILURE;
    } 
    const TrigRoiDescriptor* roi = roiHandle.cptr()->at(0);
    // push_back to toolInput
    toolInput.emplace_back( d, roi, muons, roiToDecision[roi] );

    // retrieve ViewRoIs
    auto viewlink = ElementLink< std::vector<SG::View*> >( m_viewsKey.key(), counter );
    if(!viewlink.isValid()) {
      ATH_MSG_ERROR("ReadHandle for std::vector<SG::View*> key:" << m_viewsKey.key() << " isn't Valid");
      return StatusCode::FAILURE;
    } else {
      d->setObjectLink( "view", viewlink );
      ATH_MSG_DEBUG("REGTEST: " << m_viewsKey.key() << " = " << (*viewlink));
    }
    
    // retrieve RoI descriptor
    auto roilink = ElementLink<TrigRoiDescriptorCollection>( view->name()+"_"+m_roiKey.key(), 0 ); 
    if(!roilink.isValid()) {
      ATH_MSG_ERROR("ReadHandle for TrigRoiDescriptorCollection key:" << m_roiKey.key() << " isn't Valid");
      return StatusCode::FAILURE;
    } else {
      d->setObjectLink( "roi", roilink );
      ATH_MSG_DEBUG("REGTEST: " << m_roiKey.key() << " eta/phi = " << (*roilink)->eta() << "/" << (*roilink)->phi());
    }
    
    // retrieve xAOD::Muon Decision 
    if(muons->size()>0){
      auto muonlink = ElementLink<xAOD::MuonContainer>( view->name()+"_"+m_muonKey.key(), 0 ); 
      if(!muonlink.isValid()) {
	ATH_MSG_ERROR("ReadHandle for xAOD::MuonContainer key:" << m_muonKey.key() << " isn't Valid");
	return StatusCode::FAILURE;
      } else {
	d->setObjectLink( "feature", muonlink );
	ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " pT = " << (*muonlink)->pt() << " GeV");
	ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " eta/phi = " << (*muonlink)->eta() << "/" << (*muonlink)->phi());
      }
    }
    
    counter++;
  } // End of view loops */

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

  auto handle =  SG::makeHandle(m_decisionsKey, context);     
  ATH_CHECK( handle.record( std::move(decisions), std::move(aux) ) );

  ATH_MSG_DEBUG("StatusCode TrigMuonEFMSonlyHypoAlg::execute_r success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

