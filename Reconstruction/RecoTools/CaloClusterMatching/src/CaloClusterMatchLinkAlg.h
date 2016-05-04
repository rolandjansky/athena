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
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AsgTools/ToolHandle.h"
class ICaloClusterMatchingTool;

namespace ClusterMatching {
  class CaloClusterMatchLinkAlg
    : public ::AthAlgorithm
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
    virtual StatusCode  execute();
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

    std::string m_clustersToDecorate;
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
