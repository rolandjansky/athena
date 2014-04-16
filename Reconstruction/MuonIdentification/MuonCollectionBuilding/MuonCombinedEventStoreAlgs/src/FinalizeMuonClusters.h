/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_FINALIZEMUONCLUSTERS_H
#define REC_FINALIZEMUONCLUSTERS_H
/*****************************************************************************
Name    : FinalizeMuonClusters.h
Package : Reconstruction/MuonIdentification/MuonCollectionBuilding/MuonCombinedEventStoreAlgs
Author  : Ketevi A. Assamagan
Created : July 2007
Note    : 

DESCRIPTION:

	- Finalization of Muon CaloCluster Making: CloShower links, CaloCell Links.
	- The Clusters are made in a tool; MuonIsloationTool but the tool cannot
	do the finalization. The main issue is that we have only one cluster container
        for ALL the muon algorithms; the links from Muon->Cluster must be made when both
	the Muon and the Cluster are made. This algorithm must run after ALL the Analysis::Muon
	making algorithms.

*****************************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

namespace Rec {

  class FinalizeMuonClusters : public AthAlgorithm {
  public:

    FinalizeMuonClusters(const std::string& name, ISvcLocator* pSvcLocator);
    ~FinalizeMuonClusters();

    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();

  private:

    std::string m_muonClusterContainerName;

  };
}

#endif // REC_FINALIZEMUONCLUSTER_H
