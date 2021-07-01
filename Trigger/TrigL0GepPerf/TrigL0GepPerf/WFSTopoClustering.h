/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL0GEPPERF_WFSTOPOCLUSTERING_H
#define TRIGL0GEPPERF_WFSTOPOCLUSTERING_H

#include "TrigL0GepPerf/ITopoClusterMaker.h"

#include <map>
#include <string>

namespace Gep{
  class WFSTopoClusterMaker : virtual public ITopoClusterMaker {

public:

    std::vector<Gep::CustomTopoCluster> makeTopoCluster(const std::map<unsigned int,Gep::CustomCaloCell> &caloCellsMap) override;
    bool isSeedCell (Gep::CustomCaloCell cell);
    bool isInAllowedSampling(int sampling, std::vector<int> list_of_samplings);
    bool isNewCell(unsigned int id, std::vector<unsigned int> seenCells);
    std::vector<Gep::CustomCaloCell> clusterFromCell(Gep::CustomCaloCell seed, const std::map<unsigned int, Gep::CustomCaloCell> *caloCellsMap);
    Gep::CustomTopoCluster getClusterFromListOfCells(std::vector<Gep::CustomCaloCell> cells);
    double getDeltaPhi(double phi, double seed_phi);
    double calculateClusterPhi(double seed_phi, double delta_phi);
    void orderClustersInEt(std::vector<Gep::CustomTopoCluster> &v_clusters);
    std::string getName() const override;

    WFSTopoClusterMaker() {}
    ~WFSTopoClusterMaker() {}

private:

    float m_seed_threshold = 4.0;
    float m_clustering_threshold = 2.0;
    int m_max_shells = 8;
    std::vector<int> m_allowed_seed_samplings = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    std::vector<int> m_allowed_clustering_samplings = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

  };
}

#endif
