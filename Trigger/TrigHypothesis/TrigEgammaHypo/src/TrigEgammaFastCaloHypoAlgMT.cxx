/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigEgammaFastCaloHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigEgammaFastCaloHypoAlgMT::TrigEgammaFastCaloHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigEgammaFastCaloHypoAlgMT::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_clustersKey.initialize() );
  ATH_CHECK( m_ringsKey.initialize());

  renounce( m_clustersKey );// clusters are made in views, so they are not in the EvtStore: hide them
  renounce( m_ringsKey );

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaFastCaloHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");


  // new decisions

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEgammaFastCaloHypoTool::FastClusterInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( const auto previousDecision: *previousDecisionsHandle ) {
    //get RoI  
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get cluster
    auto clusterHandle = ViewHelper::makeHandle( *viewEL, m_clustersKey, context);
    ATH_CHECK( clusterHandle.isValid() );
    ATH_MSG_DEBUG ( "Cluster handle size: " << clusterHandle->size() << "..." );

    // get Rings
    auto ringerShapeHandle = ViewHelper::makeHandle( *viewEL, m_ringsKey, context);
    ATH_CHECK( ringerShapeHandle.isValid() );

    ATH_MSG_DEBUG ( "Ringer handle size: " << ringerShapeHandle->size() << "..." );



    // create new decision
    auto d = newDecisionIn( decisions, name() );

    toolInput.emplace_back( d, roi, clusterHandle.cptr()->at(0), ringerShapeHandle.cptr()->at(0), previousDecision );
    
    // link the rings
    {
      auto el = ViewHelper::makeLink( *viewEL, ringerShapeHandle, 0 );
      ATH_CHECK( el.isValid() );
      d->setObjectLink( "ringer",  el );
    }
    // link the cluster
    { 
      auto clus = ViewHelper::makeLink( *viewEL, clusterHandle, 0 );
      ATH_CHECK( clus.isValid() );
      d->setObjectLink( featureString(),  clus );
    }
    
    d->setObjectLink( roiString(), roiELInfo.link );
    
    TrigCompositeUtils::linkToPrevious( d, previousDecision, context );
    ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision " << counter << " for view " << (*viewEL)->name()  );
    counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
