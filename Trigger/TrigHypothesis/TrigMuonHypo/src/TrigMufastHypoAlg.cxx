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

StatusCode TrigMufastHypoAlg::execute_r( const EventContext& context ) const
{

  // common for all Hypos, to move in the base class
  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute_r start");
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore(aux.get());
  // end of common
 

  std::vector<TrigMufastHypoTool::MuonClusterInfo> toolInput;
  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    //get RoI
    auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink<std::vector<SG::View*>>( "view" );
    CHECK( viewEL.isValid() );
    const SG::View* view_const = *viewEL;
    SG::View* view = const_cast<SG::View*>(view_const); // CHECK THIS!

    // get info of that view
    auto muFastHandle = SG::makeHandle( m_muFastKey, context );    
    CHECK( muFastHandle.setProxyDict( view ) );
    ATH_MSG_DEBUG ( "Muinfo handle size: " << muFastHandle->size() << "..." );

    const xAOD::L2StandAloneMuon* fast = muFastHandle.cptr()->at(0);
    
    // create new decision
    auto d = newDecisionIn( decisions.get() );

    // push_back to toolInput
    toolInput.emplace_back( d, roi, fast, previousDecision );
    
    {
      auto element = ElementLink<xAOD::L2StandAloneMuonContainer>( view->name()+"_"+m_muFastKey.key(), 0 );
      CHECK( element.isValid() );
      d->setObjectLink( "feature", element );
      ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " pT = " << (*element)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: " << m_muFastKey.key() << " eta/phi = " << (*element)->eta() << "/" << (*element)->phi());
    }
    d->setObjectLink( "roi", roiEL );
    d->setObjectLink( "view", viewEL );
    ATH_MSG_DEBUG("REGTEST:  View = " << (*viewEL));
    ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
    ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision "<<counter <<" for view "<<view->name()  );
    counter++;
  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");


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



  {// make output handle and debug, in the base class
    auto outputHandle = SG::makeHandle(decisionOutput(), context);
    CHECK( outputHandle.record( std::move( decisions ), std::move( aux ) ) );
    ATH_MSG_DEBUG ( "Exit with "<<outputHandle->size() <<" decisions");
    TrigCompositeUtils::DecisionIDContainer allPassingIDs;
    if ( outputHandle.isValid() ) {
      for ( auto decisionObject: *outputHandle )  {
	TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
      }
      for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
	ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
      }
    }
  }


  ATH_MSG_DEBUG("StatusCode TrigMufastHypoAlg::execute_r success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

