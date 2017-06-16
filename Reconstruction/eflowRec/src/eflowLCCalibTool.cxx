/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowLCCalibTool.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloRec/CaloClusterProcessor.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

eflowLCCalibTool::eflowLCCalibTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_clusterCollectionTool("eflowRecClusterCollectionTool", this),
  m_clusterLocalCalibTool("CaloClusterLocalCalib", this),
  m_clusterLocalCalibOOCCTool("CaloClusterLocalCalib", this),
  m_clusterLocalCalibOOCCPi0Tool("CaloClusterLocalCalib", this),
  m_clusterLocalCalibDMTool("CaloClusterLocalCalib", this),
  m_useLocalWeight(false) {
  declareInterface<eflowLCCalibTool>(this);
  declareProperty("eflowRecClusterCollectionTool", m_clusterCollectionTool,       "Tool to choose clusters to use");
  declareProperty("CaloClusterLocalCalib",        m_clusterLocalCalibTool,        "Cluster Calibration for EFlowRec");
  declareProperty("CaloClusterLocalCalibOOCC",    m_clusterLocalCalibOOCCTool,    "Cluster CalibrationOOCC for EFlowRec");
  declareProperty("CaloClusterLocalCalibOOCCPi0", m_clusterLocalCalibOOCCPi0Tool, "Cluster CalibrationOOCCPi0 for EFlowRec");
  declareProperty("CaloClusterLocalCalibDM",      m_clusterLocalCalibDMTool,      "Cluster CalibrationDM for EFlowRec");
  declareProperty("UseLocalWeight",               m_useLocalWeight,               "Use weight before subtraction");
}

StatusCode eflowLCCalibTool::initialize() {

  /* Retrieve the cluster collection tool */
  ATH_CHECK(m_clusterCollectionTool.retrieve());

  /* Retrieve basic local-hadron calibration tool */
  ATH_CHECK(m_clusterLocalCalibTool.retrieve());

  /* Retrieve tools for out-of-cluster corrections */
  ATH_CHECK(m_clusterLocalCalibOOCCTool.retrieve());

  ATH_CHECK(m_clusterLocalCalibOOCCPi0Tool.retrieve());

  /* Retrieve tool for DM corrections */
  ATH_CHECK(m_clusterLocalCalibDMTool.retrieve());

  return StatusCode::SUCCESS;

}

void eflowLCCalibTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer) {

  if (m_useLocalWeight) {
    std::unique_ptr<eflowRecClusterContainer> theEFRecCluster = m_clusterCollectionTool->retrieve(theEflowCaloObjectContainer, true);
    /* Calibrate each cluster */
    eflowRecClusterContainer::iterator itCluster = theEFRecCluster->begin();
    eflowRecClusterContainer::iterator endCluster = theEFRecCluster->end();
    for (; itCluster != endCluster; ++itCluster) {
      eflowRecCluster* cluster = (*itCluster);
      applyLocalWeight(cluster);
    }
  } else {
    /* Collect all the clusters in a temporary container (with VIEW_ELEMENTS!) */
    std::unique_ptr<xAOD::CaloClusterContainer> tempClusterContainer = m_clusterCollectionTool->execute(
        theEflowCaloObjectContainer, true);

    /* Calibrate each cluster */
    xAOD::CaloClusterContainer::iterator itCluster = tempClusterContainer->begin();
    xAOD::CaloClusterContainer::iterator endCluster = tempClusterContainer->end();
    for (; itCluster != endCluster; ++itCluster) {
      xAOD::CaloCluster* cluster = (*itCluster);
      /* Subsequently apply all ClusterLocalCalibTools, print debug output at each stage, if DEBUG it set */

      apply(m_clusterLocalCalibTool, cluster);

      apply(m_clusterLocalCalibOOCCTool, cluster);

      apply(m_clusterLocalCalibOOCCPi0Tool, cluster);

      apply(m_clusterLocalCalibDMTool, cluster);

    }

  }
}

StatusCode eflowLCCalibTool::finalize() {
  return StatusCode::SUCCESS;
}


void eflowLCCalibTool::apply(ToolHandle<CaloClusterCollectionProcessor>& calibTool, xAOD::CaloCluster* cluster) {

  assert(!m_useLocalWeight);
  CaloClusterCollectionProcessor* myCollectionProcessor = &(*calibTool);
  CaloClusterProcessor* myCalibProcessor = dynamic_cast<CaloClusterProcessor*>(myCollectionProcessor);
  if (myCalibProcessor) {
    if (myCalibProcessor->execute(cluster).isFailure()) {
      if (msgLvl(MSG::WARNING))
        msg(MSG::WARNING) << "Could not execute " << calibTool.name() << endmsg;
    }
  } else if (msgLvl(MSG::WARNING))
    msg(MSG::WARNING) << " dynamic_cast provided NULL pointer to CaloClusterProcessor " << endmsg;
}

void eflowLCCalibTool::applyLocalWeight(eflowRecCluster* theEFRecClusters) {
  xAOD::CaloCluster* theCluster = theEFRecClusters->getClusterForModification(
      eflowCaloObject::getClusterContainerPtr());

  /* Iterate over cells of old cluster and replicate them with energy weighted by -1 if negative and add it to the new cluster */
  const std::map<IdentifierHash, double> weightMap = theEFRecClusters->getCellsWeight();
  const CaloCell_ID* calo_id = CaloDetDescrManager::instance()->getCaloCell_ID();
  xAOD::CaloCluster::cell_iterator cellIter = theCluster->cell_begin();

  for (int cellIndex = 0; cellIter != theCluster->cell_end(); cellIter++) {
    const CaloCell* pCell = *cellIter;
    IdentifierHash myHashId = calo_id->calo_cell_hash(pCell->ID());
    double weight = weightMap.find(myHashId)->second;
    theCluster->reweightCell(cellIter, weight);
    cellIndex++;
  }

  CaloClusterKineHelper::calculateKine(theCluster, true, false);

  theCluster->recoStatus().setStatus(CaloRecoStatus::CALIBRATEDLHC);

}
