/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigTauCaloHypoAlgMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigTauCaloHypoAlgMT::TrigTauCaloHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigTauCaloHypoAlgMT::~TrigTauCaloHypoAlgMT() {}

StatusCode TrigTauCaloHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  
  ATH_CHECK( m_hypoTools.retrieve() );
  std::cout << "After m_hypoTools.retrieve() " << std::endl;
  ATH_CHECK( m_tauJetKey.initialize() );
  std::cout << "After m_clusterKey.initialize() " << std::endl;
  renounce( m_tauJetKey );// clusters are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigTauCaloHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}

StatusCode TrigTauCaloHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigTauGenericHypoTool::ClusterInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( const auto previousDecision: *previousDecisionsHandle ) {
    //get RoI
    auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, "initialRoI" );
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    auto viewELInfo = TrigCompositeUtils::findLink< ViewContainer >( previousDecision, "view" );
    ATH_CHECK( viewELInfo.isValid() );
    auto clusterHandle = ViewHelper::makeHandle( *(viewELInfo.link), m_tauJetKey, context);
    ATH_CHECK( clusterHandle.isValid() );
    ATH_MSG_DEBUG ( "Cluster handle size: " << clusterHandle->size() << "..." );

    // create new decision
    auto d = newDecisionIn( decisions, name() );

    toolInput.emplace_back( d, roi, clusterHandle.cptr(), previousDecision );

    {
      auto el = ViewHelper::makeLink( *(viewELInfo.link), clusterHandle, 0 );
      ATH_CHECK( el.isValid() );
      d->setObjectLink( "feature",  el );
    }
    d->setObjectLink( "roi", roiELInfo.link );
    TrigCompositeUtils::linkToPrevious( d, previousDecision );
    ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision " << counter << " for view " << (*viewELInfo.link)->name()  );
    counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  {// make output handle and debug
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

  return StatusCode::SUCCESS;
}
