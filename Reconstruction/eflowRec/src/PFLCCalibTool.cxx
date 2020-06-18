/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFLCCalibTool.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloRec/CaloClusterProcessor.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

PFLCCalibTool::PFLCCalibTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

StatusCode PFLCCalibTool::initialize() {

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

void PFLCCalibTool::execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer) {

  if (m_useLocalWeight) {
    std::unique_ptr<eflowRecClusterContainer> theEFRecClusterContainer = m_clusterCollectionTool->retrieve(theEflowCaloObjectContainer, true);
    /* Calibrate each cluster */
    for (auto thisEFlowRecCluster : *theEFRecClusterContainer) applyLocalWeight(thisEFlowRecCluster);
  } else {
    /* Collect all the clusters in a temporary container (with VIEW_ELEMENTS!) */
    std::unique_ptr<xAOD::CaloClusterContainer> tempClusterContainer = m_clusterCollectionTool->execute(theEflowCaloObjectContainer, true);

    /* Calibrate each cluster */
    for (auto thisCaloCluster : *tempClusterContainer){
      /* Subsequently apply all ClusterLocalCalibTools, print debug output at each stage, if DEBUG it set */
      apply(m_clusterLocalCalibTool, thisCaloCluster);

      apply(m_clusterLocalCalibOOCCTool, thisCaloCluster);

      apply(m_clusterLocalCalibOOCCPi0Tool, thisCaloCluster);

      apply(m_clusterLocalCalibDMTool, thisCaloCluster);
    }//loop on CaloCluster
  }//if not use local weight scheme
}

StatusCode PFLCCalibTool::finalize() {
  return StatusCode::SUCCESS;
}


void PFLCCalibTool::apply(ToolHandle<CaloClusterProcessor>& calibTool, xAOD::CaloCluster* cluster) {
  if (m_useLocalWeight) ATH_MSG_WARNING("Applying recalculated weights, when configuration requested to use original weights");
  if (calibTool->execute(cluster).isFailure()) ATH_MSG_WARNING("Could not execute " << calibTool.name());
}

void PFLCCalibTool::applyLocalWeight(eflowRecCluster* theEFRecClusters) {
  xAOD::CaloCluster* theCluster = theEFRecClusters->getCluster();

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
