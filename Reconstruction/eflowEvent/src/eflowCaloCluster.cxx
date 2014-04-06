/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCaloCluster.cxx
PACKAGE:  offline/Reconstruction/eflowEvent

AUTHORS:  Dan Tovey
CREATED:  Feb. 2004

PURPOSE:  Replica cluster work class for eflowRec
          Inherits from CaloCluster but includes non-const access

UPDATED:  April 9, 2004 (P Loch)
          adapt new Calorimeter EDM (new CaloCluster)

********************************************************************/

#include "eflowEvent/eflowCaloCluster.h"

eflowCaloCluster::eflowCaloCluster(const CaloCluster* clus) :  CaloCluster(clus)
{
 }


eflowCaloCluster::eflowCaloCluster(const CaloCluster* clus, bool copyMoments): CaloCluster(clus) {
  /* Clear the moment store*/
  if (!copyMoments) {
    m_momentStore.setMomentStore(CaloClusterMomentStore::moment_store());
  }
}

eflowCaloCluster::~eflowCaloCluster(){
}
