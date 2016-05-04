///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// CaloClusterMatching includes
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

    // Copy constructor: 

    /// Constructor with parameters: 
    CaloClusterMatchingTool( const std::string& name );

    /// Destructor: 
    virtual ~CaloClusterMatchingTool(); 

    // Athena algtool's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    StatusCode fillClusterMap() const;

    // return shared fraction of clustered cell energy
    float getClusterSharedEfrac(const xAOD::CaloCluster& refCluster,
				const xAOD::CaloCluster& testCluster) const;

    // return true if clusters share a given fraction of their cell energy
    bool clustersAreMatched(const xAOD::CaloCluster& refCluster,
			    const xAOD::CaloCluster& testCluster) const;

    // fill a list of clusters from the testClusters container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty.
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    const std::vector<const xAOD::CaloCluster*>& testClusters,
			    std::vector<const xAOD::CaloCluster*>& matchedClusters) const;

    // fill a list of clusters from the configured cluster container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    std::vector<const xAOD::CaloCluster*>& matchedClusters,
			    bool useLeadingCellEtaPhi) const;

    // fill a list of clusters from the testClusters container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty.
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    const std::vector<const xAOD::CaloCluster*>& testClusters,
			    std::vector<std::pair<const xAOD::CaloCluster*,float> >& matchedClusters) const;

    // fill a list of clusters from the configured cluster container that match the reference cluster
    // match criteria determined by calling clustersAreMatched
    // return true if matchedClusters list is non-empty
    bool getMatchedClusters(const xAOD::CaloCluster& refCluster,
			    std::vector<std::pair<const xAOD::CaloCluster*, float> >& matchedClusters,
			    bool useLeadingCellEtaPhi) const;

    // set ElementLinks to clusters from the configured cluster container that match the reference cluster
    // works via getMatchedClusters
    // return true if matchedClusters list is non-empty
    StatusCode linkMatchedClusters(const xAOD::CaloCluster& refCluster,
				   const std::vector<const xAOD::CaloCluster*>& testClusters,
				   bool (*gtrthan)(const std::pair<const xAOD::CaloCluster*,float>& pair1,
						   const std::pair<const xAOD::CaloCluster*,float>& pair2)) const;
    
    // set ElementLinks to clusters from the configured cluster container that match the reference cluster
    // works via getMatchedClusters
    // return true if matchedClusters list is non-empty
    StatusCode linkMatchedClusters(const xAOD::CaloCluster& refCluster,
				   bool useLeadingCellEtaPhi,
				   bool (*gtrthan)(const std::pair<const xAOD::CaloCluster*,float>& pair1,
						   const std::pair<const xAOD::CaloCluster*,float>& pair2)) const;

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  private: 

    bool m_reqPosE;
    float m_minSharedEfrac;
    std::string m_clustersIn;
    std::string m_mapName;
    std::string m_elementLinkName;

    SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::CaloClusterContainer> > > m_elementLinkDec;

    /// Default constructor: 
    CaloClusterMatchingTool();

    // Containers
  

  }; 

  // I/O operators
  //////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Inline methods: 
  /////////////////////////////////////////////////////////////////// 

}

#endif //> !CALOCLUSTERMATCHING_CALOCLUSTERMATCHINGTOOL_H
