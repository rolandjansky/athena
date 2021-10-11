/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "Gaudi/Property.h"
#include "TrigEgammaFastElectronHypoAlg.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "AthViews/ViewHelper.h"


using TrigCompositeUtils::createAndStore; 
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;

TrigEgammaFastElectronHypoAlg::TrigEgammaFastElectronHypoAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")

{
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
}


StatusCode TrigEgammaFastElectronHypoAlg::initialize() {
  
  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK(m_ringerNNTools.retrieve());
  ATH_CHECK( m_electronsKey.initialize() );

  if (m_runInView){
    renounce( m_electronsKey );// electrons are made in views, so they are not in the EvtStore: hide them
  }   
  
  if ( not m_monTool.name().empty() ) 
    ATH_CHECK( m_monTool.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaFastElectronHypoAlg::execute( const EventContext& context ) const 
{
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  auto timer = Monitored::Timer("TIME_exec");
  auto timer_predict = Monitored::Timer("TIME_NN_exec");
  auto monitoring = Monitored::Group( m_monTool, timer, timer_predict);


  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");
  
  //Printing event No.
  ATH_MSG_DEBUG("Event No.: "<<context.eventID().event_number());
  
  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();


  // // extract mapping of cluster pointer to an index in the cluster decision collection
  // prepare decisions container and link back to the clusters, and decision on clusters  
  std::map<const xAOD::TrigEMCluster*, size_t> clusterToIndexMap;
  size_t clusterCounter = 0;
  for ( auto previousDecision : *previousDecisionsHandle){

    ElementLink<xAOD::TrigEMClusterContainer> clusterLink;
    LinkInfo<xAOD::TrigEMClusterContainer> linkInfo = findLink<xAOD::TrigEMClusterContainer>(previousDecision, featureString());
    clusterLink = linkInfo.link;
    ATH_CHECK( clusterLink.isValid() );    
 
    const xAOD::TrigEMCluster* cluster = *clusterLink;
    clusterToIndexMap.insert( std::make_pair( cluster, clusterCounter ) );
    clusterCounter++;
  }


  ATH_MSG_DEBUG( "Cluster ptr to decision map has size " << clusterToIndexMap.size() );

  // prepare imput for tools
  std::vector<ITrigEgammaFastElectronHypoTool::ElectronInfo> hypoToolInput;
 
  for ( auto previousDecision: *previousDecisionsHandle ) {
      // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get electron from that view:
    size_t electronCounter = 0;
    auto electronsHandle = ViewHelper::makeHandle( *viewEL, m_electronsKey, context );
    ATH_CHECK( electronsHandle.isValid() );
    ATH_MSG_DEBUG ( "electron handle size: " << electronsHandle->size() << "..." );

    for ( auto electronIter = electronsHandle->begin(); electronIter != electronsHandle->end(); ++electronIter, electronCounter++ ) {
      
      auto d = newDecisionIn( decisions, hypoAlgNodeName() );
      d->setObjectLink( featureString(), ViewHelper::makeLink<xAOD::TrigElectronContainer>( *viewEL, electronsHandle, electronCounter ) );
      
      auto clusterPtr = (*electronIter)->emCluster();
      ATH_CHECK( clusterPtr != nullptr );

      // now find matching cluster 
      // could use geometric matching but in fact the cluster owned by the decision object and the cluster owned by the electron should be the same
      // since we have a map made in advance we can make use of the index lookup w/o the need for additional loop 
      auto origCluster = clusterToIndexMap.find( clusterPtr );
      ATH_CHECK( origCluster != clusterToIndexMap.end() );
      linkToPrevious( d, decisionInput().key(), origCluster->second );
      

      const xAOD::TrigRingerRings *rings=nullptr;
      {
        LinkInfo<xAOD::TrigRingerRingsContainer> linkInfo = findLink<xAOD::TrigRingerRingsContainer>(previousDecision, "ringer");
        auto ringerLink = linkInfo.link;
        ATH_CHECK( ringerLink.isValid() ); 
        rings = *ringerLink;
      }


      // now we have DecisionObject ready to be passed to hypo tool. it has link to electron, 
      // and decisions on clusters
      // we shall avoid calling the tools for chains which were already rejected on certain cluster, but this is left to hypo tools
      DecisionIDContainer clusterDecisionIDs;
      decisionIDs( previousDecisionsHandle->at( origCluster->second ), clusterDecisionIDs );
      
      auto el = *electronIter;
      auto cl = origCluster->first;

      ITrigEgammaFastElectronHypoTool::ElectronInfo info(d, el, cl, rings, clusterDecisionIDs);


      float avgmu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
      info.valueDecorator["avgmu"] = avgmu;

      // Decorate the info object with NN ringer decision
      if(rings && cl && el){
        int idx=0;
        for (auto& pidname : m_pidNames ){
          timer_predict.start();
          float nnOutput = m_ringerNNTools[idx]->predict(rings, el);
          timer_predict.stop();
          info.pidDecorator[pidname] = static_cast<bool>(m_ringerNNTools[idx]->accept(rings, nnOutput, avgmu));
          info.valueDecorator[pidname+"NNOutput"] = nnOutput;
          idx++;
        }   
      }
            
      hypoToolInput.push_back(info);

    }// loop over all electrons

  }// loop over all decisions

  for ( auto & tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );    
  } 

  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
