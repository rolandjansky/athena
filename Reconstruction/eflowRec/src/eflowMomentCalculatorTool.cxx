/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowMomentCalculatorTool.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"

#include "xAODCaloEvent/CaloClusterKineHelper.h"

eflowMomentCalculatorTool::eflowMomentCalculatorTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_clusterCollectionTool("eflowRecClusterCollectionTool",this),
  m_clusterMomentsMaker("CaloClusterMomentsMaker",this),
  m_LCMode(false)
{
  declareInterface<eflowMomentCalculatorTool>(this);
  declareProperty("eflowRecClusterCollectionTool",  m_clusterCollectionTool, "Tool to choose clusters to use");
  declareProperty("CaloClusterMomentsMaker",      m_clusterMomentsMaker,          "Tool to Calculate Cluster Moments");
  declareProperty("LCMode", m_LCMode, "Toggle whether we are in LC mode or not");
}

StatusCode eflowMomentCalculatorTool::initialize(){

  /* Retrieve the cluster collection tool */
  ATH_CHECK(m_clusterCollectionTool.retrieve());

  /* Retrieve the cluster moments maker */
  if ( m_clusterMomentsMaker.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterMomentsMaker Tool "<< endmsg;
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

void eflowMomentCalculatorTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer) {

  /* Collect all the clusters in a temporary container (with VIEW_ELEMENTS!) */
  bool useNonModifiedClusters = true;
  if (true == m_LCMode) useNonModifiedClusters = false;
  std::unique_ptr<xAOD::CaloClusterContainer> tempClusterContainer = m_clusterCollectionTool->execute(theEflowCaloObjectContainer, useNonModifiedClusters);

  /* Set the layer energies */
  /* This must be set before the cluster moment calculations, which use the layer energies */
  for (auto cluster : *tempClusterContainer) CaloClusterKineHelper::calculateKine(cluster, true, true);

  /* Remake the cluster moments */

  if (m_clusterMomentsMaker->execute(tempClusterContainer.get()).isFailure()) {
    msg(MSG::WARNING) << "Could not execute ClusterMomentsMaker " << endmsg;
  }

}

StatusCode eflowMomentCalculatorTool::finalize(){ return StatusCode::SUCCESS; }


