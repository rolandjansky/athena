/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFClusterCollectionTool.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"

PFClusterCollectionTool::PFClusterCollectionTool(const std::string& type,const std::string& name,const IInterface* parent) : base_class( type, name, parent)
{
}

StatusCode PFClusterCollectionTool::initialize(){
  return StatusCode::SUCCESS;
}

std::unique_ptr<eflowRecClusterContainer> PFClusterCollectionTool::retrieve(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters) {

  std::unique_ptr<eflowRecClusterContainer> result =  std::make_unique<eflowRecClusterContainer>();

  /* Loop over all eflowCaloObjects */
  for (auto thisEflowCaloObject : theEflowCaloObjectContainer){
    
    /* Add all clusters on the eflowCaloObject to the container */
    unsigned int nClusters = thisEflowCaloObject->nClusters();
    for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
      eflowRecCluster* thisEfRecCluster = thisEflowCaloObject->efRecCluster(iCluster);

      /* Only add clusters if should run calibration tools or if touchable (i.e. we modified them */
      if (!useNonModifiedClusters && !thisEfRecCluster->isTouchable()  ) {
        continue;
      }

      result->push_back(thisEfRecCluster);
  }
}
  return result;
}


std::unique_ptr<xAOD::CaloClusterContainer> PFClusterCollectionTool::execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters, xAOD::CaloClusterContainer& theCaloClusterContainer) {

  std::unique_ptr<xAOD::CaloClusterContainer> result = std::make_unique<xAOD::CaloClusterContainer>(SG::VIEW_ELEMENTS);

  /* Loop over all eflowCaloObjects */
  for (auto thisEflowCaloObject : theEflowCaloObjectContainer){
    
    /* Add all clusters on the eflowCaloObject to the container */
    unsigned int nClusters = thisEflowCaloObject->nClusters();
    for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
      eflowRecCluster* thisEfRecCluster = thisEflowCaloObject->efRecCluster(iCluster);

      /* Only add clusters if should run calibration tools or if touchable (i.e. we modified them */
      if (!useNonModifiedClusters && !thisEfRecCluster->isTouchable()  ) {
        continue;
      }

      xAOD::CaloCluster* thisCluster = thisEfRecCluster->getClusterForModification(&theCaloClusterContainer);
      result->push_back(thisCluster);

      ATH_MSG_DEBUG("Adding cluster with E, eta and phi to moments maker " << thisCluster->e() << ", " << thisCluster->eta() << " and " << thisCluster->phi());
    }
  }
  return result;
}

StatusCode PFClusterCollectionTool::finalize(){
  return StatusCode::SUCCESS;
}
