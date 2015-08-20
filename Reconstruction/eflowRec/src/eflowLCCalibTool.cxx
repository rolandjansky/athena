/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowLCCalibTool.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterMomentsMaker.h"
#include "CaloClusterCorrection/CaloClusterLocalCalib.h"
#include "CaloRec/CaloClusterProcessor.h"

eflowLCCalibTool::eflowLCCalibTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_clusterCollectionTool("eflowRecClusterCollectionTool",this),
  m_clusterLocalCalibTool("CaloClusterLocalCalib",this),
  m_clusterLocalCalibOOCCTool("CaloClusterLocalCalib",this),
  m_clusterLocalCalibOOCCPi0Tool("CaloClusterLocalCalib",this),
  m_clusterLocalCalibDMTool("CaloClusterLocalCalib",this)
{
  declareInterface<eflowLCCalibTool>(this);
  declareProperty("eflowRecClusterCollectionTool",  m_clusterCollectionTool, "Tool to choose clusters to use");
  declareProperty("CaloClusterLocalCalib",        m_clusterLocalCalibTool,        "Cluster Calibration for EFlowRec");
  declareProperty("CaloClusterLocalCalibOOCC",    m_clusterLocalCalibOOCCTool,    "Cluster CalibrationOOCC for EFlowRec");
  declareProperty("CaloClusterLocalCalibOOCCPi0", m_clusterLocalCalibOOCCPi0Tool, "Cluster CalibrationOOCCPi0 for EFlowRec");
  declareProperty("CaloClusterLocalCalibDM",      m_clusterLocalCalibDMTool,      "Cluster CalibrationDM for EFlowRec");
}

StatusCode eflowLCCalibTool::initialize(){

  /* Retrieve the cluster collection tool */
  ATH_CHECK(m_clusterCollectionTool.retrieve());

  /* Retrieve basic local-hadron calibration tool */
  if ( m_clusterLocalCalibTool.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibTool " << endreq;
    return StatusCode::SUCCESS;
  }
    
  /* Retrieve tools for out-of-cluster corrections */
  if ( m_clusterLocalCalibOOCCTool.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibTool_OOCC " << endreq;
    return StatusCode::SUCCESS;
  }

  if ( m_clusterLocalCalibOOCCPi0Tool.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibTool_OOCCPi0 " << endreq;
    return StatusCode::SUCCESS;
  }

  /* Retrieve tool for DM corrections */
  if ( m_clusterLocalCalibDMTool.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibDMTool " << endreq;
    return StatusCode::SUCCESS;
  }
    
  return StatusCode::SUCCESS;

}

void eflowLCCalibTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer) {

  /* Collect all the clusters in a temporary container (with VIEW_ELEMENTS!) */
  xAOD::CaloClusterContainer* tempClusterContainer = m_clusterCollectionTool->execute(theEflowCaloObjectContainer, true);

  /* Calibrate each cluster */
  xAOD::CaloClusterContainer::iterator  itCluster = tempClusterContainer->begin();
  xAOD::CaloClusterContainer::iterator endCluster = tempClusterContainer->end();
  for (; itCluster != endCluster; ++itCluster){
    applyLocalHadronCalibration(*itCluster);
  }

  /* Delete the container again (clusters are not deleted, as the container doesn't own its elements) */
  delete tempClusterContainer;

//  return StatusCode::SUCCESS;
}

StatusCode eflowLCCalibTool::finalize(){ return StatusCode::SUCCESS; }

void eflowLCCalibTool::applyLocalHadronCalibration(xAOD::CaloCluster* cluster) {

  /* Subsequently apply all ClusterLocalCalibTools, print debug output at each stage, if DEBUG it set */

  if (msgLvl(MSG::DEBUG)) debugPrintCluster(cluster, "Before calibration", true);

  apply(m_clusterLocalCalibTool, cluster);

  if (msgLvl(MSG::DEBUG)) debugPrintCluster(cluster, "After LC, before OOC");

  apply(m_clusterLocalCalibOOCCTool, cluster);

  if (msgLvl(MSG::DEBUG)) debugPrintCluster(cluster, "After OOC, before OOCPi0");

  apply(m_clusterLocalCalibOOCCPi0Tool, cluster);

  if (msgLvl(MSG::DEBUG)) debugPrintCluster(cluster, "After OOCPi0, before DM");

  apply(m_clusterLocalCalibDMTool, cluster);

  if (msgLvl(MSG::DEBUG)) debugPrintCluster(cluster, "After DM (final)");

}

void eflowLCCalibTool::apply(ToolHandle<CaloClusterCollectionProcessor>& calibTool, xAOD::CaloCluster* cluster) {

  CaloClusterCollectionProcessor* myCollectionProcessor = &(*calibTool);
  CaloClusterProcessor* myCalibProcessor = dynamic_cast<CaloClusterProcessor*>(myCollectionProcessor);
  if (myCalibProcessor){
    if (myCalibProcessor->execute(cluster).isFailure()) {
      if (msgLvl(MSG::WARNING))
	msg(MSG::WARNING) << "Could not execute " << calibTool.name() << endreq;
    }
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " dynamic_cast provided NULL pointer to CaloClusterProcessor " << endreq;

}

void eflowLCCalibTool::debugPrintCluster(xAOD::CaloCluster* cluster, std::string prefix, bool printMoments) {

  //CaloRecoStatus theStatus = cluster->getRecoStatus();
  msg(MSG::DEBUG) << prefix << ":\nHave cluster with e, eta and phi : "
                  << cluster->e() << ", " << cluster->eta() << " and " << cluster->phi() << endreq;
    // << "\nReco status is " << theStatus.getStatusWord() << endreq;

  if (printMoments){
    /*
    xAOD::CaloCluster::moment_iterator myMoment = cluster->beginMoment(true);
    xAOD::CaloCluster::moment_iterator myEnd = cluster->endMoment(true);
    for (; myMoment != myEnd; myMoment++) {
      msg(MSG::DEBUG) << "Moment type " << cluster->getMomentType(myMoment) << "\thas value\t"
                      << myMoment.getMoment().getValue() << endreq;
    }
    */
  }

}

