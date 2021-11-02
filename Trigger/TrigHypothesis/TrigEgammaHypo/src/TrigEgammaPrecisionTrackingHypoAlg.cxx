/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigEgammaPrecisionTrackingHypoAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaPrecisionTrackingHypoAlg::TrigEgammaPrecisionTrackingHypoAlg( const std::string& name, 
    ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigEgammaPrecisionTrackingHypoAlg::initialize() {

  ATH_CHECK( m_clustersKey.initialize() );
  ATH_CHECK( m_hypoTools.retrieve() );
  renounce( m_clustersKey );
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaPrecisionTrackingHypoAlg::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");
  ATH_MSG_DEBUG( "m_clustersKey: "<<m_clustersKey);
  
  // new decisions

  // new output decisions
  SG::WriteHandle<TCU::DecisionContainer> outputHandle = TCU::createAndStore(decisionOutput(), context );
  TCU::DecisionContainer* outputDecision = outputHandle.ptr();
 
  // input for decision
  std::vector<ITrigEgammaPrecisionTrackingHypoTool::ClusterInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( const TCU::Decision* previousDecision: *previousDecisionsHandle ) {

    std::vector< TCU::LinkInfo<ViewContainer> > previousViews = TCU::findLinks<ViewContainer>( previousDecision, TCU::viewString() );
    ATH_CHECK( previousViews.size() >= 2);
    const ElementLink<ViewContainer> viewEL = previousViews.at(1).link;
    ATH_MSG_DEBUG( "viewEL.dataID(): "<<viewEL.dataID());
    ATH_CHECK( viewEL.isValid() );
    auto clusterHandle = ViewHelper::makeHandle( *(viewEL), m_clustersKey, context);
    
    ATH_CHECK( clusterHandle.isValid() );
    ATH_MSG_DEBUG ( "Cluster handle size: " << clusterHandle->size() << "..." );
    // Loop over the clusterHandles
    size_t validclusters=0;
    for (size_t cl=0; cl< clusterHandle->size(); cl++){
	{
	    auto el = ViewHelper::makeLink( *(viewEL), clusterHandle, cl );
	    
            ATH_MSG_DEBUG ( "Checking el.isValid()...");
	    if( !el.isValid() ) {
		ATH_MSG_DEBUG ( "ClusterHandle in position " << cl << " -> invalid ElemntLink!. Skipping...");
	    }
	    ATH_CHECK(el.isValid());

	    ATH_MSG_DEBUG ( "ClusterHandle in position " << cl << " processing...");
	    auto d = TCU::newDecisionIn( outputDecision, TCU::hypoAlgNodeName() );
	    d->setObjectLink( TCU::featureString(),  el );
	    TCU::linkToPrevious( d, decisionInput().key(), counter );
	    toolInput.emplace_back( d, clusterHandle.cptr()->at(cl), previousDecision );
	    validclusters++;


	}
    }

    ATH_MSG_DEBUG( "Clusters with valid links: " << validclusters );
    
    counter++;

  }


  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}
