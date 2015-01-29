/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CROSSEDCALOCELLHELPER_H
#define CROSSEDCALOCELLHELPER_H

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "ParticleCaloExtension/ParticleCellAssociation.h"

namespace Rec {

  class CrossedCaloCellHelper {
  public:
   
    static xAOD::CaloCluster* crossedCells( const Rec::ParticleCellAssociation& association,
                                            const CaloCellContainer& cellContainer,
                                            xAOD::CaloClusterContainer& clusterContainer ) {
     
      // create cluster
      xAOD::CaloCluster* cluster = CaloClusterStoreHelper::makeCluster(&clusterContainer,&cellContainer);
      if( !cluster ){
        return nullptr;
      }
     
      // loop over intersections and add cells to cluster
      for( auto entry : association.cellIntersections() ){
        if( !entry.first || !entry.first->caloDDE() ) continue;
        int index = cellContainer.findIndex(entry.first->caloDDE()->calo_hash());
        if( index == -1 ) continue;
        cluster->addCell(index,1.);
      }
      return cluster;
    }
  };

}

#endif
