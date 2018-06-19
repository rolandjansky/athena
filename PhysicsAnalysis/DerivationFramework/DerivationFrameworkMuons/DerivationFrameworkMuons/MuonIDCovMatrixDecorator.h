/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonIDCovMatrixDecorator.h
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MuonIDCovMatrixDecorator_H
#define DERIVATIONFRAMEWORK_MuonIDCovMatrixDecorator_H

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {
  class MuonIDCovMatrixDecorator : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    MuonIDCovMatrixDecorator( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    ~MuonIDCovMatrixDecorator();
 
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual StatusCode addBranches() const;
    
  private:
    std::string m_muonSGKey;
  }; 
}
#endif //
