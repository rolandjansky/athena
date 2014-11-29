///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TauJetCnvAlg.h 
// Header file for class TauJetCnvAlg
// Author: Michel Janus , janus@cern.ch
/////////////////////////////////////////////////////////////////// 
#ifndef XAODTAUCNV_TAUJETCNVALG_H
#define XAODTAUCNV_TAUJETCNVALG_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTauCnv/ITauJetCnvTool.h"


namespace xAODMaker {
  class TauJetCnvAlg
    : public ::AthAlgorithm
  { 
    
    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    TauJetCnvAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor: 
    virtual ~TauJetCnvAlg(); 

    // Assignment operator: 
    //TauJetCnvAlg &operator=(const TauJetCnvAlg &alg); 

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
    TauJetCnvAlg();

    /// Containers
    std::string m_inputTauJetContainerName;
    std::string m_xaodTauJetContainerName;

    /** @brief Tool to perform taujet container conversion*/
    ToolHandle<ITauJetCnvTool>                 m_cnvTool;
    
  }; 

  // I/O operators
  //////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Inline methods: 
/////////////////////////////////////////////////////////////////// 

}
#endif //> !XAODTAUCNV_TAUJETCNVALG_H
