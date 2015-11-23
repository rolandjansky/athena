/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
////// ClusterAnalysis.h (c) ATLAS Detector software
////// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////////
#ifndef CLUSTERANALYSIS_H
#define CLUSTERANALYSIS_H

#include <vector>
#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"

namespace ClusterSeg {

  class ClusterAnalysis {
    public:

      ClusterAnalysis() : m_tree(NULL), m_ncalls(-1) {m_ntuple.init();}
      ClusterAnalysis( TTree& tree ) : m_tree(&tree) {m_ntuple.initForRead(tree);}

      std::vector<std::vector<SpacePoint>> analyse(std::vector<Cluster*> clust);
      void analyseWrite();

    private:

      std::vector<std::vector<SpacePoint>> createTGCSeeds(std::vector<SpacePoint> points);
      std::vector<std::vector<SpacePoint>> createRPCSeeds(std::vector<SpacePoint> points);
      void createSeedsAllLayers(std::vector<SpacePoint> layer1Points,std::vector<SpacePoint> layer2Points,std::vector<SpacePoint> layer3Points,
                                             std::vector<std::vector<SpacePoint>>& seeds);
      void createSeedsTwoLayers(std::vector<SpacePoint> layer1Points,std::vector<SpacePoint> layer2Points,
                                             std::vector<std::vector<SpacePoint>>& seeds);
      std::vector<SpacePoint> createSpacePoints(std::vector<Cluster*> clust);

      TTree*        m_tree;
      ClusterNtuple m_ntuple;
      int           m_ncalls;
    };
}

#endif
