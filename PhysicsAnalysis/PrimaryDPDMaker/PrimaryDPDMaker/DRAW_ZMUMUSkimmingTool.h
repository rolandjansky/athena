/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DRAW_ZMUMUSkimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLEXAMPLE_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLEXAMPLE_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "xAODMuon/MuonContainer.h"

namespace DerivationFramework {

  /** @class DRAW_ZMUMUSkimmingTool
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class DRAW_ZMUMUSkimmingTool : public AthAlgTool, public ISkimmingTool {
    
  public: 
    /** Constructor with parameters */
    DRAW_ZMUMUSkimmingTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~DRAW_ZMUMUSkimmingTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    mutable std::atomic<unsigned int> m_ntot{0};
    mutable std::atomic<unsigned int> m_npass{0};

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonSGKey{this,"MuonContainerKey","Muons"};
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    unsigned int m_nMuons;
    double m_muonPtCut;

  }; 
  
}


#endif
