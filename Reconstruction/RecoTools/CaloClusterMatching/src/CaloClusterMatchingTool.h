///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterMatchingTool.h 
// Header file for class CaloClusterMatchingTool
// Author: T.J. Khoo<khoo@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef CALOCLUSTERMATCHING_CALOCLUSTERMATCHINGTOOL_H
#define CALOCLUSTERMATCHING_CALOCLUSTERMATCHINGTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/AsgTool.h"
#include "StoreGate/WriteDecorHandleKey.h"

// CaloClusterMatching includes
#include "CaloClusterMatching/TopoClusterMap.h"
#include "CaloClusterMatching/ICaloClusterMatchingTool.h"

// xAOD includes
#include "xAODCaloEvent/CaloClusterContainer.h"

// Forward declaration

namespace ClusterMatching {

  class CaloClusterMatchingTool
    : virtual public asg::AsgTool,
      virtual public ::ICaloClusterMatchingTool
  { 
    ASG_TOOL_CLASS(CaloClusterMatchingTool, ICaloClusterMatchingTool)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 

    ///Delegate constructor to base-class
    using asg::AsgTool::AsgTool;

    /// Destructor: 
    virtual ~CaloClusterMatchingTool(); 

    // Athena algtool's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode  finalize() override;

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    StatusCode fillClusterMap(const EventContext& ctx, TopoClusterMap& tcmap) const override final;

    // return shared fraction of clustered cell energy
    float getClusterSharedEfrac(const xAOD::CaloCluster& refCluster,
				const xAOD::CaloCluster& testCluster) const override final;

    // return true if clusters share a given fraction of their cell energy
    bool clustersAreMatched(const xAOD::CaloCluster& refCluster,
			    const xAOD::CaloCluster& testCluster) const override final;

    // fill a list of clusters from the testClusters container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty.
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    const std::vector<const xAOD::CaloCluster*>& testClusters,
			    std::vector<const xAOD::CaloCluster*>& matchedClusters) const override final;

    // fill a list of clusters from the configured cluster container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    std::vector<const xAOD::CaloCluster*>& matchedClusters,
			    const TopoClusterMap& tcmap,
			    bool useLeadingCellEtaPhi) const override final;

    // fill a list of clusters from the testClusters container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty.
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    const std::vector<const xAOD::CaloCluster*>& testClusters,
			    std::vector<std::pair<const xAOD::CaloCluster*,float> >& matchedClusters) const override final;

    // fill a list of clusters from the configured cluster container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    std::vector<std::pair<const xAOD::CaloCluster*, float> >& matchedClusters,
			    const TopoClusterMap& tcmap,
			    bool useLeadingCellEtaPhi) const override final;

    // set ElementLinks to clusters from the configured cluster container that match the reference cluster
    // works via getMatchedClusters
    // return true if matchedClusters list is non-empty
    StatusCode linkMatchedClusters(const xAOD::CaloCluster& refCluster,
				   const std::vector<const xAOD::CaloCluster*>& testClusters,
				   bool (*gtrthan)(const std::pair<const xAOD::CaloCluster*,float>& pair1,
						   const std::pair<const xAOD::CaloCluster*,float>& pair2)) const override final;
    
    // set ElementLinks to clusters from the configured cluster container that match the reference cluster
    // works via getMatchedClusters
    // return true if matchedClusters list is non-empty
    StatusCode linkMatchedClusters(const xAOD::CaloCluster& refCluster,
				   const TopoClusterMap& tcmap,
				   bool useLeadingCellEtaPhi,
				   bool (*gtrthan)(const std::pair<const xAOD::CaloCluster*,float>& pair1,
						   const std::pair<const xAOD::CaloCluster*,float>& pair2)) const override final;

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  private:
    Gaudi::Property<bool> m_reqPosE{ this, "RequirePositiveE", true };
    Gaudi::Property<float> m_minSharedEfrac{ this, "MinSharedEfrac", 0.2 };
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clustersIn{
      this,
      "InputClusterCollection",
      "CaloCalTopoClusters",
      "The CaloCluster collection to match to "
    };
    // The typical usage is to decorate the
    // Muon Clusters with element links to the topological
    // clusters.
    // Ala what e/gamma clusters have by default
    Gaudi::Property<std::string> m_referenceClustersName{
      this,
      "ReferenceClusterToDecorate",
      "MuonClusterCollection",
      "The name of the collection the reference clusters belong to"
    };
 
    //The decorator
    SG::WriteDecorHandleKey<xAOD::CaloClusterContainer> m_elementLinkName{
      this,
      "ElementLinkName",
      "MuonClusterCollection.constituentClusterLinks"
    };

  }; 
}

#endif //> !CALOCLUSTERMATCHING_CALOCLUSTERMATCHINGTOOL_H
