/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthClassifierFallback.h
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MuonTruthClassifierFallback_H
#define DERIVATIONFRAMEWORK_MuonTruthClassifierFallback_H

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

class IMCTruthClassifier;

namespace DerivationFramework {
  class MuonTruthClassifierFallback : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    MuonTruthClassifierFallback( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    ~MuonTruthClassifierFallback();
 
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual StatusCode addBranches() const;
    
  private:
    std::string m_partSGKey;
    std::string m_truthSGKey;
    std::string m_truthPileupSGKey;
    std::string m_truthMuonSGKey;
    float m_minPt;
    float m_maxDr;
    ToolHandle<IMCTruthClassifier>   m_mcTruthClassifier;


  }; 
}
#endif //
