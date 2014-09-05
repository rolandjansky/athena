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
    
      ClusterAnalysis() {};
      ClusterAnalysis( TTree& tree ) : m_tree(&tree) {m_ntuple.initForRead(tree);}

      std::vector<std::vector<SpacePoint>> analyse(std::vector<Cluster*> clust);
      void analyseWrite();
     
    private:

      std::vector<std::vector<SpacePoint>> createSeeds(std::vector<SpacePoint>& points); 
      std::vector<SpacePoint> createSpacePoints(std::vector<Cluster*>& clust);

      TTree*        m_tree;
      ClusterNtuple m_ntuple;
      int           m_ncalls;
    };
}

#endif
