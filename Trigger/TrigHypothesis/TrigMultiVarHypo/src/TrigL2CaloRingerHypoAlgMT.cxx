/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "GaudiKernel/Property.h"
#include "TrigL2CaloRingerHypoAlgMT.h"
#include "ITrigL2CaloRingerHypoToolMT.h"
#include "AthViews/ViewHelper.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"


using namespace TrigCompositeUtils;

TrigL2CaloRingerHypoAlgMT::TrigL2CaloRingerHypoAlgMT( const std::string& name, 
              ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigL2CaloRingerHypoAlgMT::initialize() {
  
  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_ringsKey.initialize());
  ATH_CHECK( m_clustersKey.initialize());
  renounce(m_clustersKey);
  renounce(m_ringsKey);

  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloRingerHypoAlgMT::execute( const EventContext& context ) const {
  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");
  // new decisions
  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // input for decision
  std::vector<ITrigL2CaloRingerHypoToolMT::RingerInfo> hypoToolInput;
  
  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    //get RoI  

    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );     
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    const auto view = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( view.isValid() );

    auto ringerShapeHandle = ViewHelper::makeHandle( *(view), m_ringsKey, context);
    ATH_CHECK( ringerShapeHandle.isValid() );

    ATH_MSG_DEBUG ( "Ringer handle size: " << ringerShapeHandle->size() << "..." );

    // add cluster
    auto clusterHandle = ViewHelper::makeHandle( *view, m_clustersKey, context);
    ATH_CHECK( clusterHandle.isValid() );

    // create new decision
    auto d = newDecisionIn( decisions,  name() );
    hypoToolInput.emplace_back(  d, roi, ringerShapeHandle.cptr()->at(0), previousDecision );
    
    // link the rings
    {
      auto el = ViewHelper::makeLink( *(view), ringerShapeHandle, 0 );
      ATH_CHECK( el.isValid() );
      d->setObjectLink( featureString(),  el );
    }
    // link the cluster
    { 
      auto clus = ViewHelper::makeLink( *(view), clusterHandle, 0 );
      ATH_CHECK( clus.isValid() );
      d->setObjectLink( featureString(),  clus );
    }
    d->setObjectLink( roiString(), roiELInfo.link );

    TrigCompositeUtils::linkToPrevious( d, previousDecision, context );
    counter++;
  }

  ATH_MSG_DEBUG( "Found "<<hypoToolInput.size()<<" inputs to tools");
   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );
  }
 

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
