/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthIsolationTool.h
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MuonTruthIsolationTool_H
#define DERIVATIONFRAMEWORK_MuonTruthIsolationTool_H

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

class IMCTruthClassifier;

namespace DerivationFramework {
  class MuonTruthIsolationTool : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    MuonTruthIsolationTool( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    ~MuonTruthIsolationTool();
 
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual StatusCode addBranches() const;
    
  private:
    std::string m_partSGKey;
    std::string m_truthSGKey;

  }; 
}
#endif //
