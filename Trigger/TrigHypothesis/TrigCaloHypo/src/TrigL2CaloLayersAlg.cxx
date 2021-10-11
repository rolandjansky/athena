/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2CaloLayersAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "AthViews/View.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigL2CaloLayersAlg::TrigL2CaloLayersAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigL2CaloLayersAlg::initialize() {

  ATH_CHECK( m_caloContainerKey.initialize() );
  ATH_CHECK( m_hypoTools.retrieve() );
  
  renounce(m_caloContainerKey);
  return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloLayersAlg::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigL2CaloLayersHypoTool::CaloLayersInfo> toolInput;


  for (const Decision* previousDecision: *previousDecisionsHandle) {

    const auto viewELInfo = findLink<ViewContainer>(previousDecision, viewString());
    ATH_CHECK( viewELInfo.isValid() );
    SG::ReadHandle<xAOD::TrigEMClusterContainer> clustersHandle 
      = ViewHelper::makeHandle(*viewELInfo.link, m_caloContainerKey, context);
    ATH_CHECK( clustersHandle.isValid() );

    // Get new output decision, child of previousDecision
    auto d = newDecisionIn (decisions, previousDecision, hypoAlgNodeName(), context );
    
    // create summary struct
    toolInput.emplace_back( d, clustersHandle.cptr(), previousDecision );
    

    // link the cluster
    { 
      auto clusters = ElementLink<xAOD::TrigEMClusterContainer>( *clustersHandle, 0 );
      ATH_CHECK( clusters.isValid() );
      d->setObjectLink( featureString(),  clusters );
    }
  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
