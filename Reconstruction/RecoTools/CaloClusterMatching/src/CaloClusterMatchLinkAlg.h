///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterMatchLinkAlg.h 
// Header file for class CaloClusterMatchLinkAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef CALOCLUSTERMATCHING_CALOCLUSTERMATCHLINKALG_H
#define CALOCLUSTERMATCHING_CALOCLUSTERMATCHLINKALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AsgTools/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloClusterMatching/ICaloClusterMatchingTool.h"

namespace ClusterMatching {
  class CaloClusterMatchLinkAlg
    : public ::AthReentrantAlgorithm
  { 

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 
    /// Delegate constructor to base-class
    using AthReentrantAlgorithm::AthReentrantAlgorithm;

    /// Destructor: 
    virtual ~CaloClusterMatchLinkAlg(); 

    // Athena algorithm's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  execute(const EventContext& ctx) const;
    virtual StatusCode  finalize();

  private: 

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterKey{this,"ClustersToDecorate","","The input CaloClusterContainer to match to CaloCalTopoClusters"}; 
    Gaudi::Property<bool> m_useLeadCellEtaPhi{this,"UseLeadCellEtaPhi",false};
    Gaudi::Property<int> m_clusterSortMethod{this,"ClusterSortMethod",(int)MatchedE};
    ToolHandle<ICaloClusterMatchingTool> m_clusterMatch{this,"ClusterMatchTool","ClusterMatching::CaloClusterMatchingTool/CaloClusterMatch","Tool for the acutal matching"};

  }; 
}

#endif //> !CALOCLUSTERMATCHING_CALOCLUSTERMATCHLINKALG_H
