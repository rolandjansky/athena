/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
////// ClusterAnalysis.h (c) ATLAS Detector software
////// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////////
#ifndef CLUSTERANALYSIS_H
#define CLUSTERANALYSIS_H

#include <vector>
#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"
#include "TH1F.h"
#include "TH2F.h"

namespace ClusterSeg {

  class ClusterAnalysis {
  public:

    ClusterAnalysis();
    ClusterAnalysis( TTree& tree );
    virtual ~ClusterAnalysis() = default;

    std::vector<std::vector<SpacePoint>> analyse(const std::vector<Cluster*>& clust);
    void analyseWrite();

  private:

    std::vector<std::vector<SpacePoint>> createTGCSeeds(const std::vector<SpacePoint>& points);
    std::vector<std::vector<SpacePoint>> createRPCSeeds(const std::vector<SpacePoint>& points);
    void createSeedsAllLayers(const std::vector<SpacePoint>& layer1Points,const std::vector<SpacePoint>& layer2Points,const std::vector<SpacePoint>& layer3Points,
			      std::vector<std::vector<SpacePoint>>& seeds);
    void createSeedsTwoLayers(const std::vector<SpacePoint>& layer1Points,const std::vector<SpacePoint>& layer2Points,
			      std::vector<std::vector<SpacePoint>>& seeds);
    std::vector<SpacePoint> createSpacePoints(const std::vector<Cluster*>& clust);

    TTree*        m_tree;
    ClusterNtuple m_ntuple{};
    int           m_ncalls{};

    bool m_writeOut;
    double m_ang_cut;
    double m_ang2_cut;
    double m_dist_cut;
    
    std::unique_ptr<TH2F> m_h_miss_RZ;
    std::unique_ptr<TH2F> m_h_miss_XY;
    std::unique_ptr<TH2F> m_h_rz;
    std::unique_ptr<TH2F> m_h_xy;
    std::unique_ptr<TH2F> m_h_corr;
    std::unique_ptr<TH1F> m_h_phi;
    std::unique_ptr<TH1F> m_h_theta;
    std::unique_ptr<TH1F> m_h_angle;
    std::unique_ptr<TH1F> m_h_phi_t;
    std::unique_ptr<TH1F> m_h_theta_t;
    std::unique_ptr<TH1F> m_h_angle_t;

    std::unique_ptr<TH1F> m_h_phi12;
    std::unique_ptr<TH1F> m_h_theta12;
    std::unique_ptr<TH1F> m_h_angle12;
    std::unique_ptr<TH1F> m_h_phi_t12;
    std::unique_ptr<TH1F> m_h_theta_t12;
    std::unique_ptr<TH1F> m_h_angle_t12;

    std::unique_ptr<TH1F> m_h_phi13;
    std::unique_ptr<TH1F> m_h_theta13;
    std::unique_ptr<TH1F> m_h_angle13;
    std::unique_ptr<TH1F> m_h_phi_t13;
    std::unique_ptr<TH1F> m_h_theta_t13;
    std::unique_ptr<TH1F> m_h_angle_t13;

    std::unique_ptr<TH1F> m_h_phi23;
    std::unique_ptr<TH1F> m_h_theta23;
    std::unique_ptr<TH1F> m_h_angle23;
    std::unique_ptr<TH1F> m_h_phi_t23;
    std::unique_ptr<TH1F> m_h_theta_t23;
    std::unique_ptr<TH1F> m_h_angle_t23;
 
    std::unique_ptr<TH1F> m_h_R;
    std::unique_ptr<TH1F> m_h_XY;
    std::unique_ptr<TH1F> m_h_R_t;
    std::unique_ptr<TH1F> m_h_XY_t;
    std::unique_ptr<TH1F> m_h_sizeL1;
    std::unique_ptr<TH1F> m_h_sizeL2;
    std::unique_ptr<TH1F> m_h_sizeL3;
    std::unique_ptr<TH1F> m_h_sizeL1L3;
    std::unique_ptr<TH1F> m_h_sizeL1L2L3;

    std::unique_ptr<TH1F> m_h_XY121;
    std::unique_ptr<TH1F> m_h_XY122;
    std::unique_ptr<TH1F> m_h_XY121_t;
    std::unique_ptr<TH1F> m_h_XY122_t;

    std::unique_ptr<TH1F> m_h_XY131;
    std::unique_ptr<TH1F> m_h_XY133;
    std::unique_ptr<TH1F> m_h_XY131_t;
    std::unique_ptr<TH1F> m_h_XY133_t;

    std::unique_ptr<TH1F> m_h_XY232;
    std::unique_ptr<TH1F> m_h_XY233;
    std::unique_ptr<TH1F> m_h_XY232_t;
    std::unique_ptr<TH1F> m_h_XY233_t;

    std::unique_ptr<TH1F> m_h_numseeds;
    std::unique_ptr<TH2F> m_h_barcodes;

  };
}

#endif
