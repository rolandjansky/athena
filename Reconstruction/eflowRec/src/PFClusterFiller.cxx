#include "eflowRec/PFClusterFiller.h"

#include "eflowRec/eflowCaloObject.h"

void PFClusterFiller::fillClustersToRecover(PFData &data) const{

  for (auto thisEflowCaloObject : *data.caloObjects )
  {
    if (thisEflowCaloObject->nClusters() == 0)continue;

    for (unsigned i = 0; i < thisEflowCaloObject->nClusters(); ++i)
    {
      /* Skip empty clusters (subtraction remnants) */
      const CaloClusterCellLink *theCellLink = thisEflowCaloObject->efRecCluster(i)->getCluster()->getCellLinks();
      if (0 == (int)theCellLink->size()) continue;

      thisEflowCaloObject->efRecCluster(i)->clearTrackMatches();
      data.clusters.push_back(thisEflowCaloObject->efRecCluster(i));
      thisEflowCaloObject->clearClusters();
    }
  }

}

void PFClusterFiller::fillClustersToConsider(PFData &data, eflowRecClusterContainer &recClusterContainer) const{

  for (unsigned int count = 0; count < recClusterContainer.size(); count++)
    data.clusters.push_back(recClusterContainer[count]);

}