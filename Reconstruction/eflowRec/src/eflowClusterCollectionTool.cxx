/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowClusterCollectionTool.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"

eflowClusterCollectionTool::eflowClusterCollectionTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent)
{
  declareInterface<IEFlowClusterCollectionTool>(this);
}

StatusCode eflowClusterCollectionTool::initialize(){
  return StatusCode::SUCCESS;
}

std::unique_ptr<eflowRecClusterContainer> eflowClusterCollectionTool::retrieve(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters) {

  std::unique_ptr<eflowRecClusterContainer> result =  std::make_unique<eflowRecClusterContainer>();

  /* Loop over all eflowCaloObjects */
  eflowCaloObjectContainer::iterator itEFCaloObject = theEflowCaloObjectContainer->begin();
  eflowCaloObjectContainer::iterator endEFCaloObject = theEflowCaloObjectContainer->end();
  for (; itEFCaloObject != endEFCaloObject; ++itEFCaloObject) {

    eflowCaloObject* thisEflowCaloObject = *itEFCaloObject;

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


std::unique_ptr<xAOD::CaloClusterContainer> eflowClusterCollectionTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters) {

  std::unique_ptr<xAOD::CaloClusterContainer> result = std::make_unique<xAOD::CaloClusterContainer>(SG::VIEW_ELEMENTS);

  /* Loop over all eflowCaloObjects */
  eflowCaloObjectContainer::iterator itEFCaloObject = theEflowCaloObjectContainer->begin();
  eflowCaloObjectContainer::iterator endEFCaloObject = theEflowCaloObjectContainer->end();
  for (; itEFCaloObject != endEFCaloObject; ++itEFCaloObject) {

    eflowCaloObject* thisEflowCaloObject = *itEFCaloObject;

    /* Add all clusters on the eflowCaloObject to the container */
    unsigned int nClusters = thisEflowCaloObject->nClusters();
    for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
      eflowRecCluster* thisEfRecCluster = thisEflowCaloObject->efRecCluster(iCluster);

      /* Only add clusters if should run calibration tools or if touchable (i.e. we modified them */
      if (!useNonModifiedClusters && !thisEfRecCluster->isTouchable()  ) {
        continue;
      }

      xAOD::CaloCluster* thisCluster = thisEfRecCluster->getClusterForModification(eflowCaloObject::getClusterContainerPtr());
      result->push_back(thisCluster);

      msg(MSG::DEBUG) << "Adding cluster with E, eta and phi to moments maker "
          << thisCluster->e() << ", " << thisCluster->eta() << " and " << thisCluster->phi() << endmsg;
    }
  }
  return result;
}

StatusCode eflowClusterCollectionTool::finalize(){
  return StatusCode::SUCCESS;
}
