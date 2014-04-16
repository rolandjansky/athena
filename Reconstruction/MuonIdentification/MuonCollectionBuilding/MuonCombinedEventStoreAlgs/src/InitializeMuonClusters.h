/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_INITIALIZEMUONCLUSTERS_H
#define REC_INITIALIZEMUONCLUSTERS_H
/*****************************************************************************
Name    : InitialzeMuonClusters.h
Package : Reconstruction/MuonIdentification/MuonCollectionBuilding/MuonCombinedEventStoreAlgs
Author  : Ketevi A. Assamagan
Created : August 2007
Note    : 

DESCRIPTION:

	- Initalization of Muon CaloCluster Making: opens a storegate collection
	- called before any combined muon algorithm runs
	- The Clusters are made in a tool; MuonIsloationTool but the tool cannot
	do the finalization. The main issue is that we have only one cluster container
        for ALL the muon algorithms; the links from Muon->Cluster must be made when both
	the Muon and the Cluster are made. This algorithm must run before all the Analysis::Muon
	making algorithms.

*****************************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

namespace Rec {
  class InitializeMuonClusters : public AthAlgorithm {
  public:

    InitializeMuonClusters(const std::string& name, ISvcLocator* pSvcLocator);
    ~InitializeMuonClusters();

    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();

  private:

    std::string m_muonClusterContainerName;

  };
}
#endif // REC_INITIALZEMUONCLUSTER_H
