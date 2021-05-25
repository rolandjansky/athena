/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigEgammaFastCaloHypoAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoringKernel/Monitored.h"


namespace TCU = TrigCompositeUtils;

TrigEgammaFastCaloHypoAlg::TrigEgammaFastCaloHypoAlg( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ),
   m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
  {
    declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
  }


StatusCode TrigEgammaFastCaloHypoAlg::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );

  if(m_useRun3){
    ATH_CHECK(m_ringerNNTools.retrieve());
  }else{
    for( size_t idx=0; idx<m_constantsCalibPaths.size(); ++idx){
      auto* selector = new Ringer::RingerSelectorTool();
      selector->setConstantsCalibPath(m_constantsCalibPaths[idx]);
      selector->setThresholdsCalibPath(m_thresholdsCalibPaths[idx]);
      ATH_CHECK(selector->initialize());
      m_ringerTools.push_back(selector) ;
    }
  }

  ATH_CHECK( m_clustersKey.initialize() );
  ATH_CHECK( m_ringsKey.initialize(SG::AllowEmpty));

  renounce( m_clustersKey );// clusters are made in views, so they are not in the EvtStore: hide them
  renounce( m_ringsKey );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );
  

  ATH_MSG_DEBUG( "Initialization of FastCaloHypoAlg completed successfully");
  return StatusCode::SUCCESS;
}



StatusCode TrigEgammaFastCaloHypoAlg::execute( const EventContext& context ) const 
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  auto timer = Monitored::Timer("TIME_exec");
  auto timer_predict = Monitored::Timer("TIME_NN_exec");
  auto monitoring = Monitored::Group( m_monTool, timer, timer_predict);


  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");
  
  // new decisions
  // new output decisions
  SG::WriteHandle<TCU::DecisionContainer> outputHandle = TCU::createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEgammaFastCaloHypoTool::FastClusterInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( const auto previousDecision: *previousDecisionsHandle ) 
  {
    //get RoI  
    auto roiELInfo = TCU::findLink<TrigRoiDescriptorCollection>( previousDecision, TCU::initialRoIString() );
    
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( TCU::viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get cluster
    auto clusterHandle = ViewHelper::makeHandle( *viewEL, m_clustersKey, context);
    ATH_CHECK( clusterHandle.isValid() );
    ATH_MSG_DEBUG ( "Cluster handle size: " << clusterHandle->size() << "..." );

    auto d = TCU::newDecisionIn( decisions, TCU::hypoAlgNodeName() );
    
    const xAOD::TrigRingerRings* ringerShape = nullptr; 
    const xAOD::TrigEMCluster* emCluster = nullptr;

    if ( not m_ringsKey.empty() ) {  

      auto ringerShapeHandle = ViewHelper::makeHandle( *viewEL, m_ringsKey, context);      
      ATH_CHECK( ringerShapeHandle.isValid());
      auto ringHandle = ringerShapeHandle.cptr();	
      ATH_MSG_DEBUG ( "Ringer handle size: " << ringerShapeHandle->size() << "..." );

      // link the rings      
      auto el = ViewHelper::makeLink( *viewEL, ringerShapeHandle, 0 );
      ATH_CHECK( el.isValid() );
      d->setObjectLink( "ringer",  el );
      ringerShape = ringHandle->at(0);
      if(ringerShape)
        emCluster = ringerShape->emCluster();
    }

    ITrigEgammaFastCaloHypoTool::FastClusterInfo info(d, roi, clusterHandle.cptr()->at(0), ringerShape , previousDecision);



    float avgmu   = m_lumiBlockMuTool->averageInteractionsPerCrossing();
    info.valueDecorator["avgmu"] = avgmu;

    // Decorate the info object with NN ringer decision
    if(ringerShape && emCluster){
      int idx=0;
      for (auto& pidname : m_pidNames ){
        if (m_useRun3){
          auto inputs = m_ringerNNTools[0]->prepare_inputs( ringerShape , nullptr);
          timer_predict.start();
          float nnOutput = m_ringerNNTools[0]->predict(ringerShape, inputs);
          timer_predict.stop();
          info.pidDecorator[pidname] = (bool)m_ringerNNTools[idx]->accept(ringerShape, nnOutput, avgmu);
          info.valueDecorator[pidname+"NNOutput"] = nnOutput;
        }else{
          const std::vector<float> rings = ringerShape->rings();
          std::vector<float> refRings(rings.size());
          refRings.assign(rings.begin(), rings.end());
          timer_predict.start();
          float nnOutput = m_ringerTools[0]->calculate( refRings, emCluster->et(), emCluster->eta(), avgmu );
          timer_predict.stop();
          info.pidDecorator[pidname] = m_ringerTools[idx]->accept(nnOutput, emCluster->et(),emCluster->eta(),avgmu) ;
          info.valueDecorator[pidname+"NNOutput"] = nnOutput;
        }
        idx++;
      }   
    }
            
    toolInput.push_back( info );
    

    // link the cluster
    { 
      auto clus = ViewHelper::makeLink( *viewEL, clusterHandle, 0 );
      ATH_CHECK( clus.isValid() );
      d->setObjectLink( TCU::featureString(),  clus );
    }
    
    d->setObjectLink( TCU::roiString(), roiELInfo.link );
    
    TCU::linkToPrevious( d, previousDecision, context );
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
