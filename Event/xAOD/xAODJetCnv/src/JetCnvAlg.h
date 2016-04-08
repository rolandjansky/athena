///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCnvAlg.h 
// Header file for class JetCnvAlg
// Author: P-A delsart
/////////////////////////////////////////////////////////////////// 
#ifndef XAODJETCNV_JETCNVALG_H
#define XAODJETCNV_JETCNVALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODJetCnv/IJetCnvTool.h"

namespace xAODMaker {
  class JetCnvAlg
    : public ::AthAlgorithm
  { 
    
    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    JetCnvAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor: 
    virtual ~JetCnvAlg(); 

    // Assignment operator: 
    //JetCnvAlg &operator=(const JetCnvAlg &alg); 

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

    /// Default constructor: 
    JetCnvAlg();

    /// Containers
    std::string m_inputJetCollectionName;
    std::string m_xaodJetCollectionName;
    std::string m_constitContainerName;

    bool m_constitSearch;
    
    ToolHandle<IJetCnvTool> m_cnvTool;

  }; 

  // I/O operators
  //////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Inline methods: 
/////////////////////////////////////////////////////////////////// 

}
#endif //> !XAODJETCNV_JETCNVALG_H
