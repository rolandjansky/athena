/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigTauCaloHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigTauCaloHypoAlgMT::TrigTauCaloHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigTauCaloHypoAlgMT::initialize() {
  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_tauJetKey.initialize() );
  renounce( m_tauJetKey );// clusters are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}


StatusCode TrigTauCaloHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigTauGenericHypoTool::ClusterInfo> toolInput;

  // loop over previous decisions
  int counter=-1;
  for ( const auto& previousDecision: *previousDecisionsHandle ) {
    counter++;

    //get RoI
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, roiString() );
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );

    auto clusterHandle = ViewHelper::makeHandle( *viewEL, m_tauJetKey, context);
    ATH_CHECK( clusterHandle.isValid() );
    ATH_MSG_DEBUG ( "Cluster handle size: " << clusterHandle->size() << "..." );

    if( clusterHandle->size() != 1 ) {
      ATH_MSG_WARNING("Something is wrong, unexpectd number of clusters " << clusterHandle->size() << " is found (expected 1), continuing anyways skipping view");
      continue;
    }

    // create new decision
    auto d = newDecisionIn( decisions, name() );
    TrigCompositeUtils::linkToPrevious( d, previousDecision, context );
    d->setObjectLink( roiString(), roiELInfo.link );

    auto el = ViewHelper::makeLink( *viewEL, clusterHandle, 0 );
    ATH_CHECK( el.isValid() );
    d->setObjectLink( featureString(),  el );

    toolInput.emplace_back( d, roi, clusterHandle.cptr(), previousDecision );

    ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision " << counter << " for view " << (*viewEL)->name()  );
  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
