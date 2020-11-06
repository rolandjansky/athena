/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigEgammaPrecisionEtcutHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigEgammaPrecisionEtcutHypoAlgMT::TrigEgammaPrecisionEtcutHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigEgammaPrecisionEtcutHypoAlgMT::initialize() {
  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_clustersKey.initialize() );
  renounce( m_clustersKey );// clusters are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaPrecisionEtcutHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");


  // new decisions

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto outputDecision = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEgammaPrecisionEtcutHypoTool::ClusterInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
  
    const auto featureEL = findLink<xAOD::CaloClusterContainer>( previousDecision, featureString() );
    ATH_CHECK(featureEL.isValid());
    auto d = newDecisionIn( outputDecision, name() );
    d->setObjectLink<>( featureString(),  featureEL.link );
    
    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
    toolInput.emplace_back( d, previousDecision );   

    ATH_MSG_DEBUG( "previous decision to new decision " << counter << " for roi " );
    counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}
