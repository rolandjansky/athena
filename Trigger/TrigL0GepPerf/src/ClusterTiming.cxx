/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigL0GepPerf/ClusterTiming.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"
#include "StoreGate/ReadHandle.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "TrigT1Interfaces/L1METvalue.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventShape/EventShape.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


ClusterTiming::ClusterTiming( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  declareProperty("inputClusterName",m_inputClusterName="CaloTopoClusters");
  declareProperty("outputClusterName",m_outputClusterName="Clusters420Timing"); 
  declareProperty("LambdaCalDivide", m_lambdaCalDivide = 317);
  declareProperty("QualityCut", m_qualityCut = 0.02);
  declareProperty("TimeCutLargeQ", m_timeCutLargeQ = 5);
  declareProperty("TimeCutSmallQ", m_timeCutSmallQ = 15);
  declareProperty("MaxEtaForCut", m_etaCut = 5.0);//Default apply to all cluster eta regions
}


ClusterTiming::~ClusterTiming() {}


StatusCode ClusterTiming::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ClusterTiming::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode ClusterTiming::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed

  //This function will take a set of clusters, apply a timing cut, then fill a new container of clusters that satisfy that requirement 

  const xAOD::CaloClusterContainer *topoClusters = 0;
  CHECK(evtStore()->retrieve(topoClusters,m_inputClusterName));
  
  xAOD::CaloClusterContainer *topoClustersOut = new xAOD::CaloClusterContainer(); 
  xAOD::CaloClusterAuxContainer *topoClustersOutAux = new xAOD::CaloClusterAuxContainer(); 
  topoClustersOut->setStore(topoClustersOutAux); 
  CHECK(evtStore()->record(topoClustersOut,m_outputClusterName)); 
  CHECK(evtStore()->record(topoClustersOutAux,m_outputClusterName+"Aux."));
  //Timing cut tool is available, but not in 21.3. Copying the tool to this algorithm until we can move these tools more uniformly. 
  //https://gitlab.cern.ch/atlas/athena/-/blob/21.2/Reconstruction/Jet/JetRecTools/Root/ConstitTimeCutTool.cxx
  const static SG::AuxElement::ConstAccessor<float> acc_larq("AVG_LAR_Q");
  const static SG::AuxElement::ConstAccessor<float> acc_clambda("CENTER_LAMBDA");
 
  for ( const auto* cluster : *topoClusters ) {
    
    float time = cluster->time(); 
    float quality = acc_larq(*cluster)/65535; 
    float lambda_center = acc_clambda(*cluster); 
    
    float timeCut = quality > m_qualityCut ? m_timeCutLargeQ : m_timeCutSmallQ;
    if(lambda_center < m_lambdaCalDivide && std::abs(cluster->eta()) < m_etaCut) {
      if( abs(time) > timeCut )continue; 
    }

   
    xAOD::CaloCluster* cl = new xAOD::CaloCluster();
    topoClustersOut->push_back(cl);
    *cl = *cluster;

  }

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}



