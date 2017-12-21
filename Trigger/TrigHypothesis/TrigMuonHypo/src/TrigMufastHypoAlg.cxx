/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigMuonHypo/TrigMufastHypoAlg.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMufastHypoAlg::TrigMufastHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
   declareProperty("ViewRoIs", m_viewsKey = std::string("ViewRoIs"), "MUViewRoIs to read in");
   declareProperty("MuFastDecisions", m_muFastKey = std::string("MufastDecisions"), "xAOD::L2StandAloneMuonContainer to read in");
   declareProperty("RoIs", m_roiKey = std::string("RoIs"), "TrigRoiDescriptor RoIs to read in");
   declareProperty("Decisions", m_decisionsKey = std::string("TrigMufastHypoAlgDesicions"), "Decisions");
   declareProperty("L1Decisions", m_previousDecisionsKey = std::string("L1Decisions"), "Key for L1 decisions per RoI");
} 

TrigMufastHypoAlg::~TrigMufastHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK(m_hypoTools.retrieve());

  ATH_CHECK(m_viewsKey.initialize());

  renounce(m_muFastKey);
  ATH_CHECK(m_muFastKey.initialize());

  renounce(m_roiKey);
  ATH_CHECK(m_roiKey.initialize());

  ATH_CHECK(m_decisionsKey.initialize());

  ATH_CHECK(m_previousDecisionsKey.initialize());
 
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

StatusCode TrigMufastHypoAlg::execute_r( const EventContext& context ) const
{

  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute_r start");

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
  std::vector<TrigMufastHypoTool::MuonClusterInfo> toolInput;
  for (auto view: *viewsHandle) {
    auto d = newDecisionIn( decisions.get() );

    // retrieve MuFastDecisions with ReadHandle
    auto muFastHandle = SG::makeHandle( m_muFastKey, context );
    if (muFastHandle.setProxyDict(view).isFailure()) {
      ATH_MSG_ERROR("ReadHandle for xAOD::L2StandAloneMuonContainer key:" << m_muFastKey.key() << " is failed");
      return StatusCode::FAILURE;
    } 
    const xAOD::L2StandAloneMuon* fast = muFastHandle.cptr()->at(0);
 
    // retrieve MuFastDecisions with ReadHandle
    auto roiHandle = SG::makeHandle( m_roiKey, context );
    if (roiHandle.setProxyDict(view).isFailure()) {
      ATH_MSG_ERROR("ReadHandle for TrigRoiDescriptor key:" << m_roiKey.key() << " is failed");
      return StatusCode::FAILURE;
    } 
    const TrigRoiDescriptor* roi = roiHandle.cptr()->at(0);

    // push_back to toolInput
    toolInput.emplace_back( d, roi, fast, roiToDecision[roi] );

    { // retrieve MUViewRoIs
      auto element = ElementLink< std::vector<SG::View*> >( m_viewsKey.key(), counter );
      if(!element.isValid()) {
        ATH_MSG_ERROR("ReadHandle for std::vector<SG::View*> key:" << m_viewsKey.key() << " isn't Valid");
        return StatusCode::FAILURE;
      } else {
        d->setObjectLink( "view", element );
        ATH_MSG_DEBUG("REGTEST: " << m_viewsKey.key() << " = " << (*element));
      }
    }
    { // retrieve MURoIs
      auto element = ElementLink<TrigRoiDescriptorCollection>( view->name()+"_"+m_roiKey.key(), 0 ); 
      if(!element.isValid()) {
        ATH_MSG_ERROR("ReadHandle for TrigRoiDescriptorCollection key:" << m_roiKey.key() << " isn't Valid");
        return StatusCode::FAILURE;
      } else {
        d->setObjectLink( "roi", element );
        ATH_MSG_DEBUG("REGTEST: " << m_roiKey.key() << " eta/phi = " << (*element)->eta() << "/" << (*element)->phi());
      }
    }
    { // retrieve xAOD::MuonFastDecision 
      auto element = ElementLink<xAOD::L2StandAloneMuonContainer>( view->name()+"_"+m_muFastKey.key(), 0 ); 
      if(!element.isValid()) {
        ATH_MSG_ERROR("ReadHandle for xAOD::L2StandAloneMuonContainer key:" << m_muFastKey.key() << " isn't Valid");
        return StatusCode::FAILURE;
      } else {
        d->setObjectLink( "feature", element );
        ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " pT = " << (*element)->pt() << " GeV");
        ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " eta/phi = " << (*element)->eta() << "/" << (*element)->phi());
      }
    }
    counter++;
  } // End of view loops */

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

  auto handle =  SG::makeHandle(m_decisionsKey, context);     
  ATH_CHECK( handle.record( std::move(decisions), std::move(aux) ) );

  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute_r success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

