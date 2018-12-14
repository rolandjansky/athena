///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
class ICaloClusterMatchingTool;

namespace ClusterMatching {
  class CaloClusterMatchLinkAlg
    : public ::AthReentrantAlgorithm
  { 

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    CaloClusterMatchLinkAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor: 
    virtual ~CaloClusterMatchLinkAlg(); 

    // Assignment operator: 
    //CaloClusterMatchLinkAlg &operator=(const CaloClusterMatchLinkAlg &alg); 

    // Athena algorithm's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  execute(const EventContext& ctx) const;
    virtual StatusCode  finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  private: 

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterKey{this,"ClustersToDecorate","","The input CaloClusterContainer to match to CaloCalTopoClusters"};
    bool m_useLeadCellEtaPhi;
    int m_clusterSortMethod;
    ToolHandle<ICaloClusterMatchingTool> m_clusterMatch;

    /// Default constructor: 
    CaloClusterMatchLinkAlg();

    /// Containers
  

  }; 

  // I/O operators
  //////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Inline methods: 
  /////////////////////////////////////////////////////////////////// 
}

#endif //> !CALOCLUSTERMATCHING_CALOCLUSTERMATCHLINKALG_H
