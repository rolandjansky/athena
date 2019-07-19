/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SOFTBTAG_CLUSTERING_H
#define SOFTBTAG_CLUSTERING_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "TrkVertexSeedFinderUtils/SeedNewtonTrkDistanceFinder.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

namespace SoftBVrt {

  class SoftBVrtClusterTool: public ::AthAlgorithm {
  public:
    SoftBVrtClusterTool( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~SoftBVrtClusterTool();

    virtual StatusCode initialize() override;
    virtual StatusCode initializeTools();
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    const std::string m_VX_COUNT_KEY = "BTaggingNumberOfPrimaryVertices";    
    std::string m_SVFinderName;
    std::string m_jetCollectionName;
    std::string m_trackjetCollectionName;
    std::string m_operatingPoint;
    float m_seed_pt;
    float m_seed_z0_sintheta;
    float m_seed_z0;
    float m_seed_z0_significance;
    float m_seed_d0_significance;
    float m_cluster_pt;
    float m_cluster_distance;
    float m_cluster_z0;
    float m_cluster_z0_significance;
    float m_cluster_d0_significance;
    float m_cluster_dr;
    float m_trackjet_pt_threshold;
    float m_jetPtCut;
    float m_PV_x;
    float m_PV_y;
    float m_PV_z;
    bool m_jetveto;
    bool m_trackjetveto;
    float m_overlap_frac;

    // secondary vertices
    ToolHandle< InDet::ISecVertexInJetFinder > m_secVertexFinderTool; 

    //Track distance tool
    ToolHandle< Trk::ITrkDistanceFinder > m_trkDistanceFinderTool;

    // Track quality tool
    ToolHandle< InDet::IInDetTrackSelectionTool > m_selTool;

  };

}

#endif //> !SOFTBTAG_CLUSTERING_H
